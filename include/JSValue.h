#ifndef JSVALUE_H
#define JSVALUE_H

#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace minijsc {

/// JSBasicValue class represents a JavaScript primitive value, primitives
/// include numbers, booleans, strings and the undefined and null types.
/// JSBasicValue is implemented using a variant around C++ primitive types
class JSBasicValue {
    public:
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
    // JSValueType is a type that can hold all possible variants of JavaScript's
    // primitive values.
    using JSValueType =
        std::variant<JSNumber, JSBoolean, JSString, JSUndefined, JSNull>;

    // JSType enumerates the possible Javascript primitive types.
    enum class JSType {
        Undefined,
        Null,
        Number,
        Boolean,
        String,
    };

    // Default constructor sets the type to undefined, the variant is in monostate.
    JSBasicValue() : type(JSType::Undefined) {}

    // Constructor for numeric values.
    JSBasicValue(JSNumber number) : value(number), type(JSType::Number) {}

    // Constructor for null values.
    JSBasicValue(JSNull /*null*/) : type(JSType::Null) {}

    // Constructor for boolean values.
    JSBasicValue(JSBoolean boolean) : value(boolean), type(JSType::Boolean) {}

    // Constructor for string values.
    JSBasicValue(JSString str) : value(std::move(str)), type(JSType::String) {}

    // COnstructor for C-string values.
    JSBasicValue(const char* str)
        : value(std::string(str)), type(JSType::String) {}

    // Return the `JSType` of this value.
    [[nodiscard]] auto getType() const -> JSType { return type; }

    // Check if the value is undefined.
    [[nodiscard]] auto isUndefined() const -> bool {
        return type == JSType::Undefined;
    }

    // Check if the value is a number.
    [[nodiscard]] auto isNumber() const -> bool {
        return type == JSType::Number;
    }

    // Check if the value is a boolean.
    [[nodiscard]] auto isBoolean() const -> bool {
        return type == JSType::Boolean;
    }

    // Check if the value is a string.
    [[nodiscard]] auto isString() const -> bool {
        return type == JSType::String;
    }

    // Returns the underlying value stored.
    template <typename T> auto getValue() const -> T {
        if (auto val = std::get_if<T>(&value)) {
            return *val;
        }
        return T{};
    }

    // Sets the underlying stored value to the one passed as argument inferring
    // the type from the argument, the previously stored value isn't reset at
    // this point, which is something we want to explore once we start supporting
    // JSObject in the future.
    template <typename T> auto setValue(T value) {
        this->type  = getTypeFromValue(value);
        this->value = std::move(value);
    }

    private:
    // Static methods that are used in the `setValue` to store
    // the proper type.

    static auto getTypeFromValue(std::nullptr_t) -> JSType {
        return JSType::Null;
    }

    static auto getTypeFromValue(bool /*unused*/) -> JSType {
        return JSType::Boolean;
    }

    static auto getTypeFromValue(double /*unused*/) -> JSType {
        return JSType::Number;
    }

    static auto getTypeFromValue(const char* /*unused*/) -> JSType {
        return JSType::String;
    }

    static auto getTypeFromValue(const std::string& /*unused*/) -> JSType {
        return JSType::String;
    }

    // Underlying value stored in JSBasicValue.
    JSValueType value;
    // Javascript type of the underlying stored value.
    JSType type;
};

}; // namespace minijsc

#endif
