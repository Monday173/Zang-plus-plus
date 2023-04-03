#include "token.hpp"

void print_token(token_t token) {
  printf("([%d:%d], %d, %s)\n", token.line, token.col, token.type, token.value.c_str());  
}