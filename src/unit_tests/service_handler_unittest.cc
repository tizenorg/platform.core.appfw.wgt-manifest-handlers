// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/path.hpp>

#include <gtest/gtest.h>

#include <initializer_list>
#include <vector>

#include "manifest_parser/manifest.h"
#include "manifest_parser/manifest_handler.h"
#include "manifest_parser/manifest_parser_impl.h"
#include "manifest_parser/manifest_parser.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"
#include "wgt_manifest_handlers/service_handler.h"

namespace bf = boost::filesystem;

namespace {
const char kWidgetKey[] = "widget";
const char kTizenServiceKey[] = "widget.service";
const char kTizenServiceContentKey[] = "content";
const char kTizenServiceContentSrcKey[] = "@src";
const char kTizenServiceIdKey[] = "@id";
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kTizenServiceOnBootKey[] = "@on-boot";
const char kTizenServiceNameKey[] = "name";
const char kTizenServiceCategoryNameKey[] = "@name";
const char kTizenServiceCategoryKey[] = "category";
const char kTizenServiceIconKey[] = "icon";
const char kNamespaceKey[] = "@namespace";
const char kServiceKey[] = "service";
const char kTizenServiceAutoRestartKey[] = "@auto-restart";
const char kTizenServiceIconSrcKey[] = "@src";
const char kTizenServiceDescriptionKey[] = "description";
const char kTizenServiceMetadataKeyKey[] = "@key";
const char kTizenServiceMetadataValueKey[] = "@value";
const char kTizenServiceMetadataKey[] = "metadata";
const char kXmlLangKey[] = "@lang";
const char kXmlTextKey[] = "#text";
std::unique_ptr<parser::ManifestHandlerRegistry> GetRegistryForTest() {
  std::unique_ptr<parser::ManifestHandlerRegistry> registry;
  registry.reset(new parser::ManifestHandlerRegistry());
  registry->RegisterManifestHandler(new wgt::parse::ServiceHandler());
  return registry;
}

}  // namespace

namespace parser {

class ServiceHandlerTest : public testing::Test {
 public:
  void SetUp() override {
    parser_.reset(new ManifestParserImpl((GetRegistryForTest())));
  }
  void TearDown() override { parser_.reset(); }
  void SetManifest(std::shared_ptr<Manifest> manifest) {
    parser_->manifest_ = manifest;
  }
  bool ParseAppManifest() {
    std::string error;
    return parser_->ParseAppManifest(&error);
  }
  bool ValidateAppManifest() {
    std::string error;
    return parser_->ValidateAppManifest(&error);
  }
  std::shared_ptr<const ManifestData> GetManifestData(const std::string& key) {
    return parser_->GetManifestData(key);
  }

 private:
  std::unique_ptr<ManifestParserImpl> parser_;
};

TEST_F(ServiceHandlerTest, NoServiceEntry) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntryDefault) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].categories().size(), 0);
  ASSERT_EQ(service_list->services[0].metadata_set().size(), 0);
}

TEST_F(ServiceHandlerTest, SingleServiceEntryOnBootOn) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(::kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct002.appId");
  service->SetString(kTizenServiceOnBootKey, "true");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct002.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), true);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
}

TEST_F(ServiceHandlerTest, SingleServiceEntryAutoRestartOn) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct003.appId");
  service->SetString(kTizenServiceOnBootKey, "false");
  service->SetString(kTizenServiceAutoRestartKey, "true");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct003.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), true);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
}

