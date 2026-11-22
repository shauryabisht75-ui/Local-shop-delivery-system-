// ============================================================
//  Shop Management Module  (Mayank Ranjan)
//  - Product class
//  - Shop class (inventory in a chained hash table)
//  - Stock add / update / delete / view
//  - Shop order desk (linked FIFO queue, first-come-first-served)
//  - Text-file save / load
// ============================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

// ---------- Validation helpers ----------
bool isNumber(const string &s) {
    if (s.empty()) return false;
    for (char c : s) if (c < '0' || c > '9') return false;
    return true;
}
bool validName(const string &s) {
    if (s.empty() || s.size() > 40) return false;
    for (char c : s)
        if (!(isalnum((unsigned char)c) || c == ' ' || c == '-')) return false;
    return true;
}

// ---------- Product ----------
class Product {
public:
    int id;
    string name;
    string category;
    Product(int i = 0, string n = "", string c = "") : id(i), name(n), category(c) {}
    virtual void display() const {
        cout << "  [" << id << "] " << name << " (" << category << ")\n";
    }
    virtual ~Product() {}
};

// ---------- Inventory item stored in the hash table ----------
struct StockNode {
    int productId;
    string name;
    double price;
    int qty;
    StockNode *next;          // chaining for collisions
    StockNode(int id, string n, double p, int q)
        : productId(id), name(n), price(p), qty(q), next(nullptr) {}
};

// ---------- Chained hash table (key = productId) ----------
class InventoryTable {
    static const int SIZE = 31;
    StockNode *table[SIZE];
    int count;
    int hashFn(int key) const { return key % SIZE; }

public:
    InventoryTable() : count(0) { for (int i = 0; i < SIZE; i++) table[i] = nullptr; }
    ~InventoryTable() { clear(); }

