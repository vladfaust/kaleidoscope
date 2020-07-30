#pragma once

#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/Error.h"

#include <memory>

using namespace std;

class JIT {
  // Provides context for our running JIT’d code.
  // This includes the string pool, global mutex,
  // and error reporting facilities.
  llvm::orc::ExecutionSession _execution_session;

  // Used to add object files to the JIT.
  llvm::orc::RTDyldObjectLinkingLayer _object_layer;

  // Used to add LLVM Modules to the JIT
  // and which builds on the _objectLayer.
  llvm::orc::IRCompileLayer _ir_compile_layer;

  // Used for symbol mangling.
  llvm::DataLayout _data_layout;

  // ditto
  llvm::orc::MangleAndInterner _mangle;

  // The LLVM context.
  llvm::orc::ThreadSafeContext _context;

public:
  // Static named initializer to initialize with default target and data layout.
  static llvm::Expected<unique_ptr<JIT>> Create() {
    auto jtmb = llvm::orc::JITTargetMachineBuilder::detectHost();

    if (!jtmb)
      return jtmb.takeError();

    auto data_layout = jtmb->getDefaultDataLayoutForTarget();

    if (!data_layout)
      return data_layout.takeError();

    return std::make_unique<JIT>(move(*jtmb), move(*data_layout));
  }

  JIT(llvm::orc::JITTargetMachineBuilder jtmb, llvm::DataLayout data_layout) :
      _object_layer(
          // The _objectLayer requires a reference to the _executionSession ...
          _execution_session,
          // ... and a function object that will build a JIT memory manager for
          // each module that is added (a JIT memory manager manages memory
          // allocations, memory permissions, and registration of exception
          // handlers for JIT’d code)
          []() { return std::make_unique<llvm::SectionMemoryManager>(); }),

      // The CompileLayer needs three things: ...
      _ir_compile_layer(
          // ... (1) A reference to the _executionSession
          _execution_session,
          // ... (2) A reference to our object layer
          _object_layer,
          // ... (3) a compiler instance to use to perform the actual
          // compilation from IR to object files ...
          //
          // ... The ConcurrentIRCompiler utility will use the
          // JITTargetMachineBuilder to build llvm TargetMachines (which are not
          // thread safe) as needed for compiles
          llvm::orc::ConcurrentIRCompiler(std::move(jtmb))),

      _data_layout(std::move(data_layout)),
      _mangle(_execution_session, this->_data_layout),
      _context(std::make_unique<llvm::LLVMContext>()) {
    _execution_session.getMainJITDylib().setGenerator(llvm::cantFail(
        llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
            _data_layout)));
  }

  const llvm::DataLayout &data_layout() const { return _data_layout; }
  llvm::LLVMContext &context() { return *_context.getContext(); }

  void add_module(unique_ptr<llvm::Module> module) {
    llvm::cantFail(_ir_compile_layer.add(
        _execution_session.getMainJITDylib(),
        llvm::orc::ThreadSafeModule(move(module), _context)));
  }

  llvm::Expected<llvm::JITEvaluatedSymbol> lookup(llvm::StringRef name) {
    return _execution_session.lookup(
        {&_execution_session.getMainJITDylib()}, _mangle(name.str()));
  }
};
