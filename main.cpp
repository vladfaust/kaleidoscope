#include <cstdio>

#include "./lexer.cpp"
#include "./parser.cpp"

// AST
#include "./ast/expression/number.cpp"
#include "./ast/expression/variable.cpp"

int main() {
  Lexer lexer;
  Parser parser(&lexer);

  while (true) {
    auto exp = parser.parse_number_expression();
  }
};
