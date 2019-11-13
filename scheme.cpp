#include "scheme.h"
#include "parser.h"

SchemeInterpretor::SchemeInterpretor() : global_scope_(std::make_shared<Scope>()) {
    global_scope_->variables_["+"] = std::make_shared<Plus>();
    global_scope_->variables_["-"] = std::make_shared<Minus>();
    global_scope_->variables_["*"] = std::make_shared<Multiply>();
    global_scope_->variables_["/"] = std::make_shared<Divide>();
    global_scope_->variables_["if"] = std::make_shared<If>();
    global_scope_->variables_["quote"] = std::make_shared<Quote>();
    // builtint scope
}

SchemeInterpretor::~SchemeInterpretor() {
    global_scope_->variables_.clear();
}

std::shared_ptr<Object> SchemeInterpretor::Eval(std::shared_ptr<Object> in) {
    return in->Eval(global_scope_);
}

inline void PrintTo(const std::shared_ptr<Object>& obj, std::ostream* out) {
    if (!obj) {
        *out << "()";
        return;
    }
    obj->PrintTo(out);
}

inline std::string Print(const std::shared_ptr<Object>& obj) {
    std::stringstream ss;
    PrintTo(obj, &ss);
    return ss.str();
}

std::vector<std::shared_ptr<Object>> ToVector(const std::shared_ptr<Object>& head,
                                              std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> elements;
    if (!head) {
        return elements;
    } else {
        auto current = AsCell(head);
        while (current != nullptr) {
            elements.push_back(current->GetFirst()->Eval(scope));
            auto next = current->GetSecond();
            if (!IsCell(next) && next) {
                throw std::runtime_error("wrong argument list");
            }
            current = next;
        }
    }
    return elements;
}
