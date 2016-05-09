// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/appwidget_handler.h"

#include <cassert>
#include <cstdlib>
#include <limits>
#include <regex>
#include <set>

#include "manifest_parser/values.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"
#include "wgt_manifest_handlers/tizen_application_handler.h"

namespace wgt {
namespace parse {

namespace {

const char kTizenAppWidgetFullKey[] = "widget.app-widget";
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kWidgetNamespacePrefix[] = "http://www.w3.org/ns/widgets";
const char kNamespaceKey[] = "@namespace";
const char kTizenWidgetKey[] = "widget";
const char kTizenAppWidgetKey[] = "app-widget";
const char kTizenAppWidgetBoxLabelLangKey[] = "@lang";
const char kTizenAppWidgetBoxIconSrcKey[] = "@src";
const char kTizenAppWidgetBoxContentSizePreviewKey[] = "@preview";
const char kTizenAppWidgetBoxContentSizeUseDecorationKey[] = "@use-decoration";
const char kTizenAppWidgetBoxContentDropViewSrcKey[] = "@src";
const char kTizenAppWidgetBoxContentDropViewWidthKey[] = "@width";
const char kTizenAppWidgetBoxContentDropViewHeightKey[] = "@height";
const char kTizenAppWidgetBoxContentSrcKey[] = "@src";
const char kTizenAppWidgetBoxContentMouseEventKey[] = "@mouse-event";
const char kTizenAppWidgetBoxContentTouchEffectKey[] = "@touch-effect";
const char kTizenAppWidgetBoxContentSizeKey[] = "box-size";
const char kTizenAppWidgetBoxContentDropViewKey[] = "pd";
const char kTizenAppWidgetAutoLaunchKey[] = "@auto-launch";
const char kTizenAppWidgetBoxLabelKey[] = "box-label";
const char kTizenAppWidgetBoxIconKey[] = "box-icon";
const char kTizenAppWidgetBoxContentKey[] = "box-content";
const char kTizenAppWidgetIdKey[] = "@id";
const char kTizenAppWidgetPrimaryKey[] = "@primary";
const char kTizenAppWidgetUpdatePeriodKey[] = "@update-period";
const char kTizenAppWidgetBoxLabelTextKey[] = "#text";
const char kTizenAppWidgetBoxContentSizeTextKey[] = "#text";

const char kErrMsgInvalidDictionary[] =
    "Cannot get key value as a dictionary. Key name: ";
const char kErrMsgInvalidList[] = "Cannot get key value as a list. Key name: ";
const char kErrMsgNoMandatoryKey[] = "Cannot find mandatory key. Key name: ";
const char kErrMsgInvalidKeyValue[] = "Invalid key value. Key name: ";
const char kErrMsgMultipleKeys[] = "Too many keys found. Key name: ";
const char kErrMsgNoNamespace[] =
    "Element pointed by key has no namespace specified. Key name: ";
const char kErrMsgInvalidNamespace[] =
    "Invalid namespace of element pointed by key. Key name: ";
const char kErrMsgUpdatePeriodOutOfDomain[] =
    "Value of an update-period attribute in app-widget element out of domain."
    " The value: ";
const char kErrMsgNoLabel[] =
    "No box-label element in app-widget element.";
const char kErrMsgInvalidContentSrc[] =
    "Invalid path or url in a src attribute of box-content element."
    " The value: ";
const char kErrMsgNoMandatoryContentSize1x1[] =
    "No mandatory box-size element (1x1) in box-content element.";
const char kErrMsgInvalidContentDropViewSrc[] =
    "Invalid path or url in a src attribute of pd element. The value: ";
const char kErrMsgContentDropViewHeightOutOfDomain[] =
    "Value of a height attribute in box-content element out of domain."
    " The value: ";

const std::regex kStringRegex("[.][0-9a-zA-Z]+");

// If the error parameter is specified, it is filled with the given message
// otherwise it does nothing.
void SetError(const std::string& message, std::string* error) {
  if (error) *error = message;
}

// If the error parameter is specified, it is filled with concatenation
// of message and arg parameters otherwise it does nothing.
void SetError(const std::string& message, const std::string& arg,
              std::string* error) {
  if (error) *error = message + arg;
}

// Converts given text value to a value of specific type. Returns true
// if convertion is successful or false otherwise.
template <typename ValueType>
bool ConvertValue(const std::string& /*str_value*/, ValueType* /*value*/) {
  assert(false &&
         "Use one of already defined template specializations"
         " or define a new one.");
  return false;
}

// Converts given text value to a string value. Returns true
// if convertion is successful or false otherwise.
template <>
bool ConvertValue(const std::string& str_value, std::string* value) {
  assert(value);
  *value = str_value;
  return true;
}

// Converts given text value to a boolean value. Returns true
// if convertion is successful or false otherwise.
template <>
bool ConvertValue(const std::string& str_value, bool* value) {
  assert(value);
  if (str_value == "true") {
    *value = true;
    return true;
  }
  if (str_value == "false") {
    *value = false;
    return true;
  }
  return false;
}

// Converts given text value to an integer value. Returns true
// if convertion is successful or false otherwise.
template <>
bool ConvertValue(const std::string& str_value, int* value) {
  assert(value);
  char* end = nullptr;
  *value = strtol(str_value.c_str(), &end, 10);
  return end == &*str_value.end();
}

// Converts given text value to a floating point value. Returns true
// if convertion is successful or false otherwise.
template <>
bool ConvertValue(const std::string& str_value, double* value) {
  assert(value);
  char* end = nullptr;
  *value = strtod(str_value.c_str(), &end);
  return end == &*str_value.end();
}

// Retrieves a mandatory value from specified dictionary and specified key.
// Returns true, if the value is found or false otherwise. If the error
// parameter is specified, it is also filled with proper message.
template <typename ValueType>
bool GetMandatoryValue(const parser::DictionaryValue& dict,
                       const std::string& key, ValueType* value,
                       std::string* error) {
  assert(value);
  std::string tmp;
  if (!dict.GetString(key, &tmp)) {
    SetError(kErrMsgNoMandatoryKey, key, error);
    return false;
  }
  bool result = ConvertValue(tmp, value);
  if (!result) SetError(kErrMsgInvalidKeyValue, key, error);
  return result;
}

// Retrieves an optional value from specified dictionary and specified key.
// If the value is found, the function returns true and fills value
// parameter. If the value is not found, the function returns true and fills
// value parameter with default value. If an error occurs, it returns false
// and fills error parameter if it is set.
template <typename ValueType>
bool GetOptionalValue(const parser::DictionaryValue& dict,
                      const std::string& key, ValueType default_value,
                      ValueType* value, std::string* error) {
  assert(value);
  std::string tmp;
  if (!dict.GetString(key, &tmp)) {
    *value = default_value;
    return true;
  }
  bool result = ConvertValue(tmp, value);
  if (!result) SetError(kErrMsgInvalidKeyValue, key, error);
  return result;
}

// Verifies whether specified dictionary represents an element in specified
// namespace. Returns true, if the namespace is set and equal to the specified
// one or false otherwise. If the error parameter is specified, it is also
// filled with proper message.
bool VerifyElementNamespace(const parser::DictionaryValue& dict,
                            const std::string& key,
                            const std::string& desired_namespace_value,
                            std::string* error) {
  std::string namespace_value;
  if (!GetMandatoryValue(dict, kNamespaceKey, &namespace_value, nullptr)) {
    SetError(kErrMsgNoNamespace, key, error);
    return false;
  }
  if (namespace_value != desired_namespace_value) {
    SetError(kErrMsgInvalidNamespace, key, error);
    return false;
  }
  return true;
}

// Parses box-label part
bool ParseLabel(const parser::DictionaryValue& dict, const std::string& key,
                AppWidget* app_widget, std::string* error) {
  assert(app_widget);

  if (!VerifyElementNamespace(dict, key, kTizenNamespacePrefix, error))
    return false;

  std::string lang;
  if (!GetOptionalValue(dict, kTizenAppWidgetBoxLabelLangKey, std::string(),
                        &lang, error))
    return false;

  std::string text;
  if (!GetMandatoryValue(dict, kTizenAppWidgetBoxLabelTextKey, &text, error))
    return false;

  if (lang.empty()) {
    // Note: Tizen 2.2 WRT Core Spec does not determine how many times the value
    // without lang attribute can appear in one app-widget, so overwrite.
    app_widget->label.default_value = text;
  } else {
    // Note: Tizen 2.2 WRT Core Spec does not determine how many times the value
    // with specific lang attribute can appear in one app-widget, so overwrite.
    app_widget->label.lang_value_map[lang] = text;
  }

  return true;
}

// Parses box-icon part
bool ParseIcon(const parser::DictionaryValue& dict, const std::string& key,
               AppWidget* app_widget, std::string* error) {
  assert(app_widget);

  if (!VerifyElementNamespace(dict, key, kTizenNamespacePrefix, error))
    return false;

  if (!app_widget->icon_src.empty()) {
    SetError(kErrMsgMultipleKeys, key, error);
    return false;
  }

  if (!GetMandatoryValue(dict, kTizenAppWidgetBoxIconSrcKey,
      &app_widget->icon_src, error))
    return false;

  return true;
}

// Converts size type from text to enum representation
bool StringToSizeType(const std::string& str_type,
                      AppWidgetSizeType* enum_type) {
  assert(enum_type);
  if (str_type == "1x1") {
    *enum_type = AppWidgetSizeType::k1x1;
    return true;
  }
  if (str_type == "2x1") {
    *enum_type = AppWidgetSizeType::k2x1;
    return true;
  }
  if (str_type == "2x2") {
    *enum_type = AppWidgetSizeType::k2x2;
    return true;
  }
  return false;
}

// Parses box-size part
bool ParseContentSizes(const parser::DictionaryValue& dict,
                       const std::string& key, AppWidget* app_widget,
                       std::string* error) {
  assert(app_widget);

  if (!VerifyElementNamespace(dict, key, kTizenNamespacePrefix, error))
    return false;

  AppWidgetSize size;

  std::string str_type;
  if (!GetMandatoryValue(dict, kTizenAppWidgetBoxContentSizeTextKey, &str_type,
                         error))
    return false;

  AppWidgetSizeType type;
  if (!StringToSizeType(str_type, &type)) {
    SetError(kErrMsgInvalidKeyValue, kTizenAppWidgetBoxContentSizeTextKey,
             error);
    return false;
  }
  size.type = type;

  if (!GetOptionalValue(dict, kTizenAppWidgetBoxContentSizePreviewKey,
                        std::string(), &size.preview, error))
    return false;

  if (!GetOptionalValue(dict, kTizenAppWidgetBoxContentSizeUseDecorationKey,
                        true, &size.use_decoration, error))
    return false;

  app_widget->content_size.push_back(size);

  return true;
}

// Parses pd part
bool ParseContentDropView(const parser::DictionaryValue& dict,
                          const std::string& key, AppWidget* app_widget,
                          std::string* error) {
  assert(app_widget);

  if (!VerifyElementNamespace(dict, key, kTizenNamespacePrefix, error))
    return false;

  if (!app_widget->content_drop_view.empty()) {
    SetError(kErrMsgMultipleKeys, key, error);
    return false;
  }

  AppWidgetDropView drop_view;

  if (!GetMandatoryValue(dict, kTizenAppWidgetBoxContentDropViewSrcKey,
                         &drop_view.src, error))
    return false;

  if (!GetMandatoryValue(dict, kTizenAppWidgetBoxContentDropViewWidthKey,
                         &drop_view.width, error))
    return false;

  if (!GetMandatoryValue(dict, kTizenAppWidgetBoxContentDropViewHeightKey,
                         &drop_view.height, error))
    return false;

  app_widget->content_drop_view.push_back(drop_view);

  return true;
}

// Parses box-content part
bool ParseContent(const parser::DictionaryValue& dict, const std::string& key,
                  AppWidget* app_widget, std::string* error) {
  assert(app_widget);

  if (!VerifyElementNamespace(dict, key, kTizenNamespacePrefix, error))
    return false;

  if (!app_widget->content_src.empty()) {
    SetError(kErrMsgMultipleKeys, key, error);
    return false;
  }
  if (!GetMandatoryValue(dict, kTizenAppWidgetBoxContentSrcKey,
                         &app_widget->content_src, error))
    return false;

  if (!GetOptionalValue(dict, kTizenAppWidgetBoxContentMouseEventKey, false,
                        &app_widget->content_mouse_event, error))
    return false;

  if (!GetOptionalValue(dict, kTizenAppWidgetBoxContentTouchEffectKey, true,
                        &app_widget->content_touch_effect, error))
    return false;

  if (!dict.HasKey(kTizenAppWidgetBoxContentSizeKey))
    return false;

  for (const auto& dict_cs : parser::GetOneOrMany(&dict,
      kTizenAppWidgetBoxContentSizeKey, "")) {
    if (!ParseContentSizes(*dict_cs, kTizenAppWidgetBoxContentSizeKey,
        app_widget, error))
      return false;
  }

  for (const auto& dict_dv : parser::GetOneOrMany(&dict,
      kTizenAppWidgetBoxContentDropViewKey, "")) {
    if (!ParseContentDropView(*dict_dv,
        kTizenAppWidgetBoxContentDropViewKey, app_widget, error))
      return false;
  }

  return true;
}

// Parses app-widget part
bool ParseAppWidget(const parser::DictionaryValue& dict, const std::string& key,
                    AppWidgetVector* app_widgets, std::string* error) {
  assert(app_widgets);

  if (!VerifyElementNamespace(dict, key, kTizenNamespacePrefix, error))
    return false;

  AppWidget app_widget;

  if (!GetMandatoryValue(dict, kTizenAppWidgetIdKey, &app_widget.id, error))
    return false;

  if (!GetMandatoryValue(dict, kTizenAppWidgetPrimaryKey, &app_widget.primary,
                         error))
    return false;

  double update_period;
  double no_update_period = std::numeric_limits<double>::min();
  if (!GetOptionalValue(dict, kTizenAppWidgetUpdatePeriodKey, no_update_period,
                        &update_period, error))
    return false;
  if (update_period != no_update_period)
    app_widget.update_period.push_back(update_period);

  if (!GetOptionalValue(dict, kTizenAppWidgetAutoLaunchKey, false,
                        &app_widget.auto_launch, error))
    return false;

  if (!dict.HasKey(kTizenAppWidgetBoxLabelKey))
    return false;

  for (const auto& dict_l : parser::GetOneOrMany(&dict,
      kTizenAppWidgetBoxLabelKey, kTizenNamespacePrefix)) {
    if (!ParseLabel(*dict_l, kTizenAppWidgetBoxLabelKey,
        &app_widget, error))
      return false;
  }

  for (const auto& dict_i : parser::GetOneOrMany(&dict,
      kTizenAppWidgetBoxIconKey, kTizenNamespacePrefix)) {
    if (!ParseIcon(*dict_i, kTizenAppWidgetBoxIconKey,
        &app_widget, error))
      return false;
  }

  if (!dict.HasKey(kTizenAppWidgetBoxContentKey))
    return false;

  for (const auto& dict_c : parser::GetOneOrMany(&dict,
      kTizenAppWidgetBoxContentKey, kTizenNamespacePrefix)) {
    if (!ParseContent(*dict_c, kTizenAppWidgetBoxContentKey,
        &app_widget, error))
      return false;
  }

  app_widgets->push_back(app_widget);

  return true;
}

// Validates all content sizes in an app-widget
bool ValidateContentSize(const AppWidgetSizeVector& content_size,
                         std::string* error) {
  bool mandatory_1x1_found = false;

  for (const AppWidgetSize& size : content_size) {
    mandatory_1x1_found |= size.type == AppWidgetSizeType::k1x1;
  }

  if (!mandatory_1x1_found) {
    SetError(kErrMsgNoMandatoryContentSize1x1, error);
    return false;
  }

  return true;
}

}  // namespace

AppWidgetInfo::AppWidgetInfo(const AppWidgetVector& app_widgets)
    : app_widgets_(app_widgets) {}

AppWidgetInfo::~AppWidgetInfo() {}

AppWidgetHandler::AppWidgetHandler() {}

AppWidgetHandler::~AppWidgetHandler() {}


bool AppWidgetHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  if (!manifest.HasPath(kTizenAppWidgetFullKey))
    return false;

