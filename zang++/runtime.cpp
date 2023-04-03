#include "runtime.hpp"
#include "ast.hpp"

void print_value(runtime_value_t val) {
  printf("(%d, %s)\n", val.type, val.value.c_str());
}

runtime_t create_runtime(parser_t parser) {
  runtime_t res;

  res.ast = parser.ast;

  return res;
}

double __get_num(runtime_value_t val) { return std::stod(val.value); }

runtime_value_t run_node(runtime_t *runtime, ast_node_t node, std::map<std::string, runtime_value_t>* locals) {
  runtime_value_t left, right;
  double res;

  if (node.type == NODE_PROGRAM) {
    runtime_value_t res;
    for (auto &nodec : node.children) {
      res = run_node(runtime, nodec, locals);
    }

    return res;
  }

  else if(node.type == NODE_RETURN) {
    return run_node(runtime, node.children[0], locals);
  }

  else if(node.type == NODE_IDENTIFIER) {
    if(runtime->globals.count(node.value)) {
      return runtime->globals[node.value];
    } else if(locals->count(node.value)) {
      return (*locals)[node.value];
    }
  }

  else if(node.type == NODE_VAR_DECL) {
    if(!locals->count(node.value)) {
      (*locals)[node.value] = run_node(runtime, node.children[0], locals);
    }
  }

  else if(node.type == NODE_VAR_ASSIGN) {
    if(locals->count(node.value)) {
      (*locals)[node.value] = run_node(runtime, node.children[0], locals);
    } else if(runtime->globals.count(node.value)) {
      runtime->globals[node.value] = run_node(runtime, node.children[0], locals);
    }
  }

  else if (node.type == NODE_FUNCTION_CALL) {
    if (node.value == std::string("writeln")) {
      for (auto &node : node.children[0].children) {
        runtime_value_t res = run_node(runtime, node, locals);

        printf("%s ", res.value.c_str());
      }

      printf("\n");
    }

    if (runtime->functions.count(node.value)) {
      runtime_function_t func = runtime->functions[node.value];

      std::map<std::string, runtime_value_t> temp;
      ast_node_t arg_node = func.argument_node;
      for(int i = 0; i < arg_node.children.size(); ++i) {
        temp[arg_node.children[i].value] = run_node(runtime, node.children[0].children[i], locals); 
      }

      return run_node(runtime, func.program_node, &temp);
    }

    return (runtime_value_t){RUNTIME_NULL, "null"};
  }

  else if (node.type == NODE_FUNCTION_DEF) {
    if (runtime->functions.count(node.value) == 0) {
      runtime->functions[node.value].argument_node = node.children[0];
      runtime->functions[node.value].program_node = node.children[1];

      // print_ast(node, 0);
    }
  }

  else if (node.type == NODE_STRING) {
    return (runtime_value_t){RUNTIME_STRING, node.value};
  }

  else if (node.type == NODE_NUMBER) {
    return (runtime_value_t){RUNTIME_NUMBER, node.value};
  }

  else if (node.type == NODE_ARITH) {
    switch (node.value[0]) {
    case '+':
      left = run_node(runtime, node.children[0], locals);
      right = run_node(runtime, node.children[1], locals);

      if (left.type == RUNTIME_NULL || right.type == RUNTIME_NULL) {
        return (runtime_value_t){RUNTIME_NULL, "null"};
      }

      res = __get_num(left) + __get_num(right);
      return (runtime_value_t){RUNTIME_NUMBER, std::to_string(res)};

    case '-':
      left = run_node(runtime, node.children[0], locals);
      right = run_node(runtime, node.children[1], locals);

      if (left.type == RUNTIME_NULL || right.type == RUNTIME_NULL) {
        return (runtime_value_t){RUNTIME_NULL, "null"};
      }

      res = __get_num(left) - __get_num(right);
      return (runtime_value_t){RUNTIME_NUMBER, std::to_string(res)};

    case '*':
      left = run_node(runtime, node.children[0], locals);
      right = run_node(runtime, node.children[1], locals);

      if (left.type == RUNTIME_NULL || right.type == RUNTIME_NULL) {
        return (runtime_value_t){RUNTIME_NULL, "null"};
      }

      res = __get_num(left) * __get_num(right);
      return (runtime_value_t){RUNTIME_NUMBER, std::to_string(res)};

    case '/':
      left = run_node(runtime, node.children[0], locals);
      right = run_node(runtime, node.children[1], locals);

      if (left.type == RUNTIME_NULL || right.type == RUNTIME_NULL) {
        return (runtime_value_t){RUNTIME_NULL, "null"};
      }

      res = __get_num(left) / __get_num(right);
      return (runtime_value_t){RUNTIME_NUMBER, std::to_string(res)};
    }
  }

  return (runtime_value_t){RUNTIME_NULL, "null"};
}

runtime_value_t run(runtime_t *runtime) {
  return run_node(runtime, runtime->ast, &runtime->globals);
}