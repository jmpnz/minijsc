# minijsc

`minijsc` is an unfinished effort to write a compiler runtime for a subset of JS.
It followed the design of Lox and was hacked together during nights and weekends,

Lox : [craftinginterpreters.com](https://craftinginterpreters.com)

`minijsc` is largely limited to variables, primitive values, basic control flow
using if/else/for/while statements and functions.

This was mostly an exercice in writing an interpreter, runtime and compiler.

## Build

You don't have to build this since it has no use, better to just explore code.

If you do insist on building it you'll need CMake and clang 16.0.

```sh

$ mkdir build/
$ cd build
$ cmake ..
$ make all

```

Currently the only third party library part of minijsc is fmt, it uses fmt v9.1.0.
