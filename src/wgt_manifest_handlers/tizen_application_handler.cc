// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/tizen_application_handler.h"

#include <cassert>
#include <map>
#include <utility>

#include "manifest_parser/manifest_util.h"
#include "manifest_parser/utils/version_number.h"
#include "manifest_parser/values.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"
#include "wgt_manifest_handlers/platform_version.h"

namespace wgt {
namespace parse {

namespace {

const char kTizenApplicationKey[] = "widget.application";
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kNamespaceKey[] = "@namespace";
const char kTizenApplicationIdKey[] = "@id";
const char kTizenApplicationPackageKey[] = "@package";
const char kTizenApplicationLaunchModeKey[] = "@launch_mode";
const char kTizenApplicationRequiredVersionKey[] = "@required_version";
const char kTizenApplicationAmbientSupportKey[] = "@ambient_support";
const utils::VersionNumber kLaunchModeRequiredVersion("2.4");
}  // namespace

TizenApplicationInfo::TizenApplicationInfo() {}

TizenApplicationInfo::~TizenApplicationInfo() {}

TizenApplicationHandler::TizenApplicationHandler() {}

TizenApplicationHandler::~TizenApplicationHandler() {}

bool TizenApplicationHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output, std::string* error) {
  std::shared_ptr<TizenApplicationInfo> app_info(new TizenApplicationInfo);
  parser::Value* app_value = nullptr;
  manifest.Get(kTizenApplicationKey, &app_value);
  // Find an application element with tizen namespace
  parser::DictionaryValue* app_dict;
  std::string value;
  bool found = false;
  if (app_value && app_value->IsType(parser::Value::TYPE_DICTIONARY)) {
    app_value->GetAsDictionary(&app_dict);
    found = app_dict->GetString(kNamespaceKey, &value);
    found = found && (value == kTizenNamespacePrefix);
  } else if (app_value && app_value->IsType(parser::Value::TYPE_LIST)) {
    parser::ListValue* list;
    app_value->GetAsList(&list);
    for (parser::ListValue::iterator it = list->begin(); it != list->end();
         ++it) {
      (*it)->GetAsDictionary(&app_dict);
      app_dict->GetString(kNamespaceKey, &value);
      bool is_tizen = (value == kTizenNamespacePrefix);
      if (is_tizen) {
        if (found) {
          *error = "There should be no more than one tizen:application element";
          return false;
        }
        found = true;
      }
    }
  }

  if (!found) {
    *error =
        "Cannot find application element with tizen namespace "
        "or the tizen namespace prefix is incorrect.\n";
    return false;
  }
  if (app_dict->GetString(kTizenApplicationIdKey, &value))
    app_info->set_id(value);
  if (app_dict->GetString(kTizenApplicationPackageKey, &value)) {
    app_info->set_package(value);
  }
  if (app_dict->GetString(kTizenApplicationRequiredVersionKey, &value)) {
    if (!value.empty()) {
      utils::VersionNumber req_version(value);
      utils::VersionNumber min_version = parser::GetMinimumPlatformVersion();
      if (req_version < min_version) {
        app_info->set_required_version(min_version.ToString());
      } else {
        app_info->set_required_version(value);
      }
    }
  }
  std::string ambient_support;
  if (app_dict->GetString(kTizenApplicationAmbientSupportKey,
                          &ambient_support)) {
    app_info->set_ambient_support(ambient_support == "enable");
  }

  std::string launch_mode;
  app_dict->GetString(kTizenApplicationLaunchModeKey, &launch_mode);
  app_info->set_launch_mode(launch_mode);

  *output = std::static_pointer_cast<parser::ManifestData>(app_info);
  return true;
}

bool TizenApplicationHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {
  const TizenApplicationInfo& app_info =
      static_cast<const TizenApplicationInfo&>(data);

  if (!parser::ValidateTizenApplicationId(app_info.id())) {
    *error =
        "The id property of application element "
        "does not match the format\n";
    return false;
  }

  if (!parser::ValidateTizenPackageId(app_info.package())) {
    *error =
        "The package property of application element "
        "does not match the format\n";
    return false;
  }

  if (app_info.id().find(app_info.package()) != 0) {
    *error =
        "The application element property id "
        "does not start with package.\n";
    return false;
  }
  if (app_info.required_version().empty()) {
    *error =
        "The required_version property of application "
        "element does not exist.\n";
    return false;
  }
  utils::VersionNumber supported_version = parser::GetCurrentPlatformVersion();
  if (!supported_version.IsValid()) {
    *error = "Cannot retrieve supported API version from platform";
    return false;
  }
  utils::VersionNumber required_version(app_info.required_version());
  if (!required_version.IsValid()) {
    *error = "Cannot retrieve supported API version from widget";
    return false;
  }
  if (supported_version < required_version) {
    *error =
        "The required_version of Tizen Web API "
        "is not supported.\n";
    return false;
  }
  if (required_version >= kLaunchModeRequiredVersion) {
    if (app_info.launch_mode().empty()) {
      // FIXME for now, this const_cast is used, but it is not the best way.
      TizenApplicationInfo &tmp = const_cast<TizenApplicationInfo &>(app_info);
      tmp.set_launch_mode("single");  // default parameter
    } else if (app_info.launch_mode() != "caller" &&
             app_info.launch_mode() != "group" &&
             app_info.launch_mode() != "single") {
      *error = "Wrong value of launch mode";
      return false;
    }
  } else if (!app_info.launch_mode().empty()) {
    *error = "launch_mode attribute cannot be used for api version lower"
             " than 2.4";
    return false;
  }
  return true;
}

std::string TizenApplicationInfo::Key() {
  return kTizenApplicationKey;
}

std::string TizenApplicationHandler::Key() const {
  return kTizenApplicationKey;
}

bool TizenApplicationHandler::AlwaysParseForKey() const { return true; }

}  // namespace parse
}  // namespace wgt
