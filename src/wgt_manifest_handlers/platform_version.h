// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_MANIFEST_HANDLERS_PLATFORM_VERSION_H_
#define WGT_MANIFEST_HANDLERS_PLATFORM_VERSION_H_

#include <string>

#include "manifest_parser/utils/version_number.h"

namespace parser {
/**
 * @brief GetCurrentPlatformVersion
 * @return number version of the platform currently used
 */
utils::VersionNumber GetCurrentPlatformVersion();

/**
 * @brief GetMinimumPlatformVersion
 * @return number version of the minimum platform for current profile
 */
utils::VersionNumber GetMinimumPlatformVersion();

}  // namespace parser

#endif  // WGT_MANIFEST_HANDLERS_PLATFORM_VERSION_H_
