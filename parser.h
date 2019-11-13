#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include "/home/aun/SDA/shad-cpp0/scheme-tokenizer/tokenizer.h"

enum class Types { tType, cellType, numberType, symbolType, quoteType, dotType };

class Object;

class NameError : public std::runtime_error {
public:
    NameError(const std::string& name);
};

class Scope {
public:
    std::shared_ptr<Object> Lookup(const std::string& name);

    std::unordered_map<std::string, std::shared_ptr<Object>> variables_;
};

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual Types ID() const;

    virtual bool IsFalse();

    virtual void PrintTo(std::ostream* out) = 0;

    virtual std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) = 0;

    virtual ~Object();
};

class Cell : public Object {
public:
    Cell();

    Cell(std::shared_ptr<Object> head, std::shared_ptr<Object> tail);

    virtual Types ID() const override;

    virtual void PrintTo(std::ostream* out) override;

    virtual std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    const std::shared_ptr<Object>& GetFirst() const;

    void SetFirst(std::shared_ptr<Object> object);

    const std::shared_ptr<Object>& GetSecond() const;

    void SetSecond(std::shared_ptr<Object> object);

private:
    std::shared_ptr<Object> head_;
    std::shared_ptr<Object> tail_;
};

class Number : public Object {
public:
    Number();

    explicit Number(int64_t value);

    virtual Types ID() const override;

    virtual void PrintTo(std::ostream* out) override;

    virtual std::shared_ptr<Object> Eval(std::shared_ptr<Scope>) override;

    int64_t GetValue() const;

    int64_t& SetValue();

private:
    int64_t value_;
};

class Symbol : public Object {
public:
    Symbol();

    explicit Symbol(std::string name);

    virtual Types ID() const override;

    virtual void PrintTo(std::ostream* out) override;

    virtual std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    const std::string& GetName() const;

    std::string& SetName();

private:
    std::string name_;
};

class Quote : public SpecialForm {
public:
    virtual Types ID() const override;

    virtual std::shared_ptr<Object> Apply(const std::shared_ptr<Scope>,
                                          const std::vector<std::shared_ptr<Object>>& args);
};

class Dot : public Object {
    virtual Types ID() const override;
};

class Function : public Object {
public:
    virtual std::shared_ptr<Object> Eval(std::shared_ptr<Scope>) override;

    virtual std::shared_ptr<Object> Apply(const std::shared_ptr<Scope>& scope,
                                          const std::vector<std::shared_ptr<Object>>& args);

private:
};

class SpecialForm : public Object {
public:
    virtual std::shared_ptr<Object> Eval(std::shared_ptr<Scope>) override;

    virtual std::shared_ptr<Object> Apply(const std::shared_ptr<Scope>& scope,
                                          const std::vector<std::shared_ptr<Object>>& args);

private:
};

class Plus : public Function {
public:
    virtual std::shared_ptr<Object> Apply(
        const std::shared_ptr<Scope>, const std::vector<std::shared_ptr<Object>>& args) override;
};

class Minus : public Function {
public:
    virtual std::shared_ptr<Object> Apply(
        const std::shared_ptr<Scope>, const std::vector<std::shared_ptr<Object>>& args) override;
};

class Multiply : public Function {
public:
    virtual std::shared_ptr<Object> Apply(
        const std::shared_ptr<Scope>, const std::vector<std::shared_ptr<Object>>& args) override;
};

class Divide : public Function {
public:
    virtual std::shared_ptr<Object> Apply(
        const std::shared_ptr<Scope>, const std::vector<std::shared_ptr<Object>>& args) override;
};

class If : public SpecialForm {
public:
    virtual std::shared_ptr<Object> Apply(
        const std::shared_ptr<Scope>, const std::vector<std::shared_ptr<Object>>& args) override;
};

struct SyntaxError : public std::runtime_error {
    explicit SyntaxError(const std::string& what);
};

bool IsNumber(const std::shared_ptr<Object>& obj);

std::shared_ptr<Number> AsNumber(const std::shared_ptr<Object>& obj);

bool IsCell(const std::shared_ptr<Object>& obj);

std::shared_ptr<Cell> AsCell(const std::shared_ptr<Object>& obj);

bool IsSymbol(const std::shared_ptr<Object>& obj);

std::shared_ptr<Symbol> AsSymbol(const std::shared_ptr<Object>& obj);

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);

std::shared_ptr<Object> Read(Tokenizer* tokenizer);

bool IsCell(const std::shared_ptr<Object>& obj);

std::shared_ptr<Cell> AsCell(const std::shared_ptr<Object>& obj);
