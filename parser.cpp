#include <parser.h>
#include <iostream>
#include "/home/aun/SDA/shad-cpp0/scheme/scheme.h"

class Object;

NameError::NameError(const std::string& name) : std::runtime_error("variable not found: " + name) {
}

std::shared_ptr<Object> Scope::Lookup(const std::string& name) {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        throw NameError(name);
    }
    return it->second;
}

Types Object::ID() const {
    return Types::tType;
}

bool Object::IsFalse() {
    return false;
}

Object::~Object() = default;

Cell::Cell() : head_(nullptr), tail_(nullptr) {
}

Cell::Cell(std::shared_ptr<Object> head, std::shared_ptr<Object> tail) : head_(head), tail_(tail) {
}

Types Cell::ID() const {
    return Types::cellType;
}

void Cell::PrintTo(std::ostream* out) {
    *out << '(';
    ::PrintTo(head_, out);
    auto next = AsCell(tail_);
    if (!next && tail_) {
        *out << " . ";
        ::PrintTo(tail_, out);
    } else {
        while (next) {
            *out << " ";
            ::PrintTo(next->head_, out);
            auto next_obj = AsCell(next->tail_);
            if (!next_obj && next->tail_) {
                *out << " . ";
                ::PrintTo(next->tail_, out);
            }
            next = next_obj;
        }
    }
    *out << ')';
    return;
}

std::shared_ptr<Object> Cell::Eval(std::shared_ptr<Scope> scope) {
    auto ptr = head_->Eval(scope);
    auto fn = std::dynamic_pointer_cast<Function>(ptr);
    auto sf = std::dynamic_pointer_cast<SpecialForm>(ptr);
    if (!fn && !sf) {
        throw std::runtime_error("first element of the list must be a function");
    }
    std::vector<std::shared_ptr<Object>> args = ToVector(tail_);
    if (fn) {
        for (auto& arg : args) {
            arg = arg->Eval(scope);
        }
    }
    if (fn) {
        return fn->Apply(scope, args);
    } else {
        return sf->Apply(scope, args);
    }
}

const std::shared_ptr<Object>& Cell::GetFirst() const {
    return head_;
}

void Cell::SetFirst(std::shared_ptr<Object> object) {
    head_ = object;
}

const std::shared_ptr<Object>& Cell::GetSecond() const {
    return tail_;
}

void Cell::SetSecond(std::shared_ptr<Object> object) {
    tail_ = object;
}

Number::Number() : value_(0) {
}
Number::Number(int64_t value) : value_(value) {
}

Types Number::ID() const {
    return Types::numberType;
}

void Number::PrintTo(std::ostream* out) {
    *out << value_;
}

std::shared_ptr<Object> Number::Eval(std::shared_ptr<Scope>) {
    return shared_from_this();
}

int64_t Number::GetValue() const {
    return value_;
}

int64_t& Number::SetValue() {
    return value_;
}

Symbol::Symbol() : name_("") {
}

Symbol::Symbol(std::string name) : name_(name) {
}

Types Symbol::ID() const {
    return Types::symbolType;
}

void Symbol::PrintTo(std::ostream* out) {
    *out << name_;
}

std::shared_ptr<Object> Symbol::Eval(std::shared_ptr<Scope> scope) {  // only decalration here FIXME
    return scope->Lookup(name_);
}

const std::string& Symbol::GetName() const {
    return name_;
}

std::string& Symbol::SetName() {
    return name_;
}

Types Quote::ID() const {
    return Types::quoteType;
}

std::shared_ptr<Object> Quote::Apply(const std::shared_ptr<Scope>,
                                     const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw std::runtime_error("Syntax error!");  // FIXME
    }
    return args[0];
}

Types Dot::ID() const {
    return Types::dotType;
}

std::shared_ptr<Object> Function::Eval(std::shared_ptr<Scope>) {
    throw std::runtime_error("can't eval function");
}

std::shared_ptr<Object> Function::Apply(const std::shared_ptr<Scope>& scope,
                                        const std::vector<std::shared_ptr<Object>>& args) {
}

std::shared_ptr<Object> SpecialForm::Eval(std::shared_ptr<Scope>) {
    throw std::runtime_error("can't eval function");
}

std::shared_ptr<Object> SpecialForm::Apply(const std::shared_ptr<Scope>& scope,
                                           const std::vector<std::shared_ptr<Object>>& args) {
}

std::shared_ptr<Object> Plus::Apply(const std::shared_ptr<Scope>,
                                    const std::vector<std::shared_ptr<Object>>& args) {
    int64_t value = 0;
    for (const auto& arg : args) {
        auto number = std::dynamic_pointer_cast<Number>(arg);
        if (!number) {
            throw std::runtime_error{"+ arguments must be numbers"};
        }
        value += number->GetValue();
    }
    return std::make_shared<Number>(value);
}

std::shared_ptr<Object> Minus::Apply(const std::shared_ptr<Scope>,
                                     const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() == 0) {
        throw std::runtime_error("Wrongs number of arguments!");
    }
    int64_t value = std::dynamic_pointer_cast<Number>(args[0])->GetValue();
    if (!value) {
        throw std::runtime_error{"- arguments must be numbers"};
    }
    for (size_t i = 1; i < args.size(); ++i) {
        auto number = std::dynamic_pointer_cast<Number>(args[i]);
        if (!number) {
            throw std::runtime_error{"- arguments must be numbers"};
        }
        value -= number->GetValue();
    }
    return std::make_shared<Number>(value);
}

