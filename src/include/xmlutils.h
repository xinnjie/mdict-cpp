/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#pragma once

#include <cassert>
#include <map>
#include <string>

/**
 * Extracts the root element name from an MDX/MDD header.
 *
 * Version 1.x/2.x files conventionally use Dictionary for MDX and
 * Library_Data for MDD. The format is reverse engineered rather than governed
 * by a public schema, so callers should retain the returned name instead of
 * inferring it from a fixed attribute set.
 *
 * Format reference:
 * https://github.com/zhansliu/writemdict/blob/f0240b30cabd2f0470d3ee1a0641fc7f8c38dcf5/fileformat.md#header-section
 */
inline std::string parse_xml_root_element(const std::string& dicxml) {
  size_t pos = 0;
  while (pos < dicxml.size() &&
         (dicxml[pos] == ' ' || dicxml[pos] == '\n' || dicxml[pos] == '\r' ||
          dicxml[pos] == '\t' || dicxml[pos] == '\0')) {
    ++pos;
  }
  if (pos >= dicxml.size() || dicxml[pos] != '<') {
    return {};
  }

  const size_t start = ++pos;
  while (pos < dicxml.size() && dicxml[pos] != ' ' && dicxml[pos] != '\n' &&
         dicxml[pos] != '\r' && dicxml[pos] != '\t' && dicxml[pos] != '/' &&
         dicxml[pos] != '>') {
    ++pos;
  }
  return pos == start ? std::string() : dicxml.substr(start, pos - start);
}

/**
 * Parses XML header information from a string and extracts attributes into a
 * map This function handles a simplified XML format where the input is expected
 * to be a single self-closing tag with attributes in the format key="value"
 *
 * @param dicxml The XML string to parse
 * @param tagm A map to store the parsed key-value pairs
 *
 * Example input: <Dictionary GeneratedByEngineVersion="2.0"
 * RequiredEngineVersion="2.0" Encrypted="0"/>
 */
inline void parse_xml_header(const std::string& dicxml,
                             std::map<std::string, std::string>& tagm) {
  // Check if the string is too short or doesn't end with "/>"
  int length = dicxml.length();
  if (length <= 2) {
    return;
  }
  // trim trailing spaces, newline \n and null character\0
  int i = length - 1;
  while (i >= 0 && (dicxml[i] == ' ' || dicxml[i] == '\n' ||
                    dicxml[i] == '\0' || dicxml[i] == '\r')) {
    --i;
  }
  if (i < 0) {
    return;
  }

  std::string trimedxml = dicxml.substr(0, i + 1);
  length = trimedxml.length();
  if (length <= 2) {
    return;
  }
  trimedxml[length] = '\0';

  // check if the string ends with "/>"
  const char last2 = trimedxml[length - 2];
  const char last1 = trimedxml[length - 1];
  if (last2 != '/' || last1 != '>') {
    return;
  }

  // Skip leading spaces and '<'
  size_t pos = 0;
  while (pos < trimedxml.length() &&
         (trimedxml[pos] == ' ' || trimedxml[pos] == '<')) {
    ++pos;
  }

  // Parse attributes
  while (pos < trimedxml.length() - 3) {
    // Skip spaces between attributes
    while (pos < trimedxml.length() && trimedxml[pos] == ' ') {
      ++pos;
    }

    // Get key
    std::string key;
    while (pos < trimedxml.length() && trimedxml[pos] != '=' &&
           trimedxml[pos] != ' ') {
      key += trimedxml[pos++];
    }

    // Skip spaces and '='
    while (pos < trimedxml.length() &&
           (trimedxml[pos] == ' ' || trimedxml[pos] == '=')) {
      ++pos;
    }

    if (pos < trimedxml.length() && trimedxml[pos] == '\'') {
      ++pos;
      while (pos < trimedxml.length() && trimedxml[pos] != '\'') {
        ++pos;
      }
      ++pos;
    }

    // Get value
    if (pos < trimedxml.length() && (trimedxml[pos] == '"')) {
      ++pos;  // Skip opening quote
      std::string value;
      while (pos < trimedxml.length()) {
        // Check for end of tag before adding to value
        if (pos + 1 < trimedxml.length() && trimedxml[pos] == '/' &&
            trimedxml[pos + 1] == '>') {
          break;
        }
        if (trimedxml[pos] == '\\') {
          ++pos;
        }
        if (trimedxml[pos] == '"') {
          ++pos;  // Skip closing quote
          break;
        }
        value += trimedxml[pos++];
      }

      if (!key.empty()) {
        tagm[key] = value;
      }
    }

    // Check if we've reached the end of the tag
    while (pos < trimedxml.length() && trimedxml[pos] == ' ') {
      ++pos;
    }
    if (pos + 1 < trimedxml.length() && trimedxml[pos] == '/' &&
        trimedxml[pos + 1] == '>') {
      break;
    }
  }
}
