#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "string_util.h"
#include "util.h"

// Misc string functions

int prefix(const char *pre, const char *str) {
  return strncmp(pre, str, strlen(pre)) == 0;
}

int is_all_whitespace_or_empty(const char *str) {
  while (*str != '\0') {
    if (!isspace((unsigned char)*str)) {
      return 0;
    }
    str++;
  }
  return 1;
}

// String utility functions

char *format_string_with_va_list(const char *format, va_list *args) {
  int size;
  va_list args_copy_for_size;
  va_copy(args_copy_for_size, *args);
  size = vsnprintf(NULL, 0, format, args_copy_for_size) + 1;
  char *string_buffer = malloc_or_die(size);
  vsnprintf(string_buffer, size, format, *args);
  return string_buffer;
}

char *get_formatted_string(const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *formatted_string = format_string_with_va_list(format, &args);
  va_end(args);
  return formatted_string;
}

char *get_substring(const char *input_string, int start_index, int end_index) {
  if (input_string == NULL) {
    log_fatal("cannot get substring of null string\n");
  }

  int input_length = strlen(input_string);

  if (start_index < 0 || end_index < start_index ||
      start_index > input_length || end_index > input_length) {
    log_fatal("cannot get substring for invalid bounds: strlen is %d, bounds "
              "are %d to %d\n",
              input_length, start_index, end_index);
  }

  int substring_length = (end_index - start_index);

  char *result = malloc_or_die(sizeof(char) * (substring_length + 1));

  strncpy(result, input_string + start_index, substring_length);
  result[substring_length] = '\0';

  return result;
}

// String Builder function

static const size_t string_builder_min_size = 32;

struct StringBuilder {
  char *string;
  size_t alloced;
  size_t len;
};

StringBuilder *create_string_builder() {
  StringBuilder *string_builder = malloc_or_die(sizeof(StringBuilder));
  string_builder->string = malloc_or_die(string_builder_min_size);
  *string_builder->string = '\0';
  string_builder->alloced = string_builder_min_size;
  string_builder->len = 0;

  return string_builder;
}

void destroy_string_builder(StringBuilder *string_builder) {
  if (string_builder == NULL) {
    return;
  }
  free(string_builder->string);
  free(string_builder);
}

static void string_builder_ensure_space(StringBuilder *string_builder,
                                        size_t add_len) {
  if (string_builder == NULL || add_len == 0) {
    return;
  }

  if (string_builder->alloced >= string_builder->len + add_len + 1) {
    return;
  }

  while (string_builder->alloced < string_builder->len + add_len + 1) {
    /* Doubling growth strategy. */
    string_builder->alloced <<= 1;
    if (string_builder->alloced == 0) {
      /* Left shift of max bits will go to 0. An unsigned type set to
       * -1 will return the maximum possible size. However, we should
       *  have run out of memory well before we need to do this. Since
       *  this is the theoretical maximum total system memory we don't
       *  have a flag saying we can't grow any more because it should
       *  be impossible to get to this point. */
      string_builder->alloced--;
    }
  }
  string_builder->string =
      realloc_or_die(string_builder->string, string_builder->alloced);
}

void string_builder_add_string(StringBuilder *string_builder, const char *str,
                               size_t len) {
  if (string_builder == NULL || str == NULL || *str == '\0') {
    return;
  }

  if (len == 0) {
    len = strlen(str);
  }

  string_builder_ensure_space(string_builder, len);
  memmove(string_builder->string + string_builder->len, str, len);
  string_builder->len += len;
  string_builder->string[string_builder->len] = '\0';
}

void string_builder_add_formatted_string(StringBuilder *string_builder,
                                         const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *formatted_string = format_string_with_va_list(format, &args);
  va_end(args);
  string_builder_add_string(string_builder, formatted_string, 0);
  free(formatted_string);
}

void string_builder_add_spaces(StringBuilder *string_builder,
                               int number_of_spaces) {
  string_builder_add_formatted_string(string_builder, "%*s", number_of_spaces,
                                      "");
}

void string_builder_add_int(StringBuilder *string_builder, int64_t n) {
  string_builder_add_formatted_string(string_builder, "%d", n);
}

void string_builder_add_uint(StringBuilder *string_builder, uint64_t n) {
  string_builder_add_formatted_string(string_builder, "%lu", n);
}

