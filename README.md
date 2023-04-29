# minijsc

`minijsc` is a compiler suite for the JavaScript core language written in C++
that provides an interpreter, a bytecode VM and RISC-V compiler.

The RISC-V code was designed to run on the companion project
[`riscvemu`](https://github.com/jmpnz/riscvemu).

## Build

You'll need CMake

```sh

$ mkdir build/
$ cd build
$ cmake ..
$ make all

```

## Style

We follow LLVM style guide, except for variables we prefer camelCase with lower
starting character.
