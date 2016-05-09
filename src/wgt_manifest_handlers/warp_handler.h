// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_WARP_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_WARP_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "manifest_parser/values.h"
#include "manifest_parser/manifest_handler.h"

namespace wgt {
namespace parse {

class WarpInfo : public parser::ManifestData {
 public:
  static std::string Key();

  WarpInfo() {}
  ~WarpInfo() override {}
  /**
   * @brief set_access_element push single access_element inside vector
   * @param access_element
   */
  void set_access_element(const std::pair<std::string, bool>& access_element) {
    access_map_.push_back(access_element);
  }
  /**
   * @brief access_map
   * @return vector containing access_elements
   */
  const std::vector<std::pair<std::string, bool> >& access_map() const {
    return access_map_;
  }

 private:
  std::vector<std::pair<std::string, bool> > access_map_;
};

/**
 * @brief The WarpHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <w3c:access>.
 */
class WarpHandler : public parser::ManifestHandler {
 public:
  WarpHandler() {}
  ~WarpHandler() override {}

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  void ParseSingleAccessElement(
      const parser::DictionaryValue& item_dict,
      std::shared_ptr<WarpInfo> info);
  std::string Key() const override;
};
}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_WARP_HANDLER_H_
