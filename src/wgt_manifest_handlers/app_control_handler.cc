// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/app_control_handler.h"

#include "manifest_parser/utils/iri_util.h"
#include "manifest_parser/utils/version_number.h"
#include "manifest_parser/values.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"
#include "wgt_manifest_handlers/tizen_application_handler.h"

namespace {
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";

const char kEnabledValue[] = "enable";
const char kDisabledValue[] = "disable";

const utils::VersionNumber kReloadRequiredVersion("2.4");
const char kTizenApplicationKey[] = "widget.application";
const char kTizenApplicationAppControlSrcKey[] = "src";
const char kTizenApplicationAppControlOperationKey[] = "operation";
const char kTizenApplicationAppControlUriKey[] = "uri";
const char kTizenApplicationAppControlMimeKey[] = "mime";
const char kTizenApplicationAppControlReloadKey[] = "@reload";
const char kTizenApplicationAppControlChildNameAttrKey[] = "@name";
const char kTizenApplicationAppControlsKey[] = "widget.app-control";

}  // namespace

namespace wgt {
namespace parse {

namespace {

void ParseAppControlEntryAndStore(
    const parser::DictionaryValue& control_dict,
    AppControlInfoList* aplist) {
  std::string src;
  std::string reload;
  const parser::DictionaryValue* src_dict;
  if (control_dict.GetDictionary(kTizenApplicationAppControlSrcKey,
      &src_dict)) {
    src_dict->GetString(
        kTizenApplicationAppControlChildNameAttrKey, &src);
    src_dict->GetString(kTizenApplicationAppControlReloadKey, &reload);
  }

  std::string operation;
  const parser::DictionaryValue* operation_dict;
  if (control_dict.GetDictionary(
      kTizenApplicationAppControlOperationKey,
      &operation_dict)) {
    operation_dict->GetString(
        kTizenApplicationAppControlChildNameAttrKey, &operation);
  }

  std::string uri;
  const parser::DictionaryValue* uri_dict;
  if (control_dict.GetDictionary(kTizenApplicationAppControlUriKey,
      &uri_dict)) {
    uri_dict->GetString(
        kTizenApplicationAppControlChildNameAttrKey, &uri);
  }

  std::string mime;
  const parser::DictionaryValue* mime_dict;
  if (control_dict.GetDictionary(kTizenApplicationAppControlMimeKey,
      &mime_dict)) {
    mime_dict->GetString(
        kTizenApplicationAppControlChildNameAttrKey, &mime);
  }

  aplist->controls.emplace_back(src, operation, uri, mime, reload);
}

}  // namespace

AppControlHandler::AppControlHandler() {
}

AppControlHandler::~AppControlHandler() {
}

bool AppControlHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* /*error*/) {

  if (!manifest.HasPath(kTizenApplicationAppControlsKey))
    return true;

  auto aplist = std::make_shared<AppControlInfoList>();

  for (const auto& dict : parser::GetOneOrMany(manifest.value(),
      kTizenApplicationAppControlsKey, kTizenNamespacePrefix)) {
    ParseAppControlEntryAndStore(*dict, aplist.get());
  }

  *output = std::static_pointer_cast<parser::ManifestData>(aplist);
  return true;
}

bool AppControlHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& handlers_output,
    std::string* error) const {
  const AppControlInfoList& app_controls =
       static_cast<const AppControlInfoList&>(data);

  for (const auto& item : app_controls.controls) {
    if (item.src().empty()) {
      *error = "The src child element of app-control element is obligatory";
      return false;
    }

    const std::string& operation = item.operation();
    if (operation.empty()) {
      *error =
          "The operation child element of app-control element is obligatory";
      return false;
    }

    if (!parser::utils::IsValidIRI(operation)) {
      *error =
          "The operation child element of app-control element is not valid url";
      return false;
    }

    const TizenApplicationInfo& app_info =
      static_cast<const TizenApplicationInfo&>(
        *handlers_output.find(kTizenApplicationKey)->second);
    utils::VersionNumber required_version(app_info.required_version());
    if (!required_version.IsValid()) {
      *error = "Cannot retrieve required API version from widget";
      return false;
    }

    if (required_version >= kReloadRequiredVersion) {
      if (item.reload().empty()) {
        // FIXME for now, this const_cast is used, but it is not the best way.
        AppControlInfo &tmp = const_cast<AppControlInfo &>(item);
        tmp.set_reload(kEnabledValue);  // default parameter
      } else if (item.reload() != kEnabledValue &&
                 item.reload() != kDisabledValue) {
        *error = "The improper value was given for appcontrol reload";
        return false;
      }
    } else if (!item.reload().empty()) {
      *error = "reload attribute cannot be used for api version lower "
               "than 2.4";
      return false;
    }
  }
  return true;
}

std::vector<std::string> AppControlHandler::PrerequisiteKeys() const {
  return { kTizenApplicationKey };
}

std::string AppControlInfo::Key() {
  return kTizenApplicationAppControlsKey;
}

std::string AppControlHandler::Key() const {
  return kTizenApplicationAppControlsKey;
}

AppControlInfo::AppControlInfo(
    const std::string& src,
    const std::string& operation,
    const std::string& uri,
    const std::string& mime,
    const std::string& reload)
    : src_(src),
      operation_(operation),
      uri_(uri),
      mime_(mime),
      reload_(reload) {}

}   // namespace parse
}   // namespace wgt
