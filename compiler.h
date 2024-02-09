#ifndef clox_compiler_h
#define clox_compiler_h

#include "vm.h"

// single-pass compiler, 
// parses source code and generates bytecode at the same time
// bytecode == low-level series of binary instructions
ObjFunction* compile(const char* source);
void markCompilerRoots();

#endif