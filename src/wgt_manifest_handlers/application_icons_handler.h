// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_MANIFEST_HANDLERS_APPLICATION_ICONS_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_APPLICATION_ICONS_HANDLER_H_

#include <string>
#include <vector>

#include "manifest_parser/manifest_handler.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

class ApplicationIcon {
 public:
  explicit ApplicationIcon(const std::string& path, int height = -1,
                           int width = -1);
  /**
   * @brief GetWidth
   * @param width
   * @return true if successful
   */
  bool GetWidth(int* width) const;
  /**
   * @brief GetHeight
   * @param height
   * @return true if successful
   */
  bool GetHeight(int* height) const;
  /**
   * @brief path
   * @return string to the path
   */
  const std::string& path() const;

 private:
  std::string path_;
  int height_;
  int width_;
};

class ApplicationIconsInfo : public parser::ManifestData {
 public:
  static std::string Key();

  const std::vector<ApplicationIcon>& icons() const;
  /**
   * @brief AddIcon adds icon
   * @param icon_path
   */
  void AddIcon(const ApplicationIcon& icon_path);
 private:
  std::vector<ApplicationIcon> icons_;
};

/**
 * @brief The ApplicationIconsHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <w3c:icon>.
 */
class ApplicationIconsHandler : public parser::ManifestHandler {
 public:
  ApplicationIconsHandler();
  ~ApplicationIconsHandler();

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool AlwaysParseForKey() const override;
  std::string Key() const override;
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_APPLICATION_ICONS_HANDLER_H_
