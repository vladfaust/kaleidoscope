#pragma once

#include <memory>
#include <string>
#include <vector>

#include "./expression/base.cpp"
#include "./prototype.cpp"

using namespace std;

namespace AST {
class Function {
  unique_ptr<Prototype> Proto;
  unique_ptr<Expression::Base> Body;

public:
  Function(unique_ptr<Prototype> Proto, unique_ptr<Expression::Base> Body)
      : Proto(move(Proto)), Body(move(Body)) {}
};
} // namespace AST
