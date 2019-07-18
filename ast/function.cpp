#pragma once

#include <memory>
#include <string>
#include <vector>

#include "./expression/base.cpp"
#include "./prototype.cpp"

using namespace std;

namespace AST {
class Function {
  unique_ptr<Prototype> _prototype;
  unique_ptr<Expression::Base> _body;

public:
  Prototype *prototype() const { return _prototype.get(); }
  Expression::Base *body() const { return _body.get(); }

  Function(unique_ptr<Prototype> Proto, unique_ptr<Expression::Base> Body)
      : _prototype(move(Proto)), _body(move(Body)) {}
};
} // namespace AST
