// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "wgt_manifest_handlers/platform_version.h"

namespace parser {

utils::VersionNumber GetCurrentPlatformVersion() {
  return utils::VersionNumber(TIZEN_VERSION);
}

utils::VersionNumber GetMinimumPlatformVersion() {
#ifdef TIZEN_TV
  return utils::VersionNumber("2.3");
#elif TIZEN_WEARABLE
  return utils::VersionNumber("2.3");
#else
  return utils::VersionNumber("2.2.1");
#endif
}

}  // namespace parser
