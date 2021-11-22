#ifndef PYTHON_INTERPRETER_UTILS_H
#define PYTHON_INTERPRETER_UTILS_H

#include <string>

static int stringToInt(const std::string& str) {
    int ret = 0;
    for (const auto& ch : str) {
        ret = ret * 10 + ch - '0';
    }
    return ret;
}

static bool validateVarName(const std::string& str) {
    for (const auto& ch : str) {
        if (!(ch >= 'a' && ch <= 'z') && !(ch >= 'A' && ch <= 'Z')) {
            return false;
        }
    }
    return true;
}

#endif 