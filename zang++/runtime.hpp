#pragma once
#include "parser.hpp"

#include <map>

enum _runtime_value_type_t {
  RUNTIME_NUMBER,
  RUNTIME_STRING,
  RUNTIME_NULL,
};

typedef enum _runtime_value_type_t runtime_value_type_t;

struct _runtime_value_t {
  runtime_value_type_t type;
  std::string value;
};

typedef struct _runtime_value_t runtime_value_t;

struct _runtime_function_t {
  ast_node_t argument_node;
  ast_node_t program_node;
};

typedef struct _runtime_function_t runtime_function_t;

struct _runtime_t {
  ast_node_t ast;
  std::map<std::string, runtime_function_t> functions;
  std::map<std::string, runtime_value_t> globals;
};

typedef struct _runtime_t runtime_t;

void            print_value(runtime_value_t val);
runtime_t       create_runtime(parser_t parser);
runtime_value_t run(runtime_t* runtime);
runtime_value_t run_node(runtime_t *runtime, ast_node_t node, std::map<std::string, runtime_value_t>* locals);