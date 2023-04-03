#pragma once
#include <string>
#include <cstdio>

enum _token_type_t {
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_ARITHMETIC_OPERATOR,
  TOKEN_BOOLEAN_OPERATOR,
  TOKEN_ASSIGNMENT_OPERATOR,

  TOKEN_LEFT_BRACKET,
  TOKEN_RIGHT_BRACKET,

  TOKEN_SEPARATOR,
  TOKEN_NEWLINE,
};

typedef enum _token_type_t token_type_t;

struct _token_t {
  int line, col;

  token_type_t type;
  std::string  value;
};

typedef struct _token_t token_t;

void print_token(token_t token);