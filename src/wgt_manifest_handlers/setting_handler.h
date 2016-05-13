// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WGT_MANIFEST_HANDLERS_SETTING_HANDLER_H_
#define WGT_MANIFEST_HANDLERS_SETTING_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "manifest_parser/values.h"
#include "manifest_parser/manifest_handler.h"

namespace wgt {
namespace parse {

class SettingInfo : public parser::ManifestData {
 public:
  enum class ScreenOrientation {
    PORTRAIT,
    LANDSCAPE,
    AUTO
  };

  enum class InstallLocation {
    INTERNAL,
    EXTERNAL,
    AUTO
  };

  enum class SoundMode {
    SHARED,
    EXCLUSIVE
  };

  SettingInfo();
  virtual ~SettingInfo();

  static std::string Key();
  /**
   * @brief set_hwkey_enabled sets value of hw_key to true or false
   * @param enabled
   */
  void set_hwkey_enabled(bool enabled) { hwkey_enabled_ = enabled; }
  /**
   * @brief hwkey_enabled
   * @return  current value of hw_key
   */
  bool hwkey_enabled() const { return hwkey_enabled_; }
  /**
   * @brief set_screen_orientation sets screen_orientation
   * @param orientation
   */
  void set_screen_orientation(ScreenOrientation orientation) {
    screen_orientation_ = orientation;
    orientation_defaulted_ = false;
  }
  /**
   * @brief screen_orientation
   * @return currently selected screen orientation
   */

  ScreenOrientation screen_orientation() const { return screen_orientation_; }
  /**
   * @brief set_encryption_enabled sets if encryption is enabled or disabled
   * @param enabled
   */
  void set_encryption_enabled(bool enabled) { encryption_enabled_ = enabled; }
  /**
   * @brief encryption_enabled
   * @return true if encryption is enable
   */
  bool encryption_enabled() const { return encryption_enabled_; }
  /**
   * @brief set_context_menu_enabled sets context menu enabled
   * @param enabled
   */
  void set_context_menu_enabled(bool enabled) {
    context_menu_enabled_ = enabled;
  }
  /**
   * @brief context_menu_enabled
   * @return true if context menu is enabled
   */
  bool context_menu_enabled() const { return context_menu_enabled_; }
  /**
   * @brief set_background_support_enabled whether it is enabled
   * @param enabled
   */
  void set_background_support_enabled(bool enabled) {
    background_support_enabled_ = enabled;
  }
  /**
   * @brief background_support_enabled
   * @return true if background support is enabled
   */
  bool background_support_enabled() const {
    return background_support_enabled_;
  }
  /**
   * @brief set_install_location sets install location
   * @param installLocation
   */
  void set_install_location(InstallLocation installLocation) {
    install_location_ = installLocation;
  }
  /**
   * @brief install_location
   * @return what InstallLocation is chosen
   */
  InstallLocation install_location() const { return install_location_; }
  /**
   * @brief set_no_display sets is there is no display
   * @param enabled
   */
  void set_no_display(bool enabled) { no_display_ = enabled; }
  /**
   * @brief no_display
   * @return if there is no supoort for display
   */
  bool no_display() const { return no_display_; }
  /**
   * @brief set_indicator_presence sets indicator presence
   * @param enabled
   */
  void set_indicator_presence(bool enabled) { indicator_presence_ = enabled; }
  /**
   * @brief indicator_presence
   * @return  if the indicator is present
   */
  bool indicator_presence() const { return indicator_presence_; }
  /**
   * @brief set_backbutton_presence sets if backbutton is present
   * @param enabled
   */
  void set_backbutton_presence(bool enabled) { backbutton_presence_ = enabled; }
  /**
   * @brief backbutton_presence
   * @return true if backbutton is present
   */
  bool backbutton_presence() const { return backbutton_presence_; }
  /**
   * @brief set_user_agent sets user_agent()
   * @param user_agent
   */
  void set_user_agent(std::string user_agent) { user_agent_ = std::move(user_agent); }
  /**
   * @brief user_agent
   * @return actual user agent
   */
  std::string user_agent() const { return user_agent_; }
  /**
   * @brief set_sound_mode sets sound mode
   * @param soundMode
   */
  void set_sound_mode(SoundMode soundMode) { sound_mode_ = soundMode; }
  /**
   * @brief sound_mode
   * @return current sound mode
   */
  SoundMode sound_mode() const { return sound_mode_; }
  /**
   * @brief set_background_vibration set background vibration
   * @param enabled
   */
  void set_background_vibration(bool enabled) {
    background_vibration_ = enabled;
  }
  /**
   * @brief background_vibration
   * @return background vibration
   */
  bool background_vibration() const { return background_vibration_; }

  /**
   * @brief set_orientation_defaulted
   * @param orientation_defaulted
   */
  void set_orientation_defaulted(bool orientation_defaulted) {
    orientation_defaulted_ = orientation_defaulted;
  }

  /**
   * @brief orientation_defaulted
   * @return information if orientation has default value
   */
  bool orientation_defaulted() const { return orientation_defaulted_; }

 private:
  bool hwkey_enabled_;
  ScreenOrientation screen_orientation_;
  bool encryption_enabled_;
  bool context_menu_enabled_;
  bool background_support_enabled_;
  InstallLocation install_location_;
  bool no_display_;
  bool indicator_presence_;
  bool backbutton_presence_;
  std::string user_agent_;
  SoundMode sound_mode_;
  bool background_vibration_;

  bool orientation_defaulted_;
};

/**
 * @brief The SettingHandler class
 *
 * Handler of config.xml for xml elements:
 *  - <tizen:setting>.
 */
class SettingHandler : public parser::ManifestHandler {
 public:
  SettingHandler();
  virtual ~SettingHandler();

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& application,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  std::string Key() const override;
  std::vector<std::string> PrerequisiteKeys() const override;
};
}  // namespace parse
}  // namespace wgt

#endif  // WGT_MANIFEST_HANDLERS_SETTING_HANDLER_H_
