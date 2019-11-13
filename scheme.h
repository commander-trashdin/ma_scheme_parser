#pragma once

#include <memory>
#include <string>
#include "/home/aun/SDA/shad-cpp0/scheme-parser/parser.h"
#include <functional>
#include <sstream>

std::vector<std::shared_ptr<Object>> ToVector(const std::shared_ptr<Object>& head,
                                              std::shared_ptr<Scope> scope);

class SchemeInterpretor {
public:
    SchemeInterpretor();

    ~SchemeInterpretor();

    std::shared_ptr<Object> Eval(std::shared_ptr<Object> in);

private:
    std::shared_ptr<Scope> global_scope_;
};

inline void PrintTo(const std::shared_ptr<Object>& obj, std::ostream* out);

inline std::string Print(const std::shared_ptr<Object>& obj);
