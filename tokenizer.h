#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <tuple>

struct SymbolToken {
    SymbolToken(std::string new_name) : name(new_name) {
        if (name == "+" || name == "-" || name == "*") {
            IsExceptional = true;
        }
    }
    bool operator==(const SymbolToken& rhs) const {
        return (name == rhs.name);
    }
    std::string name;
    bool IsExceptional = false;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

struct NullToken {
    bool operator==(const NullToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    ConstantToken(int number) : value(number) {
    }
    bool operator==(const ConstantToken& rhs) const {
        return (value == rhs.value);
    }
    int value;
};

typedef std::variant<SymbolToken, ConstantToken, BracketToken, DotToken, QuoteToken, NullToken>
    Token;

inline Token MakeLongToken(std::string symbols) {
    if (isdigit(symbols.at(0)) ||
        (symbols.at(0) == '-' && symbols.length() > 1 && isdigit(symbols.at(1)))) {
        return ConstantToken(std::stoi(symbols));
    }
    return SymbolToken(symbols);
}

class Tokenizer {
public:
    Tokenizer(std::istream* in)
        : this_token_(NullToken()), working_stream_(in), last_token_(false) {
        Next();
    }

    bool IsEnd() {
        return last_token_;
    }

    void Next() {
        this_token_ = NullToken();
        char cur;
        std::string accum_token;
        if (working_stream_->peek() == EOF) {
            if (!accum_token.empty()) {
                this_token_ = MakeLongToken(accum_token);
                accum_token.clear();
            }
            last_token_ = true;
        }
        while (working_stream_->peek() != EOF) {
            cur = working_stream_->peek();
            if (cur == '(') {
                if (accum_token.empty()) {
                    this_token_ = BracketToken::OPEN;
                    working_stream_->get();
                    break;
                } else {
                    this_token_ = MakeLongToken(accum_token);
                    accum_token.clear();
                    break;
                }
            } else if (cur == ' ') {
                if (accum_token.empty()) {
                    working_stream_->get();
                } else {
                    this_token_ = MakeLongToken(accum_token);
                    accum_token.clear();
                    working_stream_->get();
                    break;
                }
            } else if (cur == ')') {
                if (accum_token.empty()) {
                    this_token_ = BracketToken::CLOSE;
                    working_stream_->get();
                    break;
                } else {
                    this_token_ = MakeLongToken(accum_token);
                    accum_token.clear();
                    break;
                }
            } else if (cur == '\'') {
                if (accum_token.empty()) {
                    this_token_ = QuoteToken();
                    working_stream_->get();
                    break;
                } else {
                    this_token_ = MakeLongToken(accum_token);
                    accum_token.clear();
                    break;
                }
            } else if (cur == '.') {
                if (accum_token.empty()) {
                    this_token_ = DotToken();
                    working_stream_->get();
                    break;
                } else {
                    this_token_ = MakeLongToken(accum_token);
                    accum_token.clear();
                    break;
                }
            } else if (isdigit(cur) || isalpha(cur) || cur == '?' || cur == '!') {
                accum_token += cur;
                working_stream_->get();
            } else if (cur == '+') {
                if (accum_token.empty()) {
                    this_token_ = SymbolToken("+");
                    working_stream_->get();
                    break;
                } else {
                    this_token_ = MakeLongToken(accum_token);
                    accum_token.clear();
                    break;
                }
            } else if (cur == '*') {
                if (accum_token.empty()) {
                    this_token_ = SymbolToken("*");
                    working_stream_->get();
                    break;
                } else {
                    this_token_ = MakeLongToken(accum_token);
                    accum_token.clear();
                    break;
                }
            } else if (cur == '-') {
                if (!accum_token.empty()) {
                    if (isalpha(accum_token.at(0))) {
                        working_stream_->get();
                        accum_token += cur;
                    } else {
                        this_token_ = MakeLongToken(accum_token);
                        accum_token.clear();
                        break;
                    }
                } else {
                    working_stream_->get();
                    if (!isdigit(working_stream_->peek())) {
                        this_token_ = SymbolToken("-");
                        accum_token.clear();
                        break;
                    } else {
                        accum_token += cur;
                    }
                }
            }
        }
        if (!accum_token.empty()) {
            this_token_ = MakeLongToken(accum_token);
            accum_token.clear();
        }
        if (std::get_if<NullToken>(&this_token_) != nullptr) {
            last_token_ = true;
        }
    }

    Token GetToken() {
        return this_token_;
    }

private:
    Token this_token_;
    std::istream* working_stream_;
    bool last_token_;
};
