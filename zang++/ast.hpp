#pragma once
#include <cstdio>
#include <string>
#include <vector>

enum _ast_node_type_t {
  NODE_ARITH,
  NODE_BINARY,
  NODE_NUMBER,
  NODE_PROGRAM,
  NODE_VAR_DECL,
  NODE_IDENTIFIER,
  NODE_FUNCTION_DEF,
  NODE_FUNCTION_CALL,
  NODE_VAR_ASSIGN,
  NODE_ARGUMENT,
  NODE_STRING,
  NODE_RETURN,
  NODE_ERR,
  NODE_IF,
};

typedef enum _ast_node_type_t ast_node_type_t;

struct _ast_node_t {
  int line, col;

  ast_node_type_t type;
  std::string value;

  std::vector<struct _ast_node_t> children;
};

typedef struct _ast_node_t ast_node_t;

void print_ast(ast_node_t node, int indentation = 0);