cmake_minimum_required(VERSION 3.13.4)
project(kaleidoscope VERSION 0.1.0)

find_package(LLVM 8.0 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(main main.cpp)

llvm_map_components_to_libnames(llvm_libs core passes orcjit)
target_link_libraries(main ${llvm_libs})