std::shared_ptr<Object> Divide::Apply(const std::shared_ptr<Scope>,
                                      const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() == 0) {
        throw std::runtime_error("Wrongs number of arguments!");
    }
    int64_t value = std::dynamic_pointer_cast<Number>(args[0])->GetValue();
    if (!value) {
        throw std::runtime_error{"- arguments must be numbers"};
    }
    for (size_t i = 1; i < args.size(); ++i) {
        auto number = std::dynamic_pointer_cast<Number>(args[i]);
        if (!number) {
            throw std::runtime_error{"/ arguments must be numbers"};
        }
        value /= number->GetValue();
    }
    return std::make_shared<Number>(value);
}

std::shared_ptr<Object> Multiply::Apply(const std::shared_ptr<Scope>,
                                        const std::vector<std::shared_ptr<Object>>& args) {
    int64_t value = 1;
    for (const auto& arg : args) {
        auto number = std::dynamic_pointer_cast<Number>(arg);
        if (!number) {
            throw std::runtime_error{"* arguments must be numbers"};
        }
        value *= number->GetValue();
    }
    return std::make_shared<Number>(value);
}

std::shared_ptr<Object> If::Apply(const std::shared_ptr<Scope> scope,
                                  const std::vector<std::shared_ptr<Object>>& args) {
    auto condition = args[0];  // FIXME
    auto if_true = args[1];
    auto if_false = args[2];
    auto result = condition->Eval(scope);
    if (result && !result->IsFalse()) {
        return if_true->Eval(scope);
    } else {
        return if_false->Eval(scope);
    }
    /*if (args.size() != 3) {
        throw std::runtime_error("Syntax error!");  // FIXME
    }*/
}

SyntaxError::SyntaxError(const std::string& what) : std::runtime_error(what) {
}

bool IsNumber(const std::shared_ptr<Object>& obj) {
    return Types::numberType == obj.get()->ID();
}

std::shared_ptr<Number> AsNumber(const std::shared_ptr<Object>& obj) {
    if (IsNumber(obj)) {
        return std::static_pointer_cast<Number>(obj);
    }
    return nullptr;
}

bool IsCell(const std::shared_ptr<Object>& obj) {
    return Types::cellType == obj->ID();
}

std::shared_ptr<Cell> AsCell(const std::shared_ptr<Object>& obj) {
    if (IsCell(obj)) {
        return std::static_pointer_cast<Cell>(obj);
    }
    return nullptr;
}

bool IsSymbol(const std::shared_ptr<Object>& obj) {
    return Types::symbolType == obj->ID();
}

std::shared_ptr<Symbol> AsSymbol(const std::shared_ptr<Object>& obj) {
    if (IsSymbol(obj)) {
        return std::static_pointer_cast<Symbol>(obj);
    }
    return nullptr;
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        return nullptr;
    }
    auto current_object = tokenizer->GetToken();
    if (SymbolToken* symbol = std::get_if<SymbolToken>(&current_object)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(symbol->name);
    } else if (ConstantToken* constant = std::get_if<ConstantToken>(&current_object)) {
        tokenizer->Next();
        return std::make_shared<Number>(constant->value);
    } else if (std::holds_alternative<QuoteToken>(current_object)) {
        tokenizer->Next();
        auto new_cell = std::make_shared<Cell>(Cell());
        new_cell->SetFirst(std::make_shared<Symbol>("quote"));
        new_cell->SetSecond(Read(tokenizer));
        return new_cell;
    } else if (std::holds_alternative<DotToken>(current_object)) {
        throw SyntaxError("Unexpected symbol");
    } else if (BracketToken* bracket = std::get_if<BracketToken>(&current_object)) {
        if (*bracket == BracketToken::CLOSE) {
            throw SyntaxError("Unexpected closing parentheses");
        }
        tokenizer->Next();
        return ReadList(tokenizer);
    }
    throw SyntaxError("Unexpected symbol");
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Input not complete");
    }
    std::shared_ptr<Object> head = nullptr;
    std::shared_ptr<Cell> tail = nullptr;
    while (!tokenizer->IsEnd()) {
        auto current_token = tokenizer->GetToken();
        if (std::holds_alternative<BracketToken>(current_token) &&
            std::get<BracketToken>(current_token) == BracketToken::CLOSE) {
            tokenizer->Next();
            return head;
        } else if (std::holds_alternative<DotToken>(current_token)) {
            tokenizer->Next();
            if (tail == nullptr) {
                throw SyntaxError("Improper list syntax");
            }
            tail->SetSecond(Read(tokenizer));
            if (!std::holds_alternative<BracketToken>(tokenizer->GetToken()) ||
                std::get<BracketToken>(tokenizer->GetToken()) != BracketToken::CLOSE) {
                throw SyntaxError("Improper list syntax");
            }
        } else {
            auto current_object = Read(tokenizer);
            auto new_cell = std::make_shared<Cell>(Cell());
            new_cell->SetFirst(current_object);
            if (head == nullptr) {
                head = new_cell;
                tail = new_cell;
            } else {
                tail->SetSecond(new_cell);
                tail = new_cell;
            }
        }
    }
    throw SyntaxError("Unmatched opening parentheses");
}
