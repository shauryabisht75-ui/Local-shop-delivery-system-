// order.h - order queue and linked list (Shaurya Bisht)
#include <iostream>
using namespace std;

// status: 0 = Pending, 1 = Accepted, 2 = Delivered
struct Order {
    int id;
    char customer[30];
    char item[30];
    float price;
    int status;
    Order *next;
};

Order *head = NULL;      // first order of the linked list
Order *tail = NULL;      // last order of the linked list

Order *queueArr[100];    // queue of pending orders
int front = 0;
int rear = 0;

int orderCount = 0;      // used to give id to orders

void printStatus(int s) {
    if (s == 0)
        cout << "Pending";
    else if (s == 1)
        cout << "Accepted";
    else
        cout << "Delivered";
}

void placeOrder() {
    if (rear == 100) {
        cout << "Queue is full\n";
        return;
    }

    Order *n = new Order;
    orderCount++;
    n->id = orderCount;

    cout << "Enter customer name: ";
    cin >> n->customer;
    cout << "Enter item name: ";
    cin >> n->item;

    cout << "Enter price: ";
    cin >> n->price;
    while (cin.fail() || n->price <= 0) {
        cout << "Wrong price, enter again: ";
        cin.clear();
        cin.ignore(1000, '\n');
        cin >> n->price;
    }

    n->status = 0;
    n->next = NULL;

    // add in linked list
    if (head == NULL) {
        head = n;
        tail = n;
    } else {
        tail->next = n;
        tail = n;
    }

    // add in queue
    queueArr[rear] = n;
    rear++;

    cout << "Order placed, your order id is " << n->id << endl;
}

void acceptOrder() {
    if (front == rear) {
        cout << "No pending orders\n";
        return;
    }

    Order *n = queueArr[front];   // first order in queue
    front++;
    n->status = 1;
    cout << "Order " << n->id << " accepted\n";
}

Order *searchOrder(int id) {
    Order *temp = head;
    while (temp != NULL) {
        if (temp->id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void deliverOrder() {
    int id;
    cout << "Enter order id: ";
    cin >> id;
    while (cin.fail()) {
        cout << "Enter a number: ";
        cin.clear();
        cin.ignore(1000, '\n');
        cin >> id;
    }

    Order *n = searchOrder(id);
    if (n == NULL)
        cout << "Order not found\n";
    else if (n->status != 1)
        cout << "Order is not accepted yet\n";
    else {
        n->status = 2;
        cout << "Order " << id << " delivered\n";
    }
}

void trackOrder() {
    int id;
    cout << "Enter order id: ";
    cin >> id;
    while (cin.fail()) {
        cout << "Enter a number: ";
        cin.clear();
        cin.ignore(1000, '\n');
        cin >> id;
    }

    Order *n = searchOrder(id);
    if (n == NULL) {
        cout << "Order not found\n";
    } else {
        cout << "Order " << n->id << " (" << n->item << ") is ";
        printStatus(n->status);
        cout << endl;
    }
}

void showOrders() {
    if (head == NULL) {
        cout << "No orders yet\n";
        return;
    }
    Order *temp = head;
    while (temp != NULL) {
        cout << temp->id << "  " << temp->customer << "  " << temp->item
             << "  Rs." << temp->price << "  ";
        printStatus(temp->status);
        cout << endl;
        temp = temp->next;
    }
}
