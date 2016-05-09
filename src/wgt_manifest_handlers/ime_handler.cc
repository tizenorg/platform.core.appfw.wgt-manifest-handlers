// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/ime_handler.h"

#include <cassert>
#include <memory>
#include <regex>

#include "manifest_parser/utils/logging.h"
#include "manifest_parser/values.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

namespace {

const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kWidgetNamespacePrefix[] = "http://www.w3.org/ns/widgets";
const char kTizenImeUuidKey[] = "uuid";
const char kTizenImeUuidTextKey[] = "#text";
const char kTizenImeLanguagesKey[] = "languages";
const char kTizenImeLanguageKey[] = "language";
const char kTizenImeLanguageTextKey[] = "#text";
const char kTizenImeKey[] = "widget.ime";

const char kErrMsgLanguages[] =
    "At least and only ONE tizen:languages tag should be specified";
const char kErrMsgEmptyLanguage[] = "Language cannot be empty";
const char kErrMsgParsingIme[] = "Only one ime tag should be specified";
const char kErrMsgParsingUuid[] = "Only one uuid tag should be specified";
const char kErrMsgValidatingUuidEmpty[] =
    "The UUID of ime element is obligatory";
const char kErrMsgUuidFormat[] = "Uuid should be in proper format (8-4-4-4-12)";
const char kErrMsgNoLanguages[] =
    "At least one language of ime element should be specified";

const std::regex kUuidRegex(
    "^[0-9a-zA-Z]{8}([-][0-9a-zA-Z]{4}){3}[-][0-9a-zA-Z]{12}$");

bool GetLanguage(const parser::Value* item, ImeInfo* ime_info,
                 std::string* error) {
  const parser::DictionaryValue* language_dict;
  if (item->GetAsDictionary(&language_dict)) {
    std::string language;
    if (!language_dict->GetString(kTizenImeLanguageTextKey, &language) ||
        language.empty()) {
      *error = kErrMsgEmptyLanguage;
      return false;
    }
    ime_info->AddLanguage(language);
  }
  return true;
}

bool ParseImeEntryAndStore(const parser::DictionaryValue& control_dict,
                           ImeInfo* ime_info, std::string* error) {
  // parsing uuid element
  const parser::DictionaryValue* uuid_dict;
  std::string uuid;
  if (control_dict.GetDictionary(kTizenImeUuidKey, &uuid_dict) &&
      uuid_dict->GetString(kTizenImeUuidTextKey, &uuid)) {
    ime_info->set_uuid(uuid);
  } else {
    *error = kErrMsgParsingUuid;
    return false;
  }

  const parser::DictionaryValue* languages_dict;
  if (!control_dict.GetDictionary(kTizenImeLanguagesKey, &languages_dict)) {
    *error = kErrMsgLanguages;
    return false;
  }

  const parser::Value* languages;
  if (!languages_dict->Get(kTizenImeLanguageKey, &languages)) {
    *error = kErrMsgNoLanguages;
    return false;
  }

  if (languages->GetType() == parser::Value::TYPE_LIST) {
    // many languages
    const parser::ListValue* list;
    languages->GetAsList(&list);
    for (const auto& item : *list) {
      if (!GetLanguage(item, ime_info, error)) return false;
    }
  } else if (languages->GetType() == parser::Value::TYPE_DICTIONARY) {
    if (!GetLanguage(languages, ime_info, error)) return false;
  }

  return true;
}

// UUID is string of 36 characters in form 8-4-4-4-12
bool IsValidUuid(const std::string& uuid) {
  return std::regex_match(uuid, kUuidRegex);
}

}  // namespace

ImeInfo::ImeInfo() {}

ImeInfo::~ImeInfo() {}

ImeHandler::ImeHandler() {}

ImeHandler::~ImeHandler() {}

void ImeInfo::AddLanguage(const std::string& language) {
  languages_.push_back(language);
}


bool ImeHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  if (!manifest.HasPath(kTizenImeKey))
    return true;

  auto ime_info = std::make_shared<ImeInfo>();

  const auto dict = parser::GetOneOrMany(manifest.value(),
      kTizenImeKey, kTizenNamespacePrefix).front();

  if (!ParseImeEntryAndStore(*dict, ime_info.get(), error)) {
    return false;
  }

  *output = std::static_pointer_cast<parser::ManifestData>(ime_info);

  return true;
}

bool ImeHandler::Validate(const parser::ManifestData& data,
                          const parser::ManifestDataMap& /*handlers_output*/,
                          std::string* error) const {
  const ImeInfo& ime_info = static_cast<const ImeInfo&>(data);

  if (!ime_info.exists()) return true;

  if (ime_info.uuid().empty()) {
    *error = kErrMsgValidatingUuidEmpty;
    return false;
  }

  if (!IsValidUuid(ime_info.uuid())) {
    *error = kErrMsgUuidFormat;
    return false;
  }

  if (ime_info.languages().empty()) {
    *error = kErrMsgNoLanguages;
    return false;
  }

  return true;
}

std::string ImeHandler::Key() const { return kTizenImeKey; }

}  // namespace parse
}  // namespace wgt
