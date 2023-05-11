//===----------------------------------------------------------------------===//
// JSValue.h: This header defines the class and interfaces used to represent
// JavaScript primitive values, such as `null`, `undefined`, `number`, `string`
// and `boolean`.
//===----------------------------------------------------------------------===//
#ifndef JSVALUE_H
#define JSVALUE_H

#include "fmt/core.h"

#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace minijsc {

// JSUndefined aliases the std::monostate type for empty values.
using JSUndefined = std::monostate;
// JSNull aliases the nullptr type.
using JSNull = std::nullptr_t;
// JSNumber aliases the double type.
using JSNumber = double;
// JSBoolean aliases the bool type.
using JSBoolean = bool;
// JSString aliases the string types (includes std::string & char*).
using JSString = std::string;

// JSPrimitiveValue is a type that can hold all possible variants of JavaScript's
// primitive values.
using JSPrimitiveValue =
    std::variant<JSNumber, JSBoolean, JSString, JSUndefined, JSNull>;

// JSType enumerates the possible Javascript primitive types.
enum class JSValueKind {
    Undefined,
    Null,
    Number,
    Boolean,
    String,
    Function,
    Object,
    Array,
};

/// JSValue defines a virtual interface for representing all JavaScript values
/// including primitives and compound literals and types.
class JSValue {
    public:
    virtual ~JSValue() = default;

    virtual auto getKind() -> JSValueKind = 0;
};

/// JSBasicValue class represents a JavaScript primitive value, primitives
/// include numbers, booleans, strings and the undefined and null types.
/// JSBasicValue is implemented using a variant around C++ primitive types
class JSBasicValue : public JSValue {
    public:
    // Default constructor sets the type to undefined, the variant is in monostate.
    JSBasicValue() : type(JSValueKind::Undefined) {}

    // Constructor for numeric values.
    JSBasicValue(JSNumber number) : value(number), type(JSValueKind::Number) {}

    // Constructor for null values.
    JSBasicValue(JSNull /*null*/) : type(JSValueKind::Null) {}

    // Constructor for boolean values.
    JSBasicValue(JSBoolean boolean)
        : value(boolean), type(JSValueKind::Boolean) {}

    // Constructor for string values.
    JSBasicValue(JSString str)
        : value(std::move(str)), type(JSValueKind::String) {}

    // COnstructor for C-string values.
    JSBasicValue(const char* str)
        : value(std::string(str)), type(JSValueKind::String) {}

    // Return the `JSValueKind` of this value.
    [[nodiscard]] auto getKind() -> JSValueKind override { return type; }

    // Check if the value is undefined.
    [[nodiscard]] auto isUndefined() const -> bool {
        return type == JSValueKind::Undefined;
    }

    // Check if the value is a number.
    [[nodiscard]] auto isNumber() const -> bool {
        return type == JSValueKind::Number;
    }

    // Check if the value is a boolean.
    [[nodiscard]] auto isBoolean() const -> bool {
        return type == JSValueKind::Boolean;
    }

    // Check if the value is a string.
    [[nodiscard]] auto isString() const -> bool {
        return type == JSValueKind::String;
    }

    // Return a string representation of the value.
    [[nodiscard]] auto toString() const -> std::string {
        switch (type) {
        case JSValueKind::Undefined:
            return "undefined";
        case JSValueKind::Null:
            return "null";
        case JSValueKind::Boolean:
            return getValue<bool>() ? "true" : "false";
        case JSValueKind::Number:
            return std::to_string(getValue<double>());
        case JSValueKind::String:
            return getValue<std::string>();
        case JSValueKind::Function:
            return "Function";
        case JSValueKind::Object:
            return "Object";
        case JSValueKind::Array:
            return "Array";
        }
    }

    // Returns the underlying value stored.
    template <typename T> [[nodiscard]] auto getValue() const -> T {
        if (const auto* val = std::get_if<T>(&value)) {
            return *val;
        }
        return T{};
    }

    // Sets the underlying stored value to the one passed as argument inferring
    // the type from the argument, the previously stored value isn't reset at
    // this point, which is something we want to explore once we start supporting
    // JSObject in the future.
    template <typename T> auto setValue(T value) {
        this->type  = getKindFromValue(value);
        this->value = std::move(value);
    }

    private:
    // Static methods that are used in the `setValue` to store
    // the proper type.
    static auto getKindFromValue(std::nullptr_t) -> JSValueKind {
        return JSValueKind::Null;
    }

    static auto getKindFromValue(bool /*unused*/) -> JSValueKind {
        return JSValueKind::Boolean;
    }

    static auto getKindFromValue(double /*unused*/) -> JSValueKind {
        return JSValueKind::Number;
    }

    static auto getKindFromValue(const char* /*unused*/) -> JSValueKind {
        return JSValueKind::String;
    }

    static auto getKindFromValue(const std::string& /*unused*/) -> JSValueKind {
        return JSValueKind::String;
    }

    // Underlying value stored in JSBasicValue.
    JSPrimitiveValue value;
    // Javascript type of the underlying stored value.
    JSValueKind type;
};

}; // namespace minijsc

#endif
