// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/setting_handler.h"

#include <boost/algorithm/string.hpp>
#include <string.h>

#include <cassert>
#include <map>
#include <utility>

#include "manifest_parser/utils/version_number.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"
#include "wgt_manifest_handlers/tizen_application_handler.h"

namespace {

const char kTrueValue[] = "true";
const char kFalseValue[] = "false";

const char kTizenHardwareKey[] = "@hwkey-event";
const char kTizenScreenOrientationKey[] = "@screen-orientation";
const char kTizenEncryptionKey[] = "@encryption";
const char kTizenContextMenuKey[] = "@context-menu";
const char kTizenBackgroundSupportKey[] = "@background-support";
const char kTizenNoDisplayKey[] = "@nodisplay";
const char kTizenIndicatorPresenceKey[] = "@indicator-presence";
const char kTizenBackbuttonPresenceKey[] = "@backbutton-presence";
const char kTizenInstallLocationKey[] = "@install-location";
const char kTizenUserAgentKey[] = "@user-agent";
const char kTizenSoundModeKey[] = "@sound-mode";
const char kTizenBackgroundVibrationKey[] = "@background-vibration";
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kTizenSettingKey[] = "widget.setting";
const utils::VersionNumber kDefaultAutoOrientationVersion("3.0");

bool ForAllFindKey(const parser::Value* value, const std::string& key,
                   std::string* result) {
  if (value->GetType() == parser::Value::TYPE_DICTIONARY) {
    const parser::DictionaryValue* dict = nullptr;
    value->GetAsDictionary(&dict);
    if (!parser::VerifyElementNamespace(*dict, kTizenNamespacePrefix))
      return false;
    if (dict->GetString(key, result)) return true;
  } else if (value->GetType() == parser::Value::TYPE_LIST) {
    const parser::ListValue* list = nullptr;
    value->GetAsList(&list);
    for (auto& item : *list) {
      const parser::DictionaryValue* dict = nullptr;
      if (item->GetAsDictionary(&dict)) {
        if (!parser::VerifyElementNamespace(*dict, kTizenNamespacePrefix))
          continue;
        if (dict->GetString(key, result)) return true;
      }
    }
  }
  return false;
}

}  // namespace

