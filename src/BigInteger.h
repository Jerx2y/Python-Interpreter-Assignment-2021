#ifndef PYTHON_INTERPRETER_BIGINTEGER_H
#define PYTHON_INTERPRETER_BIGINTEGER_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <complex>

#define base 1000000000

class int2048 {

private:
    bool opt;
    std::vector<long long> d;

public:
    int2048() { opt = 0; d.clear(); }
    int2048(long long x) {
        if (x < 0) x = -x, opt = 1;
        else opt = 0;
        while (x) {
            d.push_back(x % base);
            x /= base;
        }
    }
    int2048(const std::string &s) {
        opt = (s[0] == '-');
        d.clear();
        long long k = 1;
        for (int i = s.size() - 1, j = 0; i >= opt; --i, j = (j + 1) % 9, k *= 10) {
            if (!j) d.push_back(0), k = 1;
            d.back() += k * (s[i] - '0');
        }
        while (!d.empty() && !d.back()) d.pop_back();
        if (d.empty()) opt = 0;
    }
    int2048(const int2048 &rhs) {
        *this = rhs;
    }
    const int2048 operator-() const {
        int2048 tmp = *this;
        if (!tmp.d.empty()) tmp.opt ^= 1;
        return tmp;
    }
    inline friend bool operator==(const int2048 &lhs, const int2048 &rhs) {
        if (lhs.d.size() != rhs.d.size() || lhs.opt != rhs.opt)
            return false;
        for (int i = 0, sz = lhs.d.size(); i < sz; ++i)
            if (lhs.d[i] != rhs.d[i]) return false;
        return true;
    }
    inline friend bool operator!=(const int2048 &lhs, const int2048 &rhs) { return !(lhs == rhs); }
    inline friend bool operator<(const int2048 &lhs, const int2048 &rhs) {
        if (lhs.opt ^ rhs.opt)
            return lhs.opt > rhs.opt;
        if (lhs.d.size() != rhs.d.size()) 
            return (lhs.d.size() < rhs.d.size()) ^ lhs.opt;
        for (int i = lhs.d.size() - 1; i >= 0; --i)
            if (lhs.d[i] != rhs.d[i])
                return (lhs.d[i] < rhs.d[i]) ^ lhs.opt;
        return false;
    }
    inline friend bool operator>(const int2048 &lhs, const int2048 &rhs) { return rhs < lhs; }
    inline friend bool operator<=(const int2048 &lhs, const int2048 &rhs) { return !(rhs < lhs); }
    inline friend bool operator>=(const int2048 &lhs, const int2048 &rhs) { return !(lhs < rhs); }

    void read(const std::string &s) {
        *this = int2048(s);
    }
    std::string tostring() const {
        if (d.empty()) return "0";
        std::string res;
        if (opt) res = "-";
        for (int i = d.size() - 1; i >= 0; --i) {
            char tmp[12];
            sprintf(tmp, i == d.size() - 1 ? "%lld" : "%09lld", d[i]);
            for (char *p = tmp; *p; ++p)
                res += *p;
        }
        return res;
    }
    void print(char ch = 0) const {
        std::cout << tostring();
        if (ch) putchar(ch);
    }

    static int2048 add(int2048 lhs, const int2048 &rhs) {
        if (lhs.opt ^ rhs.opt)
            return !lhs.opt ? minus(lhs, -rhs) : minus(rhs, -lhs);
        int len = std::max(lhs.d.size(), rhs.d.size()) + 1;
        lhs.d.resize(len);
        for (int i = 0; i < len - 1; ++i) {
            lhs.d[i] += (i < rhs.d.size() ? rhs.d[i] : 0);
            if (lhs.d[i] >= base) {
                lhs.d[i + 1] += lhs.d[i] / base;
                lhs.d[i] %= base;
            }
        }
        while (!lhs.d.empty() && !lhs.d.back()) lhs.d.pop_back();
        return lhs;
    }
    int2048 &add(const int2048 &rhs) {
        *this = add(*this, rhs);
        return *this;
    }

    static int2048 minus(int2048 lhs, int2048 rhs) {
        if (lhs.opt ^ rhs.opt) 
            return add(lhs, -rhs);
        const int &opt = (lhs < rhs);
        if (opt ^ rhs.opt) 
            std::swap(lhs, rhs);
        lhs.opt = opt;
        for (int i = 0; i < lhs.d.size(); ++i) {
            lhs.d[i] -= (i < rhs.d.size() ? rhs.d[i] : 0);
            while (lhs.d[i] < 0)
                lhs.d[i] += base, lhs.d[i + 1]--;
        }
        while (!lhs.d.empty() && !lhs.d.back())
            lhs.d.pop_back();
        return lhs;
    }
    int2048 &minus(const int2048 &rhs) {
        *this = minus(*this, rhs);
        return *this;
    }

    int2048 &operator+=(const int2048 & rhs) { add(rhs); return *this; }
    friend int2048 operator+(const int2048 &lhs, const int2048 &rhs) { return add(lhs, rhs); }

    int2048 &operator-=(const int2048 & rhs) { minus(rhs); return *this; }
    friend int2048 operator-(const int2048 &lhs, const int2048 &rhs) { return minus(lhs, rhs); }

