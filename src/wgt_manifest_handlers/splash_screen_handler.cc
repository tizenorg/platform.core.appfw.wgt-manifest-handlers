// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/splash_screen_handler.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <map>
#include <utility>

#include "manifest_parser/utils/logging.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace bf = boost::filesystem;
namespace keys = wgt::application_widget_keys;
namespace wgt_parse = wgt::parse;

namespace {
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kTagDelimiter[] = " ";
const char kFirstPaint[] = "first-paint";
const char kComplete[] = "complete";
const char kCustom[] = "custom";
const char kSplashScreenDefault[] = "widget.launch_screen.ls_default";
const char kSplashScreenPortrait[] = "widget.launch_screen.ls_portrait";
const char kSplashScreenLandscape[] = "widget.launch_screen.ls_landscape";
const char kSplashScreenKey[] = "widget.launch_screen";
const char kSplashScreenReadyWhen[] = "@ready_when";
const char kSplashScreenBgColor[] = "@background_color";
const char kSplashScreenBgImage[] = "@background_image";
const char kSplashScreenImage[] = "@image";
const char kSplashScreenImageBorder[] = "@image_border";
const std::map<wgt_parse::ScreenOrientation, const char*> kOrientationMap = {
    {wgt_parse::ScreenOrientation::AUTO, kSplashScreenDefault},
    {wgt_parse::ScreenOrientation::LANDSCAPE, kSplashScreenLandscape},
    {wgt_parse::ScreenOrientation::PORTRAIT, kSplashScreenPortrait}};

std::shared_ptr< wgt_parse::Color>
ConvertStringToColor(const std::string& color_str) {
  auto to_long = [&color_str](unsigned int start, unsigned int end) -> int {
    return std::strtoul(color_str.substr(start, end).c_str(), 0, 16);
  };
  std::shared_ptr<wgt_parse::Color> color(new wgt_parse::Color);
  switch (color_str.size()) {
    case 3:
      color->red = to_long(0, 0);
      color->green = to_long(1, 1);
      color->blue = to_long(2, 1);
      break;
    case 6:
      color->red = to_long(0, 2);
      color->green = to_long(2, 2);
      color->blue = to_long(4, 2);
      break;
    default:
      return nullptr;
  }
  return color;
}

bool IsStringToColorConvertable(const std::string& string_color) {
  return string_color[0] == '#' &&
         (string_color.size() == 4 || string_color.size() == 7);
}

}  // namespace

namespace wgt {
namespace parse {

std::string SplashScreenInfo::Key() {
  return kSplashScreenKey;
}

bool SplashScreenHandler::ParseSingleOrientation(
    const parser::Manifest& manifest, ScreenOrientation orientation,
    SplashScreenInfo* ss_info) {
  auto dict_element_parser =
      [this](SplashScreenData& splash_screen,
             const parser::DictionaryValue* dict) -> bool {
    if (!parser::VerifyElementNamespace(*dict, kTizenNamespacePrefix))
      return false;

    bool parsed_color = ParseColor(dict, &splash_screen);
    ParseElement(dict, &splash_screen.background_image, kSplashScreenBgImage);
    ParseElement(dict, &splash_screen.image, kSplashScreenImage);
    ParseElement(dict, &splash_screen.image_border, kSplashScreenImageBorder);

    return parsed_color && (!splash_screen.background_image.empty() ||
        !splash_screen.image.empty() || splash_screen.background_color);
  };

  auto orientation_chosen = kOrientationMap.at(orientation);
  SplashScreenData splash_screen;

  auto& dict_element =
      parser::GetOneOrMany(manifest.value(),
                           orientation_chosen,
                           kTizenNamespacePrefix);
  if (dict_element.empty())
    return true;

  if (dict_element.size() > 1)
    return false;

  if (!dict_element_parser(splash_screen, dict_element[0]))
    return false;

  ss_info->set_splash_screen_data(std::make_pair(orientation, splash_screen));
  return true;
}

bool SplashScreenHandler::ParseElement(const parser::DictionaryValue* dict,
                                       std::vector<std::string>* to_be_saved,
                                       const char* keyToParse) {
  // TODO(w.kosowicz) check which elements not declared should cause fail
  std::string element;
  dict->GetString(keyToParse, &element);
  boost::split(*to_be_saved, element, boost::is_any_of(kTagDelimiter));
  return true;
}

bool SplashScreenHandler::ParseColor(const parser::DictionaryValue* dict,
                                     SplashScreenData* splash_screen) {
  std::string background_color;
  if (!dict->GetString(kSplashScreenBgColor, &background_color))
    return true;

  if (!IsStringToColorConvertable(background_color))
    return false;

  std::string only_hex = background_color.substr(1);
  if (!std::all_of(only_hex.begin(), only_hex.end(), isxdigit)) return false;

  splash_screen->background_color = ConvertStringToColor(only_hex);
  return true;
}

bool SplashScreenHandler::ParseReadyWhen(const parser::Manifest& manifest,
                                         SplashScreenInfo* ss_info) {
  auto dict_values = parser::GetOneOrMany(manifest.value(),
                                          kSplashScreenKey,
                                          kTizenNamespacePrefix);
  if (dict_values.empty())
    return false;

  std::string ready_when;
  dict_values[0]->GetString(kSplashScreenReadyWhen, &ready_when);

  if (ready_when.empty()) {
      ready_when = kFirstPaint;
  }
  if (ready_when != kFirstPaint && ready_when != kComplete &&
      ready_when != kCustom)
    return false;

  std::map<std::string, std::function<void()>> ready_when_functors;
  ready_when_functors[kFirstPaint] = std::bind(
      &SplashScreenInfo::set_ready_when, ss_info, ReadyWhen::FIRSTPAINT);
  ready_when_functors[kComplete] = std::bind(&SplashScreenInfo::set_ready_when,
                                             ss_info, ReadyWhen::COMPLETE);
  ready_when_functors[kCustom] =
      std::bind(&SplashScreenInfo::set_ready_when, ss_info, ReadyWhen::CUSTOM);
  ready_when_functors[ready_when]();

  return true;
}

bool SplashScreenHandler::Parse(const parser::Manifest& manifest,
                                std::shared_ptr<parser::ManifestData>* output,
                                std::string* error) {
  auto ss_info = std::make_shared<SplashScreenInfo>();
  if (!ParseReadyWhen(manifest, ss_info.get())) return false;

  if (!ParseSingleOrientation(manifest, ScreenOrientation::AUTO,
                              ss_info.get())) {
    *error = "Failed to parse launch screen default orientation";
    return false;
  }
  if (!ParseSingleOrientation(manifest, ScreenOrientation::LANDSCAPE,
                              ss_info.get())) {
    *error = "Failed to parse launch screen landscape orientation";
    return false;
  }
  if (!ParseSingleOrientation(manifest, ScreenOrientation::PORTRAIT,
                              ss_info.get())) {
    *error = "Failed to parse launch screen portrait orientation";
    return false;
  }

  *output = std::static_pointer_cast<parser::ManifestData>(ss_info);
  return true;
}

std::string SplashScreenHandler::Key() const {
  return kSplashScreenKey;
}

}  // namespace parse
}  // namespace wgt
