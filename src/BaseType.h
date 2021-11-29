#ifndef PYTHON_INTERPRETER_BASETYPE_H
#define PYTHON_INTERPRETER_BASETYPE_H

#include <string>
#include <iostream>
#include "BigInteger.h"
#include <iomanip>
using std::string;
using std::cout;
using std::endl;

inline int max(const int &a, const int &b) {
    return a < b ? b : a;
}

class BaseType{
public:
    int t;
    bool b;
    int2048 i;
    double d;
    string s;
public:
    BaseType() { t = b = d = 0, s.clear(); }
    BaseType(bool _b) { t = 1, b = _b; }
    BaseType(int2048 _i) { t = 2, i = _i; }
    BaseType(double _d) { t = 3, d = _d; }
    BaseType(string _s) { t = 4, s = _s; }
    BaseType(int err, int _t) { t = _t; }
    bool isVar() { return t > 0; }
    bool isBreak() { return t == -2; }
    bool isContinue() { return t == -3; }
    bool isReturn() { return t == -4; }
    explicit operator bool() const {
        if (t == 0) return false;
        if (t == 1) return b;
        if (t == 2) return (bool) i;
        if (t == 3) return (bool) d;
        if (t == 4) return !s.empty();
    }
    explicit operator int2048() const {
        if (t == 1) return int2048(b ? 1 : 0);
        if (t == 2) return i;
        if (t == 3) {
            string res = std::to_string(d);
            res.resize(res.size() - 7);
            return int2048(res);
        }
        if (t == 4) return int2048(s);
    }
    explicit operator double() const {
        if (t == 1) return b;
        if (t == 2) return (double) i;
        if (t == 3) return d;
        if (t == 4) return stod(s);
    }
    explicit operator string() const {
        if (t == 1) return b ? "True" : "False";
        if (t == 2) return i.tostring();
        if (t == 3) return std::to_string(d);
        return s;
    }
    BaseType operator-() {
        if (t == 1) return BaseType((int2048)(-b));
        if (t == 2) return BaseType(-i);
        if (t == 3) return BaseType(-d);
    }
    friend BaseType operator+(const BaseType &lhs, const BaseType &rhs) {
        int t = max(lhs.t, rhs.t);
        if (t <= 2) return BaseType((int2048) lhs + (int2048) rhs);
        if (t == 3) return BaseType((double) lhs + (double) rhs);
        if (t == 4) return BaseType((string) lhs + (string) rhs);
    }
    friend BaseType operator-(const BaseType &lhs, const BaseType &rhs) {
        int t = max(lhs.t, rhs.t);
        if (t <= 2) return BaseType((int2048) lhs - (int2048) rhs);
        if (t == 3) return BaseType((double) lhs - (double) rhs);
    }
    friend BaseType mul(const BaseType &lhs, const BaseType &rhs) {
        int t = max(lhs.t, rhs.t);
        if (t <= 2) return BaseType((int2048) lhs * (int2048) rhs);
        if (t == 3) return BaseType((double) lhs * (double) rhs);
        if (t == 4) {
            int k = lhs.t == 4 ? (int) rhs.i : (int) lhs.i;
            string t = lhs.t == 4 ? lhs.s : rhs.s;
            string res;
            res.clear();
            while (k) {
                if (k & 1) res = res + t;
                t = t + t;
                k >>= 1;
            }
            return res;
        }
    }
    friend BaseType ddiv(const BaseType &lhs, const BaseType &rhs) {
        return BaseType((double) lhs / (double) rhs);
    }
    friend BaseType idiv(const BaseType &lhs, const BaseType &rhs) {
        return BaseType((int2048) lhs / (int2048) rhs);
    }
    friend BaseType mod(const BaseType &lhs, const BaseType &rhs) {
        return BaseType((int2048) lhs % (int2048) rhs);
    }
    friend bool operator<(const BaseType &lhs, const BaseType &rhs) {
        const int &t = max(lhs.t, rhs.t);
        if (!lhs.t && rhs.t) return false;
        if (lhs.t && !rhs.t) return true;
        if (!lhs.t && !rhs.t) return false;
        if (t == 1) return lhs.b < rhs.b;
        if (t == 2) return (int2048) lhs < (int2048) rhs;
        if (t == 3) return (double) lhs < (double) rhs;
        if (t == 4) return lhs.s < rhs.s;
    }
    friend bool operator>(const BaseType &lhs, const BaseType &rhs) { return rhs < lhs; }
    friend bool operator<=(const BaseType &lhs, const BaseType &rhs) { return !(rhs < lhs); }
    friend bool operator>=(const BaseType &lhs, const BaseType &rhs) { return !(lhs < rhs); }
    friend bool operator==(const BaseType &lhs, const BaseType &rhs) { return lhs <= rhs && rhs <= lhs; }
    friend bool operator!=(const BaseType &lhs, const BaseType &rhs) { return lhs < rhs || rhs < lhs; }
    void print(char ch = 0) {
        if (t == 0) std::cout << "None";
        if (t == 1) std::cout << (b ? "True" : "False");
        if (t == 2) std::cout << i;
        if (t == 3) std::cout << std::setprecision(6) << d;
        if (t == 4) std::cout << s;
        if (ch) putchar(ch);
    }
};

#endif