TEST_F(ServiceHandlerTest, SingleServiceEntryWrongId) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "wrongid.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_FALSE(ValidateAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntryIdTypeMismatch) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetInteger(kTizenServiceIdKey, 1410);
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_FALSE(ParseAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntryNameMissing) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  service->Set(kTizenServiceContentKey, content.release());
  service->SetInteger(kTizenServiceIdKey, 1410);
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_FALSE(ParseAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntryIdSingleNameNotInTizen) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  service->Set(kTizenServiceContentKey, content.release());
  service->SetInteger(kTizenServiceIdKey, 1410);
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_FALSE(ParseAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntryMultipleNames) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);

  std::unique_ptr<ListValue> name_list(new ListValue());
  for (auto& pair : {std::make_pair(std::string(), "first"),
                     std::make_pair(std::string("en"), "second"),
                     std::make_pair(std::string("de"), "third")}) {
    std::unique_ptr<DictionaryValue> name(new DictionaryValue());
    name->SetString(kNamespaceKey, kTizenNamespacePrefix);
    if (!pair.first.empty()) name->SetString(kXmlLangKey, pair.first);
    name->SetString(kXmlTextKey, pair.second);
    name_list->Append(name.release());
  }
  service->Set(kTizenServiceNameKey, name_list.release());

  service->Set(kTizenServiceContentKey, content.release());
  service->SetString(kTizenServiceIdKey, "correct003.appId");
  service->SetString(kTizenServiceOnBootKey, "false");
  service->SetString(kTizenServiceAutoRestartKey, "true");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct003.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), true);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 3);
  ASSERT_EQ(service_list->services[0].names()[0].first, "");
  ASSERT_EQ(service_list->services[0].names()[0].second, "first");
  ASSERT_EQ(service_list->services[0].names()[1].first, "en");
  ASSERT_EQ(service_list->services[0].names()[1].second, "second");
  ASSERT_EQ(service_list->services[0].names()[2].first, "de");
  ASSERT_EQ(service_list->services[0].names()[2].second, "third");
}

TEST_F(ServiceHandlerTest, SingleServiceEntryIdSingleContentNotInTizen) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  service->Set(kTizenServiceNameKey, name.release());
  service->SetInteger(kTizenServiceIdKey, 1410);
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_FALSE(ParseAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntryContentMissing) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  service->Set(kTizenServiceNameKey, name.release());
  service->SetInteger(kTizenServiceIdKey, 1410);
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_FALSE(ParseAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntryMultipleContents) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());

  std::unique_ptr<ListValue> content_list(new ListValue());
  for (auto& start_file : {"content1.js", "content2.js"}) {
    std::unique_ptr<DictionaryValue> content(new DictionaryValue());
    content->SetString(kTizenServiceContentSrcKey, start_file);
    content->SetString(kNamespaceKey, kTizenNamespacePrefix);
    content_list->Append(content.release());
  }
  service->Set(kTizenServiceContentKey, content_list.release());

  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  service->Set(kTizenServiceNameKey, name.release());
  service->SetInteger(kTizenServiceIdKey, 1410);
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_FALSE(ParseAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntrySingleIcon) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  std::unique_ptr<DictionaryValue> icon(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  icon->SetString(kNamespaceKey, kTizenNamespacePrefix);
  icon->SetString(kTizenServiceIconSrcKey, "my_icon.png");
  service->Set(kTizenServiceIconKey, icon.release());
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].icon(), "my_icon.png");
}

TEST_F(ServiceHandlerTest, SingleServiceEntrySingleIconNotInTizen) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  std::unique_ptr<DictionaryValue> icon(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  icon->SetString(kTizenServiceIconSrcKey, "my_icon.png");
  service->Set(kTizenServiceIconKey, icon.release());
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].icon(), "");
}

TEST_F(ServiceHandlerTest, SingleServiceEntryMultipleIcon) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");

  std::unique_ptr<ListValue> icon_list(new ListValue());
  for (auto& icon_value : {"icon1.png", "icon2.png"}) {
    std::unique_ptr<DictionaryValue> icon(new DictionaryValue());
    icon->SetString(kNamespaceKey, kTizenNamespacePrefix);
    icon->SetString(kTizenServiceIconSrcKey, icon_value);
    icon_list->Append(icon.release());
  }

  service->Set(kTizenServiceIconKey, icon_list.release());
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_FALSE(ParseAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntrySingleDescription) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  std::unique_ptr<DictionaryValue> description(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  description->SetString(kNamespaceKey, kTizenNamespacePrefix);
  description->SetString(kXmlTextKey, "my description");
  service->Set(kTizenServiceDescriptionKey, description.release());
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].description(), "my description");
}

TEST_F(ServiceHandlerTest, SingleServiceEntrySingleDescriptionNotInTizen) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  std::unique_ptr<DictionaryValue> description(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  description->SetString(kXmlTextKey, "my description");
  service->Set(kTizenServiceDescriptionKey, description.release());
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].description(), "");
}

