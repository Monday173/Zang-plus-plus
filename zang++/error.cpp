#include "error.hpp"

void print_error(lang_error_t error) {
  printf("(Ln %d, Col %d) ERROR : %s (Err %d)\n", error.line + 1, error.col + 1,
         error.error_message.c_str(), error.error_code);
}