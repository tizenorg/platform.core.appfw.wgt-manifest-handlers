// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/content_handler.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <map>
#include <set>

#include "manifest_parser/utils/iri_util.h"
#include "manifest_parser/utils/logging.h"
#include "manifest_parser/utils/string_util.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace ba = boost::algorithm;

namespace {

const char kNamespaceKey[] = "@namespace";
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kWidgetNamespacePrefix[] = "http://www.w3.org/ns/widgets";
const char kTizenContentEncodingKey[] = "@encoding";
const char kTizenContentTypeKey[] = "@type";
const char kTizenContentKey[] = "widget.content";
const char kMimeMainComponent[] = "";
const char kMimeCharsetComponent[] = "charset";
const char kDefaultMimeType[] = "text/html";
const char kDefaultEncoding[] = "UTF-8";
const char kTizenContentSrcKey[] = "@src";

const std::set<std::string> ValidMimeTypeStartFile = {
    "text/html", "application/xhtml+xml", "image/svg+xml"};

std::map<std::string, std::string> ParseMimeComponents(
    const std::string& type) {
  std::map<std::string, std::string> mime_components;
  std::vector<std::string> components;

  ba::split(components, type, ba::is_any_of(";"));
  for (auto& component : components) {
    auto split = component.find("=");
    std::string key;
    std::string value;
    if (split != std::string::npos) {
      key = component.substr(0, split);
      value = component.substr(split + 1);
    } else {
      key = kMimeMainComponent;
      value = component;
    }
    ba::trim(key);
    ba::trim(value);
    mime_components.insert(std::make_pair(key, value));
  }
  return mime_components;
}

bool ValidateMimeTypeStartFile(const std::string& type) {
  return ValidMimeTypeStartFile.find(parser::utils::CollapseWhitespaceUTF8(
             type)) != ValidMimeTypeStartFile.end();
}

}  // namespace

namespace wgt {
namespace parse {

ContentHandler::ContentHandler()
    : w3c_content_found_(false), tizen_content_found_(false) {}

ContentHandler::~ContentHandler() {}

/**
 * @brief ParseAndSetContentValue
 *
 * This function should return parse result and set output parameter content
 * to valid content if returning OK
 *
 * @param dict input dictionary for content element
 * @param content content element to be set if valid one found
 * @param error parser error to be set if parsing failed with error
 * @return enum value describing result of parsing
 */
ContentHandler::ParseResult ContentHandler::ParseAndSetContentValue(
    const parser::DictionaryValue& dict,
    std::shared_ptr<wgt::parse::ContentInfo>* content, std::string* error) {
  std::string element_namespace;
  dict.GetString(kNamespaceKey, &element_namespace);

  if (element_namespace == kTizenNamespacePrefix) {
    if (tizen_content_found_) {
      // tizen:content already found
      return ParseResult::IGNORE;
    }
    tizen_content_found_ = true;
  } else {
    if (w3c_content_found_) {
      // ignore all content element that are not first
      return ParseResult::IGNORE;
    }
    w3c_content_found_ = true;
  }

  std::string src;
  if (!dict.GetString(kTizenContentSrcKey, &src)) {
    return ParseResult::IGNORE;
  }

  // error if empty
  if (src.empty()) {
    // ignore this element
    return ParseResult::IGNORE;
  }

  std::string type = kDefaultMimeType;
  dict.GetString(kTizenContentTypeKey, &type);
  // TODO(t.iwanek): this will fail for "quoted-string"
  //                 use/implement proper mime parsing...
  std::map<std::string, std::string> mime_components =
      ParseMimeComponents(type);

  auto mime_iter = mime_components.find(kMimeMainComponent);
  if (mime_iter != mime_components.end()) {
    if (!ValidateMimeTypeStartFile(mime_iter->second)) {
      *error = "Not proper type of starting file";
      return ParseResult::IGNORE;
    }
  }

  std::string encoding = kDefaultEncoding;
  if (!dict.GetString(kTizenContentEncodingKey, &encoding)) {
    auto charset_iter = mime_components.find(kMimeCharsetComponent);
    if (charset_iter != mime_components.end()) {
      encoding = charset_iter->second;
    }
  }
  ba::trim(encoding);

  if (*content && (*content)->is_tizen_content()) {
    // Prefer tizen:content if both are correct
    return ParseResult::IGNORE;
  }

  content->reset(new wgt::parse::ContentInfo());
  (*content)->set_src(src);
  (*content)->set_type(type);
  (*content)->set_encoding(encoding);
  (*content)->set_is_tizen_content(element_namespace == kTizenNamespacePrefix);
  return ParseResult::OK;
}

bool ContentHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  if (!manifest.HasPath(kTizenContentKey))
    return true;

  std::shared_ptr<ContentInfo> content_info;

  for (const auto& dict : parser::GetOneOrMany(manifest.value(),
      kTizenContentKey, "")) {
    if (ParseAndSetContentValue(*dict, &content_info, error)
        == ParseResult::ERROR) {
        return false;
    }
  }

  *output = std::static_pointer_cast<parser::ManifestData>(content_info);

  return true;
}

std::string ContentHandler::Key() const {
  return kTizenContentKey;
}

std::string ContentInfo::Key() {
  return kTizenContentKey;
}

}  // namespace parse
}  // namespace wgt
