#include "lexer.hpp"
#include "token.hpp"

lexer_t create_lexer(std::string input) {
  lexer_t res;

  if (input == "") {
    res.status = LEXER_ERROR;
    res.error = (lang_error_t){0, 0, LEX_NO_INPUT, "No input given to lexer."};

    return res;
  }

  res.line = res.col = 0;
  res.status = LEXER_READY;
  res.error = (lang_error_t){0, 0, NO_ERR, "No Error"};
  res.input = input;
  res.tokens = {};

  return res;
}

bool __is_digit(char c) {
  std::string s = "0123456789";

  return s.find(c) != std::string::npos;
}

bool __is_upper(char c) {
  std::string s = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  return s.find(c) != std::string::npos;
}

bool __is_lower(char c) {
  std::string s = "abcdefghijklmnopqrstuvwxyz";

  return s.find(c) != std::string::npos;
}

bool __is_letter(char c) { return (__is_lower(c) || __is_upper(c)); }

bool __is_identifier_start(char c) { return (__is_letter(c) || c == '_'); }

bool __is_identifier_tail(char c) {
  return (__is_identifier_start(c) || __is_digit(c));
}

bool __is_arith_op(char c) {
  std::string s = "+-*/%";

  return s.find(c) != std::string::npos;
}

void lexer_run(lexer_t *lexer) {
  while (lexer->input.size() > 0) {
    char c = lexer->input[0];

    if (c == ' ') {
      lexer->col++;
      lexer->input = lexer->input.substr(1);

      continue;
    } else if (c == '\n') {
      lexer->line++;
      lexer->col = 0;
      lexer->input = lexer->input.substr(1);

      continue;
    } else if (c == '(') {
      lexer->tokens.push_back(
          (token_t){lexer->line, lexer->col, TOKEN_LEFT_BRACKET, "("});
    } else if (c == ')') {
      lexer->tokens.push_back(
          (token_t){lexer->line, lexer->col, TOKEN_RIGHT_BRACKET, ")"});
    } else if (c == ',') {
      lexer->tokens.push_back(
          (token_t){lexer->line, lexer->col, TOKEN_SEPARATOR, ","});
    } else if (c == '=') {
      lexer->col++;
      lexer->input = lexer->input.substr(1);
      c = lexer->input[0];

      if(c == '=') {
        lexer->tokens.push_back((token_t) { lexer->line, lexer->col, TOKEN_BOOLEAN_OPERATOR, "==" });
        lexer->col++;
        lexer->input = lexer->input.substr(1);
        continue;
      }

      lexer->tokens.push_back(
          (token_t){lexer->line, lexer->col, TOKEN_ASSIGNMENT_OPERATOR, "="});

      continue;
    } else {
      if (c == '"') {
        lexer->input = lexer->input.substr(1);
        lexer->col++;
        c = lexer->input[0];

        std::string str = "";

        while (c != '"' && lexer->input.size() > 0) {
          str += c;
          lexer->input = lexer->input.substr(1);
          lexer->col++;
          c = lexer->input[0];
        }

        if (lexer->input.size() == 0) {
          lexer->status = LEXER_ERROR;
          lexer->error = (lang_error_t){
              lexer->line, lexer->col, LEX_INVALID_TOK, "Unterminated string."};

          return;
        }

        lexer->tokens.push_back(
            (token_t){lexer->line, lexer->col, TOKEN_STRING, str});
      } else if (__is_digit(c)) {
        std::string num = "";

        while ((__is_digit(c) || c == '.') && lexer->input.size() > 0) {
          num += c;
          lexer->input = lexer->input.substr(1);
          lexer->col++;
          c = lexer->input[0];
        }

        lexer->tokens.push_back(
            (token_t){lexer->line, lexer->col, TOKEN_NUMBER, num});

        continue;
      } else if (__is_identifier_start(c)) {
        std::string identifier = "";

        while (__is_identifier_tail(c) && lexer->input.size() > 0) {
          identifier += c;
          lexer->input = lexer->input.substr(1);
          lexer->col++;
          c = lexer->input[0];
        }

        lexer->tokens.push_back(
            (token_t){lexer->line, lexer->col, TOKEN_IDENTIFIER, identifier});

        continue;
      } else if (__is_arith_op(c)) {
        std::string op = "";

        while (__is_arith_op(c) && lexer->input.size() > 0) {
          op += c;
          lexer->input = lexer->input.substr(1);
          lexer->col++;
          c = lexer->input[0];
        }

        lexer->tokens.push_back(
            (token_t){lexer->line, lexer->col, TOKEN_ARITHMETIC_OPERATOR, op});

        continue;
      } else {
        lexer->status = LEXER_ERROR;
        lexer->error = (lang_error_t){0, lexer->col, LEX_INVALID_TOK};

        lexer->error.error_message = "Invalid character '";
        lexer->error.error_message += c;
        lexer->error.error_message += "'.";

        print_error(lexer->error);
        return;
      }
    }

    lexer->col++;

    if (lexer->input.size() > 0) {
      lexer->input = lexer->input.substr(1);
    }
  }
}