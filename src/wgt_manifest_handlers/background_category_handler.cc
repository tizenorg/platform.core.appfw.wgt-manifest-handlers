// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/background_category_handler.h"

#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace {
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kTizenBackgroundCategoryValueKey[] = "@value";
const char kErrMsgElementParse[] = "Parsing background-category element failed";
}  // namespace

namespace wgt {
namespace parse {

namespace keys = wgt::application_widget_keys;

bool BackgroundCategoryHandler::ParseBackgroundCategoryElement(
    const parser::DictionaryValue& element_dict,
    BackgroundCategoryInfoList* bclist) {
  std::string value;

  if (!element_dict.GetString(kTizenBackgroundCategoryValueKey, &value))
    return false;

  bclist->background_categories.emplace_back(value);

  return true;
}

bool BackgroundCategoryHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  if (!manifest.HasPath(keys::kTizenBackgroundCategoryKey))
    return true;

  std::shared_ptr<BackgroundCategoryInfoList> bclist(
      new BackgroundCategoryInfoList());

  for (const auto& dict : parser::GetOneOrMany(manifest.value(),
      keys::kTizenBackgroundCategoryKey, kTizenNamespacePrefix)) {
    if (!ParseBackgroundCategoryElement(*dict, bclist.get())) {
      *error = kErrMsgElementParse;
      return false;
    }
  }

  *output = std::static_pointer_cast<parser::ManifestData>(bclist);
  return true;
}

std::string BackgroundCategoryHandler::Key() const {
  return keys::kTizenBackgroundCategoryKey;
}

BackgroundCategoryInfo::BackgroundCategoryInfo(const std::string& value) :
    value_(value) {}

}  // namespace parse
}  // namespace wgt
