// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_CSP_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_CSP_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "manifest_parser/values.h"
#include "manifest_parser/manifest_handler.h"

namespace wgt {
namespace parse {

class CSPInfo : public parser::ManifestData {
 public:
  CSPInfo() {}
  ~CSPInfo() override {}

  static std::string Key();
  static std::string Report_only_key();
  /**
   * @brief set_security_rules sets up string with security rules
   * @param security_rules
   */
  void set_security_rules(std::string security_rules) {
    security_rules_ = std::move(security_rules);
  }
  /**
   * @brief security_rules
   * @return string with security_rules
   */
  const std::string security_rules() const {
    return security_rules_;
  }

 private:
  std::string security_rules_;
};

/**
 * @brief The CSPHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:content-security-policy>,
 *  - <tizen:content-security-policy-report-only>.
 */
class CSPHandler : public parser::ManifestHandler {
 public:
  enum class SecurityType {
    CSP,
    CSP_REPORT_ONLY
  };
  explicit CSPHandler(SecurityType securityType)
    : security_type_(securityType) {}
  ~CSPHandler() override {}

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  std::string Key() const override;
 private:
  SecurityType security_type_;
};
}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_CSP_HANDLER_H_
