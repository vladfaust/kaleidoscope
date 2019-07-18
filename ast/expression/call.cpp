#pragma once

#include "./base.cpp"
#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace AST {
namespace Expression {
class Call : public Base {
  string _callee;
  vector<unique_ptr<Base>> _args;

public:
  Call(const string &Callee, vector<unique_ptr<Base>> Args)
      : _callee(Callee), _args(move(Args)) {}

  string callee() const { return _callee; }
  int args_size() const { return _args.size(); }
  Base *arg(int index) const { return _args[index].get(); }
};
} // namespace Expression
} // namespace AST
