#pragma once
#include "Product.h"

struct DeskOrder {
    int orderId;
    int productId;
    int qty;
    string status;                 // PLACED, ACCEPTED, PACKED
    DeskOrder *next;
    DeskOrder(int o, int p, int q) : orderId(o), productId(p), qty(q), status("PLACED"), next(nullptr) {}
};

// Linked FIFO queue: first-come-first-served
class OrderDesk {
    DeskOrder *front, *rear;
    int count;
public:
    OrderDesk() : front(nullptr), rear(nullptr), count(0) {}
    ~OrderDesk() { while (front) dequeue(); }
    OrderDesk(const OrderDesk &) = delete;
    OrderDesk &operator=(const OrderDesk &) = delete;

    void enqueue(int orderId, int productId, int qty) {
        DeskOrder *n = new DeskOrder(orderId, productId, qty);
        if (!rear) front = rear = n; else { rear->next = n; rear = n; }
        count++;
    }
    DeskOrder *peek() const { return front; }
    void dequeue() {
        if (!front) return;
        DeskOrder *t = front;
        front = front->next;
        if (!front) rear = nullptr;
        delete t; count--;
    }
    bool empty() const { return front == nullptr; }
    int size() const { return count; }
    void display() const {
        if (!front) { cout << "  Order desk is empty.\n"; return; }
        int pos = 1;
        for (DeskOrder *c = front; c; c = c->next, pos++)
            cout << "  " << pos << ". Order #" << c->orderId << "  product " << c->productId
                 << "  qty " << c->qty << "  [" << c->status << "]\n";
    }
};
