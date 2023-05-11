#ifndef JSRUNTIME_H
#define JSRUNTIME_H

#include "AST.h"
#include "JSValue.h"

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace minijsc {

/// EnvPtr is an index in a heap allocated vector.
using EnvPtr = int64_t;

// Each runtime environment is scoped to a block, blocks are enclosed within
// curly braces and each block can have one or more inner blocks.
// In order to be able to resolve variables in an outer scope we assign each
// environment a pointer denoted by an index in the global runtime environment
// the interpreter's core assign and resolve methods will walk the runtime.
//
// Each environment is protected by a guard, when entering a new scope the top
// EnvPtr is updated to the new scope's environment and keeps track of the old
// parent by saving the old value in PrevEnvPtr.
// At first the design used a pointer oriented approach but this ended up to be
// very bug prone especially since the ownership of the runtime changes and old
// values must be destroyed.
///
/// Core environment implementation.
class Environment {
    public:
    // Default constructor.
    explicit Environment() = default;

    // Set a reference to the parent environment.
    auto setParentPtr(EnvPtr ptr) -> void { parent = ptr; }

    // Get a reference to the parent environment.
    [[nodiscard]] auto getParentPtr() const -> EnvPtr { return parent; }

    // Define a new binding from a variable identifier to a value.
    auto defineBinding(const std::string& name, const JSBasicValue& value)
        -> void {
        values[name] = value;
    }

    // Resolve a binding.
    auto resolveBinding(const std::string& name)
        -> std::optional<JSBasicValue> {
        if (values.contains(name)) {
            return values[name];
        }
        return std::nullopt;
    }

    // Assign a new value to an existing binding, returns a boolean value
    // to signal success or failure.
    // Failure of an assignment means the binding doesn't existing in JS
    // terms the variable is undefined.
    auto assign(const std::string& name, const JSBasicValue& value) -> bool {
        if (values.contains(name)) {
            values[name] = value;
            return true;
        }
        return false;
    }

    private:
    /// Values map stores variable declarations by their variable name mapping
    /// them to the variable values.
    std::unordered_map<std::string, JSBasicValue> values;
    /// Parent scope's environment.
    EnvPtr parent;
};
} // namespace minijsc

#endif
