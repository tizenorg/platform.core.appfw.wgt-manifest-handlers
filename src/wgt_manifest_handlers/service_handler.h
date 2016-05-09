// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_SERVICE_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_SERVICE_HANDLER_H_

#include <string>
#include <utility>
#include <vector>

#include "manifest_parser/manifest_handler.h"
#include "manifest_parser/values.h"

namespace wgt {
namespace parse {

using LangNameVector = std::vector<std::pair<std::string, std::string>>;
using KeyValueVector = std::vector<std::pair<std::string, std::string>>;

class ServiceInfo {
 public:
  explicit ServiceInfo(const std::string& id, bool auto_restart = false,
      bool on_boot = false);
  virtual ~ServiceInfo();

  static std::string Key();
  /**
   * @brief id
   * @return string id
   */
  const std::string& id() const { return id_; }
  void set_id(const std::string& id) {
    id_ = id;
  }
  /**
   * @brief auto_restart
   * @return true is auto-restart is set
   */
  bool auto_restart() const { return auto_restart_; }
  /**
   * @brief set_auto_restart sets auto restart
   * @param auto_restart
   */
  void set_auto_restart(bool auto_restart) {
    auto_restart_ = auto_restart;
  }
  /**
   * @brief on_boot
   * @return if is on boot
   */
  bool on_boot() const { return on_boot_; }
  void set_on_boot(bool on_boot) {
    on_boot_ = on_boot;
  }
  /**
   * @brief names
   * @return lang names vector
   */
  const LangNameVector& names() const {
    return names_;
  }
  /**
   * @brief set_names sets name lang vector
   * @param names
   */
  void set_names(const LangNameVector& names) {
    names_ = names;
  }
  /**
   * @brief icon
   * @return string to the icon
   */
  const std::string& icon() const {
    return icon_;
  }
  /**
   * @brief set_icon set string to the icon
   * @param icon
   */
  void set_icon(const std::string& icon) {
    icon_ = icon;
  }
  /**
   * @brief content
   * @return  content string
   */

  const std::string& content() const {
    return content_;
  }
  /**
   * @brief set_content sets content string
   * @param content
   */
  void set_content(const std::string& content) {
    content_ = content;
  }
  /**
   * @brief description
   * @return description string
   */
  const std::string& description() const {
    return description_;
  }
  /**
   * @brief set_description sets description
   * @param description
   */
  void set_description(const std::string& description) {
    description_ = description;
  }
  /**
   * @brief categories
   * @return categories vector
   */

  const std::vector<std::string>& categories() const {
    return categories_;
  }
  /**
   * @brief set_categories sets categories
   * @param categories
   */
  void set_categories(const std::vector<std::string>& categories) {
    categories_ = categories;
  }
  /**
   * @brief metadata_set
   * @return key value vector
   */
  const KeyValueVector& metadata_set() const {
    return metadata_set_;
  }
  /**
   * @brief set_metadata_set sets metadata
   * @param metadata_set
   */
  void set_metadata_set(const KeyValueVector& metadata_set) {
    metadata_set_ = metadata_set;
  }

 private:
  std::string id_;
  bool auto_restart_;
  bool on_boot_;
  LangNameVector names_;
  std::string icon_;
  std::string content_;
  std::string description_;
  std::vector<std::string> categories_;
  KeyValueVector metadata_set_;
};

struct ServiceList : public parser::ManifestData {
  std::vector<ServiceInfo> services;
};

/**
 * @brief The ServiceHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:service>.
 */
class ServiceHandler : public parser::ManifestHandler {
 public:
  ServiceHandler();
  virtual ~ServiceHandler();

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

#endif  // WGT_MANIFEST_HANDLERS_SERVICE_HANDLER_H_
