#ifndef JSCALLABLE_H
#define JSCALLABLE_H

#include "AST.h"
#include "Interpreter.h"
#include "JSValue.h"

#include <utility>
#include <vector>

namespace minijsc {

/// JSCallable interface defines JavaScript callable objects such as functions
/// and methods.
class JSCallable {
    public:
    /// Default destructor.
    virtual ~JSCallable() = default;

    /// The JSCallable interface defines a single method, `call`.
    virtual auto call(Interpreter* interpreter,
                      std::vector<JSBasicValue> arguments) -> JSBasicValue = 0;
};

/// JSFunction is a concrete implementation of JSCallable and represents
/// a JavaScript function.
class JSFunction : public JSCallable, public JSValue {
    public:
    /// Default constructor takes the function declaration as argument.
    explicit JSFunction(std::shared_ptr<JSFuncDecl> funcDecl)
        : funcDecl(std::move(funcDecl)) {}

    auto getKind() -> JSValueKind override { return JSValueKind::Function; }

    /// Function calls are dispatched by the runtime after creating the new
    /// function scope.
    auto call(Interpreter* interpreter, std::vector<JSBasicValue> arguments)
        -> JSBasicValue override {
        // Get the top level scope pointer.
        auto currentScopeIdx = interpreter->getCurrIdx();
        // Create the function scope.
        auto funcScope = Environment();
        // Set the function scope pointer to the top level environment.
        funcScope.setParentPtr(currentScopeIdx);
        // Define the parameters as part of the function scope.
        auto params = funcDecl->getParams();
        for (size_t i = 0; i < params.size(); i++) {
            auto paramName  = params[i].getLexeme();
            auto paramValue = arguments[i];
            funcScope.defineBinding(paramName,
                                    std::make_shared<JSBasicValue>(paramValue));
        }
        // Append the new environment to the global env stack.
        interpreter->appendSymbolTable(funcScope);
        // Increment the env scope index.
        interpreter->setCurrIdx(currentScopeIdx++);
        // Execute the statements in a new block.
        interpreter->executeBlock(funcDecl->getBody().get(), funcScope);

        return {};
    }

    private:
    std::shared_ptr<JSFuncDecl> funcDecl;
};

} // namespace minijsc

#endif