namespace wgt {
namespace parse {

SettingInfo::SettingInfo()
    : hwkey_enabled_(true),
      screen_orientation_(ScreenOrientation::AUTO),
      encryption_enabled_(false),
      context_menu_enabled_(true),
      background_support_enabled_(false),
      install_location_(InstallLocation::AUTO),
      no_display_(false),
      indicator_presence_(true),
      backbutton_presence_(false),
      sound_mode_(SoundMode::SHARED),
      background_vibration_(false),
      orientation_defaulted_(true) {}

SettingInfo::~SettingInfo() {}

SettingHandler::SettingHandler() {}

SettingHandler::~SettingHandler() {}

bool SettingHandler::Parse(const parser::Manifest& manifest,
                           std::shared_ptr<parser::ManifestData>* output,
                           std::string* /*error*/) {
  const parser::Value* value = nullptr;
  manifest.Get(kTizenSettingKey, &value);

  std::shared_ptr<SettingInfo> app_info(new SettingInfo);
  std::string hwkey;
  ForAllFindKey(value, kTizenHardwareKey, &hwkey);
  app_info->set_hwkey_enabled(hwkey != "disable");

  std::string screen_orientation;
  ForAllFindKey(value, kTizenScreenOrientationKey, &screen_orientation);
  if (strcasecmp("portrait", screen_orientation.c_str()) == 0)
    app_info->set_screen_orientation(SettingInfo::ScreenOrientation::PORTRAIT);
  else if (strcasecmp("landscape", screen_orientation.c_str()) == 0)
    app_info->set_screen_orientation(SettingInfo::ScreenOrientation::LANDSCAPE);
  else if (strcasecmp("auto", screen_orientation.c_str()) == 0)
    app_info->set_screen_orientation(SettingInfo::ScreenOrientation::AUTO);

  std::string encryption;
  ForAllFindKey(value, kTizenEncryptionKey, &encryption);
  app_info->set_encryption_enabled(encryption == "enable");

  std::string context_menu;
  ForAllFindKey(value, kTizenContextMenuKey, &context_menu);
  app_info->set_context_menu_enabled(context_menu != "disable");

  std::string background_support;
  ForAllFindKey(value, kTizenBackgroundSupportKey, &background_support);
  app_info->set_background_support_enabled(background_support == "enable");

  std::string install_location;
  ForAllFindKey(value, kTizenInstallLocationKey, &install_location);
  if (strcasecmp("internal-only", install_location.c_str()) == 0)
    app_info->set_install_location(SettingInfo::InstallLocation::INTERNAL);
  else if (strcasecmp("prefer-external", install_location.c_str()) == 0)
    app_info->set_install_location(SettingInfo::InstallLocation::EXTERNAL);

  std::string no_display;
  ForAllFindKey(value, kTizenNoDisplayKey, &no_display);
  app_info->set_no_display(boost::iequals(no_display, kTrueValue));

  std::string indicator_presence;
  ForAllFindKey(value, kTizenIndicatorPresenceKey, &indicator_presence);
  app_info->set_indicator_presence(indicator_presence == "enable");

  std::string backbutton_presence;
  ForAllFindKey(value, kTizenBackbuttonPresenceKey, &backbutton_presence);
  app_info->set_backbutton_presence(backbutton_presence == "enable");

  std::string user_agent;
  ForAllFindKey(value, kTizenUserAgentKey, &user_agent);
  app_info->set_user_agent(user_agent);

  std::string background_vibration;
  ForAllFindKey(value, kTizenBackgroundVibrationKey, &background_vibration);
  app_info->set_background_vibration(background_vibration == "enable");

  std::string sound_mode;
  ForAllFindKey(value, kTizenSoundModeKey, &sound_mode);
  if (strcasecmp("exclusive", sound_mode.c_str()) == 0)
    app_info->set_sound_mode(SettingInfo::SoundMode::EXCLUSIVE);

  *output = std::static_pointer_cast<parser::ManifestData>(app_info);
  return true;
}

bool SettingHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& handlers_output,
    std::string* error) const {
  const SettingInfo& setting_info = static_cast<const SettingInfo&>(data);

#if defined(TIZEN_MOBILE) || defined(TIZEN_WEARABLE)
  // backward compatibility
  if (setting_info.orientation_defaulted()) {
    const TizenApplicationInfo& app_info =
      static_cast<const TizenApplicationInfo&>(
        *handlers_output.find(TizenApplicationInfo::Key())->second);
    utils::VersionNumber required_version(app_info.required_version());
    if (!required_version.IsValid()) {
      *error = "Cannot retrieve required API version from widget";
      return false;
    }
    if (required_version < kDefaultAutoOrientationVersion) {
      auto constless_setting_info = const_cast<SettingInfo&>(setting_info);
      constless_setting_info.set_screen_orientation(
          SettingInfo::ScreenOrientation::PORTRAIT);
      // keep claiming it is default
      constless_setting_info.set_orientation_defaulted(true);
    }
  }
#endif

  if (setting_info.screen_orientation() !=
          SettingInfo::ScreenOrientation::AUTO &&
      setting_info.screen_orientation() !=
          SettingInfo::ScreenOrientation::PORTRAIT &&
      setting_info.screen_orientation() !=
          SettingInfo::ScreenOrientation::LANDSCAPE) {
    *error = "Wrong value of screen orientation";
    return false;
  }

  if (setting_info.install_location() != SettingInfo::InstallLocation::AUTO &&
      setting_info.install_location() !=
          SettingInfo::InstallLocation::INTERNAL &&
      setting_info.install_location() !=
          SettingInfo::InstallLocation::EXTERNAL) {
    *error = "Wrong value of install location";
    return false;
  }

  if (setting_info.sound_mode() != SettingInfo::SoundMode::SHARED &&
      setting_info.sound_mode() != SettingInfo::SoundMode::EXCLUSIVE) {
    *error = "Wrong value of screen sound mode";
    return false;
  }

  return true;
}


std::string SettingHandler::Key() const {
  return kTizenSettingKey;
}

std::vector<std::string> SettingHandler::PrerequisiteKeys() const {
  return { TizenApplicationInfo::Key() };
}

std::string SettingInfo::Key() {
  return kTizenSettingKey;
}

}  // namespace parse
}  // namespace wgt
