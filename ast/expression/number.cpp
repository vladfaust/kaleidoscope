#pragma once

#include "./base.cpp"

namespace AST {
namespace Expression {
class Number : public Base {
  double _value;

public:
  Number(double Val) : _value(Val) {}
  double value() const { return _value; }
};
} // namespace Expression
} // namespace AST
