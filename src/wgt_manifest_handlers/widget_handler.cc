// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/widget_handler.h"

#include <string.h>

#include <algorithm>
#include <cassert>
#include <map>
#include <utility>
#include <set>

#include "manifest_parser/utils/iri_util.h"
#include "manifest_parser/utils/language_tag_validator.h"
#include "manifest_parser/utils/logging.h"
#include "manifest_parser/values.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

namespace {
const char kWidgetKey[] = "widget";

const char kAuthorKey[] = "widget.author";
const char kWidgetNamespacePrefix[] = "http://www.w3.org/ns/widgets";
const char kWidgetNamespaceKey[] = "widget.@namespace";
const char kAuthorHrefKey[] = "@href";
const char kAuthorEmailKey[] = "@email";
const char kVersionKey[] = "widget.@version";
const char kNameKey[] = "widget.name";
const char kPreferencesNameKey[] = "@name";
const char kPreferencesValueKey[] = "@value";
const char kPreferencesReadonlyKey[] = "@readonly";
const char kXmlLangKey[] = "@lang";
const char kXmlHrefKey[] = "@href";
const char kLicenseKey[] = "widget.license";
const char kShortKey[] = "@short";
const char kWidgetLangKey[] = "widget.@lang";
const char kIDKey[] = "widget.@id";
const char kHeightKey[] = "widget.@height";
const char kWidthKey[] = "widget.@width";
const char kDefaultLocaleKey[] = "widget.@defaultlocale";
const char kViewModesKey[] = "widget.@viewmodes";
const char kPreferencesKey[] = "widget.preference";
const char kXmlTextKey[] = "#text";
const char kDescriptionKey[] = "widget.description";

bool ParserPreferenceItem(const parser::Value* val, Preference** output,
                          std::string* error) {
  const parser::DictionaryValue* pref_dict;
  if (!val->GetAsDictionary(&pref_dict)) {
    *error = "Parsing preference element failed";
    return false;
  }
  std::string name;
  std::string value;
  std::string readonly = "false";
  pref_dict->GetString(kPreferencesNameKey, &name);
  pref_dict->GetString(kPreferencesValueKey, &value);
  pref_dict->GetString(kPreferencesReadonlyKey, &readonly);
  *output = new Preference(name, value, readonly == "true");
  return true;
}

}  // namespace

void WidgetHandler::ParseSingleLocalizedLicenseElement(
    const parser::DictionaryValue* item_dict, const std::string& parent_lang,
    std::shared_ptr<WidgetInfo> info) {
  bool lang_overwriten = false;
  std::string lang;
  std::string text;
  std::string href;

  if (item_dict->HasKey(kXmlLangKey)) {
    lang_overwriten = true;
    item_dict->GetString(kXmlLangKey, &lang);
    if (!utils::w3c_languages::ValidateLanguageTag(lang)) {
      LOG(ERROR) << "Tag " << lang << " is invalid";
      return;
    }
  }
  if (item_dict->HasKey(kXmlHrefKey)) {
    item_dict->GetString(kXmlHrefKey, &href);
  }
  item_dict->GetString(kXmlTextKey, &text);
  // TODO(w.kosowicz) check where href should be put...
  if (lang_overwriten) {
    info->license_set_.insert(std::make_pair(lang, text + href));
  } else {
    info->license_set_.insert(std::make_pair(parent_lang, text + href));
  }
}

void WidgetHandler::ParseLocalizedLicenseElements(
    const parser::Manifest& manifest, const std::string& parent_lang,
    std::shared_ptr<WidgetInfo> info) {
  if (!manifest.HasPath(kLicenseKey)) return;

  const parser::Value* val = nullptr;
  const parser::DictionaryValue* dict = nullptr;
  const parser::ListValue* list = nullptr;
  if (manifest.Get(kLicenseKey, &val)) {
    if (val->GetAsDictionary(&dict)) {
      if (parser::VerifyElementNamespace(*dict, kWidgetNamespacePrefix))
        ParseSingleLocalizedLicenseElement(dict, parent_lang, info);
    } else if (val->GetAsList(&list)) {
      for_each(list->begin(), list->end(),
               [list, &dict, parent_lang, info, this](parser::Value* item) {
        if (item->GetAsDictionary(&dict))
          if (parser::VerifyElementNamespace(*dict, kWidgetNamespacePrefix))
            ParseSingleLocalizedLicenseElement(dict, parent_lang, info);
      });
    }
  }
}