TEST_F(ServiceHandlerTest, SingleServiceEntryMultipleDescription) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");

  std::unique_ptr<ListValue> description_list(new ListValue());
  for (auto& desc_value : {"1", "2"}) {
    std::unique_ptr<DictionaryValue> description(new DictionaryValue());
    description->SetString(kNamespaceKey, kTizenNamespacePrefix);
    description->SetString(kXmlTextKey, desc_value);
    description_list->Append(description.release());
  }
  service->Set(kTizenServiceDescriptionKey, description_list.release());

  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_FALSE(ParseAppManifest());
}

TEST_F(ServiceHandlerTest, SingleServiceEntrySingleMetadata) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  std::unique_ptr<DictionaryValue> metadata(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  metadata->SetString(kNamespaceKey, kTizenNamespacePrefix);
  metadata->SetString(kTizenServiceMetadataKeyKey, "unique key");
  metadata->SetString(kTizenServiceMetadataValueKey, "some value");
  service->Set(kTizenServiceMetadataKey, metadata.release());
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].metadata_set().size(), 1);
  ASSERT_EQ(service_list->services[0].metadata_set()[0].first, "unique key");
  ASSERT_EQ(service_list->services[0].metadata_set()[0].second, "some value");
}

TEST_F(ServiceHandlerTest, SingleServiceEntrySingleMetadataNotInTizen) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  std::unique_ptr<DictionaryValue> metadata(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  metadata->SetString(kTizenServiceMetadataKeyKey, "unique key");
  metadata->SetString(kTizenServiceMetadataValueKey, "some value");
  service->Set(kTizenServiceMetadataKey, metadata.release());
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].metadata_set().size(), 0);
}

TEST_F(ServiceHandlerTest, SingleServiceEntryMultipleMetadata) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");

  std::unique_ptr<ListValue> metadata_list(new ListValue());
  std::unique_ptr<DictionaryValue> metadata(new DictionaryValue());
  metadata->SetString(kNamespaceKey, kTizenNamespacePrefix);
  metadata->SetString(kTizenServiceMetadataKeyKey, "unique key");
  metadata->SetString(kTizenServiceMetadataValueKey, "some value");
  metadata_list->Append(metadata.release());
  std::unique_ptr<DictionaryValue> metadata2(new DictionaryValue());
  metadata2->SetString(kNamespaceKey, kTizenNamespacePrefix);
  metadata2->SetString(kTizenServiceMetadataKeyKey, "unique key 2");
  metadata_list->Append(metadata2.release());
  service->Set(kTizenServiceMetadataKey, metadata_list.release());

  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].metadata_set().size(), 2);
  ASSERT_EQ(service_list->services[0].metadata_set()[0].first, "unique key");
  ASSERT_EQ(service_list->services[0].metadata_set()[0].second, "some value");
  ASSERT_EQ(service_list->services[0].metadata_set()[1].first, "unique key 2");
  ASSERT_EQ(service_list->services[0].metadata_set()[1].second, "");
}

TEST_F(ServiceHandlerTest, SingleServiceEntryMultipleMetadataMixedNamespaces) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");

  std::unique_ptr<ListValue> metadata_list(new ListValue());

  std::unique_ptr<DictionaryValue> metadata(new DictionaryValue());
  metadata->SetString(kNamespaceKey, kTizenNamespacePrefix);
  metadata->SetString(kTizenServiceMetadataKeyKey, "unique key");
  metadata->SetString(kTizenServiceMetadataValueKey, "some value");
  metadata_list->Append(metadata.release());

  std::unique_ptr<DictionaryValue> metadata2(new DictionaryValue());
  metadata2->SetString(kNamespaceKey, kTizenNamespacePrefix);
  metadata2->SetString(kTizenServiceMetadataKeyKey, "unique key 2");
  metadata_list->Append(metadata2.release());

  std::unique_ptr<DictionaryValue> metadata3(new DictionaryValue());
  metadata3->SetString(kTizenServiceMetadataKeyKey, "unique key 3");
  metadata_list->Append(metadata3.release());

  service->Set(kTizenServiceMetadataKey, metadata_list.release());

  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].metadata_set().size(), 2);
  ASSERT_EQ(service_list->services[0].metadata_set()[0].first, "unique key");
  ASSERT_EQ(service_list->services[0].metadata_set()[0].second, "some value");
  ASSERT_EQ(service_list->services[0].metadata_set()[1].first, "unique key 2");
  ASSERT_EQ(service_list->services[0].metadata_set()[1].second, "");
}

