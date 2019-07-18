#pragma once

#include <string>
#include <vector>

using namespace std;

namespace AST {
class Prototype {
  string _name;
  vector<string> _args;

public:
  const string &name() const { return _name; };
  vector<string> args() { return _args; };

  Prototype(const string &name, vector<string> args)
      : _name(name), _args(move(args)) {}
};
} // namespace AST
