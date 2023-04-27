# minijsc 

`minijsc` is a compiler suite for the JavaScript core language written in C++
that provides an interpreter, a bytecode VM and RISC-V compiler.

The RISC-V code was designed to run on the companion project [`riscvemu`].

## Build

You'll need CMake & Ninja

```sh

$ mkdir build/
$ cd build
$ cmake ..
$ ninja all

```

## Style

Use Google's style guide for C++ code and follow some of the advice
from the LLVM style guide.

