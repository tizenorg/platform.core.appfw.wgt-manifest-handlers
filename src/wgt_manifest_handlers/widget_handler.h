// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_WIDGET_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_WIDGET_HANDLER_H_

#include <map>
#include <string>
#include <vector>

#include "manifest_parser/manifest_handler.h"
#include "manifest_parser/values.h"

namespace wgt {
namespace parse {

class Preference {
 public:
  Preference(const std::string& name, const std::string& value,
             bool readonly)
      : name_(name),
        value_(value),
        readonly_(readonly) {}

  std::string Name() const { return name_; }
  std::string Value() const { return value_; }
  bool ReadOnly() const { return readonly_; }
 private:
  std::string name_;
  std::string value_;
  bool readonly_;
};

typedef std::map<std::string, std::string> LangValMap;

class WidgetInfo : public parser::ManifestData {
 public:
  WidgetInfo() {}
  WidgetInfo(const WidgetInfo&) = delete;
  WidgetInfo& operator=(const WidgetInfo&) = delete;
  virtual ~WidgetInfo() {
    for (auto p : preferences_)
      delete p;
  }
  static std::string Key();

  const LangValMap& name_set() const { return name_set_; }
  const LangValMap& short_name_set() const { return short_name_set_; }
  const LangValMap& description_set() const { return description_set_; }
  const LangValMap& license_set() const { return license_set_; }

  const std::string& id() const { return id_; }
  const std::string& version() const { return version_; }
  const std::string& view_modes() const { return viewmodes_; }
  const std::string& default_locale() const { return default_locale_; }
  const std::string& author() const { return author_; }
  const std::string& author_email() const { return author_email_; }
  const std::string& author_href() const { return author_href_; }
  const std::string& widget_namespace() const { return widget_namespace_; }
  const std::vector<Preference*>& preferences() const { return preferences_; }
  unsigned int height() const { return height_; }
  unsigned int width() const { return width_; }

 private:
  std::vector<Preference*> preferences_;
  std::string id_;
  std::string version_;
  std::string viewmodes_;
  std::string default_locale_;
  std::string author_;
  LangValMap description_set_;
  LangValMap name_set_;
  LangValMap short_name_set_;
  LangValMap license_set_;
  std::string author_email_;
  std::string author_href_;
  std::string widget_namespace_;
  unsigned int height_;
  unsigned int width_;

  friend class WidgetHandler;
};

/**
 * @brief The WidgetHandler class
 *
 * Handler of config.xml for root xml element <w3c:widget>.
 * Children parsed within this handlers:
 *  - <w3c:author>,
 *  - <w3c:description>,
 *  - <w3c:license>,
 *  - <w3c:name>,
 *  - <w3c:preference>.
 *
 * Code parsing rest of children is located in other handlers.
 */
class WidgetHandler : public parser::ManifestHandler {
 public:
  WidgetHandler() {}
  virtual ~WidgetHandler() {}

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool AlwaysParseForKey() const override { return true; }
  std::string Key() const override;
  /**
   * @brief ParseSingleLocalizedLicenseElement parses single license element
   * @param item_dict
   * @param parent_lang
   * @param info
   */
  void ParseSingleLocalizedLicenseElement(
      const parser::DictionaryValue* item_dict,
      const std::string& parent_lang,
      std::shared_ptr<WidgetInfo> info);
  /**
   * @brief ParseLocalizedLicenseElements parser localized element
   * @param manifest
   * @param parent_lang
   * @param info
   */
  void ParseLocalizedLicenseElements(
      const parser::Manifest& manifest,
      const std::string& parent_lang,
      std::shared_ptr<WidgetInfo> info);
  /**
   * @brief ParseSingleLocalizedDescriptionElement parses
   * single localized description
   * @param item_dict
   * @param parent_lang
   * @param info
   */
  void ParseSingleLocalizedDescriptionElement(
      const parser::DictionaryValue* item_dict,
      const std::string& parent_lang,
      std::shared_ptr<WidgetInfo> info);
  /**
   * @brief ParseLocalizedDescriptionElements parses localized description
   * element
   * @param manifest
   * @param parent_lang
   * @param info
   */
  void ParseLocalizedDescriptionElements(
      const parser::Manifest& manifest,
      const std::string& parent_lang,
      std::shared_ptr<WidgetInfo> info);
  /**
   * @brief ParseSingleLocalizedNameElement parses single localized name
   * @param item_dict
   * @param parent_lang
   * @param info
   */
  void ParseSingleLocalizedNameElement(
      const parser::DictionaryValue* item_dict,
      const std::string& parent_lang,
      std::shared_ptr<WidgetInfo> info);
  /**
   * @brief ParseLocalizedNameElements parses localized name
   * @param manifest
   * @param parent_lang
   * @param info
   */
  void ParseLocalizedNameElements(
      const parser::Manifest& manifest,
      const std::string& parent_lang,
      std::shared_ptr<WidgetInfo> info);
  /**
   * @brief ParseAuthorElements parses author element
   * @param manifest
   * @param info
   */
  void ParseAuthorElements(
      const parser::Manifest& manifest,
      std::shared_ptr<WidgetInfo> info);
  /**
   * @brief ParseSingleAuthorElement parses single author element
   * @param author_dict
   * @param info
   */
  void ParseSingleAuthorElement(
      const parser::DictionaryValue* author_dict,
      std::shared_ptr<WidgetInfo> info);

  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_WIDGET_HANDLER_H_
