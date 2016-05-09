// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_

#include <string>
#include <vector>

#include "manifest_parser/manifest_handler.h"
#include "manifest_parser/permission_types.h"

namespace wgt {
namespace parse {

class PermissionsInfo: public parser::ManifestData {
 public:
  PermissionsInfo();
  virtual ~PermissionsInfo();

  static std::string Key();
  /**
   * @brief GetAPIPermissions
   * @return permission set
   */
  const parser::PermissionSet& GetAPIPermissions() const {
    return api_permissions_;
  }
  /**
   * @brief SetAPIPermissions sets api permissions
   * @param api_permissions
   */
  void SetAPIPermissions(const parser::PermissionSet& api_permissions) {
    api_permissions_ = api_permissions;
  }

 private:
  parser::PermissionSet api_permissions_;
};

/**
 * @brief The PermissionsHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:privilege>.
 */
class PermissionsHandler: public parser::ManifestHandler {
 public:
  PermissionsHandler();
  virtual ~PermissionsHandler();

  bool Parse(
      const parser::Manifest& data,
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

#endif  // WGT_MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_
