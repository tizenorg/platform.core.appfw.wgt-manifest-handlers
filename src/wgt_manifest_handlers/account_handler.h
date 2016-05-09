// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_MANIFEST_HANDLERS_ACCOUNT_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_ACCOUNT_HANDLER_H_

#include <utility>
#include <string>
#include <vector>

#include "manifest_parser/manifest_handler.h"

namespace wgt {
namespace parse {

struct SingleAccountInfo {
  bool multiple_account_support;
  std::vector<std::pair<std::string, std::string>> names;
  std::vector<std::pair<std::string, std::string>> icon_paths;
  std::vector<std::string> capabilities;
};

class AccountInfo : public parser::ManifestData {
 public:
  AccountInfo() {}
  ~AccountInfo() override {}
  static std::string Key();

  /**
   * @brief This method returns all the accounts successfully parsed
   */
  const std::vector<SingleAccountInfo>& accounts() const {
    return accounts_;
  }

  /**
    * @brief  Method add to the vector successfully created account
    */
  void set_account(SingleAccountInfo single_account) {
    accounts_.push_back(std::move(single_account));
  }

 private:
  std::vector<SingleAccountInfo> accounts_;
};

/**
 * @brief The AccountHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:account>.
 */
class AccountHandler : public parser::ManifestHandler {
 public:
  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;

  std::string Key() const override;

 private:
  /**
   * @brief ParseSingleAccountElement everything between <account> </account>
   * @param item_dict dictionary to be parsed
   * @param info ptr for entity that stores successfully parsed results
   * @param error information about error that happened
   * @return true if operation of parsing was successful
   */
  bool ParseSingleAccountElement(const parser::DictionaryValue* item_dict,
                                 std::shared_ptr<AccountInfo> info,
                                 std::string* error);
  /**
   * @brief ParseAccountIcons icons of account
   * @param item_dict dictionary to be parsed
   * @param singel_account the single account entity
   * @return true parsed successfully
   */
  bool ParseAccountIcons(const parser::DictionaryValue* item_dict,
                         SingleAccountInfo* singel_account);
  bool ParseSingleAccountIcon(const parser::DictionaryValue* item_dict,
                              SingleAccountInfo* singel_account);
  /**
   * @brief ParseAccountNames parse and validates account name
   * @param item_dict dictionary to be parsed
   * @param singel_account the single account entity
   * @return true if parsed successfully
   */
  bool ParseAccountNames(const parser::DictionaryValue* item_dict,
                         SingleAccountInfo* singel_account);
  bool ParseSingleAccountName(const parser::DictionaryValue* item_dict,
                              SingleAccountInfo* singel_account);
  /**
   * @brief ParseCapabilities parses capabilities and validates them
   * @param item_dict dictionary to be parsed
   * @param singel_account the single account entity
   * @return true if parsed successfully
   */
  bool ParseCapabilities(const parser::DictionaryValue* item_dict,
                         SingleAccountInfo* singel_account);
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_ACCOUNT_HANDLER_H_
