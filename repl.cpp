#include <cstdio>
#include <iostream>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "./codegen.cpp"
#include "./jit.cpp"
#include "./parser.cpp"

using namespace std;

class REPL {
  Parser *_parser;

  unique_ptr<JIT> _jit;
  unique_ptr<llvm::LLVMContext> _context;
  unique_ptr<llvm::Module> _module;
  unique_ptr<Codegen> _codegen;
  unique_ptr<llvm::IRBuilder<>> _builder;
  unique_ptr<llvm::legacy::FunctionPassManager> _fpm;

public:
  REPL(Parser *parser) : _parser(parser) {
    _context = std::make_unique<llvm::LLVMContext>();
    _builder = std::make_unique<llvm::IRBuilder<>>(*_context);
    _module = std::make_unique<llvm::Module>("REPL", *_context);

    // Initialize the Function Pass Manager. Currently using the legacy one
    _fpm = std::make_unique<llvm::legacy::FunctionPassManager>(_module.get());
    _fpm->add(llvm::createInstructionCombiningPass());
    _fpm->add(llvm::createReassociatePass());
    _fpm->add(llvm::createGVNPass());
    _fpm->add(llvm::createCFGSimplificationPass());
    _fpm->doInitialization();

    _codegen = std::make_unique<Codegen>(
        Codegen(_context.get(), _module.get(), _builder.get(), _fpm.get()));
  }

  void loop() {
    int await = 1;

    while (true) {
      if (await) {
        fprintf(stdout, "ready> ");
        _parser->lexer()->consume_token();
        await = 0;
      }

      switch (_parser->lexer()->current_token()) {
      case Lexer::Token::Eof:
        exit(1); // EOF!
      case Lexer::Token::Newline:
        await = 1;
        _parser->lexer()->reset();
        break;
      case ';':
        _parser->lexer()->consume_token(); // Consume top-level semicolon
        break;
      case Lexer::Token::Def:
        handle_def();
        break;
      case Lexer::Token::Extern:
        handle_extern();
        break;
      default:
        handle_top_level_expression();
        break;
      }
    }
  }

private:
  void handle_def() {
    if (auto node = _parser->parse_function_definition()) {
      if (auto *ir = _codegen->gen(node.get())) {
        fprintf(stdout, "Read function definition:");
        ir->print(llvm::outs());
        fprintf(stdout, "\n");
      }
    } else {
      // That's a error, skip one token
      _parser->lexer()->consume_token();
    }
  }

  void handle_extern() {
    if (auto node = _parser->parse_extern()) {
      if (auto *ir = _codegen->gen(node.get())) {
        fprintf(stdout, "Read extern:");
        ir->print(llvm::outs());
        fprintf(stdout, "\n");
      }
    } else {
      // That's a error, skip one token
      _parser->lexer()->consume_token();
    }
  }

  void handle_top_level_expression() {
    if (auto node = _parser->parse_top_level_expression()) {
      if (auto *ir = _codegen->gen(node.get())) {
        fprintf(stdout, "Read top-level expression:");
        ir->print(llvm::outs());
        fprintf(stdout, "\n");
      }
    } else {
      // That's a error, skip one token
      _parser->lexer()->consume_token();
    }
  }

  static void new_module() {}
};
