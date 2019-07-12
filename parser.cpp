#pragma once

#include "./ast/expression/binary.cpp"
#include "./ast/expression/call.cpp"
#include "./ast/expression/number.cpp"
#include "./ast/expression/variable.cpp"
#include "./ast/function.cpp"
#include "./ast/prototype.cpp"
#include "lexer.cpp"

#include <map>
#include <memory>

using namespace std;

class Parser {
  Lexer *_lexer;
  static map<char, int> _binop_precedence;

public:
  // Static getters
  //

  static map<char, int> *binop_precedence() { return &_binop_precedence; };

  // Instance getters
  //

  Lexer *lexer() { return _lexer; };

  // Constructors
  //

  Parser(Lexer *lexer) : _lexer(lexer) {}

  static unique_ptr<AST::Expression::Base> log_error(const char *string) {
    fprintf(stderr, "LogError: %s\n", string);
    return nullptr;
  }

  static unique_ptr<AST::Prototype> log_prototype_error(const char *string) {
    log_error(string);
    return nullptr;
  }

  // If it returns -1, then it's an invalid binary operator.
  int get_current_token_binop_precedence() {
    if (!isascii(_lexer->current_token()))
      return -1;

    int token_precedence = this->_binop_precedence[_lexer->current_token()];
    if (token_precedence <= 0)
      return -1;

    return token_precedence;
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
    int ct = _lexer->current_token();

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

  unique_ptr<AST::Expression::Base>
  parse_binop_rhs(int precedence, unique_ptr<AST::Expression::Base> lhs) {
    while (1) {
      int token_precedence = get_current_token_binop_precedence();

      if (token_precedence < precedence)
        return lhs;

      int binop = _lexer->current_token();
      _lexer->consume_token(); // Consume the binary operator

      auto rhs = parse_primary_expression();
      if (!rhs)
        return nullptr;

      int next_precedence = get_current_token_binop_precedence();
      if (token_precedence < next_precedence) {
        rhs = parse_binop_rhs(token_precedence + 1, move(rhs));

        if (!rhs)
          return nullptr;
      }

      lhs = make_unique<AST::Expression::Binary>(binop, move(lhs), move(rhs));
    }
  }

  unique_ptr<AST::Prototype> parse_function_prototype() {
    if (_lexer->current_token() != Lexer::Token::Identifier)
      return this->log_prototype_error("Expected function name in prototype");

    string function_name = _lexer->identifier_string();
    _lexer->consume_token();

    if (_lexer->current_token() != '(')
      return this->log_prototype_error("Expected '(' in prototype");

    vector<string> argument_names;
    while (_lexer->consume_token() == Lexer::Token::Identifier)
      argument_names.push_back(_lexer->identifier_string());

    if (_lexer->current_token() != ')')
      return this->log_prototype_error("Expected ')' in prototype");

    _lexer->consume_token(); // Consume ')'

    return make_unique<AST::Prototype>(function_name, move(argument_names));
  }

  unique_ptr<AST::Function> parse_function_definition() {
    _lexer->consume_token(); // Consume 'def'

    auto proto = parse_function_prototype();
    if (!proto)
      return nullptr;

    if (auto expression = parse_expression())
      return make_unique<AST::Function>(move(proto), move(expression));

    return nullptr;
  }

  unique_ptr<AST::Prototype> parse_extern() {
    _lexer->consume_token(); // Consume 'extern'
    return parse_function_prototype();
  }

  unique_ptr<AST::Function> parse_top_level_expression() {
    if (auto expression = parse_expression()) {
      auto proto = make_unique<AST::Prototype>("", vector<string>());
      return make_unique<AST::Function>(move(proto), move(expression));
    }

    return nullptr;
  }
};

map<char, int> Parser::_binop_precedence;
