#pragma once
#include "Product.h"

// ---------- Inventory item ----------
struct StockNode {
    int productId;
    string name;
    double price;
    int qty;
    StockNode *next;                 // chaining
    StockNode(int id, string n, double p, int q)
        : productId(id), name(n), price(p), qty(q), next(nullptr) {}
};

// ---------- Chained hash table: key = productId ----------
class InventoryTable {
    static const int SIZE = 31;
    StockNode *table[SIZE];
    int count;
    int hashFn(int key) const { return key % SIZE; }
public:
    InventoryTable() : count(0) { for (int i = 0; i < SIZE; i++) table[i] = nullptr; }
    ~InventoryTable() { clear(); }
    InventoryTable(const InventoryTable &) = delete;
    InventoryTable &operator=(const InventoryTable &) = delete;

    void clear() {
        for (int i = 0; i < SIZE; i++) {
            StockNode *c = table[i];
            while (c) { StockNode *t = c; c = c->next; delete t; }
            table[i] = nullptr;
        }
        count = 0;
    }
    StockNode *find(int id) const {
        for (StockNode *c = table[hashFn(id)]; c; c = c->next)
            if (c->productId == id) return c;
        return nullptr;
    }
    bool insert(int id, const string &name, double price, int qty) {
        if (find(id)) return false;
        StockNode *n = new StockNode(id, name, price, qty);
        int h = hashFn(id);
        n->next = table[h];
        table[h] = n;
        count++;
        return true;
    }
    bool remove(int id) {
        int h = hashFn(id);
        StockNode *cur = table[h], *prev = nullptr;
        while (cur) {
            if (cur->productId == id) {
                if (prev) prev->next = cur->next; else table[h] = cur->next;
                delete cur; count--; return true;
            }
            prev = cur; cur = cur->next;
        }
        return false;
    }
    int size() const { return count; }
    template <typename F> void forEach(F f) const {
        for (int i = 0; i < SIZE; i++)
            for (StockNode *c = table[i]; c; c = c->next) f(c);
    }
};

// ---------- Chained hash table: key = product name (case-insensitive) ----------
struct NameNode {
    string key;
    int productId;
    NameNode *next;
    NameNode(string k, int id) : key(k), productId(id), next(nullptr) {}
};

class NameTable {
    static const int SIZE = 31;
    NameNode *table[SIZE];
    unsigned hashFn(const string &s) const {
        unsigned h = 5381;
        for (char c : s) h = h * 33 + (unsigned char)c;
        return h % SIZE;
    }
public:
    NameTable() { for (int i = 0; i < SIZE; i++) table[i] = nullptr; }
    ~NameTable() { clear(); }
    NameTable(const NameTable &) = delete;
    NameTable &operator=(const NameTable &) = delete;

    void clear() {
        for (int i = 0; i < SIZE; i++) {
            NameNode *c = table[i];
            while (c) { NameNode *t = c; c = c->next; delete t; }
            table[i] = nullptr;
        }
    }
    int find(const string &name) const {            // returns productId or -1
        string k = toLower(name);
        for (NameNode *c = table[hashFn(k)]; c; c = c->next)
            if (c->key == k) return c->productId;
        return -1;
    }
    void insert(const string &name, int id) {
        string k = toLower(name);
        NameNode *n = new NameNode(k, id);
        unsigned h = hashFn(k);
        n->next = table[h];
        table[h] = n;
    }
    void remove(const string &name) {
        string k = toLower(name);
        unsigned h = hashFn(k);
        NameNode *cur = table[h], *prev = nullptr;
        while (cur) {
            if (cur->key == k) {
                if (prev) prev->next = cur->next; else table[h] = cur->next;
                delete cur; return;
            }
            prev = cur; cur = cur->next;
        }
    }
};
