// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/service_handler.h"

#include <cassert>
#include <map>
#include <utility>
#include <set>

#include "manifest_parser/manifest_util.h"
#include "manifest_parser/values.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace {
const char kTizenServiceKey[] = "widget.service";
const char kTizenServiceIdKey[] = "@id";
const char kTizenServiceAutoRestartKey[] = "@auto-restart";
const char kTizenServiceOnBootKey[] = "@on-boot";
const char kTizenServiceCategoryKey[] = "category";
const char kTizenServiceCategoryNameKey[] = "@name";
const char kTizenServiceContentKey[] = "content";
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kWidgetNamespacePrefix[] = "http://www.w3.org/ns/widgets";
const char kTizenServiceNameKey[] = "name";
const char kTizenServiceContentSrcKey[] = "@src";
const char kTizenServiceIconKey[] = "icon";
const char kTizenServiceIconSrcKey[] = "@src";
const char kTizenServiceDescriptionKey[] = "description";
const char kTizenServiceMetadataKey[] = "metadata";
const char kTizenServiceMetadataKeyKey[] = "@key";
const char kTizenServiceMetadataValueKey[] = "@value";
const char kXmlLangKey[] = "@lang";
const char kXmlTextKey[] = "#text";

bool ParseServiceContent(const parser::DictionaryValue* dict,
                         wgt::parse::ServiceInfo* service_info,
                         std::string* error) {
  std::string content;
  bool found = false;
  for (auto& item : parser::GetOneOrMany(dict, kTizenServiceContentKey,
                                         kTizenNamespacePrefix)) {
    if (found) {
      *error = "tizen:content element of tizen:service "
               "should be declared only once";
      return false;
    }
    found = true;
    if (!item->GetString(kTizenServiceContentSrcKey, &content)) {
      *error =
          "Missing 'src' attribute in tizen:content tag in tizen:service";
      return false;
    }
  }
  if (!found) {
    *error = "Missing tizen:content tag in tizen:service";
    return false;
  }
  service_info->set_content(content);
  return true;
}

bool ParseServiceIcon(const parser::DictionaryValue* dict,
                      wgt::parse::ServiceInfo* service_info,
                      std::string* error) {
  std::string icon;
  const auto& items = parser::GetOneOrMany(dict, kTizenServiceIconKey,
                                           kTizenNamespacePrefix);
  if (!items.empty()) {
    if (items.size() > 1) {
      *error = "tizen:icon element of tizen:service "
               "should be declared only once";
      return false;
    }
    if (!items[0]->GetString(kTizenServiceIconSrcKey, &icon)) {
      *error = "Missing 'src' attribute in tizen:icon tag in tizen:service";
      return false;
    }
    service_info->set_icon(icon);
  }
  return true;
}

bool ParseServiceDescription(const parser::DictionaryValue* dict,
                             wgt::parse::ServiceInfo* service_info,
                             std::string* error) {
  const parser::Value* value = nullptr;
  if (!dict->Get(kTizenServiceDescriptionKey, &value)) return true;
  std::string description;

  const auto& items = parser::GetOneOrMany(dict,
                                           kTizenServiceDescriptionKey,
                                           kTizenNamespacePrefix);
  if (!items.empty()) {
    if (items.size() > 1) {
      *error = "tizen:description element of tizen:service "
               "should be declared only once";
      return false;
    }
    items[0]->GetString(kXmlTextKey, &description);
    service_info->set_description(description);
  }
  return true;
}

bool ParseServiceCategory(const parser::DictionaryValue* dict,
                          wgt::parse::ServiceInfo* service_info,
                          std::string* error) {
  std::vector<std::string> categories;
  for (auto& item : parser::GetOneOrMany(dict, kTizenServiceCategoryKey,
                                         kTizenNamespacePrefix)) {
    std::string category;
    if (!item->GetString(kTizenServiceCategoryNameKey, &category)) {
      *error =
          "Missing 'name' attribute of tizen:category tag in tizen:service";
      return false;
    }
    categories.push_back(category);
  }
  service_info->set_categories(categories);
  return true;
}