TEST_F(ServiceHandlerTest, SingleServiceEntrySingleCategory) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  std::unique_ptr<DictionaryValue> category(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  category->SetString(kNamespaceKey, kTizenNamespacePrefix);
  category->SetString(kTizenServiceCategoryNameKey, "category name");
  service->Set(kTizenServiceCategoryKey, category.release());
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].categories().size(), 1);
  ASSERT_EQ(service_list->services[0].categories()[0], "category name");
}

TEST_F(ServiceHandlerTest, SingleServiceEntrySingleCategoryNotInTizen) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  std::unique_ptr<DictionaryValue> category(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");
  category->SetString(kTizenServiceCategoryNameKey, "category name");
  service->Set(kTizenServiceCategoryKey, category.release());
  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].categories().size(), 0);
}

TEST_F(ServiceHandlerTest, SingleServiceEntryMultipleCategory) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  std::unique_ptr<DictionaryValue> name(new DictionaryValue());
  content->SetString(kTizenServiceContentSrcKey, "service.js");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kNamespaceKey, kTizenNamespacePrefix);
  name->SetString(kXmlTextKey, "name");

  std::unique_ptr<ListValue> catogory_list(new ListValue());
  for (auto& name : {"category name 1", "category name 2", "category name 3"}) {
    std::unique_ptr<DictionaryValue> category(new DictionaryValue());
    category->SetString(kNamespaceKey, kTizenNamespacePrefix);
    category->SetString(kTizenServiceCategoryNameKey, name);
    catogory_list->Append(category.release());
  }
  service->Set(kTizenServiceCategoryKey, catogory_list.release());

  service->Set(kTizenServiceContentKey, content.release());
  service->Set(kTizenServiceNameKey, name.release());
  service->SetString(kTizenServiceIdKey, "correct001.appId");
  service->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kServiceKey, service.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(kTizenServiceKey));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[0].categories().size(), 3);
  ASSERT_EQ(service_list->services[0].categories()[0], "category name 1");
  ASSERT_EQ(service_list->services[0].categories()[1], "category name 2");
  ASSERT_EQ(service_list->services[0].categories()[2], "category name 3");
}

TEST_F(ServiceHandlerTest, MultipleServiceEntry) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<ListValue> list(new ListValue());
  std::unique_ptr<DictionaryValue> service1(new DictionaryValue());
  std::unique_ptr<DictionaryValue> service2(new DictionaryValue());
  service1->SetString(kTizenServiceIdKey, "correct004.appId");
  service1->SetString(kNamespaceKey, kTizenNamespacePrefix);
  service2->SetString(kTizenServiceIdKey, "correct005.appId");
  service2->SetString(kNamespaceKey, kTizenNamespacePrefix);
  for (auto& service : {service1.get(), service2.get()}) {
    std::unique_ptr<DictionaryValue> content(new DictionaryValue());
    std::unique_ptr<DictionaryValue> name(new DictionaryValue());
    content->SetString(kTizenServiceContentSrcKey, "service.js");
    content->SetString(kNamespaceKey, kTizenNamespacePrefix);
    name->SetString(kNamespaceKey, kTizenNamespacePrefix);
    name->SetString(kXmlTextKey, "name");
    service->Set(kTizenServiceContentKey, content.release());
    service->Set(kTizenServiceNameKey, name.release());
  }
  list->Append(service1.release());
  list->Append(service2.release());
  widget->Set(kServiceKey, list.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!!GetManifestData(kTizenServiceKey));
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::dynamic_pointer_cast<const wgt::parse::ServiceList>(
          GetManifestData(wgt::parse::ServiceInfo::Key()));
  ASSERT_TRUE(!!service_list);
  ASSERT_EQ(service_list->services.size(), 2);
  ASSERT_EQ(service_list->services[0].id(), "correct004.appId");
  ASSERT_EQ(service_list->services[1].id(), "correct005.appId");
  ASSERT_EQ(service_list->services[0].content(), "service.js");
  ASSERT_EQ(service_list->services[0].names().size(), 1);
  ASSERT_EQ(service_list->services[0].names()[0].second, "name");
  ASSERT_EQ(service_list->services[1].content(), "service.js");
  ASSERT_EQ(service_list->services[1].names().size(), 1);
  ASSERT_EQ(service_list->services[1].names()[0].second, "name");
}

}  // namespace parser
