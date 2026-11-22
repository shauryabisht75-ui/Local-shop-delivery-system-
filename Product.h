#pragma once
#include <string>
#include <iostream>
#include <cctype>
using namespace std;

// ---------- Validation helpers ----------
inline bool isNumber(const string &s) {
    if (s.empty() || s.size() > 9) return false;
    for (char c : s) if (c < '0' || c > '9') return false;
    return true;
}
inline bool isPrice(const string &s) {            // digits with at most one dot
    if (s.empty() || s.size() > 10) return false;
    int dots = 0;
    for (char c : s) {
        if (c == '.') { if (++dots > 1) return false; }
        else if (c < '0' || c > '9') return false;
    }
    return s != ".";
}
inline bool validName(const string &s) {
    if (s.empty() || s.size() > 40) return false;
    for (char c : s)
        if (!(isalnum((unsigned char)c) || c == ' ' || c == '-')) return false;
    return true;
}
inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ---------- Product ----------
class Product {
public:
    int id;
    string name;
    string category;
    Product(int i = 0, string n = "", string c = "General") : id(i), name(n), category(c) {}
    virtual void display() const {
        cout << "  [" << id << "] " << name << " (" << category << ")\n";
    }
    virtual ~Product() {}
};
