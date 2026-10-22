// ============================================================
// order_module.h  -  Order, OrderList (linked list), OrderQueue
// Part of: Order Management (Shaurya Bisht, 2510012399, Team Lead)
// Team ID: DSCPP-III-2026-T113
//
// DSA used:
//   Linked list : keeps every order and its status
//   Queue       : pending orders are accepted in FIRST-COME-FIRST-SERVED order
// Status flow   : Pending -> Accepted -> Delivered
// ============================================================
#ifndef ORDER_MODULE_H
#define ORDER_MODULE_H

#include <iostream>
#include <string>
using namespace std;

const int PENDING = 0;
const int ACCEPTED = 1;
const int DELIVERED = 2;
const int QUEUE_SIZE = 100;

// ---------- One order ----------
class Order
{
private:
    int id;
    int customerId;
    string customerName;
    string shopName;
    string productName;
    int qty;
    int total;
    string address;
    int location;      // area number of the customer (node of the delivery graph)
    int status;        // PENDING / ACCEPTED / DELIVERED
    int agentId;       // 0 = no agent assigned yet

public:
    Order(int customerId, string customerName, string shopName, string productName,
          int qty, int total, string address, int location)
    {
        this->id = 0;                  // real id is given by OrderList::add()
        this->customerId = customerId;
        this->customerName = customerName;
        this->shopName = shopName;
        this->productName = productName;
        this->qty = qty;
        this->total = total;
        this->address = address;
        this->location = location;
        this->status = PENDING;
        this->agentId = 0;
    }

    int getId() const { return id; }
    int getCustomerId() const { return customerId; }
    string getCustomerName() const { return customerName; }
    string getShopName() const { return shopName; }
    string getProductName() const { return productName; }
    int getQty() const { return qty; }
    int getTotal() const { return total; }
    string getAddress() const { return address; }
    int getLocation() const { return location; }
    int getStatus() const { return status; }
    int getAgentId() const { return agentId; }

    void setId(int newId) { id = newId; }
    void setStatus(int newStatus) { status = newStatus; }
    void setAgentId(int newAgentId) { agentId = newAgentId; }

    string getStatusText() const
    {
        if (status == PENDING)
            return "Pending";
        if (status == ACCEPTED)
            return "Accepted";
        return "Delivered";
    }
};

// ---------- Linked list of all orders ----------
class OrderList
{
private:
    struct Node
    {
        Order data;
        Node *next;
        Node(const Order &o) : data(o), next(NULL) {}
    };

    Node *head;
    Node *tail;
    int count;
    int lastId;

public:
    OrderList() : head(NULL), tail(NULL), count(0), lastId(0) {}

    OrderList(const OrderList &) = delete;
    OrderList &operator=(const OrderList &) = delete;

    ~OrderList()
    {
        while (head != NULL)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
        }
    }

    // adds an order at the end of the list and returns its new id
    int add(Order o)
    {
        lastId++;
        o.setId(lastId);
        Node *n = new Node(o);
        if (head == NULL)
        {
            head = n;
            tail = n;
        }
        else
        {
            tail->next = n;
            tail = n;
        }
        count++;
        return lastId;
    }

    int size() const { return count; }

    // get the order at position index (0 = first order)
    Order *get(int index)
    {
        if (index < 0 || index >= count)
            return NULL;
        Node *temp = head;
        for (int i = 0; i < index; i++)
            temp = temp->next;
        return &temp->data;
    }

    // search an order by its id
    Order *findById(int id)
    {
        Node *temp = head;
        while (temp != NULL)
        {
            if (temp->data.getId() == id)
                return &temp->data;
            temp = temp->next;
        }
        return NULL;
    }
};

// ---------- Queue of pending order ids (circular array) ----------
class OrderQueue
{
private:
    int data[QUEUE_SIZE];
    int front;
    int count;

public:
    OrderQueue() : front(0), count(0) {}

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == QUEUE_SIZE; }
    int freeSpace() const { return QUEUE_SIZE - count; }

    bool enqueue(int orderId)
    {
        if (isFull())
            return false;
        data[(front + count) % QUEUE_SIZE] = orderId;
        count++;
        return true;
    }

    // returns the order id at the front, or -1 if the queue is empty
    int dequeue()
    {
        if (isEmpty())
            return -1;
        int id = data[front];
        front = (front + 1) % QUEUE_SIZE;
        count--;
        return id;
    }
};

// Accept the oldest pending order: Pending -> Accepted
inline void acceptNextOrder(OrderList &orders, OrderQueue &pending)
{
    int id = pending.dequeue();
    if (id == -1)
    {
        cout << "No pending orders.\n";
        return;
    }
    Order *o = orders.findById(id);
    if (o == NULL)
    {
        cout << "Order not found.\n";
        return;
    }
    o->setStatus(ACCEPTED);
    cout << "Order #" << id << " (" << o->getQty() << " x " << o->getProductName()
         << ") accepted.\n";
}

#endif
