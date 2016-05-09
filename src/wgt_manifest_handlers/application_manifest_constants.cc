// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/application_manifest_constants.h"

namespace wgt {

// manifest keys for widget applications.
namespace application_widget_keys {

const char kCSPKey[] = "widget.content-security-policy";
const char kCSPReportOnlyKey[] = "widget.content-security-policy-report-only";
const char kIconsKey[] = "icons";
const char kAccountKey[] = "widget.account";
const char kDefaultLocaleKey[] = "widget.@defaultlocale";
const char kViewModesKey[] = "widget.@viewmodes";
const char kWidgetKey[] = "widget";
const char kAuthorKey[] = "widget.author";
const char kDescriptionKey[] = "widget.description";
const char kShortKey[] = "@short";
const char kIDKey[] = "widget.@id";
const char kHeightKey[] = "widget.@height";
const char kWidthKey[] = "widget.@width";
const char kPreferencesKey[] = "widget.preference";
const char kAccessKey[] = "widget.access";
const char kLicenseKey[] = "widget.license";

const char kTizenWidgetKey[] = "widget";
// Child keys inside 'kPreferencesKey'.
const char kPreferencesNameKey[] = "@name";

const char kWidgetLangKey[] = "widget.@lang";

const char kTizenApplicationKey[] = "widget.application";
const char kTizenAppWidgetFullKey[] = "widget.app-widget";
// Child keys inside 'kTizenApplicationKey'

const char kTizenApplicationLaunchModeKey[] = "@launch_mode";
const char kTizenApplicationPackageKey[] = "@package";

const char kAllowNavigationKey[] = "widget.allow-navigation";
const char kTizenBackgroundCategoryKey[] = "widget.background-category";
const char kTizenSettingKey[] = "widget.setting";
const char kTizenInstallLocationKey[] = "@install-location";

const char kTizenNoDisplayKey[] = "@nodisplay";
const char kTizenIndicatorPresenceKey[] = "@indicator-presence";
const char kTizenBackbuttonPresenceKey[] = "@backbutton-presence";
const char kTizenUserAgentKey[] = "@user-agent";
const char kTizenSoundModeKey[] = "@sound-mode";
const char kTizenBackgroundVibrationKey[] = "@background-vibration";
const char kTizenMetaDataKey[] = "widget.metadata";
const char kTizenSplashScreenKey[] = "widget.splash-screen";
// Child keys inside 'kTizenMetaDataKey'
const char kTizenPermissionsKey[] = "widget.privilege";

// App control
const char kTizenApplicationAppControlsKey[] = "widget.app-control";
// IME
const char kTizenImeKey[] = "widget.ime";
// Content
const char kTizenContentKey[] = "widget.content";
// Category
const char kTizenCategoryKey[] = "widget.category";
// Service
const char kTizenServiceKey[] = "widget.service";

// launch_screen
const char kSplashScreenKey[] = "widget.launch_screen";

}  // namespace application_widget_keys

}  // namespace wgt
