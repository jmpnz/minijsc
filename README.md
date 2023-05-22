# minijsc

`minijsc` is an unfinished effort to write a compiler runtime for a subset of JS.
It followed the design of Lox and was hacked together during nights and weekends,

Lox : [craftinginterpreters.com](https://craftinginterpreters.com)

`minijsc` is largely limited to variables, primitive values, basic control flow
using if/else/for/while statements and functions.

This was mostly an exercice in writing an interpreter, runtime and compiler.

I've stopped working on this as the initial design was suffering from extra
complexity and was essentially faulty:

* The initial approach to representing values made writing a GC impenetrable
* The AST representation used pointers and was very easy to break when given
  bad programs.
* C++ has a few oddities that made debugging hard and very time consuming
* When building the GC I used smart pointers and represented the heap as
  as a vector of smart pointers but this turned out to be a bad idea as
  erase and reset were very easy to get wrong and ended up with leaks.

All in all I learned a lot building this and now it goes into the graveyard.

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
