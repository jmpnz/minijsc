# minijsc

`minijsc` is an unfinished effort to write compiler suite for a subset of JS
core language written in C++ that provides an interpreter, a bytecode VM
and a JIT compiler.

`minijsc` is largely limited to variables, primitive values, basic control flow
using if/else/for/while statements and functions.

This was mostly an exercice in writing an interpreter, runtime and compiler.

## Build

You'll need CMake and a compiler that supports C++20.

```sh

$ mkdir build/
$ cd build
$ cmake ..
$ make all

```

Currently the only third party library part of minijsc is fmt, we use fmt v9.1.0.
