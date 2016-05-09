// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/path.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "manifest_parser/manifest.h"
#include "manifest_parser/manifest_handler.h"
#include "manifest_parser/manifest_parser_impl.h"
#include "wgt_manifest_handlers/application_manifest_constants.h"
#include "wgt_manifest_handlers/content_handler.h"

namespace bf = boost::filesystem;

namespace {
const char kWidgetKey[] = "widget";
const char kNamespaceKey[] = "@namespace";
const char kTizenContentTagKey[] = "content";
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


std::unique_ptr<parser::ManifestHandlerRegistry> GetRegistryForTest() {
  std::unique_ptr<parser::ManifestHandlerRegistry> registry;
  registry.reset(new parser::ManifestHandlerRegistry());
  registry->RegisterManifestHandler(new wgt::parse::ContentHandler());
  return registry;
}

}  // namespace

namespace parser {

/**
 * @brief The ContentHandlerTest class
 *
 * Those tests are using ManifestParserImpl so they are testing all behaviour
 * beside file existance verification which is done in WidgetConfigParser.
 * It works as if all valid content positions exist in widget package.
 */
class ContentHandlerTest : public testing::Test {
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

TEST_F(ContentHandlerTest, NoContentEntry) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  ASSERT_TRUE(!GetManifestData(kTizenContentKey));
}

TEST_F(ContentHandlerTest, SingleContentEntry) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  content->SetString(kTizenContentSrcKey, "my_index.html");
  widget->Set(kTizenContentTagKey, content.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_TRUE(!!content_info);
  ASSERT_EQ(content_info->is_tizen_content(), false);
  ASSERT_EQ(content_info->src(), "my_index.html");
}

TEST_F(ContentHandlerTest, SingleContentEntrySrcEmpty) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  content->SetString(kTizenContentSrcKey, "");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kTizenContentTagKey, content.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check empty
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_FALSE(!!content_info);
}

TEST_F(ContentHandlerTest, SingleTizenContentEntry) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  content->SetString(kTizenContentSrcKey, "http://www.tizen.app/my_index.html");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kTizenContentTagKey, content.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_TRUE(!!content_info);
  ASSERT_EQ(content_info->is_tizen_content(), true);
  ASSERT_EQ(content_info->src(), "http://www.tizen.app/my_index.html");
}

TEST_F(ContentHandlerTest, SingleTizenContentEntryNotURL) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content(new DictionaryValue());
  content->SetString(kTizenContentSrcKey, "relative_NOT_url");
  content->SetString(kNamespaceKey, kTizenNamespacePrefix);
  widget->Set(kTizenContentTagKey, content.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check error
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_TRUE(!!content_info);
  ASSERT_EQ(content_info->is_tizen_content(), true);
  ASSERT_EQ(content_info->src(), "relative_NOT_url");
}

