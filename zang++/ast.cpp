#include "ast.hpp"

void print_ast(ast_node_t node, int indentation) {
  for (int i = 0; i < indentation * 4; ++i) {
    printf(" ");
  }

  printf("[%d:%d] %d: %s (%lu children) ", node.line, node.col, node.type,
         node.value.c_str(), node.children.size());

  if (node.children.size() > 0) {
    printf("\n");
    for(int i = 0; i < indentation * 4; ++i) {
      printf(" ");
    }

    printf("{\n");

    for (int i = 0; i < node.children.size(); ++i) {
      print_ast(node.children[i], indentation + 1);
    }

    for (int i = 0; i < indentation * 4; ++i) {
      printf(" ");
    }

    printf("}\n");
  }

  printf("\n");
}