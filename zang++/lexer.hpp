#pragma once
#include "token.hpp"
#include "error.hpp"

#include <cstdio>
#include <vector>

enum _lexer_status_t {
  LEXER_READY,
  LEXER_RUNNING,
  LEXER_ERROR
};

typedef enum _lexer_status_t lexer_status_t;

struct _lexer_t {
  int line, col;

  lexer_status_t       status;
  lang_error_t         error;
  std::string          input;
  std::vector<token_t> tokens;
};

typedef struct _lexer_t lexer_t;

void    lexer_run(lexer_t* lexer);
lexer_t create_lexer(std::string input);