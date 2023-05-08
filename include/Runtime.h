#ifndef RUNTIME_H
#define RUNTIME_H

#include "AST.h"
#include "JSValue.h"

#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace minijsc {

/// Environment for the interpreter runtime, environment is used to resolve
/// variable bindings at runtime.
class Environment {
    public:
    // Default constructor.
    explicit Environment() = default;

    // Define a new binding.
    auto defineBinding(const std::string& name, const JSBasicValue& value)
        -> void {
        values[name] = value;
    }

    // Resolve an variable binding.
    auto resolveBinding(const JSToken& name) {
        if (values.contains(name.getLexeme())) {
            return values[name.getLexeme()];
        }
        throw std::runtime_error("Undefined variable : '" + name.getLexeme() +
                                 "'.");
    }

    // Assign a new value to an existing binding.
    auto assign(const std::string& name, const JSBasicValue& value) -> void {
        if (values.contains(name)) {
            values[name] = value;
            return;
        }

        throw std::runtime_error("Undefined variable: '" + name + "'.");
    }

    private:
    /// Values map stores variable declarations by their variable name mapping
    /// them to the variable values.
    std::unordered_map<std::string, JSBasicValue> values;
};
} // namespace minijsc

#endif
