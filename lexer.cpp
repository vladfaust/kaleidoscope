#pragma once

#include <cstdio>
#include <string>

using namespace std;

class Lexer {
  FILE *_input;
  string _identifier_string;
  double _number_value;
  int _current_token;
  int _last_char = ' ';

public:
  enum Token {
    Eof = -1,
    Newline = -2,

    // commands
    Def = -3,
    Extern = -4,

    // primary
    Identifier = -5,
    Number = -6,
  };

  double number_value() { return _number_value; };
  string identifier_string() { return _identifier_string; };
  int current_token() { return _current_token; };

  int consume_token() { return _current_token = get_token(); };

  Lexer(FILE *input) : _input(input) {}
  void reset() { _last_char = ' '; }

private:
  int read_char() { return fgetc(_input); }

  int get_token() {
    while (isspace(_last_char) && _last_char != '\n')
      _last_char = read_char();

    if (isalpha(_last_char)) {
      _identifier_string = _last_char;

      while (isalnum((_last_char = read_char())))
        _identifier_string += _last_char;

      if (_identifier_string == "def")
        return Token::Def;
      if (_identifier_string == "extern")
        return Token::Extern;

      return Token::Identifier;
    }

    if (isdigit(_last_char) || _last_char == '.') {
      string NumString;

      do {
        NumString += _last_char;
        _last_char = read_char();
      } while (isdigit(_last_char) || _last_char == '.');

      _number_value = strtod(NumString.c_str(), 0);
      return Token::Number;
    }

    if (_last_char == '#') {
      do
        _last_char = read_char();
      while (_last_char != EOF && _last_char != '\n' && _last_char != '\r');

      if (_last_char != EOF)
        return get_token();
    }

    if (_last_char == '\n')
      return Token::Newline;

    if (_last_char == EOF)
      return Token::Eof;

    int this_char = _last_char;
    _last_char = read_char(); // Move cursor to the next char

    return this_char;
  };
};
