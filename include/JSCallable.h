#ifndef JSCALLABLE_H
#define JSCALLABLE_H

#include "AST.h"
#include "Interpreter.h"
#include "JSValue.h"

#include <utility>
#include <vector>

namespace minijsc {

/// JSReturn inherits from runtime exception to unwind the stack after the return
/// back to a function call.
class JSReturn : public std::exception {
    public:
    JSReturn(std::shared_ptr<JSValue> value) : value(std::move(value)) {}

    auto getValue() -> std::shared_ptr<JSValue> { return value; }

    const char* what() const throw() override { return "Return"; }

    private:
    std::shared_ptr<JSValue> value;
};

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

    auto getName() -> std::string { return funcDecl->getName().getLexeme(); }

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
            fmt::print("Argument : {} has Value : {}\n", paramName,
                       paramValue.toString());
            funcScope.defineBinding(paramName,
                                    std::make_shared<JSBasicValue>(paramValue));
        }
        // Append the new environment to the global env stack.
        interpreter->appendSymbolTable(funcScope);
        // Increment the env scope index.
        // interpreter->setCurrIdx(currentScopeIdx++);
        try {
            // Execute the statements in a new block.
            interpreter->executeBlock(funcDecl->getBody().get(),
                                      std::move(funcScope));
        } catch (JSReturn& ret) {
            auto res = (std::static_pointer_cast<JSBasicValue>(ret.getValue()));
            fmt::print("call => Got return value {}\n", res->toString());
            interpreter->pushValue(res);
            // Pop function scope
            //  interpreter->setCurrIdx(currentScopeIdx--);
            interpreter->popSymbolTable();
            return *res;
        }
        interpreter->popSymbolTable();
        // interpreter->setCurrIdx(currentScopeIdx--);
        return {};
    }

    private:
    std::shared_ptr<JSFuncDecl> funcDecl;
};

} // namespace minijsc

#endif
