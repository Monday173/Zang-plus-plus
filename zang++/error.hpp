#pragma once
#include <string>
#include <vector>

enum _error_code_t {
  NO_ERR,
  LEX_NO_INPUT,
  LEX_INVALID_TOK,
  PARSER_NO_TOKS,
  PARSER_NO_PARSE
};

typedef enum _error_code_t error_code_t;

struct _lang_error_t {
  int line, col;

  error_code_t error_code;
  std::string error_message;
};

typedef struct _lang_error_t lang_error_t;

void print_error(lang_error_t error);