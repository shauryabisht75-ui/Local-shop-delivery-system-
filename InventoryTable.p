#pragma once
#include "Product.h"

struct StockNode {
    int productId;
    string name;
    double price;
    int qty;
    StockNode *next;
    StockNode(int id, string n, double p, int q)
        : productId(id), name(n), price(p), qty(q), next(nullptr) {}
};

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
