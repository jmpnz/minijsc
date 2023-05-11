#ifndef JSCALLABLE_H
#define JSCALLABLE_H

#include "AST.h"
#include "Interpreter.h"
#include "JSValue.h"

#include <vector>

namespace minijsc {

/// JSCallable interface defines JavaScript callable objects such as functions
/// and methods.
class JSCallable : public JSValue {
    public:
    /// Default destructor.
    ~JSCallable() override = default;
    /// The JSCallable interface defines a single method, `call`.
    virtual auto call(Interpreter* interpreter,
                      std::vector<JSBasicValue> arguments) -> JSBasicValue = 0;
};

} // namespace minijsc

#endif
