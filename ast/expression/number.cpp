#pragma once

#include "./base.cpp"

namespace AST {
namespace Expression {
class Number : public Base {
  double Val;

public:
  Number(double Val) : Val(Val) {}
};
} // namespace Expression
} // namespace AST
