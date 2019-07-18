#pragma once

#include "./base.cpp"
#include <string>

using namespace std;

namespace AST {
namespace Expression {
class Variable : public Base {
  string _name;

public:
  Variable(const string &Name) : _name(Name) {}
  string name() const { return _name; }
};
} // namespace Expression
} // namespace AST
