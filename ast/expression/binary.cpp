#pragma once

#include "base.cpp"
#include <memory>

using namespace std;

namespace AST {
namespace Expression {
class Binary : public Base {
  char _op;
  unique_ptr<Base> _lhs, _rhs;

public:
  Binary(char Op, unique_ptr<Base> LHS, unique_ptr<Base> RHS)
      : _op(Op), _lhs(move(LHS)), _rhs(move(RHS)) {}

  char op() const { return _op; }
  Base *lhs() const { return _lhs.get(); }
  Base *rhs() const { return _rhs.get(); }
};
} // namespace Expression
} // namespace AST
