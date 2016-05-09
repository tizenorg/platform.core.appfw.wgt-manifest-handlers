// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/category_handler.h"

#include "manifest_parser/utils/logging.h"
#include "manifest_parser/values.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

namespace {

const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kWidgetNamespacePrefix[] = "http://www.w3.org/ns/widgets";
const char kTizenCategoryNameKey[] = "@name";
const char kErrMsgCategory[] = "Parsing category element failed";
const char kTizenCategoryKey[] = "widget.category";
const char kErrMsgCategoryName[] =
    "The name element inside category element is obligatory";

bool ParseCategoryEntryAndStore(const parser::DictionaryValue& control_dict,
                                CategoryInfoList* aplist) {
  std::string name;
  if (!control_dict.GetString(kTizenCategoryNameKey, &name)) return false;
  aplist->categories.push_back(name);
  return true;
}

}  // namespace

CategoryHandler::CategoryHandler() {}

CategoryHandler::~CategoryHandler() {}


bool CategoryHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* /*error*/) {
  if (!manifest.HasPath(kTizenCategoryKey))
    return true;

  auto aplist = std::make_shared<CategoryInfoList>();

  for (const auto& dict : parser::GetOneOrMany(manifest.value(),
      kTizenCategoryKey, kTizenNamespacePrefix)) {
    if (!ParseCategoryEntryAndStore(*dict, aplist.get()))
      return false;
  }

  *output = std::static_pointer_cast<parser::ManifestData>(aplist);
  return true;
}

bool CategoryHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {
  const CategoryInfoList& categories_list =
      static_cast<const CategoryInfoList&>(data);

  for (const auto& item : categories_list.categories) {
    if (item.empty()) {
      *error = kErrMsgCategoryName;
      return false;
    }
  }
  return true;
}

std::string CategoryHandler::Key() const {
  return kTizenCategoryKey;
}


std::string CategoryInfoList::Key() {
    return kTizenCategoryKey;
}

}  // namespace parse
}  // namespace wgt
