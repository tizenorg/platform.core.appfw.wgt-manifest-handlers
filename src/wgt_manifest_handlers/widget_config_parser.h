// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_MANIFEST_HANDLERS_WIDGET_CONFIG_PARSER_H_
#define WGT_MANIFEST_HANDLERS_WIDGET_CONFIG_PARSER_H_

#include <boost/filesystem/path.hpp>

#include <memory>
#include <string>

#include "manifest_parser/manifest_parser.h"

namespace wgt {
namespace parse {

/**
 * @brief The WidgetConfigParser class
 *        Parser class of widget's config.xml configuration file.
 *
 * Instance of this class may be used to parse single configuration file.
 * Depending on boolean result of @ref ParseManifest method, client code may
 * call:
 *  - on success -> @ref GetManifestData(), passing the key of ManifestData
 *                  instance that it is interested in.
 *  - on failure -> @ref GetErrorMessage(), to get value of error which was set
 *                  during the processing of config.xml
 *
 * Additional check of presence of start files may be done via calling
 * @ref HasValidStartFile(). This method is extracted separetly as missing start
 * file will not cause the failure of parsing the config.xml file.
 *
 * To investigate which key do you need to get certain parsed piece of data,
 * check the key reported by handler's @ref ManifestHandler::Key() method.
 * Key returned by this method is the key to access data set by handler.
 */
class WidgetConfigParser {
 public:
  WidgetConfigParser();

  std::shared_ptr<const parser::ManifestData> GetManifestData(
      const std::string& key);
  const std::string& GetErrorMessage() const;
  bool ParseManifest(const boost::filesystem::path& path);
  /**
   * @brief HasValidStartFile
   * @return true valid start file exists
   */
  bool HasValidStartFile() const;
  /**
   * @brief HasValidServicesStartFiles
   * @return true if has valid service start files
   */
  bool HasValidServicesStartFiles() const;

 private:
  /**
   * @brief CheckStartFile
   * @return  true if start file is correct
   */
  bool CheckStartFile();
  /**
   * @brief CheckServicesStartFiles
   * @return true if services start files are correct
   */
  bool CheckServicesStartFiles();
  // This function creates list of icons that are valid disregarding
  // (in actual version) current system's locale as installer do not need to
  // know about it.
  // Edited/Created list is present as kIconsKey key in parser.
  /**
   * @brief CheckWidgetIcons
   * @return true if widget icons are correct
   */
  bool CheckWidgetIcons();

  std::unique_ptr<parser::ManifestParser> parser_;
  boost::filesystem::path widget_path_;
  std::string error_;
  bool has_valid_start_file_;
  bool has_valid_services_start_files_;
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_WIDGET_CONFIG_PARSER_H_
