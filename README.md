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

We largely follow the LLVM style guide which is documented [here](https://llvm.org/docs/CodingStandards.html)
with slight deviations.

* We use camelBack (CamelCase with starting lower case for variable names).
* Doxygen style comment blocks `///` regardless of visibility.
* Use enum classes and the enum type prefix, for example `enum class ValueKind`
    would be used as `ValueKind::JSFloat64`.

Running `clang-format` and `clang-tidy` should help find any mechanical styling
issues.
