#ifndef PYTHON_INTERPRETER_BASETYPE_H
#define PYTHON_INTERPRETER_BASETYPE_H

#include <string>
#include <iostream>
#include "BigInteger.h"
using std::string;

inline int max(const int &a, const int &b) {
    return a < b ? b : a;
}

class BaseType{
private:
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
    explicit operator bool() const {
        if (t == 1) return b;
        if (t == 2) return (bool) i;
        if (t == 3) return (bool) d;
        if (t == 4) return !s.empty();
    }
    explicit operator int2048() const { return i; }
    explicit operator double() const { return d; }
    explicit operator string() const { return s; }
    BaseType operator-() {
        if (t == 2) return BaseType(-i);
        if (t == 3) return BaseType(-d);
    }
    friend BaseType operator+(const BaseType &lhs, const BaseType &rhs) {
        int t = max(lhs.t, rhs.t);
        if (t == 1) return BaseType(bool((int2048) lhs + (int2048) rhs));
        if (t == 2) return BaseType((int2048) lhs + (int2048) rhs);
        if (t == 3) return BaseType((double) lhs + (double) rhs);
        if (t == 4) return BaseType((string) lhs + (string) rhs);
    }
    friend BaseType operator-(const BaseType &lhs, const BaseType &rhs) {
        int t = max(lhs.t, rhs.t);
        if (t == 1) return BaseType(bool((int2048) lhs - (int2048) rhs));
        if (t == 2) return BaseType((int2048) lhs - (int2048) rhs);
        if (t == 3) return BaseType((double) lhs - (double) rhs);
    }
    friend BaseType mul(const BaseType &lhs, const BaseType &rhs) {
        int t = max(lhs.t, rhs.t);
        if (t == 1) return BaseType(bool((bool) lhs * (bool) rhs));
        if (t == 2) return BaseType((int2048) lhs * (int2048) rhs);
        if (t == 3) return BaseType((double) lhs * (double) rhs);
        if (t == 4) {
            int k = (int) rhs.i;
            string t = lhs.s;
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
        const int &t = lhs.t;
        if (t == 1) return lhs.b < rhs.b;
        if (t == 2) return lhs.i < rhs.i;
        if (t == 3) return lhs.d < rhs.d;
        if (t == 4) return lhs.s < rhs.s;
    }
    friend bool operator>(const BaseType &lhs, const BaseType &rhs) { return rhs < lhs; }
    friend bool operator<=(const BaseType &lhs, const BaseType &rhs) { return !(rhs < lhs); }
    friend bool operator>=(const BaseType &lhs, const BaseType &rhs) { return !(lhs < rhs); }
    friend bool operator==(const BaseType &lhs, const BaseType &rhs) { return lhs <= rhs && rhs <= lhs; }
    friend bool operator!=(const BaseType &lhs, const BaseType &rhs) { return lhs < rhs || rhs < lhs; }
    void print(char ch = 0) {
        if (t == 0) printf("None");
        if (t == 1) printf(b ? "True" : "False");
        if (t == 2) std::cout << i;
        if (t == 3) printf("%.6lf", d);
        if (t == 4) std::cout << s;
        if (ch) putchar(ch);
    }
};

#endif