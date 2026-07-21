/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#pragma once

#ifdef __cplusplus
#include <string>

// C++ function declarations
/**
 * Detect the MIME type of a file
 * @param filename The path to the file to detect
 * @return A string containing the MIME type
 */
std::string mime_detect(const std::string &filename);

extern "C" {
#endif

#include "mdict_simple_key.h"

/**
 * Encoding type for locate function
 */
typedef enum {
  MDICT_ENCODING_BASE64 = 0,  // Default encoding, returns base64 string
  MDICT_ENCODING_HEX = 1      // Returns raw hex string
} mdict_encoding_t;

/** Opaque, immutable snapshot of an MDX/MDD XML header. */
typedef struct mdict_header mdict_header_t;

/**
 * Read only the XML header from an MDX/MDD file.
 *
 * This does not read key or record indexes. Returns NULL for invalid input or
 * when the file cannot be opened. Strings returned by the accessors are
 * borrowed and remain valid until mdict_header_close is called.
 */
mdict_header_t *mdict_header_open(const char *path);

/** Return the XML root element, or NULL when header is NULL. */
const char *mdict_header_root_element(const mdict_header_t *header);

/** Return the number of XML attributes, or zero when header is NULL. */
uint64_t mdict_header_attribute_count(const mdict_header_t *header);

/**
 * Return one attribute by index.
 *
 * Returns 0 on success and non-zero for invalid arguments or an out-of-range
 * index. key and value receive borrowed pointers owned by header.
 */
int mdict_header_attribute_at(const mdict_header_t *header, uint64_t index,
                              const char **key, const char **value);

/** Destroy a header snapshot. A NULL header is accepted. */
int mdict_header_close(mdict_header_t *header);

/**
 * Initialize a dictionary from a file
 * @param dictionary_path Path to the dictionary file (.mdx or .mdd)
 * @return A pointer to the initialized dictionary object, or NULL if
 * initialization fails
 */
void *mdict_init(const char *dictionary_path);

/**
 * Look up a word in the dictionary and get its definition
 * @param dict Dictionary object pointer returned by mdict_init
 * @param word The word to look up
 * @param result Pointer to store the definition result (memory will be
 * allocated)
 */
void mdict_lookup(void *dict, const char *word, char **result);

/**
 * Locate a word in the dictionary without getting its definition
 * @param dict Dictionary object pointer returned by mdict_init
 * @param word The word to locate
 * @param result Pointer to store the location result (memory will be allocated)
 * @param encoding The encoding type for the result (MDICT_ENCODING_BASE64 or
 * MDICT_ENCODING_HEX)
 */
void mdict_locate(void *dict, const char *word, char **result,
                  mdict_encoding_t encoding);

/**
 * Parse a word's definition from its record start position
 * @param dict Dictionary object pointer returned by mdict_init
 * @param word The word to parse
 * @param record_start The starting position of the record
 * @param result Pointer to store the parsed definition (memory will be
 * allocated)
 */
void mdict_parse_definition(void *dict, const char *word,
                            unsigned long record_start, char **result);

/**
 * Get a list of all keys in the dictionary
 * @param dict Dictionary object pointer returned by mdict_init
 * @param len Pointer to store the number of keys
 * @return Array of simple_key_item structures containing the keys
 */
simple_key_item **mdict_keylist(void *dict, uint64_t *len);

/**
 * Free the memory allocated for a key list
 * @param key_items The key list to free
 * @param len Number of items in the key list
 * @return 0 on success, non-zero on failure
 */
int free_simple_key_list(simple_key_item **key_items, uint64_t len);

/**
 * Get the type of dictionary file
 * @param dict Dictionary object pointer returned by mdict_init
 * @return 0 for .mdx files, 1 for .mdd files
 */
int mdict_filetype(void *dict);

/**
 * Get word suggestions based on input
 * @param dict Dictionary object pointer returned by mdict_init
 * @param word The input word to get suggestions for
 * @param suggested_words Array to store suggested words
 * @param length Maximum number of suggestions to return
 */
void mdict_suggest(void *dict, char *word, char **suggested_words, int length);

/**
 * Get word stems based on input
 * @param dict Dictionary object pointer returned by mdict_init
 * @param word The input word to get stems for
 * @param suggested_words Array to store stem words
 * @param length Maximum number of stems to return
 */
void mdict_stem(void *dict, char *word, char **suggested_words, int length);

/**
 * Destroy a dictionary object and free its resources
 * @param dict Dictionary object pointer returned by mdict_init
 * @return 0 on success, non-zero on failure
 */
int mdict_destory(void *dict);

// C wrapper for mime_detect
const char* c_mime_detect(const char* filename);
  
//-------------------------

#ifdef __cplusplus
}
#endif
