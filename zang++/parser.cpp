#include "parser.hpp"
#include "ast.hpp"
#include "token.hpp"

parser_t create_parser(lexer_t lexer) {
  parser_t res;

  if (lexer.tokens.size() <= 0) {
    res.status = PARSER_ERROR;
    res.error = (lang_error_t){0, 0, PARSER_NO_TOKS, "No tokens specified."};
    return res;
  }

  res.col = res.line = 0;
  res.status = PARSER_READY;
  res.error = (lang_error_t){0, 0, NO_ERR, "No Error"};
  res.tokens = lexer.tokens;
  res.ast = (ast_node_t){0};

  return res;
}

token_t next(parser_t *parser) {
  if (parser->tokens.size() == 0)
    return {};

  token_t res = parser->tokens[0];
  parser->tokens.erase(parser->tokens.begin());

  parser->line = res.line;
  parser->col  = res.col;

  return res;
}

token_t at(parser_t *parser) {
  if (parser->tokens.size() == 0)
    return {};

  parser->line = parser->tokens[0].line;
  parser->col  = parser->tokens[0].col;

  return parser->tokens[0];
}

ast_node_t parse_args(parser_t *parser) {
  next(parser);

  ast_node_t value = (ast_node_t){parser->line, parser->col, NODE_ARGUMENT, ""};

  while (at(parser).type != TOKEN_RIGHT_BRACKET) {
    value.children.push_back(parse_expression(parser));
    if (at(parser).type == TOKEN_SEPARATOR) {
      next(parser);
    }
  }

  next(parser);
  return value;
}

ast_node_t parse_function(parser_t *parser) {
  token_t n = next(parser);

  if (n.type != TOKEN_IDENTIFIER) {
    parser->status = PARSER_ERROR;
    parser->error =
        (lang_error_t){parser->line, parser->col, PARSER_NO_PARSE,
                       "Expected identifier in function definition."};

    return (ast_node_t){parser->line, parser->col, NODE_ERR};
  }

  std::string name = n.value;
  ast_node_t args = parse_args(parser);

  ast_node_t body =
      (ast_node_t){parser->line, parser->col, NODE_PROGRAM, "", {}};

  token_t t = at(parser);
  int opened = 1;
  int closed = 0;

  while (opened != closed) {
    body.children.push_back(parse_expression(parser));
    t = at(parser);

    if(t.value == std::string("then") || t.value == std::string("do")) {
      opened++;
    } else if(t.value == std::string("end") || t.value == std::string("elif")) {
      closed++;
    }
  }

  next(parser);
  return (ast_node_t){
      parser->line, parser->col, NODE_FUNCTION_DEF, name, {args, body}};
}

ast_node_t parse_call(parser_t *parser, token_t tok) {
  ast_node_t args = parse_args(parser);
  return (ast_node_t){
      parser->line, parser->col, NODE_FUNCTION_CALL, tok.value, {args}};
}

ast_node_t parse_var_decl(parser_t* parser) {
  token_t name_tok = next(parser);

  if(at(parser).type == TOKEN_ASSIGNMENT_OPERATOR) {
    next(parser);
    ast_node_t value = parse_expression(parser);

    return (ast_node_t){ parser->line, parser->col, NODE_VAR_DECL, name_tok.value, { value } };
  } else {
    parser->status = PARSER_ERROR;
    parser->error =
        (lang_error_t){parser->line, parser->col, PARSER_NO_PARSE,
                       "Expected identifier in variable definition."};

    return (ast_node_t){parser->line, parser->col, NODE_ERR};
  }
}

ast_node_t parse_var_assign(parser_t* parser, token_t name_tok) { 
  if(at(parser).type == TOKEN_ASSIGNMENT_OPERATOR) {
    next(parser);
    ast_node_t value = parse_expression(parser);

    return (ast_node_t){ parser->line, parser->col, NODE_VAR_ASSIGN, name_tok.value, { value } };
  } else {
    parser->status = PARSER_ERROR;
    parser->error =
        (lang_error_t){parser->line, parser->col, PARSER_NO_PARSE,
                       "Expected identifier in variable definition."};

    return (ast_node_t){parser->line, parser->col, NODE_ERR};
  }
}

