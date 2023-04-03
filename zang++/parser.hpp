#include "ast.hpp"
#include "lexer.hpp"

enum _parser_status_t {
  PARSER_READY,
  PARSER_RUNNING,
  PARSER_ERROR
};

typedef enum _parser_status_t parser_status_t;

struct _parser_t {
  int line, col;
  std::vector<token_t>    tokens;
  ast_node_t              ast;
  parser_status_t         status;
  lang_error_t            error;
};

typedef struct _parser_t parser_t;

ast_node_t parse_expression(parser_t* parser);
void       parse_tokens(parser_t* parser);
parser_t   create_parser(lexer_t lexer);