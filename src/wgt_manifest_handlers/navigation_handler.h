// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_NAVIGATION_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_NAVIGATION_HANDLER_H_

#include <memory>
#include <string>
#include <vector>

#include "manifest_parser/manifest_handler.h"

namespace wgt {
namespace parse {

class AllowedNavigationInfo : public parser::ManifestData {
 public:
  explicit AllowedNavigationInfo(const std::string& allowed_domains);
  virtual ~AllowedNavigationInfo();

  static std::string Key();
  /**
   * @brief GetAllowedDomains
   * @return vector with allowed domains
   */
  const std::vector<std::string>& GetAllowedDomains() const {
    return allowed_domains_;
  }

 private:
  std::vector<std::string> allowed_domains_;
};

/**
 * @brief The NavigationHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:allow-navigation>.
 */
class NavigationHandler : public parser::ManifestHandler {
 public:
  NavigationHandler();
  virtual ~NavigationHandler();

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;

  std::string Key() const override;
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_NAVIGATION_HANDLER_H_
