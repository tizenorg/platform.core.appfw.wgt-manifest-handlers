// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_APP_CONTROL_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_APP_CONTROL_HANDLER_H_

#include <memory>
#include <string>
#include <vector>

#include "manifest_parser/manifest.h"
#include "manifest_parser/manifest_handler.h"

namespace wgt {
namespace parse {

/**
 * @brief The AppControlHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:app-control>.
 */
class AppControlHandler : public parser::ManifestHandler {
 public:
  AppControlHandler();
  ~AppControlHandler() override;
  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  std::string Key() const override;
  std::vector<std::string> PrerequisiteKeys() const override;
};

class AppControlInfo {
 public:
  AppControlInfo(const std::string& src, const std::string& operation,
      const std::string& uri, const std::string& mime,
      const std::string& reload);

  static std::string Key();
  /**
   * @brief src return the source attribute
   * @return the value of source attribute
   */
  const std::string& src() const {
    return src_;
  }
  /**
   * @brief
   * @return the value of operation attribute
   */
  const std::string& operation() const {
    return operation_;
  }
  /**
   * @brief uri
   * @return the value of the uri attribute
   */
  const std::string& uri() const {
    return uri_;
  }
  /**
   * @brief mime
   * @return the value of the mime attribute
   */
  const std::string& mime() const {
    return mime_;
  }
  /**
   * @brief reload
   * @return the value of reload attribute
   */
  const std::string& reload() const {
    return reload_;
  }
  void set_reload(std::string rel)  {
    reload_ = std::move(rel);
  }

 private:
  std::string src_;
  std::string operation_;
  std::string uri_;
  std::string mime_;
  std::string reload_;
};

struct AppControlInfoList : public parser::ManifestData {
  std::vector<AppControlInfo> controls;
};

}   // namespace parse
}   // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_APP_CONTROL_HANDLER_H_