TEST_F(ContentHandlerTest, MultipleContentEntryW3CTakeFirst) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<ListValue> list(new ListValue());
  std::unique_ptr<DictionaryValue> content1(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content2(new DictionaryValue());
  content1->SetString(kTizenContentSrcKey, "w3c_1_index.html");
  content2->SetString(kTizenContentSrcKey, "w3c_2_index.html");
  list->Append(content1.release());
  list->Append(content2.release());
  widget->Set(kTizenContentTagKey, list.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_TRUE(!!content_info);
  ASSERT_EQ(content_info->is_tizen_content(), false);
  ASSERT_EQ(content_info->src(), "w3c_1_index.html");
}

TEST_F(ContentHandlerTest, MultipleContentEntryTizenTakeFirst) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<ListValue> list(new ListValue());
  std::unique_ptr<DictionaryValue> content1(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content2(new DictionaryValue());
  content1->SetString(kTizenContentSrcKey,
                      "http://www.tizen.app/tizen_1_index.html");
  content1->SetString(kNamespaceKey, kTizenNamespacePrefix);
  content2->SetString(kTizenContentSrcKey,
                      "http://www.tizen.app/tizen_2_index.html");
  content2->SetString(kNamespaceKey, kTizenNamespacePrefix);
  list->Append(content1.release());
  list->Append(content2.release());
  widget->Set(kTizenContentTagKey, list.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_TRUE(!!content_info);
  ASSERT_EQ(content_info->is_tizen_content(), true);
  ASSERT_EQ(content_info->src(), "http://www.tizen.app/tizen_1_index.html");
}

TEST_F(ContentHandlerTest, MultipleContentEntryTizenPrioritizedAsFirst) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<ListValue> list(new ListValue());
  std::unique_ptr<DictionaryValue> content1(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content2(new DictionaryValue());
  content1->SetString(kTizenContentSrcKey,
                      "http://www.tizen.app/tizen_index.html");
  content1->SetString(kNamespaceKey, kTizenNamespacePrefix);
  content2->SetString(kTizenContentSrcKey, "my_index.html");
  list->Append(content1.release());
  list->Append(content2.release());
  widget->Set(kTizenContentTagKey, list.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_TRUE(!!content_info);
  ASSERT_EQ(content_info->is_tizen_content(), true);
  ASSERT_EQ(content_info->src(), "http://www.tizen.app/tizen_index.html");
}

TEST_F(ContentHandlerTest, MultipleContentEntryTizenPrioritizedAsSecond) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<ListValue> list(new ListValue());
  std::unique_ptr<DictionaryValue> content1(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content2(new DictionaryValue());
  content1->SetString(kTizenContentSrcKey, "my_index.html");
  content2->SetString(kTizenContentSrcKey,
                      "http://www.tizen.app/tizen_index.html");
  content2->SetString(kNamespaceKey, kTizenNamespacePrefix);
  list->Append(content1.release());
  list->Append(content2.release());
  widget->Set(kTizenContentTagKey, list.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_TRUE(!!content_info);
  ASSERT_EQ(content_info->is_tizen_content(), true);
  ASSERT_EQ(content_info->src(), "http://www.tizen.app/tizen_index.html");
}

TEST_F(ContentHandlerTest, MultipleContentEntryW3CIgnoreIfNotFirst) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<ListValue> list(new ListValue());
  std::unique_ptr<DictionaryValue> content1(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content2(new DictionaryValue());
  content2->SetString(kTizenContentSrcKey, "w3c_2_index.html");
  list->Append(content1.release());
  list->Append(content2.release());
  widget->Set(kTizenContentTagKey, list.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check empty
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_FALSE(!!content_info);
}

TEST_F(ContentHandlerTest, MultipleContentEntryW3CIgnoreIfFirstEmpty) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<ListValue> list(new ListValue());
  std::unique_ptr<DictionaryValue> content1(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content2(new DictionaryValue());
  content1->SetString(kTizenContentSrcKey, "");
  content2->SetString(kTizenContentSrcKey, "w3c_2_index.html");
  list->Append(content1.release());
  list->Append(content2.release());
  widget->Set(kTizenContentTagKey, list.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check empty
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_FALSE(!!content_info);
}

TEST_F(ContentHandlerTest, MultipleContentBackToW3CWhenTizenEntryBroken) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<ListValue> list(new ListValue());
  std::unique_ptr<DictionaryValue> content1(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content2(new DictionaryValue());
  content1->SetString(kNamespaceKey, kTizenNamespacePrefix);
  content2->SetString(kTizenContentSrcKey, "w3c_1_index.html");
  list->Append(content1.release());
  list->Append(content2.release());
  widget->Set(kTizenContentTagKey, list.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check w3c when tizen:content broken
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_TRUE(!!content_info);
  ASSERT_EQ(content_info->is_tizen_content(), false);
  ASSERT_EQ(content_info->src(), "w3c_1_index.html");
}

TEST_F(ContentHandlerTest, MultipleMoreContentEntry) {
  // Set test values
  std::unique_ptr<DictionaryValue> value(new DictionaryValue());
  std::unique_ptr<DictionaryValue> widget(new DictionaryValue());
  std::unique_ptr<ListValue> list(new ListValue());
  std::unique_ptr<DictionaryValue> content1(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content2(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content3(new DictionaryValue());
  std::unique_ptr<DictionaryValue> content4(new DictionaryValue());
  content1->SetString(kTizenContentSrcKey, "w3c_1_index.html");
  content2->SetString(kTizenContentSrcKey, "w3c_2_index.html");
  content3->SetString(kTizenContentSrcKey,
                      "http://www.tizen.app/tizen_3_index.html");
  content3->SetString(kNamespaceKey, kTizenNamespacePrefix);
  content4->SetString(kTizenContentSrcKey,
                      "http://www.tizen.app/tizen_4_index.html");
  content4->SetString(kNamespaceKey, kTizenNamespacePrefix);
  list->Append(content1.release());
  list->Append(content2.release());
  list->Append(content3.release());
  list->Append(content4.release());
  widget->Set(kTizenContentTagKey, list.release());
  value->Set(kWidgetKey, widget.release());
  std::shared_ptr<Manifest> manifest(new Manifest(std::move(value)));
  SetManifest(manifest);
  // Check correctness
  ASSERT_TRUE(ParseAppManifest());
  ASSERT_TRUE(ValidateAppManifest());
  std::shared_ptr<const wgt::parse::ContentInfo> content_info =
      std::dynamic_pointer_cast<const wgt::parse::ContentInfo>(
          GetManifestData(kTizenContentKey));
  ASSERT_TRUE(!!content_info);
  ASSERT_EQ(content_info->is_tizen_content(), true);
  ASSERT_EQ(content_info->src(), "http://www.tizen.app/tizen_3_index.html");
}

}  // namespace parser
