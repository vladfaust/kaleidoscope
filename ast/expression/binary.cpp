#pragma once

#include "base.cpp"
#include <memory>

using namespace std;

namespace AST {
namespace Expression {
class Binary : public Base {
  char Op;
  unique_ptr<Base> LHS, RHS;

public:
  Binary(char Op, unique_ptr<Base> LHS, unique_ptr<Base> RHS)
      : Op(Op), LHS(move(LHS)), RHS(move(RHS)) {}
};
} // namespace Expression
} // namespace AST
