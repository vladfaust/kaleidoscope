#include "./parser.cpp"
#include <cstdio>

class REPL {
  Parser *_parser;

public:
  REPL(Parser *parser) : _parser(parser) {}

  void loop() {
    int await = 1;

    while (true) {
      if (await) {
        fprintf(stdout, "ready> ");
        _parser->lexer()->consume_token();
        await = 0;
      }

      switch (_parser->lexer()->current_token()) {
      case Lexer::Token::Eof:
        exit(1); // EOF!
      case Lexer::Token::Newline:
        await = 1;
        _parser->lexer()->reset();
        break;
      case ';':
        _parser->lexer()->consume_token(); // Consume top-level semicolon
        break;
      case Lexer::Token::Def:
        handle_def();
        break;
      case Lexer::Token::Extern:
        handle_extern();
        break;
      default:
        handle_top_level_expression();
        break;
      }
    }
  }

private:
  void handle_def() {
    if (_parser->parse_function_definition()) {
      fprintf(stdout, "Parsed a function definition\n");
    } else {
      // That's a error, skip one token
      _parser->lexer()->consume_token();
    }
  }

  void handle_extern() {
    if (_parser->parse_extern()) {
      fprintf(stdout, "Parsed an extern\n");
    } else {
      // That's a error, skip one token
      _parser->lexer()->consume_token();
    }
  }

  void handle_top_level_expression() {
    if (_parser->parse_top_level_expression()) {
      fprintf(stdout, "Parsed a top-level expression\n");
    } else {
      // That's a error, skip one token
      _parser->lexer()->consume_token();
    }
  }
};