void WidgetHandler::ParseSingleLocalizedDescriptionElement(
    const parser::DictionaryValue* item_dict, const std::string& parent_lang,
    std::shared_ptr<WidgetInfo> info) {
  bool lang_overwriten = false;
  std::string lang;
  std::string text;

  if (item_dict->HasKey(kXmlLangKey)) {
    lang_overwriten = true;
    item_dict->GetString(kXmlLangKey, &lang);
    if (!utils::w3c_languages::ValidateLanguageTag(lang)) {
      LOG(ERROR) << "Tag " << lang << " is invalid";
      return;
    }
  }
  item_dict->GetString(kXmlTextKey, &text);
  if (lang_overwriten) {
    info->description_set_.insert(std::make_pair(lang, text));
  } else {
    info->description_set_.insert(std::make_pair(parent_lang, text));
  }
}

void WidgetHandler::ParseLocalizedDescriptionElements(
    const parser::Manifest& manifest, const std::string& parent_lang,
    std::shared_ptr<WidgetInfo> info) {
  if (!manifest.HasPath(kDescriptionKey)) return;

  const parser::Value* val = nullptr;
  const parser::DictionaryValue* dict = nullptr;
  const parser::ListValue* list = nullptr;
  if (manifest.Get(kDescriptionKey, &val)) {
    if (val->GetAsDictionary(&dict)) {
      if (parser::VerifyElementNamespace(*dict, kWidgetNamespacePrefix))
        ParseSingleLocalizedDescriptionElement(dict, parent_lang, info);
    } else if (val->GetAsList(&list)) {
      for (auto& item : *list)
        if (item->GetAsDictionary(&dict))
          if (parser::VerifyElementNamespace(*dict, kWidgetNamespacePrefix))
            ParseSingleLocalizedDescriptionElement(dict, parent_lang, info);
    }
  }
}

void WidgetHandler::ParseSingleLocalizedNameElement(
    const parser::DictionaryValue* item_dict, const std::string& parent_lang,
    std::shared_ptr<WidgetInfo> info) {
  bool lang_overwriten = false;
  std::string lang;
  std::string name;
  std::string short_name;

  if (item_dict->HasKey(kXmlLangKey)) {
    lang_overwriten = true;
    item_dict->GetString(kXmlLangKey, &lang);
    if (!utils::w3c_languages::ValidateLanguageTag(lang)) {
      LOG(ERROR) << "Tag " << lang << " is invalid";
      return;
    }
  }
  if (item_dict->HasKey(kShortKey)) {
    item_dict->GetString(kShortKey, &short_name);
  }
  item_dict->GetString(kXmlTextKey, &name);

  // ignore if given language already spotted
  if (info->name_set_.find(lang) != info->name_set_.end()) return;

  if (lang_overwriten) {
    info->name_set_.insert(std::make_pair(lang, name));
    if (!short_name.empty())
      info->short_name_set_.insert(std::make_pair(lang, short_name));
  } else {
    info->name_set_.insert(std::make_pair(parent_lang, name));
    if (!short_name.empty())
      info->short_name_set_.insert(std::make_pair(parent_lang, short_name));
  }
}

void WidgetHandler::ParseLocalizedNameElements(
    const parser::Manifest& manifest, const std::string& parent_lang,
    std::shared_ptr<WidgetInfo> info) {
  if (!manifest.HasPath(kNameKey)) return;

  const parser::Value* val = nullptr;
  const parser::DictionaryValue* dict = nullptr;
  const parser::ListValue* list = nullptr;
  if (manifest.Get(kNameKey, &val)) {
    if (val->GetAsDictionary(&dict)) {
      if (parser::VerifyElementNamespace(*dict, kWidgetNamespacePrefix))
        ParseSingleLocalizedNameElement(dict, parent_lang, info);
    } else if (val->GetAsList(&list)) {
      for (auto& item : *list)
        if (item->GetAsDictionary(&dict))
          if (parser::VerifyElementNamespace(*dict, kWidgetNamespacePrefix))
            ParseSingleLocalizedNameElement(dict, parent_lang, info);
    }
  }
}

