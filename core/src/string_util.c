#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// String Builder function

static const size_t string_builder_min_size = 32;

struct StringBuilder {
  char *string;
  size_t alloced;
  size_t len;
};

StringBuilder *create_string_builder() {
  StringBuilder *string_builder;

  string_builder = calloc(1, sizeof(*string_builder));
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
      realloc(string_builder->string, string_builder->alloced);
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
                               int number_of_spaces, size_t len) {
  // TODO: low priority, but find a better way than malloc_or_die'ing
  char *spaces_string = malloc_or_die(sizeof(char) * (number_of_spaces + 1));
  spaces_string[0] = '\0';
  sprintf(spaces_string, "%*s", number_of_spaces, "");
  string_builder_add_string(string_builder, spaces_string, len);
  free(spaces_string);
}

void string_builder_add_int(StringBuilder *string_builder, int64_t n,
                            size_t len) {
  char integer_string[200] = "";
  sprintf(integer_string, "%ld", n);
  string_builder_add_string(string_builder, integer_string, len);
}

void string_builder_add_uint(StringBuilder *string_builder, uint64_t n,
                             size_t len) {
  char integer_string[200] = "";
  sprintf(integer_string, "%lu", n);
  string_builder_add_string(string_builder, integer_string, len);
}

void string_builder_add_double(StringBuilder *string_builder, double val,
                               size_t len) {
  // TODO: low priority, but find a better way than malloc_or_die'ing
  char float_string[200] = "";
  sprintf(float_string, "%0.2f", val);
  string_builder_add_string(string_builder, float_string, len);
}

void string_builder_add_char(StringBuilder *string_builder, char c,
                             size_t len) {
  char char_as_string[2];
  char_as_string[0] = c;
  char_as_string[1] = '\0';
  string_builder_add_string(string_builder, char_as_string, len);
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