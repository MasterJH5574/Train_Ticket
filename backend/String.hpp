#include <cstring>
#include <cstdio>
#include <iostream>

#ifndef _STRING
#define _STRING

template<int len>
class String {
private:
    char str[len + 1];

public:
    String() {
        str[0] = '\0';
    }
    String(char* c) {
        if (c == NULL) {
            str[0] = '\0';
        }
        else {
            int i = 0;
            for (; c[i] != '\0'; ++i) {
                str[i] = c[i];
            }
            str[i] = '\0';
        }
    }
    String(const String& other) {
        int i = 0;
        for (; other.str[i] != '\0'; ++i) {
            str[i] = other.str[i];
        }
        str[i] = '\0';
    }
    String(const std::string& other) {
        int i;
        for (i = 0; i < other.length(); ++i)
            str[i] = other[i];
        str[i] = '\0';
    }
    String operator= (const String & other) {
        if (&other == this) return *this;
        int i = 0;
        for (; other.str[i] != '\0'; ++i) {
            str[i] = other.str[i];
        }
        str[i] = '\0';
        return *this;
    }
    String operator= (const char* c) {
        int i = 0;
        for (; c[i] != '\0'; ++i) {
            str[i] = c[i];
        }
        str[i] = '\0';
        return *this;
    }
    bool operator < (const String & other) const {
        int la = strlen(str), lb = strlen(other.str), i = 0;
        for (int i = 0; i < la && i < lb; ++i) {
            if (str[i] < other.str[i]) return 1;
            if (str[i] > other.str[i]) return 0;
        }
        return (la < lb);
    }
    bool operator > (const String & other) const {
        int la = strlen(str), lb = strlen(other.str), i = 0;
        for (int i = 0; i < la && i < lb; ++i) {
            if (str[i] > other.str[i]) return 1;
            if (str[i] < other.str[i]) return 0;
        }
        return (la > lb);
    }
    bool operator <= (const String & other) const {
        return !(this->operator>(other));
    }
    bool operator >= (const String & other) const {
        return !(this->operator<(other));
    }
    bool operator == (const String & other) const {
        int la = strlen(str), lb = strlen(other.str), i = 0;
        if (la != lb) return 0;
        for (int i = 0; i < la; ++i) {
            if (str[i] != other.str[i]) return 0;
        }
        return 1;
    }
    bool operator != (const String & other) const {
        return !(this->operator==(other));
    }
    char& operator[] (size_t index) {
        return str[index];
    }
    const char& operator[] (size_t index) const {
        return str[index];
    }
    void Reverse(int pos) {
        reverse(str, str + pos);
    }
    friend ostream& operator << (ostream & os, const String & obj) {
        os << obj.str;
        return os;
    }
    friend istream& operator >> (istream & in, String & obj) {
        in >> obj.str;
        return in;
    }
};

typedef String<10> catalogList; typedef char Catalog; typedef String<20> location; typedef String<20> trainID;
typedef String<40> trainName; typedef String<20> Seat; typedef String<40> userName; typedef String<20> password;
typedef String<20> email; typedef String<20> phone; typedef long long userID; typedef String<5> Time;

int turnToMinute(const Time & t) {
    int h = ((int)(t[0] - '0') * 10 + (int)(t[1] - '0'));
    int m = ((int)(t[3] - '0') * 10 + (int)(t[4] - '0'));
    return h * 60 + m;
}

int operator-(const Time & a, const Time & b) {
    return turnToMinute(a) - turnToMinute(b);
}

template<int la, int lb>
String<la + lb> operator + (const String<la> & a, const String<lb> & b) {
    String <la + lb> c;
    for (int i = 0; i < la; ++i) c[i] = a[i];
    for (int i = 0; i < lb; ++i) c[la + i] = b[i];
    c[la + lb] = '\0';
    return c;
}

template<int len>
String<len> intToString(int num) {
    String<len> obj;
    int current_len = 0;
    while (num > 0) {
        obj[current_len++] = num % 10 + '0';
        num /= 10;
    }
    obj.Reverse(current_len);
    obj[current_len] = '\0';
    return obj;
}

#endif