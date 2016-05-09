// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_TIZEN_APPLICATION_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_TIZEN_APPLICATION_HANDLER_H_

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "manifest_parser/manifest_handler.h"
#include "manifest_parser/values.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

class TizenApplicationInfo : public parser::ManifestData {
 public:
  TizenApplicationInfo();
  virtual ~TizenApplicationInfo();

  static std::string Key();

  void set_id(std::string id) {
    id_ = std::move(id);
  }
  void set_launch_mode(std::string launch_mode) {
    launch_mode_ = std::move(launch_mode);
  }
  void set_package(std::string package) {
    package_ = std::move(package);
  }
  void set_required_version(
      std::string required_version) {
    required_version_ = std::move(required_version);
  }
  void set_ambient_support(bool ambient_support) {
    ambient_support_ = ambient_support;
  }
  const std::string& id() const {
    return id_;
  }
  const std::string& launch_mode() const {
    return launch_mode_;
  }
  const std::string& package() const {
    return package_;
  }
  const std::string& required_version() const {
    return required_version_;
  }
  bool ambient_support() const {
    return ambient_support_;
  }

 private:
  std::string id_;
  std::string launch_mode_;
  std::string package_;
  std::string required_version_;
  bool ambient_support_;
};

/**
 * @brief The TizenApplicationHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:application>.
 */
class TizenApplicationHandler : public parser::ManifestHandler {
 public:
  TizenApplicationHandler();
  virtual ~TizenApplicationHandler();

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  bool AlwaysParseForKey() const override;
  std::string Key() const override;
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_TIZEN_APPLICATION_HANDLER_H_
