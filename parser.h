#pragma once

#include <memory>

#include <tokenizer.h>

enum class Types { tType, cellType, numberType, symbolType, quoteType, dotType };

class Object {
public:
    virtual Types ID() const {
        return Types::tType;
    }

    virtual ~Object() = default;
};

class Cell : public Object {
public:
    Cell() : head_(nullptr), tail_(nullptr) {
    }

    Cell(std::shared_ptr<Object> head, std::shared_ptr<Object> tail) : head_(head), tail_(tail) {
    }

    virtual Types ID() const override {
        return Types::cellType;
    }

    const std::shared_ptr<Object>& GetFirst() const {
        return head_;
    }

    std::shared_ptr<Object>& SetFirst() {
        return head_;
    }

    const std::shared_ptr<Object>& GetSecond() const {
        return tail_;
    }

    std::shared_ptr<Object>& SetSecond() {
        return tail_;
    }

private:
    std::shared_ptr<Object> head_;
    std::shared_ptr<Object> tail_;
};

class Number : public Object {
public:
    Number() : value_(0) {
    }
    explicit Number(int value) : value_(value) {
    }

    virtual Types ID() const override {
        return Types::numberType;
    }

    int GetValue() const {
        return value_;
    }

    int& SetValue() {
        return value_;
    }

private:
    int value_;
};

class Symbol : public Object {
public:
    Symbol() : name_("") {
    }

    explicit Symbol(std::string name) : name_(name) {
    }

    virtual Types ID() const override {
        return Types::symbolType;
    }

    const std::string& GetName() const {
        return name_;
    }

    std::string& SetName() {
        return name_;
    }

private:
    std::string name_;
};

class Quote : public Object {
public:
    virtual Types ID() const override {
        return Types::quoteType;
    }
};

class Dot : public Object {
    virtual Types ID() const override {
        return Types::dotType;
    }
};

struct SyntaxError : public std::runtime_error {
    explicit SyntaxError(const std::string& what) : std::runtime_error(what) {
    }
};

bool IsNumber(const std::shared_ptr<Object>& obj);

std::shared_ptr<Number> AsNumber(const std::shared_ptr<Object>& obj);

bool IsCell(const std::shared_ptr<Object>& obj);

std::shared_ptr<Cell> AsCell(const std::shared_ptr<Object>& obj);

bool IsSymbol(const std::shared_ptr<Object>& obj);

std::shared_ptr<Symbol> AsSymbol(const std::shared_ptr<Object>& obj);

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);

std::shared_ptr<Object> Read(Tokenizer* tokenizer);
