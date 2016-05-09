// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_METADATA_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_METADATA_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "manifest_parser/manifest_handler.h"
#include "manifest_parser/values.h"

namespace wgt {
namespace parse {

class MetaDataInfo : public parser::ManifestData {
 public:
  MetaDataInfo();
  virtual ~MetaDataInfo();

  static std::string Key();
  /**
   * @brief HasKey
   * @param key
   * @return true if has key
   */
  bool HasKey(const std::string& key) const;
  /**
   * @brief GetValue gets value
   * @param key
   * @return
   */
  std::string GetValue(const std::string& key) const;
  /**
   * @brief SetValue sets value
   * @param key
   * @param value
   */
  void SetValue(const std::string& key, const std::string& value);
  /**
   * @brief metadata
   * @return map of metadata
   */
  const std::map<std::string, std::string>& metadata() const {
    return metadata_;
  }

 private:
  std::map<std::string, std::string> metadata_;
};

/**
 * @brief The MetaDataHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:metadata>.
 */
class MetaDataHandler : public parser::ManifestHandler {
 public:
  MetaDataHandler();
  virtual ~MetaDataHandler();

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

#endif  // WGT_MANIFEST_HANDLERS_METADATA_HANDLER_H_
