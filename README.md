# minijsc

`minijsc` is a compiler suite for the JavaScript core language written in C++
that provides an interpreter, a bytecode VM and a RISC-V JIT compiler.

The RISC-V code was designed to run on one of my other projects
[`riscvemu`](https://github.com/jmpnz/riscvemu).

## Usage

You can use `minijsc` as a library by linking against `libminijsc` or you
can use the main binary `minijsc` which provides two execution modes :

* *Interactive mode* : `minijsc --interactive` provides a REPL with a tree walk interpreter.

* *Interactive bytecode mode* : `minijsc --interactive -bc` provides a REPL with the VM.

* *Native mode* : `minijsc --native` provides a REPL with the VM and the JIT backend.

* *Debug mode* : `minijsc --debug` provides you with a debugging environment.

In interactive mode you can introspect the abstract syntax tree using annotated
commands such as :

```sh

$ minijsc -i

> let a = 5;
> @introspect(a);

```

Annotations such as `@introspect` can be used in the REPL or in code as well
the above call to `@introspect` would print the AST in S-expr format :

```sh

Let(
    Ident(a)
    Assign(Numeric(5))
)

```

Introspecting the virtual machine can also be done by calling the `@trace`
annotations, which allows you to see the VM state before or after execution.

In *debug mode* you can add annotations such as `@debug` which allow you
to trace the execution step by step.

```sh

> let adder = function(a, b) { return a + b; }
> @debug(adder)
> adder(5, 3)
> @step
<CallFn "adder"> => (a = 5, b = 3)
> @step
<Return from "adder">
8

```


## Build

You'll need CMake and a compiler that supports C++20.

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
* Doxygen style comment blocks `///` for classes, enums and functions.

We leverage several modern C++ features notably lambdas, variants and optional
the project was built using **clang** (version 14).

For tests we use [doctest](https://github.com/doctest/doctest) because it's easy
to setup, header only and doesn't require much gymnastics to add to CMake.

Running `clang-format` and `clang-tidy` should help find any mechanical styling
issues.
