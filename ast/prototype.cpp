#pragma once

#include <string>
#include <vector>

using namespace std;

namespace AST {
class Prototype {
  string _name;
  vector<string> _args;

public:
  Prototype(const string &name, vector<string> args)
      : _name(name), _args(move(args)) {}

  const string &get_name() const { return _name; }
};
} // namespace AST
