// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/navigation_handler.h"

#include <boost/tokenizer.hpp>

#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

namespace {
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kAllowNavigationKey[] = "widget.allow-navigation";
const char kWidgetNamespacePrefix[] = "http://www.w3.org/ns/widgets";
const char kXmlTextKey[] = "#text";
const boost::char_separator<char> navigation_separator(" ");

}  // namespace

AllowedNavigationInfo::AllowedNavigationInfo(
    const std::string& allowed_domains) {
  boost::tokenizer<boost::char_separator<char>> tokens(allowed_domains,
                                                       navigation_separator);
  for (auto& item : tokens) {
    allowed_domains_.push_back(item);
  }
}

AllowedNavigationInfo::~AllowedNavigationInfo() {}

std::string AllowedNavigationInfo::Key() {
  return kAllowNavigationKey;
}

NavigationHandler::NavigationHandler() {}

NavigationHandler::~NavigationHandler() {}


bool NavigationHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* /*error*/) {
  if (!manifest.HasPath(kAllowNavigationKey))
    return true;

  std::string allowed_domains;

  for (const auto& dict : parser::GetOneOrMany(manifest.value(),
      kAllowNavigationKey, kTizenNamespacePrefix)) {
    if (!dict->GetString(kXmlTextKey, &allowed_domains))
      return true;
  }

  *output = std::static_pointer_cast<parser::ManifestData>(
      std::make_shared<AllowedNavigationInfo>(allowed_domains));
  return true;
}

bool NavigationHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* /*error*/) const {
  const AllowedNavigationInfo& navi_info =
      static_cast<const AllowedNavigationInfo&>(data);
  // TODO(j.izydorczyk): There should be done 'navi_info' validity check.
  (void)navi_info;
  return true;
}

std::string NavigationHandler::Key() const {
  return kAllowNavigationKey;
}

}  // namespace parse
}  // namespace wgt