void string_builder_add_double(StringBuilder *string_builder, double val) {
  string_builder_add_formatted_string(string_builder, "%0.2f", val);
}

void string_builder_add_char(StringBuilder *string_builder, char c) {
  string_builder_add_formatted_string(string_builder, "%c", c);
}

void string_builder_truncate(StringBuilder *string_builder, size_t len) {
  if (string_builder == NULL || len >= string_builder->len) {
    return;
  }

  string_builder->len = len;
  string_builder->string[string_builder->len] = '\0';
}

void string_builder_clear(StringBuilder *string_builder) {
  if (string_builder == NULL) {
    return;
  }
  string_builder_truncate(string_builder, 0);
}

size_t string_builder_length(const StringBuilder *string_builder) {
  if (string_builder == NULL) {
    return 0;
  }
  return string_builder->len;
}

const char *string_builder_peek(const StringBuilder *string_builder) {
  if (string_builder == NULL) {
    return NULL;
  }
  return string_builder->string;
}

char *string_builder_dump(const StringBuilder *string_builder, size_t *len) {
  char *out;

  if (string_builder == NULL) {
    return NULL;
  }

  if (len != NULL) {
    *len = string_builder->len;
  }
  out = malloc_or_die(string_builder->len + 1);
  memcpy(out, string_builder->string, string_builder->len + 1);
  return out;
}

// String splitter

struct StringSplitter {
  int number_of_items;
  char **items;
};

StringSplitter *create_string_splitter() {
  StringSplitter *string_splitter = malloc_or_die(sizeof(StringSplitter));
  string_splitter->number_of_items = 0;
  string_splitter->items = NULL;
  return string_splitter;
}

void destroy_string_splitter(StringSplitter *string_splitter) {
  for (int i = 0; i < string_splitter->number_of_items; i++) {
    free(string_splitter->items[i]);
  }
  free(string_splitter->items);
  free(string_splitter);
}

int string_splitter_get_number_of_items(StringSplitter *string_splitter) {
  return string_splitter->number_of_items;
}

char *string_splitter_get_item(StringSplitter *string_splitter,
                               int item_index) {
  if (item_index >= string_splitter->number_of_items || item_index < 0) {
    log_fatal("string item out of range: %d\n", item_index);
  }
  return string_splitter->items[item_index];
}

int split_string_scan(StringSplitter *string_splitter, const char *input_string,
                      const char delimiter, bool ignore_empty, bool set_items) {
  int current_number_of_items = 0;
  char previous_char = delimiter;
  int item_start_index = 0;
  int item_end_index = 0;
  size_t string_length = strlen(input_string);
  for (size_t i = 0; i < string_length; i++) {
    char current_char = input_string[i];
    if (set_items) {
      item_end_index++;
    }
    if (current_char == delimiter) {
      if (!ignore_empty || previous_char != delimiter) {
        if (set_items) {
          string_splitter->items[current_number_of_items] =
              get_substring(input_string, item_start_index, item_end_index - 1);
        }
        current_number_of_items++;
      }
      item_start_index = item_end_index;
    }
    previous_char = current_char;
  }

  if (!ignore_empty || previous_char != delimiter) {
    if (set_items) {
      string_splitter->items[current_number_of_items] =
          get_substring(input_string, item_start_index, item_end_index);
    }
    current_number_of_items++;
  }

  return current_number_of_items;
}

StringSplitter *split_string_internal(const char *input_string,
                                      const char delimiter, bool ignore_empty) {

  int number_of_items =
      split_string_scan(NULL, input_string, delimiter, ignore_empty, false);

  StringSplitter *string_splitter = create_string_splitter();
  string_splitter->number_of_items = number_of_items;
  if (string_splitter->number_of_items > 0) {
    string_splitter->items =
        malloc_or_die(sizeof(char *) * string_splitter->number_of_items);
    split_string_scan(string_splitter, input_string, delimiter, ignore_empty,
                      true);
  }

  return string_splitter;
}

StringSplitter *split_string_ignore_empty(const char *input_string,
                                          const char delimiter) {
  return split_string_internal(input_string, delimiter, true);
}

StringSplitter *split_string(const char *input_string, const char delimiter) {
  return split_string_internal(input_string, delimiter, false);
}