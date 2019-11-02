#include <parser.h>
#include <iostream>

bool IsNumber(const std::shared_ptr<Object>& obj) {
    return Types::numberType == obj.get()->ID();
}

std::shared_ptr<Number> AsNumber(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<Number>(obj);
}

bool IsCell(const std::shared_ptr<Object>& obj) {
    return Types::cellType == obj->ID();
}

std::shared_ptr<Cell> AsCell(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<Cell>(obj);
}

bool IsSymbol(const std::shared_ptr<Object>& obj) {
    return Types::symbolType == obj->ID();
}

std::shared_ptr<Symbol> AsSymbol(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<Symbol>(obj);
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
        return std::make_shared<Quote>();
    } else if (std::holds_alternative<DotToken>(current_object)) {
        tokenizer->Next();
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
            tail->SetSecond() = Read(tokenizer);
            if (!std::holds_alternative<BracketToken>(tokenizer->GetToken()) ||
                std::get<BracketToken>(tokenizer->GetToken()) != BracketToken::CLOSE) {
                throw SyntaxError("Improper list syntax");
            }
        } else {
            auto current_object = Read(tokenizer);
            auto new_cell = std::make_shared<Cell>(Cell());
            new_cell->SetFirst() = current_object;
            if (head == nullptr) {
                head = new_cell;
                tail = new_cell;
            } else {
                tail->SetSecond() = new_cell;
                tail = new_cell;
            }
        }
    }
    throw SyntaxError("Unmatched opening parentheses");
}
