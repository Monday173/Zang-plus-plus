#include "runtime.hpp"
#include <cstdlib>
#include <fstream>
#include <string>

std::string read_file(std::string path) {
  std::fstream file;
  file.open(path, std::ios::in);

  std::string res = "";

  if(file.is_open()) {
    std::string res_line;
    while(std::getline(file, res_line)) {
      res += res_line;
      res += '\n';
    }
  }

  return res;
}

void usage() {
  printf("Usage: zang++ [filename]\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
  if(argc < 2) {
    usage();
  }

  std::string input = read_file(argv[1]);

  lexer_t lexer = create_lexer(input);

  if (lexer.status == LEXER_ERROR) {
    print_error(lexer.error);
    return -1;
  }

  lexer_run(&lexer);

  if (lexer.status == LEXER_ERROR) {
    printf("An error occurred when collecting tokens.\n");
    return -1;
  }

  parser_t parser = create_parser(lexer);

  if (parser.status == PARSER_ERROR) {
    print_error(parser.error);
    return -1;
  }

  parse_tokens(&parser);

  if (parser.status == PARSER_ERROR) {
    print_error(parser.error);
    return -1;
  }

  print_ast(parser.ast);

  runtime_t runtime = create_runtime(parser);
  runtime_value_t res = run(&runtime);

  //print_value(res);

  return 0;
}