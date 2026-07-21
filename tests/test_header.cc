#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

#include "include/mdict.h"
#include "include/mdict_extern.h"

namespace {

std::filesystem::path write_header_only_file(const std::string &name,
                                             const std::string &xml) {
  const auto path = std::filesystem::temp_directory_path() / name;
  std::ofstream output(path, std::ios::binary | std::ios::trunc);
  const uint32_t byte_count = static_cast<uint32_t>((xml.size() + 1) * 2);
  const std::array<unsigned char, 4> size = {
      static_cast<unsigned char>(byte_count >> 24),
      static_cast<unsigned char>(byte_count >> 16),
      static_cast<unsigned char>(byte_count >> 8),
      static_cast<unsigned char>(byte_count)};
  output.write(reinterpret_cast<const char *>(size.data()), size.size());
  for (const unsigned char character : xml) {
    output.put(static_cast<char>(character));
    output.put('\0');
  }
  output.put('\0');
  output.put('\0');
  const std::array<char, 4> checksum = {};
  output.write(checksum.data(), checksum.size());
  return path;
}

}  // namespace

TEST(Header, ReadsRealMdxMetadata) {
  mdict::Mdict dictionary("../testdict/testdict.mdx");
  dictionary.init_header();

  EXPECT_EQ(dictionary.header_root_element(), "Dictionary");
  EXPECT_FALSE(dictionary.header_attributes().empty());
  EXPECT_NE(dictionary.header_attributes().find("Encoding"),
            dictionary.header_attributes().end());
}

TEST(Header, HeaderOnlyCInterfaceDoesNotReadIndexes) {
  const auto path = write_header_only_file(
      "mdict-cpp-header-only.mdd",
      "<Library_Data Encoding=\"\" Title=\"Resources\" Custom=\"value\"/>");

  mdict_header_t *header = mdict_header_open(path.string().c_str());
  ASSERT_NE(header, nullptr);
  EXPECT_STREQ(mdict_header_root_element(header), "Library_Data");
  EXPECT_EQ(mdict_header_attribute_count(header), 3);

  bool saw_empty_encoding = false;
  for (uint64_t index = 0; index < mdict_header_attribute_count(header); ++index) {
    const char *key = nullptr;
    const char *value = nullptr;
    ASSERT_EQ(mdict_header_attribute_at(header, index, &key, &value), 0);
    if (std::string(key) == "Encoding") {
      saw_empty_encoding = true;
      EXPECT_STREQ(value, "");
    }
  }
  EXPECT_TRUE(saw_empty_encoding);
  EXPECT_EQ(mdict_header_close(header), 0);
  std::filesystem::remove(path);
}

TEST(Header, CInterfaceRejectsInvalidArguments) {
  EXPECT_EQ(mdict_header_open(nullptr), nullptr);
  EXPECT_EQ(mdict_header_open("/path/that/does/not/exist.mdx"), nullptr);
  EXPECT_EQ(mdict_header_root_element(nullptr), nullptr);
  EXPECT_EQ(mdict_header_attribute_count(nullptr), 0);

  const char *key = nullptr;
  const char *value = nullptr;
  EXPECT_NE(mdict_header_attribute_at(nullptr, 0, &key, &value), 0);
  EXPECT_EQ(mdict_header_close(nullptr), 0);
}

TEST(Header, DictionaryCInterfaceContainsInitializationErrors) {
  EXPECT_EQ(mdict_init(nullptr), nullptr);
  EXPECT_EQ(mdict_init("/path/that/does/not/exist.mdx"), nullptr);

  const auto path = write_header_only_file(
      "mdict-cpp-invalid-index.mdx", "<Dictionary Encoding=\"UTF-8\"/>");
  EXPECT_EQ(mdict_init(path.string().c_str()), nullptr);
  std::filesystem::remove(path);
}
