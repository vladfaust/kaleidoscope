#include <cstdio>

#include "./lexer.cpp"
#include "./parser.cpp"
#include "./repl.cpp"

int main() {
  Parser::binop_precedence()->insert_or_assign('>', 10);
  Parser::binop_precedence()->insert_or_assign('+', 20);
  Parser::binop_precedence()->insert_or_assign('-', 20);
  Parser::binop_precedence()->insert_or_assign('*', 40);

  // FILE *input;
  // fopen_s(&input, "input.txt", "r");

  Lexer lexer(stdin);
  Parser parser(&lexer);
  REPL repl(&parser);

  repl.loop();

  // fclose(input);

  return 0;
};
