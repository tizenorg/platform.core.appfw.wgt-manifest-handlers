// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_APPLICATION_MANIFEST_CONSTANTS_H_
#define WGT_MANIFEST_HANDLERS_APPLICATION_MANIFEST_CONSTANTS_H_

#include "manifest_parser/manifest.h"

// Keys used in JSON representation of applications.
namespace wgt {

namespace application_widget_keys {

extern const char kAccessKey[];
extern const char kAccountKey[];
extern const char kAllowNavigationKey[];
extern const char kAuthorKey[];
extern const char kCSPKey[];
extern const char kCSPReportOnlyKey[];
extern const char kDescriptionKey[];
extern const char kIconsKey[];
extern const char kTizenApplicationAppControlsKey[];
extern const char kTizenApplicationKey[];
extern const char kTizenAppWidgetFullKey[];
extern const char kTizenBackgroundCategoryKey[];
extern const char kTizenCategoryKey[];
extern const char kTizenContentKey[];
extern const char kTizenImeKey[];
extern const char kTizenMetaDataKey[];
extern const char kTizenPermissionsKey[];
extern const char kTizenServiceKey[];
extern const char kTizenSettingKey[];
extern const char kTizenSplashScreenKey[];
extern const char kTizenWidgetKey[];
extern const char kVersionKey[];
extern const char kWidgetKey[];
extern const char kSplashScreenKey[];

}  // namespace application_widget_keys

}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_APPLICATION_MANIFEST_CONSTANTS_H_
