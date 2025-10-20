#include "symbol_table.hpp"

bool SymbolTable::exists(const std::string &name) const
{
    return table.find(name) != table.end();
}

int SymbolTable::insert(const std::string &name, const std::string &type)
{
    if (!exists(name))
    {
        table[name] = {name, type, nextAddress++};
    }
    return table[name].address;
}

const Symbol *SymbolTable::lookup(const std::string &name) const
{
    auto it = table.find(name);
    return (it != table.end()) ? &it->second : nullptr; // returns the address of the token in the symbol table if found else return nullptr
}