    void clear() {
        for (int i = 0; i < SIZE; i++) {
            StockNode *cur = table[i];
            while (cur) { StockNode *t = cur; cur = cur->next; delete t; }
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
        if (find(id)) return false;                  // duplicate id
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
                delete cur;
                count--;
                return true;
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

// ---------- Shop order desk: linked FIFO queue ----------
struct DeskOrder {
    int orderId;
    int productId;
    int qty;
    string status;            // PLACED, ACCEPTED, PACKED, REJECTED
    DeskOrder *next;
    DeskOrder(int o, int p, int q) : orderId(o), productId(p), qty(q), status("PLACED"), next(nullptr) {}
};

class OrderDesk {
    DeskOrder *front, *rear;
    int count;
public:
    OrderDesk() : front(nullptr), rear(nullptr), count(0) {}
    ~OrderDesk() { while (front) dequeue(); }

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
        delete t;
        count--;
    }
    bool empty() const { return front == nullptr; }
    int size() const { return count; }
    void display() const {
        if (!front) { cout << "  Order desk is empty.\n"; return; }
        for (DeskOrder *c = front; c; c = c->next)
            cout << "  Order #" << c->orderId << "  product " << c->productId
                 << "  qty " << c->qty << "  [" << c->status << "]\n";
    }
};

// ---------- Shop ----------
class Shop {
public:
    int shopId;
    string name;
    string area;
    InventoryTable inventory;
    OrderDesk desk;

    Shop(int id = 0, string n = "", string a = "") : shopId(id), name(n), area(a) {}

    // ----- stock operations -----
    bool addStock(int pid, const string &pname, double price, int qty, string &err) {
        if (!validName(pname)) { err = "Invalid product name"; return false; }
        if (price <= 0)        { err = "Price must be positive"; return false; }
        if (qty < 0)           { err = "Quantity cannot be negative"; return false; }
        if (!inventory.insert(pid, pname, price, qty)) { err = "Product id already exists"; return false; }
        return true;
    }

    bool updateStock(int pid, double newPrice, int newQty, string &err) {
        StockNode *n = inventory.find(pid);
        if (!n)             { err = "Product not found"; return false; }
        if (newPrice <= 0)  { err = "Price must be positive"; return false; }
        if (newQty < 0)     { err = "Quantity cannot be negative"; return false; }
        n->price = newPrice;
        n->qty = newQty;
        return true;
    }

    bool deleteStock(int pid, string &err) {
        if (!inventory.remove(pid)) { err = "Product not found"; return false; }
        return true;
    }

    void viewStock() const {
        cout << "Stock of " << name << " (" << area << "):\n";
        if (inventory.size() == 0) { cout << "  (no products)\n"; return; }
        inventory.forEach([](StockNode *n) {
            cout << "  [" << n->productId << "] " << n->name
                 << "  Rs." << n->price << "  qty " << n->qty << "\n";
        });
    }

    // ----- order desk -----
    // Stock is reserved at order time so two customers cannot buy the last item.
    bool receiveOrder(int orderId, int pid, int qty, string &err) {
        StockNode *n = inventory.find(pid);
        if (!n)          { err = "Product not found"; return false; }
        if (qty <= 0)    { err = "Invalid quantity"; return false; }
        if (n->qty < qty){ err = "Not enough stock"; return false; }
        n->qty -= qty;
        desk.enqueue(orderId, pid, qty);
        return true;
    }

    // Always works on the order at the FRONT (first-come-first-served)
    bool acceptNext(string &err) {
        DeskOrder *o = desk.peek();
        if (!o || o->status != "PLACED") { err = "No order waiting to be accepted"; return false; }
        o->status = "ACCEPTED";
        return true;
    }
    bool packNext(string &err) {
        DeskOrder *o = desk.peek();
        if (!o || o->status != "ACCEPTED") { err = "Front order is not accepted yet"; return false; }
        o->status = "PACKED";
        return true;
    }
    bool rejectNext(string &err) {
        DeskOrder *o = desk.peek();
        if (!o || o->status != "PLACED") { err = "No order waiting"; return false; }
        StockNode *n = inventory.find(o->productId);
        if (n) n->qty += o->qty;          // release the reserved stock
        desk.dequeue();
        return true;
    }
    // Hand a packed order over to dispatch (removes it from the desk)
    bool handToDispatch(int &orderId, string &err) {
        DeskOrder *o = desk.peek();
        if (!o || o->status != "PACKED") { err = "Front order is not packed yet"; return false; }
        orderId = o->orderId;
        desk.dequeue();
        return true;
    }

    // ----- file storage -----
    void save(const string &path) const {
        ofstream f(path);
        inventory.forEach([&](StockNode *n) {
            f << n->productId << '|' << n->name << '|' << n->price << '|' << n->qty << '\n';
        });
    }
    void load(const string &path) {
        ifstream f(path);
        if (!f) return;
        inventory.clear();
        string line;
        while (getline(f, line)) {
            stringstream ss(line);
            string a, b, c, d;
            getline(ss, a, '|'); getline(ss, b, '|'); getline(ss, c, '|'); getline(ss, d, '|');
            if (isNumber(a) && isNumber(d))
                inventory.insert(stoi(a), b, stod(c), stoi(d));
        }
    }
};

// ---------- Demo / console test ----------
int main() {
    Shop shop(1, "Sharma General Store", "Civil Lines");
    string err;

    cout << "--- Add stock ---\n";
    shop.addStock(101, "Milk 500ml", 30, 20, err);
    shop.addStock(102, "Bread", 40, 10, err);
    shop.addStock(103, "Eggs 12", 90, 5, err);
    if (!shop.addStock(101, "Duplicate", 10, 1, err)) cout << "Error: " << err << "\n";
    if (!shop.addStock(104, "Bad@Name", 10, 1, err)) cout << "Error: " << err << "\n";
    shop.viewStock();

    cout << "\n--- Update stock ---\n";
    shop.updateStock(102, 45, 12, err);
    shop.viewStock();

    cout << "\n--- Delete stock ---\n";
    shop.deleteStock(103, err);
    shop.viewStock();

    cout << "\n--- Orders (FIFO) ---\n";
    shop.receiveOrder(1, 101, 5, err);
    shop.receiveOrder(2, 102, 3, err);
    if (!shop.receiveOrder(3, 101, 100, err)) cout << "Error: " << err << "\n";
    shop.desk.display();

    shop.acceptNext(err);
    shop.packNext(err);
    int oid;
    if (shop.handToDispatch(oid, err)) cout << "Order #" << oid << " handed to dispatch\n";
    shop.desk.display();

    cout << "\n--- Save / Load ---\n";
    shop.save("inventory.txt");
    Shop shop2(2, "Reload test", "Civil Lines");
    shop2.load("inventory.txt");
    shop2.viewStock();
    return 0;
}