bool ParseServiceName(const parser::DictionaryValue* dict,
                      wgt::parse::ServiceInfo* service_info,
                      std::string* error) {
  wgt::parse::LangNameVector names;
  for (auto& item : parser::GetOneOrMany(dict, kTizenServiceNameKey,
                                         kTizenNamespacePrefix)) {
    std::string lang;
    std::string name;
    item->GetString(kXmlLangKey, &lang);
    item->GetString(kXmlTextKey, &name);
    names.emplace_back(lang, name);
  }
  if (names.empty()) {
    *error = "Cannot find tizen:name element for tizen:service. "
             "At least one must be provided.";
    return false;
  }
  service_info->set_names(names);
  return true;
}

bool ParseServiceMetadata(const parser::DictionaryValue* dict,
                          wgt::parse::ServiceInfo* service_info,
                          std::string* error) {
  wgt::parse::KeyValueVector metadata_set;
  for (auto& item : parser::GetOneOrMany(dict, kTizenServiceMetadataKey,
                                         kTizenNamespacePrefix)) {
    std::string key;
    std::string value;
    if (!item->GetString(kTizenServiceMetadataKeyKey, &key)) {
      *error = "'key' attribute of metadata is obligatory";
      return false;
    }
    item->GetString(kTizenServiceMetadataValueKey, &value);
    metadata_set.emplace_back(key, value);
  }
  service_info->set_metadata_set(metadata_set);
  return true;
}

std::unique_ptr<wgt::parse::ServiceInfo> ParseService(
    const parser::DictionaryValue* dict, std::string* error) {
  std::string id;
  if (!dict->GetString(kTizenServiceIdKey, &id)) {
    *error = "Cannot get appid for tizen:service";
    return nullptr;
  }

  std::unique_ptr<wgt::parse::ServiceInfo> service(
      new wgt::parse::ServiceInfo(id));

  std::string auto_restart = "false";
  if (dict->GetString(kTizenServiceAutoRestartKey, &auto_restart))
    service->set_auto_restart(auto_restart == "true");

  std::string on_boot = "false";
  if (dict->GetString(kTizenServiceOnBootKey, &on_boot))
    service->set_on_boot(on_boot == "true");

  if (!ParseServiceContent(dict, service.get(), error)) return nullptr;

  if (!ParseServiceIcon(dict, service.get(), error)) return nullptr;

  if (!ParseServiceDescription(dict, service.get(), error)) return nullptr;

  if (!ParseServiceCategory(dict, service.get(), error)) return nullptr;

  if (!ParseServiceName(dict, service.get(), error)) return nullptr;

  if (!ParseServiceMetadata(dict, service.get(), error)) return nullptr;

  return service;
}

}  // namespace

namespace wgt {

namespace parse {

ServiceInfo::ServiceInfo(const std::string& id, bool auto_restart, bool on_boot)
    : id_(id), auto_restart_(auto_restart), on_boot_(on_boot) {}

ServiceInfo::~ServiceInfo() {}

ServiceHandler::ServiceHandler() {}

ServiceHandler::~ServiceHandler() {}


bool ServiceHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  if (!manifest.HasPath(kTizenServiceKey)) {
    return true;
  }
  std::shared_ptr<ServiceList> services_data(new ServiceList());

  for (auto& item : parser::GetOneOrMany(manifest.value(),
      kTizenServiceKey, kTizenNamespacePrefix)) {
    auto service = ParseService(item, error);
    if (!service)
      return false;
    services_data->services.push_back(*service);
  }
  *output = std::static_pointer_cast<parser::ManifestData>(services_data);
  return true;
}

bool ServiceHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {
  const auto services = static_cast<const wgt::parse::ServiceList&>(data);

  for (auto& service : services.services) {
    if (!parser::ValidateTizenApplicationId(service.id())) {
      *error =
          "The id property of application element "
          "does not match the format\n";
      return false;
    }
  }

  return true;
}

std::string ServiceInfo::Key() {
  return kTizenServiceKey;
}

std::string ServiceHandler::Key() const {
  return kTizenServiceKey;
}

}  // namespace parse
}  // namespace wgt
