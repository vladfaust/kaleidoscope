#pragma once

#include "./base.cpp"
#include <string>

using namespace std;

namespace AST {
namespace Expression {
class Variable : public Base {
  string Name;

public:
  Variable(const string &Name) : Name(Name) {}
};
} // namespace Expression
} // namespace AST