    friend int2048 operator*(const int2048 &lhs, const int2048 &rhs) {
        int2048 res;
        res.opt = lhs.opt ^ rhs.opt;
        res.d.resize(lhs.d.size() + rhs.d.size());
        for (int i = 0, szi = lhs.d.size(); i < szi; ++i)
            for (int j = 0, szj = rhs.d.size(); j < szj; ++j) {
                res.d[i + j] += lhs.d[i] * rhs.d[j];
                if (res.d[i + j] >= base) {
                    res.d[i + j + 1] += res.d[i + j] / base;
                    res.d[i + j] %= base;
                }
            }
        while (!res.d.empty() && !res.d.back())
            res.d.pop_back();
        return res;
    }

    friend int2048 operator*(const int2048 &lhs, long long rhs) {
        if (rhs >= base || rhs <= -base) {
            int2048 tmp(rhs);
            return tmp * lhs;
        }

        int2048 res;
        res.opt = lhs.opt ^ (rhs < 0);
        if (rhs < 0) rhs = -rhs;

        res.d.resize(lhs.d.size() + 1);
        for (int i = res.d.size() - 1; i >= 0; --i)
            res.d[i] = 0;
        for (int i = 0, szi = lhs.d.size(); i < szi; ++i) {
            res.d[i] += lhs.d[i] * rhs;
            if (res.d[i] >= base) {
                res.d[i + 1] += res.d[i] / base;
                res.d[i] %= base;
            }
        }

        while (!res.d.empty() && !res.d.back()) res.d.pop_back(); 

        return res;
    }

    int2048 &operator*=(const int2048 &rhs) {
        *this = *this * rhs;
        return *this;
    }

    void shift(int x) {
        d.resize(d.size() + 1);
        for (int i = d.size() - 1; i; --i)
            d[i] = d[i - 1];
        d[0] = x;
    }


    friend void div(const int2048 &lhs, const int2048 &rhs, int2048 &rem, int2048 &res) {
        int rhs_l = rhs.d.back() + 1, rhs_r = rhs.d.back();
        for (int i = lhs.d.size()  - 1; i >= 0; --i) {
            rem.shift(lhs.d[i]);
            if (rem < rhs) {
                res.d.push_back(0);
                continue;
            }
            int l, r;
            if (rem.d.size() == rhs.d.size()) {
                l = rem.d.back() / rhs_l;
                r = (rem.d.back() + 1) / rhs_r;
            } else {
                l = (1ll * rem.d.back() * base + rem.d[rem.d.size() - 2]) / rhs_l;
                r = (1ll * rem.d.back() * base + rem.d[rem.d.size() - 2] + 1) / rhs_r;
            }
            if (l < 1) l = 1;
            if (r >= base) r = base - 1;
            while (l < r) {
                int mid = (l + r + 1) >> 1;
                if (rhs * int2048(mid) <= rem) 
                    l = mid;
                else r = mid - 1;
            }
            res.d.push_back(l);
            rem -= rhs * int2048(l);
        }
        std::reverse(res.d.begin(), res.d.end());
        while (!res.d.empty() && !res.d.back()) res.d.pop_back();
    }
    friend int2048 operator/(const int2048 &lhs, const int2048 &rhs) {
        int2048 rem, res;
        if (!lhs.opt && !rhs.opt) {
            div(lhs, rhs, rem, res);
            return res;
        } else if (lhs.opt && rhs.opt) {
            div(-lhs, -rhs, rem, res);
            return res;
        } else if (!lhs.opt && rhs.opt) {
            div(lhs, -rhs, rem, res);
            if (rem.d.size()) res = res + int2048(1);
            return -res;
        } else {
            div(-lhs, rhs, rem, res);
            if (rem.d.size()) res = res + int2048(1);
            return -res;
        }
    }
    friend int2048 operator%(const int2048 &lhs, const int2048 &rhs) {
        int2048 rem, res;
        if (!lhs.opt && !rhs.opt) {
            div(lhs, rhs, rem, res);
            return rem;
        } else if (lhs.opt && !rhs.opt) {
            div(-lhs, rhs, rem, res);
            if (rem.d.size())
                rem = rhs - rem;
            return rem;
        } else if (!lhs.opt && rhs.opt) {
            div(lhs, -rhs, rem, res);
            if (rem.d.size())
                rem = rhs + rem;
            return rem;
        } else {
            div(-lhs, -rhs, rem, res);
            return -rem;
        }
    }
    int2048 &operator/=(const int2048 &rhs) {
        *this = *this / rhs;
        return *this;
    }

    friend std::istream &operator>>(std::istream &lhs, int2048 &rhs) {
        static std::string s;
        lhs >> s;
        rhs = int2048(s);
        return lhs;
    }
    friend std::ostream &operator<<(std::ostream &lhs, const int2048 &rhs) {
        rhs.print();
        return lhs;
    }
    explicit operator bool() const {
        return !d.empty();
    }
    explicit operator int() const {
        return d.size() ? (opt ? -d[0] : d[0]) : 0;
    }
    explicit operator double() const {
        double res = 0;
        for (auto x : d)
            res = res * base + x;
        if (opt) res = -res;
        return res;
    }
};

static int2048 add(int2048 lhs, const int2048 &rhs) { lhs.add(rhs); return lhs; }
static int2048 minus(int2048 lhs, const int2048 &rhs) { lhs.minus(rhs); return lhs; }

#endif