ast_node_t parse_if(parser_t* parser) {
  ast_node_t if_node = (ast_node_t){ parser->line, parser->col, NODE_IF, "if", {} };

  ast_node_t condition = parse_expression(parser);

  token_t n = at(parser);
  if(n.value != std::string("then")) {
    parser->status = PARSER_ERROR;
    parser->error =
        (lang_error_t){parser->line, parser->col, PARSER_NO_PARSE,
                       "Expected `then` in `if` statement"};

    return (ast_node_t){parser->line, parser->col, NODE_ERR};
  }

  if_node.children.push_back(condition);

  next(parser);
  n = at(parser);

  ast_node_t block_node = (ast_node_t){parser->line, parser->col, NODE_PROGRAM, "if_blk", {}};
  while(n.value != std::string("end")) {
    block_node.children.push_back(parse_expression(parser));
    n = at(parser);
  }
  next(parser);

  if_node.children.push_back(block_node);

  return if_node;
}

ast_node_t parse_atom(parser_t *parser) {
  token_t tok = next(parser);
  token_t n;

  token_type_t type = tok.type;

  ast_node_t res;

  switch (type) {
  case TOKEN_IDENTIFIER:
    if(tok.value == std::string("let")) {
      return parse_var_decl(parser);
    }

    if (tok.value == std::string("fn")) {
      return parse_function(parser);
    }

    if(tok.value == std::string("return")) {
      return (ast_node_t){ tok.line, tok.col, NODE_RETURN, "", { parse_expression(parser) } };
    }

    if(tok.value == std::string("if")) {
      return parse_if(parser);
    }

    n = at(parser);
    if (n.type == TOKEN_LEFT_BRACKET) {
      return parse_call(parser, tok);
    }

    if (n.type == TOKEN_ASSIGNMENT_OPERATOR) {
      return parse_var_assign(parser, tok);
    }

    return (ast_node_t){
        parser->line, parser->col, NODE_IDENTIFIER, tok.value, {}};

  case TOKEN_NUMBER:
    return (ast_node_t){parser->line, parser->col, NODE_NUMBER, tok.value, {}};

  case TOKEN_STRING:
    return (ast_node_t){parser->line, parser->col, NODE_STRING, tok.value, {}};

  case TOKEN_SEPARATOR:
    return (ast_node_t){parser->line, parser->col, NODE_ERR, tok.value, {}};

  default:
    for (auto &token : parser->tokens) {
      print_token(token);
    }
    res.type = NODE_ERR;
    parser->status = PARSER_ERROR;
    parser->error = (lang_error_t){parser->line, parser->col, PARSER_NO_PARSE,
                                   "Could not parse expression."};

    return res;
  }
}

ast_node_t parse_muldiv(parser_t *parser) {
  ast_node_t left = parse_atom(parser);

  token_t tok = at(parser);

  while (tok.type == TOKEN_ARITHMETIC_OPERATOR &&
         (tok.value[0] == '*' || tok.value[0] == '/')) {
    tok = next(parser);
    ast_node_t right = parse_atom(parser);

    ast_node_t cur = {tok.line, tok.col, NODE_ARITH, tok.value};
    cur.children = {};

    cur.children.push_back(left);
    cur.children.push_back(right);

    left = cur;
    tok = at(parser);
  }

  return left;
}

ast_node_t parse_addsub(parser_t *parser) {
  ast_node_t left = parse_muldiv(parser);

  token_t tok = at(parser);

  while (tok.type == TOKEN_ARITHMETIC_OPERATOR &&
         (tok.value[0] == '+' || tok.value[0] == '-')) {
    tok = next(parser);
    ast_node_t right = parse_muldiv(parser);

    ast_node_t cur = {tok.line, tok.col, NODE_ARITH, tok.value};
    cur.children = {};

    cur.children.push_back(left);
    cur.children.push_back(right);

    left = cur;
    tok = at(parser);
  }

  return left;
}

ast_node_t parse_binary(parser_t* parser) {
  ast_node_t left = parse_addsub(parser);

  token_t tok = at(parser);
  while(tok.type == TOKEN_BOOLEAN_OPERATOR && (tok.value[0] == '=')) {
    tok = next(parser);

    ast_node_t right = parse_addsub(parser);
    ast_node_t cur = {tok.line, tok.col, NODE_BINARY, tok.value };
    cur.children = {left, right};

    left = cur;
    tok = at(parser);
  }

  return left;
}

ast_node_t parse_expression(parser_t *parser) { return parse_binary(parser); }

void parse_tokens(parser_t *parser) {
  ast_node_t node = (ast_node_t){0, 0, NODE_PROGRAM, "", {}};

  while (parser->tokens.size() > 0) {
    node.children.push_back(parse_expression(parser));
  }

  parser->ast = node;
}