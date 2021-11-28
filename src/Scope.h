#ifndef PYTHON_INTERPRETER_SCOPE_H
#define PYTHON_INTERPRETER_SCOPE_H

#include <map>
#include <string>
#include "BaseType.h"
#include <iostream>
using std::cout;
using std::endl;

class Scope {

    private:
        std::unordered_map<std::string, BaseType> varTable;

    public:
        Scope(): varTable() {}
        void varRegister(const std::string &varName, const BaseType &varData) {
            varTable[varName] = varData;
        }    

        std::pair<bool, BaseType> varQuery(const std::string& varName) const {
            const auto &it = varTable.find(varName);
            if (it == varTable.end()) return std::make_pair(false, BaseType());
            return std::make_pair(true, it->second);
        }
};

#endif 