  AppWidgetVector app_widgets;

  for (const auto& dict : parser::GetOneOrMany(manifest.value(),
      kTizenAppWidgetFullKey, kTizenNamespacePrefix)) {
    if (!ParseAppWidget(*dict, kTizenAppWidgetKey, &app_widgets, error))
      return false;
  }

  *output = std::static_pointer_cast<parser::ManifestData>(
      std::make_shared<AppWidgetInfo>(app_widgets));
  return true;
}

bool AppWidgetHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {
  const AppWidgetInfo& app_widget_info =
      static_cast<const AppWidgetInfo&>(data);
  const AppWidgetVector& app_widgets = app_widget_info.app_widgets();

  for (const AppWidget& app_widget : app_widgets) {
    if (!app_widget.update_period.empty() &&
        app_widget.update_period.front() < 1800) {
      SetError(kErrMsgUpdatePeriodOutOfDomain,
               std::to_string(app_widget.update_period.front()), error);
      return false;
    }

    if (app_widget.label.default_value.empty() &&
        app_widget.label.lang_value_map.empty()) {
      SetError(kErrMsgNoLabel, error);
      return false;
    }

    if (!ValidateContentSize(app_widget.content_size, error))
      return false;

    if (!app_widget.content_drop_view.empty()) {
      const AppWidgetDropView& drop_view = app_widget.content_drop_view.front();

      if (drop_view.height < 1 || drop_view.height > 380) {
        SetError(kErrMsgContentDropViewHeightOutOfDomain,
                 std::to_string(drop_view.height), error);
        return false;
      }
    }
  }

  return true;
}

std::string AppWidgetHandler::Key() const {
  return kTizenAppWidgetFullKey;
}

}  // namespace parse
}  // namespace wgt
