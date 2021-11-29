#ifndef PYTHON_INTERPRETER_UTILS_H
#define PYTHON_INTERPRETER_UTILS_H

#include <string>
#include "BigInteger.h"

static bool validateVarName(const std::string& str) {
    for (const auto& ch : str) {
        if (!(ch >= 'a' && ch <= 'z') && !(ch >= 'A' && ch <= 'Z')) {
            return false;
        }
    }
    return true;
}


static void getAugassign(BaseType &lhs, const BaseType &rhs, const int &opt) {
    if (opt == 1) lhs = lhs + rhs;
    if (opt == 2) lhs = lhs - rhs;
    if (opt == 3) lhs = mul(lhs, rhs);
    if (opt == 4) lhs = ddiv(lhs, rhs);
    if (opt == 5) lhs = idiv(lhs, rhs);
    if (opt == 6) lhs = mod(lhs, rhs);
}

static bool mycmp(const BaseType &lhs, const BaseType &rhs, const int &opt) {
    if (opt == 1) return lhs < rhs;
    if (opt == 2) return lhs > rhs;
    if (opt == 3) return lhs == rhs;
    if (opt == 4) return lhs >= rhs;
    if (opt == 5) return lhs <= rhs;
    if (opt == 6) return lhs != rhs;
    // '<'|'>'|'=='|'>='|'<=' | '!='
}

static std::pair<bool, double> stringToDouble(const string &number) { // TODO: Utils.h
    int idx = -1, sz = number.size();
    for (int i = 0; i < sz; ++i)
        if (number[i] == '.') idx = i;
    if (idx == -1) return std::make_pair(false, 0);
    double res = 0;
    for (int i = (number[0] == '-'); i < idx; ++i)
        res = res * 10 + number[i] - '0';

    double tmp = 0.1;
    for (int i = idx + 1; i < sz; ++i) {
        res += tmp * (number[i] - '0');
        tmp /= 10.0;
    }

    if (number[0] == '-') res = -res;
    return std::make_pair(true, res);
}


#endif 