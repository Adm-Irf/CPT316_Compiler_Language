#pragma once
#include <string>
#include <unordered_map>

struct Symbol {
    std::string name;
    std::string type;
    int address;  // optional — can represent memory offset or index
};

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> table;
    int nextAddress = 0;
public:
    bool exists(const std::string& name) const;
    int insert(const std::string& name, const std::string& type);
    const Symbol* lookup(const std::string& name) const;
};
