#pragma once

#include "./base.cpp"
#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace AST {
namespace Expression {
class Call : public Base {
  string Callee;
  vector<unique_ptr<Base>> Args;

public:
  Call(const string &Callee, vector<unique_ptr<Base>> Args)
      : Callee(Callee), Args(move(Args)) {}
};
} // namespace Expression
} // namespace AST
