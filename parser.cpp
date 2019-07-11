#pragma once

#include "./ast/expression/call.cpp"
#include "./ast/expression/number.cpp"
#include "./ast/expression/variable.cpp"
#include "./ast/prototype.cpp"
#include "lexer.cpp"
#include <memory>

using namespace std;

class Parser {
  Lexer *_lexer;

public:
  Parser(Lexer *lexer) : _lexer(lexer) {}

  static unique_ptr<AST::Expression::Base> log_error(const char *string) {
    fprintf(stderr, "LogError: %s\n", string);
    return nullptr;
  }

  static unique_ptr<AST::Prototype> log_prototype_error(const char *string) {
    log_error(string);
    return nullptr;
  }

  unique_ptr<AST::Expression::Base> parse_number_expression() {
    auto result = make_unique<AST::Expression::Number>(_lexer->number_value());
    _lexer->consume_token(); // Consume the number
    return move(result);
  }

  unique_ptr<AST::Expression::Base> parse_parenthesis_expression() {
    _lexer->consume_token(); // Consume "("

    auto e = parse_expression();

    if (!e)
      return nullptr;

    if (_lexer->current_token() != ')')
      return this->log_error("expected ')'");

    _lexer->consume_token();

    return e;
  }

  // Returns either AST::Expression::Variable or AST::Expression::Call.
  unique_ptr<AST::Expression::Base> parse_identifier_expression() {
    string identifier_name = _lexer->identifier_string();

    _lexer->consume_token(); // Consume the identifier

    // If there is no "(", then that's a simple identifier, not a function call
    if (_lexer->current_token() != '(')
      return make_unique<AST::Expression::Variable>(identifier_name);

    // Otherwise it's a function call
    _lexer->consume_token(); // Consume '('
    vector<unique_ptr<AST::Expression::Base>> args;
    if (_lexer->current_token() != ')') {
      while (1) {
        if (auto arg = parse_expression())
          args.push_back(move(arg));
        else
          return nullptr;

        if (_lexer->current_token() == ')')
          break;

        if (_lexer->current_token() != ',')
          return this->log_error("Expected ')' or ',' in the arguments list");

        _lexer->consume_token(); // Consume the token
      }
    }

    _lexer->consume_token();

    return make_unique<AST::Expression::Call>(identifier_name, move(args));
  }

  unique_ptr<AST::Expression::Base> parse_primary_expression() {
    switch (_lexer->current_token()) {
    default:
      return this->log_error("Unexpected token, expecting expression");
    case Lexer::Token::Identifier:
      return this->parse_identifier_expression();
    case Lexer::Token::Number:
      return this->parse_number_expression();
    case '(':
      return this->parse_parenthesis_expression();
    }
  }

  unique_ptr<AST::Expression::Base> parse_expression() {
    auto lhs = parse_primary_expression();

    if (!lhs)
      return nullptr;

    return parse_binop_rhs(0, move(lhs));
  }
};
