#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

#include "./ast/expression/binary.cpp"
#include "./ast/expression/call.cpp"
#include "./ast/expression/number.cpp"
#include "./ast/expression/variable.cpp"
#include "./ast/function.cpp"

using namespace std;
mutex m;

class Codegen {
  llvm::LLVMContext *_context;
  llvm::Module *_module;
  llvm::IRBuilder<> *_builder;

  map<string, llvm::Value *> _named_values;

public:
  static llvm::Value *log_error(const char *string) {
    fprintf(stderr, "Codegen error: %s\n", string);
    return nullptr;
  }

  Codegen(llvm::LLVMContext *context, llvm::Module *module, llvm::IRBuilder<> *builder)
      : _context(context), _module(module), _builder(builder) {}

  // Generate base expression IR.
  // It automatically determines which derived type the expression node is.
  llvm::Value *gen(AST::Expression::Base *node) {
    if (auto binary = dynamic_cast<AST::Expression::Binary *>(node))
      return gen(binary);
    else if (auto call = dynamic_cast<AST::Expression::Call *>(node))
      return gen(call);
    else if (auto number = dynamic_cast<AST::Expression::Number *>(node))
      return gen(number);
    else if (auto variable = dynamic_cast<AST::Expression::Variable *>(node))
      return gen(variable);
    else
      return (llvm::Value *)log_error("Unrecognized expression node type");
  }

  // Generate number literal IR.
  llvm::Value *gen(AST::Expression::Number *node) {
    return llvm::ConstantFP::get(*_context, llvm::APFloat(node->value()));
  }

  // Generate variable IR.
  llvm::Value *gen(AST::Expression::Variable *node) {
    // Lookup the value by name in the current scope
    llvm::Value *value = _named_values[node->name()];

    if (!value)
      return (llvm::Value *)log_error("Unknown variable name");

    return value;
  }

  // Generate call IR.
  llvm::Value *gen(AST::Expression::Call *node) {
    llvm::Function *callee = _module->getFunction(node->callee());

    if (!callee)
      return (llvm::Value *)log_error("Unknown function referenced");

    if (callee->arg_size() != node->args_size())
      return (llvm::Value *)log_error("Incorrect number of arguments");

    vector<llvm::Value *> args_vector;

    for (unsigned i = 0, args_size = node->args_size(); i != args_size; ++i) {
      args_vector.push_back(gen(node->arg(i)));

      if (!args_vector.back())
        return nullptr;
    }

    return _builder->CreateCall(callee, args_vector, "calltmp");
  }

  // Generate binary expression IR.
  llvm::Value *gen(AST::Expression::Binary *node) {
    llvm::Value *lhs = gen(node->lhs());
    llvm::Value *rhs = gen(node->rhs());
    llvm::Value *result;

    if (!lhs || !rhs)
      return nullptr;

    switch (node->op()) {
    case '+':
      return _builder->CreateFAdd(lhs, rhs);
    case '-':
      return _builder->CreateFSub(lhs, rhs);
    case '*':
      return _builder->CreateFMul(lhs, rhs);
    case '<':
      result = _builder->CreateFCmpULT(lhs, rhs);

      // Convert bool (0 or 1) to double (0.0 or 1.0)
      return _builder->CreateUIToFP(result, llvm::Type::getDoubleTy(*_context));
    default:
      return (llvm::Value *)log_error("Incorrect number of arguments");
    }
  }

  // Generate function IR.
  llvm::Value *gen(AST::Function *node) {
    llvm::Function *function = (llvm::Function *)gen(node->prototype());

    if (!function)
      return nullptr;

    // The entry block
    llvm::BasicBlock *basic_block = llvm::BasicBlock::Create(*_context, "entry", function);
    _builder->SetInsertPoint(basic_block);

    _named_values.clear();
    for (auto &arg : function->args())
      _named_values[arg.getName()] = &arg;

    if (llvm::Value *return_value = gen(node->body())) {
      _builder->CreateRet(return_value);
      llvm::verifyFunction(*function);
      return function;
    }

    function->eraseFromParent();
    return nullptr;
  }

  // Generate function prototype IR. In the LLVM world it really means "llvm::Function without body".
  llvm::Value *gen(AST::Prototype *node) {
    llvm::Function *function = _module->getFunction(node->name());

    if (function)
      return log_error("Function cannot be redefined");

    vector<llvm::Type *> doubles(node->args().size(), llvm::Type::getDoubleTy(*_context));

    llvm::FunctionType *prototype = llvm::FunctionType::get(llvm::Type::getDoubleTy(*_context), doubles, false);
    function = llvm::Function::Create(prototype, llvm::Function::ExternalLinkage, node->name(), _module);

    unsigned idx = 0;

    for (auto &arg : function->args())
      arg.setName(node->args()[idx++]);

    return function;
  }
};
