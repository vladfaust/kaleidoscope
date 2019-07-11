#pragma once

#include <cstdio>
#include <string>

using namespace std;

class Lexer {
  string _identifier_string;
  double _number_value;
  int _current_token;

public:
  enum Token {
    Eof = -1,

    // commands
    Def = -2,
    Extern = -3,

    // primary
    Identifier = -4,
    Number = -5,
  };

  double number_value() { return _number_value; };
  string identifier_string() { return _identifier_string; };
  int current_token() { return _current_token; };

  int consume_token() { return _current_token = get_token(); };

private:
  int get_token() {
    static int __last_char = ' ';

    while (isspace(__last_char))
      __last_char = getchar();

    if (isalpha(__last_char)) {
      _identifier_string = __last_char;

      while (isalnum((__last_char = getchar())))
        _identifier_string += __last_char;

      if (_identifier_string == "def")
        return Token::Def;
      if (_identifier_string == "extern")
        return Token::Extern;

      return Token::Identifier;
    }

    if (isdigit(__last_char) || __last_char == '.') {
      string NumString;

      do {
        NumString += __last_char;
        __last_char = getchar();
      } while (isdigit(__last_char) || __last_char == '.');

      _number_value = strtod(NumString.c_str(), 0);
      return Token::Number;
    }

    if (__last_char == '#') {
      do
        __last_char = getchar();
      while (__last_char != EOF && __last_char != '\n' && __last_char != '\r');

      if (__last_char != EOF)
        return get_token();
    }

    if (__last_char == EOF)
      return Token::Eof;

    int ThisChar = __last_char;
    __last_char = getchar(); // Move cursor to the next char

    return ThisChar;
  };
};