void WidgetHandler::ParseSingleAuthorElement(
    const parser::DictionaryValue* author_dict,
    std::shared_ptr<WidgetInfo> info) {
  author_dict->GetString(kXmlTextKey, &info->author_);
  author_dict->GetString(kAuthorEmailKey, &info->author_email_);
  std::string author_href;
  author_dict->GetString(kAuthorHrefKey, &author_href);
  if (!author_href.empty() && parser::utils::IsValidIRI(author_href))
    info->author_href_ = author_href;
}

void WidgetHandler::ParseAuthorElements(const parser::Manifest& manifest,
                                        std::shared_ptr<WidgetInfo> info) {
  if (manifest.HasPath(kAuthorKey)) {
    const parser::Value* author_value = nullptr;
    manifest.Get(kAuthorKey, &author_value);

    auto& authors = parser::GetOneOrMany(manifest.value(), kAuthorKey,
                                         kWidgetNamespacePrefix);
    if (!authors.empty())
      ParseSingleAuthorElement(authors[0], info);
  }
}

bool WidgetHandler::Parse(const parser::Manifest& manifest,
                          std::shared_ptr<parser::ManifestData>* output,
                          std::string* error) {
  std::shared_ptr<WidgetInfo> widget_info(new WidgetInfo());
  widget_info->preferences_ = std::vector<Preference*>();

  std::string parent_lang;

  if (manifest.HasPath(kWidgetNamespaceKey)) {
    manifest.GetString(kWidgetNamespaceKey, &widget_info->widget_namespace_);
    manifest.GetString(kWidgetLangKey, &parent_lang);
  }

  if (widget_info->widget_namespace_ != kWidgetNamespacePrefix) {
    *error = "Wrong namespace of <widget> element. Config.xml is invalid";
    return false;
  }

  ParseAuthorElements(manifest, widget_info);

  ParseLocalizedDescriptionElements(manifest, parent_lang, widget_info);
  ParseLocalizedNameElements(manifest, parent_lang, widget_info);
  ParseLocalizedLicenseElements(manifest, parent_lang, widget_info);

  if (manifest.HasPath(kVersionKey))
    manifest.GetString(kVersionKey, &widget_info->version_);
  if (manifest.HasPath(kIDKey)) {
    std::string id;
    manifest.GetString(kIDKey, &id);
    if (!id.empty() && parser::utils::IsValidIRI(id)) widget_info->id_ = id;
  }
  if (manifest.HasPath(kHeightKey)) {
    int h;
    manifest.GetInteger(kHeightKey, &h);
    widget_info->height_ = static_cast<unsigned int>(h);
  }
  if (manifest.HasPath(kWidthKey)) {
    int w;
    manifest.GetInteger(kWidthKey, &w);
    widget_info->width_ = static_cast<unsigned int>(w);
  }
  if (manifest.HasPath(kDefaultLocaleKey))
    manifest.GetString(kDefaultLocaleKey, &widget_info->default_locale_);
  if (manifest.HasPath(kViewModesKey))
    manifest.GetString(kViewModesKey, &widget_info->viewmodes_);

  for (auto& pref_dict : parser::GetOneOrMany(manifest.value(),
                                              kPreferencesKey,
                                              kWidgetNamespacePrefix)) {
    Preference* preference = nullptr;
    if (!ParserPreferenceItem(pref_dict, &preference, error))
      return false;
    widget_info->preferences_.push_back(preference);
  }

  *output = std::static_pointer_cast<parser::ManifestData>(widget_info);
  return true;
}

bool WidgetHandler::Validate(const parser::ManifestData& data,
                             const parser::ManifestDataMap& /*handlers_output*/,
                             std::string* error) const {
  const WidgetInfo& widget_info = static_cast<const WidgetInfo&>(data);
  if (widget_info.widget_namespace() != kWidgetNamespacePrefix) {
    *error = "The widget namespace is invalid.";
    return false;
  }
  return true;
}


std::string WidgetHandler::Key() const {
  return kWidgetKey;
}

std::string WidgetInfo::Key() {
  return kWidgetKey;
}

}  // namespace parse
}  // namespace wgt
