// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_IME_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_IME_HANDLER_H_

#include <memory>
#include <string>
#include <vector>

#include "manifest_parser/manifest_handler.h"

namespace wgt {
namespace parse {

class ImeInfo : public parser::ManifestData {
 public:
  ImeInfo();
  virtual ~ImeInfo();
  /**
   * @brief uuid
   * @return uuid string
   */
  const std::string& uuid() const {
    return uuid_;
  }
  /**
   * @brief set_uuid sets uuid string
   * @param uuid
   */
  void set_uuid(const std::string& uuid) { uuid_ = uuid; }
  const std::vector<std::string>& languages() const {
    return languages_;
  }
  /**
   * @brief AddLanguage adds language
   * @param language
   */
  void AddLanguage(const std::string& language);

 private:
  std::string uuid_;
  std::vector<std::string> languages_;
};

/**
 * @brief The ImeHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:ime>.
 */
class ImeHandler : public parser::ManifestHandler {
 public:
  ImeHandler();
  virtual ~ImeHandler();
  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& application,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  std::string Key() const override;
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_IME_HANDLER_H_
