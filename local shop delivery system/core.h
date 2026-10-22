// ============================================================
// Local Shop Delivery System (LocalKart) - COMBINED SINGLE FILE
// Team ID: DSCPP-III-2026-T113
// Shaurya Bisht, Mayank Ranjan, Raj Vardhan, Aradhay Singh
// Compile: g++ -std=c++11 local_shop_delivery_combined.cpp -o localkart
// ============================================================

// ######## from common_input.h ########
// ============================================================
// common_input.h  -  safe input helpers used by every module
// Part of: Customer module (Raj Vardhan, 2510010483)
// Team ID: DSCPP-III-2026-T113
// ============================================================

#include <iostream>
#include <string>
#include <cctype>    // isdigit, tolower
#include <cstdlib>   // exit
using namespace std;

// Reads a whole number between low and high. Keeps asking until valid.
inline int getNumber(string message, int low, int high)
{
    while (true)
    {
        cout << message;
        string s;
        if (!getline(cin, s))
        {
            cout << "\nInput ended. Bye!\n";
            exit(0);
        }
        try
        {
            if (s.empty() || s.size() > 6)
                throw 1;
            int x = 0;
            for (int i = 0; i < (int)s.size(); i++)
            {
                if (!isdigit((unsigned char)s[i]))
                    throw 1;
                x = x * 10 + (s[i] - '0');
            }
            if (x < low || x > high)
                throw 2;
            return x;
        }
        catch (int code)
        {
            if (code == 1)
                cout << "Please type digits only (a whole number).\n";
            else
                cout << "Number must be between " << low << " and " << high << ".\n";
        }
    }
}

// Reads a line of text (spaces allowed). It cannot be empty.
inline string getText(string message)
{
    string s;
    while (true)
    {
        cout << message;
        if (!getline(cin, s))
        {
            cout << "\nInput ended. Bye!\n";
            exit(0);
        }
        for (int i = 0; i < (int)s.size(); i++)
        {
            if (s[i] != ' ')
                return s;
        }
        cout << "It cannot be empty. Try again.\n";
    }
}

// Converts a string to lower case (used for searching product names)
inline string toLower(string s)
{
    for (int i = 0; i < (int)s.size(); i++)
        s[i] = tolower((unsigned char)s[i]);
    return s;
}


// ######## from order_module.h ########
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
    int size() const { return count; }   // added for web version

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


// ######## from user_agent_module.h ########
// ============================================================
// user_agent_module.h  -  User base class, DeliveryAgent,
//                         DeliveryManager (graph + Dijkstra)
// Part of: Delivery Agent module (Aradhay Singh, 2510011780)
// Team ID: DSCPP-III-2026-T113
//
// Concepts used: classes, inheritance, virtual functions,
//                vector, priority_queue, weighted graph, Dijkstra
// ============================================================

#include <vector>
#include <queue>        // priority_queue for Dijkstra

const int INF = 1000000000;

// ---------- Areas of the town (nodes of the graph) ----------
const int AREA_COUNT = 6;
const string AREA_NAMES[AREA_COUNT] = {
    "Main Market", "Bus Stand", "Temple Road",
    "School Road", "Hill Colony", "River Side"};

inline void showAreas()
{
    for (int i = 0; i < AREA_COUNT; i++)
        cout << "  " << i << ". " << AREA_NAMES[i] << "\n";
}

// ---------- Base class for every kind of user ----------
class User
{
protected:
    int id;
    string name;

public:
    User(int id, string name)
    {
        this->id = id;
        this->name = name;
    }

    int getId() const { return id; }
    string getName() const { return name; }

    virtual void display() const
    {
        cout << "ID: " << id << "\nName: " << name << "\n";
    }

    virtual void showMenu() {}      // each user type shows its own menu

    virtual ~User() {}
};

// ---------- Delivery agent ----------
class DeliveryAgent : public User
{
public:
    int location;
    bool available;

    DeliveryAgent(int id, string name, int location) : User(id, name)
    {
        this->location = location;
        available = true;
    }

    void display() const override
    {
        User::display();
        cout << "Location: " << AREA_NAMES[location]
             << "\nStatus: " << (available ? "Available" : "Busy") << "\n";
    }
};

// ---------- Handles agents, the town map and delivery ----------
class DeliveryManager
{
private:
    vector<vector<pair<int, int> > > graph;   // graph[u] = list of (v, distance)
    vector<DeliveryAgent> agents;
    OrderList &orders;
    int nextAgentId;

    // shortest distance from source to every area
    vector<int> dijkstra(int source)
    {
        vector<int> dist(AREA_COUNT, INF);
        priority_queue<pair<int, int>, vector<pair<int, int> >,
                       greater<pair<int, int> > > pq;

        dist[source] = 0;
        pq.push(make_pair(0, source));

        while (!pq.empty())
        {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();
            if (d != dist[u])
                continue;
            for (int i = 0; i < (int)graph[u].size(); i++)
            {
                int v = graph[u][i].first;
                int w = graph[u][i].second;
                if (dist[u] + w < dist[v])
                {
                    dist[v] = dist[u] + w;
                    pq.push(make_pair(dist[v], v));
                }
            }
        }
        return dist;
    }

    DeliveryAgent *findAgent(int agentId)
    {
        for (int i = 0; i < (int)agents.size(); i++)
        {
            if (agents[i].getId() == agentId)
                return &agents[i];
        }
        return NULL;
    }

public:
    DeliveryManager(OrderList &orders) : orders(orders), nextAgentId(1)
    {
        graph.resize(AREA_COUNT);
    }

    void addRoad(int u, int v, int distance)
    {
        graph[u].push_back(make_pair(v, distance));
        graph[v].push_back(make_pair(u, distance));
    }

    // fixed map of the town (distance in km)
    void loadDefaultMap()
    {
        addRoad(0, 1, 4);
        addRoad(0, 2, 2);
        addRoad(1, 3, 5);
        addRoad(2, 3, 3);
        addRoad(2, 4, 6);
        addRoad(3, 5, 4);
        addRoad(4, 5, 2);
    }

    int addAgent(string name, int location)
    {
        agents.push_back(DeliveryAgent(nextAgentId, name, location));
        nextAgentId++;
        return nextAgentId - 1;
    }

    void registerAgent()
    {
        string name = getText("Enter agent name: ");
        cout << "Areas:\n";
        showAreas();
        int location = getNumber("Enter agent's current area number: ", 0, AREA_COUNT - 1);
        int agentId = addAgent(name, location);
        cout << "Agent registered successfully. Agent ID: " << agentId << "\n";
    }

    void viewAgents() const
    {
        if (agents.empty())
        {
            cout << "No delivery agents registered.\n";
            return;
        }
        for (int i = 0; i < (int)agents.size(); i++)
        {
            cout << "\n--------------------\n";
            agents[i].display();
        }
    }

    // Accepted order -> find the nearest free agent with Dijkstra
    void assignAgent()
    {
        if (agents.empty())
        {
            cout << "Register an agent first.\n";
            return;
        }

        bool any = false;
        cout << "\nAccepted orders waiting for an agent:\n";
        for (int i = 0; i < orders.size(); i++)
        {
            Order *o = orders.get(i);
            if (o->getStatus() == ACCEPTED && o->getAgentId() == 0)
            {
                cout << "Order #" << o->getId() << " | " << o->getCustomerName()
                     << " | " << o->getQty() << " x " << o->getProductName()
                     << " | Area: " << AREA_NAMES[o->getLocation()] << "\n";
                any = true;
            }
        }
        if (!any)
        {
            cout << "None. (Orders must be accepted by the shopkeeper first.)\n";
            return;
        }

        int orderId = getNumber("Enter order ID to assign (0 to go back): ", 0, 999999);
        if (orderId == 0)
            return;
        Order *o = orders.findById(orderId);
        if (o == NULL || o->getStatus() != ACCEPTED || o->getAgentId() != 0)
        {
            cout << "That order is not waiting for an agent.\n";
            return;
        }

        vector<int> dist = dijkstra(o->getLocation());
        int best = -1;
        int bestDistance = INF;
        for (int i = 0; i < (int)agents.size(); i++)
        {
            if (agents[i].available && dist[agents[i].location] < bestDistance)
            {
                bestDistance = dist[agents[i].location];
                best = i;
            }
        }

        if (best == -1)
        {
            cout << "No available agent can reach this location.\n";
            return;
        }

        agents[best].available = false;      // one order per agent at a time
        o->setAgentId(agents[best].getId());

        cout << "\nDelivery assigned successfully!\n";
        cout << "Order ID: " << o->getId() << "\n";
        cout << "Customer: " << o->getCustomerName() << "\n";
        cout << "Agent: " << agents[best].getName() << " (ID " << agents[best].getId() << ")\n";
        cout << "Agent distance from customer: " << bestDistance << " km\n";
    }

    void viewDeliveries()
    {
        if (orders.size() == 0)
        {
            cout << "No orders available.\n";
            return;
        }
        for (int i = 0; i < orders.size(); i++)
        {
            Order *o = orders.get(i);
            cout << "\n--------------------\n";
            cout << "Order ID: " << o->getId() << "\n";
            cout << "Customer: " << o->getCustomerName() << "\n";
            cout << "Item: " << o->getQty() << " x " << o->getProductName()
                 << " (" << o->getShopName() << ")\n";
            cout << "Area: " << AREA_NAMES[o->getLocation()] << "\n";
            if (o->getAgentId() != 0)
                cout << "Agent ID: " << o->getAgentId() << "\n";
            else
                cout << "Agent: not assigned\n";
            cout << "Status: " << o->getStatusText() << "\n";
        }
    }

    // Accepted -> Delivered, agent becomes free again
    void completeDelivery()
    {
        int orderId = getNumber("Enter order ID (0 to go back): ", 0, 999999);
        if (orderId == 0)
            return;
        Order *o = orders.findById(orderId);
        if (o == NULL)
        {
            cout << "Order not found.\n";
            return;
        }
        if (o->getStatus() != ACCEPTED || o->getAgentId() == 0)
        {
            cout << "This order is not out for delivery.\n";
            return;
        }

        o->setStatus(DELIVERED);
        DeliveryAgent *agent = findAgent(o->getAgentId());
        if (agent != NULL)
        {
            agent->available = true;
            agent->location = o->getLocation();   // agent is now at the customer's area
        }
        cout << "Order " << o->getId() << " status updated to Delivered.\n";
        cout << "Delivery agent is now available.\n";
    }


    // ---------- added for the web version (no cin/cout prompts) ----------
    string agentsJson()
    {
        string s = "[";
        for (int i = 0; i < (int)agents.size(); i++)
        {
            if (i) s += ",";
            s += "{\"id\":" + to_string(agents[i].getId()) + ",\"name\":\"" + agents[i].getName() +
                 "\",\"area\":\"" + AREA_NAMES[agents[i].location] + "\",\"available\":" +
                 (agents[i].available ? "true" : "false") + "}";
        }
        return s + "]";
    }

    // returns: 0 ok, 1 no agents, 2 order not waiting, 3 no free agent
    int assignNearest(int orderId, string &agentName, int &distance)
    {
        if (agents.empty()) return 1;
        Order *o = orders.findById(orderId);
        if (o == NULL || o->getStatus() != ACCEPTED || o->getAgentId() != 0) return 2;
        vector<int> dist = dijkstra(o->getLocation());
        int best = -1, bestDistance = INF;
        for (int i = 0; i < (int)agents.size(); i++)
            if (agents[i].available && dist[agents[i].location] < bestDistance)
            {
                bestDistance = dist[agents[i].location];
                best = i;
            }
        if (best == -1) return 3;
        agents[best].available = false;
        o->setAgentId(agents[best].getId());
        agentName = agents[best].getName();
        distance = bestDistance;
        return 0;
    }

    // returns: 0 ok, 1 not found, 2 not out for delivery
    int finishDelivery(int orderId)
    {
        Order *o = orders.findById(orderId);
        if (o == NULL) return 1;
        if (o->getStatus() != ACCEPTED || o->getAgentId() == 0) return 2;
        o->setStatus(DELIVERED);
        DeliveryAgent *agent = findAgent(o->getAgentId());
        if (agent != NULL)
        {
            agent->available = true;
            agent->location = o->getLocation();
        }
        return 0;
    }

    void menu()
    {
        while (true)
        {
            cout << "\n===== DELIVERY AGENT MENU =====\n";
            cout << "1. Register delivery agent\n";
            cout << "2. View delivery agents\n";
            cout << "3. Assign nearest agent to an order\n";
            cout << "4. View all orders\n";
            cout << "5. Complete delivery\n";
            cout << "0. Back\n";
            int choice = getNumber("Enter choice: ", 0, 5);

            if (choice == 1)
                registerAgent();
            else if (choice == 2)
                viewAgents();
            else if (choice == 3)
                assignAgent();
            else if (choice == 4)
                viewDeliveries();
            else if (choice == 5)
                completeDelivery();
            else
                break;
        }
    }
};


// ######## from shop_module.h ########
// ============================================================
// shop_module.h  -  HashTable, Product, Shop, Shopkeeper
// Part of: Shop Management (Mayank Ranjan, 2510011761)
// Team ID: DSCPP-III-2026-T113
//
// DSA used: hash table (index = id % SIZE, linear probing)
// state[i] : 0 = empty, 1 = filled, 2 = deleted
// ============================================================


const int PRODUCT_SIZE = 50;   // max products in one shop
const int SHOP_SIZE = 20;      // max shops in the system

// ---------- Hash table (works for Product and Shop) ----------
// T must have a default constructor and a getId() function
template <class T, int SIZE>
class HashTable
{
private:
    T items[SIZE];
    int state[SIZE];
    int count;

    int hashFunction(int id) const { return id % SIZE; }

    // position of an id, -1 if not found
    int findIndex(int id)
    {
        int i = hashFunction(id);
        for (int k = 0; k < SIZE; k++)
        {
            if (state[i] == 0)
                return -1;                       // empty place, so it is not here
            if (state[i] == 1 && items[i].getId() == id)
                return i;                        // found
            i = (i + 1) % SIZE;                  // check next place
        }
        return -1;
    }

public:
    HashTable() : count(0)
    {
        for (int i = 0; i < SIZE; i++)
            state[i] = 0;
    }

    // false if the id already exists or the table is full
    bool insert(const T &item)
    {
        if (findIndex(item.getId()) != -1)
            return false;
        if (count == SIZE)
            return false;
        int i = hashFunction(item.getId());
        while (state[i] == 1)
            i = (i + 1) % SIZE;
        items[i] = item;
        state[i] = 1;
        count++;
        return true;
    }

    T *find(int id)
    {
        int i = findIndex(id);
        if (i == -1)
            return NULL;
        return &items[i];
    }

    bool remove(int id)
    {
        int i = findIndex(id);
        if (i == -1)
            return false;
        state[i] = 2;                            // mark as deleted
        count--;
        return true;
    }

    int size() const { return count; }
    int capacity() const { return SIZE; }

    // item stored at a slot, NULL if the slot is empty/deleted (used to loop over all items)
    T *getAt(int slot)
    {
        if (slot < 0 || slot >= SIZE || state[slot] != 1)
            return NULL;
        return &items[slot];
    }
};

// ---------- Product ----------
class Product
{
private:
    int id;
    string name;
    int price;
    int stock;

public:
    Product() : id(0), name(""), price(0), stock(0) {}

    Product(int id, string name, int price, int stock)
    {
        this->id = id;
        this->name = name;
        this->price = price;
        this->stock = stock;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    int getPrice() const { return price; }
    int getStock() const { return stock; }

    void setDetails(string newName, int newPrice, int newStock)
    {
        name = newName;
        price = newPrice;
        stock = newStock;
    }

    void reduceStock(int qty) { stock -= qty; }

    void display() const
    {
        cout << id << "\t" << name << "\t" << price << "\t" << stock << endl;
    }
};

// ---------- Shop ----------
class Shop
{
private:
    int id;
    string name;
    string owner;
    string area;
    HashTable<Product, PRODUCT_SIZE> products;

public:
    Shop() : id(0), name(""), owner(""), area("") {}

    Shop(int id, string name, string owner, string area)
    {
        this->id = id;
        this->name = name;
        this->owner = owner;
        this->area = area;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    string getOwner() const { return owner; }
    string getArea() const { return area; }
    HashTable<Product, PRODUCT_SIZE> &getProducts() { return products; }

    void addProduct(int pid, string pname, int price, int qty)
    {
        if (products.find(pid) != NULL)
        {
            cout << "Product with this ID already exists!" << endl;
            return;
        }
        if (!products.insert(Product(pid, pname, price, qty)))
        {
            cout << "Shop is full, cannot add more products!" << endl;
            return;
        }
        cout << "Product added successfully." << endl;
    }

    void updateProduct(int pid, string pname, int price, int qty)
    {
        Product *p = products.find(pid);
        if (p == NULL)
        {
            cout << "Product not found!" << endl;
            return;
        }
        p->setDetails(pname, price, qty);
        cout << "Product updated successfully." << endl;
    }

    void deleteProduct(int pid)
    {
        if (!products.remove(pid))
        {
            cout << "Product not found!" << endl;
            return;
        }
        cout << "Product deleted successfully." << endl;
    }

    void searchProduct(int pid)
    {
        Product *p = products.find(pid);
        if (p == NULL)
        {
            cout << "Product not found!" << endl;
            return;
        }
        cout << "ID\tName\tPrice\tQty" << endl;
        p->display();
    }

    void viewStock()
    {
        if (products.size() == 0)
        {
            cout << "No products in stock." << endl;
            return;
        }
        cout << "\n--- Stock of " << name << " ---" << endl;
        cout << "ID\tName\tPrice\tQty" << endl;
        for (int i = 0; i < products.capacity(); i++)
        {
            Product *p = products.getAt(i);
            if (p != NULL)
                p->display();
        }
    }
};

typedef HashTable<Shop, SHOP_SIZE> ShopTable;

// ---------- Shopkeeper (a type of User) ----------
class Shopkeeper : public User
{
private:
    Shop &shop;
    OrderList &orders;
    OrderQueue &pending;

public:
    Shopkeeper(int id, string name, Shop &shop, OrderList &orders, OrderQueue &pending)
        : User(id, name), shop(shop), orders(orders), pending(pending) {}

    void showMenu() override
    {
        while (true)
        {
            cout << "\n===== Shopkeeper Menu (" << shop.getName() << ") =====\n";
            cout << "1. Add product\n";
            cout << "2. Update product\n";
            cout << "3. Delete product\n";
            cout << "4. View stock\n";
            cout << "5. Search product\n";
            cout << "6. Accept next pending order\n";
            cout << "7. View orders of my shop\n";
            cout << "0. Logout\n";
            int choice = getNumber("Enter choice: ", 0, 7);

            if (choice == 1)
            {
                int pid = getNumber("Enter product ID (1-9999): ", 1, 9999);
                string pname = getText("Enter product name: ");
                int price = getNumber("Enter price (Rs): ", 1, 100000);
                int qty = getNumber("Enter quantity: ", 0, 100000);
                shop.addProduct(pid, pname, price, qty);
            }
            else if (choice == 2)
            {
                int pid = getNumber("Enter product ID to update: ", 1, 9999);
                if (shop.getProducts().find(pid) == NULL)
                {
                    cout << "Product not found!" << endl;
                    continue;
                }
                string pname = getText("Enter new name: ");
                int price = getNumber("Enter new price (Rs): ", 1, 100000);
                int qty = getNumber("Enter new quantity: ", 0, 100000);
                shop.updateProduct(pid, pname, price, qty);
            }
            else if (choice == 3)
            {
                int pid = getNumber("Enter product ID to delete: ", 1, 9999);
                shop.deleteProduct(pid);
            }
            else if (choice == 4)
                shop.viewStock();
            else if (choice == 5)
            {
                int pid = getNumber("Enter product ID to search: ", 1, 9999);
                shop.searchProduct(pid);
            }
            else if (choice == 6)
                acceptNextOrder(orders, pending);       // from order_module.h
            else if (choice == 7)
                showMyOrders();
            else
                break;
        }
    }

    void showMyOrders()
    {
        bool found = false;
        for (int i = 0; i < orders.size(); i++)
        {
            Order *o = orders.get(i);
            if (o->getShopName() == shop.getName())
            {
                cout << "Order #" << o->getId() << " | " << o->getCustomerName()
                     << " | " << o->getQty() << " x " << o->getProductName()
                     << " | Rs " << o->getTotal() << " | " << o->getStatusText() << "\n";
                found = true;
            }
        }
        if (!found)
            cout << "No orders for your shop yet.\n";
    }
};

// ---------- Sample data so the program can be tried at once ----------
inline void loadSampleData(ShopTable &shops)
{
    Shop s1(1, "Negi General Store", "Negi", "Main Market");
    s1.getProducts().insert(Product(1, "Rice", 60, 50));
    s1.getProducts().insert(Product(2, "Sugar", 45, 30));
    s1.getProducts().insert(Product(3, "Tea", 120, 20));
    shops.insert(s1);

    Shop s2(2, "Devi Kirana", "Devi", "Bus Stand");
    s2.getProducts().insert(Product(1, "Rice", 55, 40));
    s2.getProducts().insert(Product(2, "Sugar", 48, 25));
    s2.getProducts().insert(Product(3, "Salt", 20, 60));
    shops.insert(s2);

    Shop s3(3, "Sharma General Store", "Ramesh Sharma", "Temple Road");
    s3.getProducts().insert(Product(1, "Atta", 230, 30));
    s3.getProducts().insert(Product(2, "Rice", 58, 50));
    shops.insert(s3);
}


// ######## from customer_module.h ########
// ============================================================
// customer_module.h  -  Offer, Cart (linked list), Customer
// Part of: Customer module (Raj Vardhan, 2510010483)
// Team ID: DSCPP-III-2026-T113
//
// DSA used: linked list (cart), insertion sort (price comparison),
//           hash table lookups (shops and products)
// ============================================================


// One shop's offer for a product (used in price comparison)
class Offer
{
private:
    int shopId;
    int productId;
    string shopName;
    string area;
    string productName;
    int price;
    int stock;

public:
    Offer(int shopId, int productId, string shopName, string area,
          string productName, int price, int stock)
    {
        this->shopId = shopId;
        this->productId = productId;
        this->shopName = shopName;
        this->area = area;
        this->productName = productName;
        this->price = price;
        this->stock = stock;
    }

    int getShopId() const { return shopId; }
    int getProductId() const { return productId; }
    int getPrice() const { return price; }

    friend ostream &operator<<(ostream &out, const Offer &o)
    {
        out << o.productName << "\t" << o.shopName << "\t" << o.area
            << "\t" << o.price << "\t" << o.stock;
        return out;
    }
};

// One node of the cart (linked list)
struct CartItem
{
    int shopId;
    int productId;
    string shopName;
    string productName;
    int price;
    int qty;
    CartItem *next;

    CartItem(int shopId, int productId, string shopName, string productName, int price, int qty)
    {
        this->shopId = shopId;
        this->productId = productId;
        this->shopName = shopName;
        this->productName = productName;
        this->price = price;
        this->qty = qty;
        this->next = NULL;
    }
};

// Shopping cart = singly linked list of CartItem
class Cart
{
private:
    CartItem *head;

public:
    Cart() : head(NULL) {}
    Cart(const Cart &) = delete;
    Cart &operator=(const Cart &) = delete;
    ~Cart() { clear(); }

    bool isEmpty() { return head == NULL; }
    CartItem *getHead() { return head; }

    int size()
    {
        int count = 0;
        CartItem *temp = head;
        while (temp != NULL)
        {
            count++;
            temp = temp->next;
        }
        return count;
    }

    int getQty(int shopId, int productId)
    {
        CartItem *temp = head;
        while (temp != NULL)
        {
            if (temp->shopId == shopId && temp->productId == productId)
                return temp->qty;
            temp = temp->next;
        }
        return 0;
    }

    void addItem(int shopId, int productId, string shopName, string productName, int price, int qty)
    {
        CartItem *temp = head;
        while (temp != NULL)
        {
            if (temp->shopId == shopId && temp->productId == productId)
            {
                temp->qty += qty;       // same item again, only increase quantity
                return;
            }
            temp = temp->next;
        }
        CartItem *n = new CartItem(shopId, productId, shopName, productName, price, qty);
        if (head == NULL)
            head = n;
        else
        {
            temp = head;
            while (temp->next != NULL)
                temp = temp->next;
            temp->next = n;
        }
    }

    // remove the item at position (1 = first item)
    bool removeItem(int position)
    {
        if (position < 1 || head == NULL)
            return false;
        CartItem *current = head;
        CartItem *previous = NULL;
        int count = 1;
        while (current != NULL && count < position)
        {
            previous = current;
            current = current->next;
            count++;
        }
        if (current == NULL)
            return false;
        if (previous == NULL)
            head = current->next;
        else
            previous->next = current->next;
        delete current;
        return true;
    }

    void clear()
    {
        while (head != NULL)
        {
            CartItem *temp = head;
            head = head->next;
            delete temp;
        }
    }

    int total()
    {
        int sum = 0;
        CartItem *temp = head;
        while (temp != NULL)
        {
            sum += temp->price * temp->qty;
            temp = temp->next;
        }
        return sum;
    }

    void display()
    {
        if (head == NULL)
        {
            cout << "Your cart is empty.\n";
            return;
        }
        cout << "\nNo.\tProduct\t\tShop\t\t\tPrice\tQty\tSubtotal\n";
        cout << "---------------------------------------------------------------------------\n";
        int number = 1;
        CartItem *temp = head;
        while (temp != NULL)
        {
            cout << number << "\t" << temp->productName << "\t" << temp->shopName << "\t"
                 << temp->price << "\t" << temp->qty << "\t" << temp->price * temp->qty << "\n";
            temp = temp->next;
            number++;
        }
        cout << "---------------------------------------------------------------------------\n";
        cout << "Cart total: Rs " << total() << "\n";
    }
};

// ---------- Customer (a type of User) ----------
class Customer : public User
{
private:
    ShopTable &shops;
    OrderList &orders;
    OrderQueue &pending;
    Cart cart;

    // insertion sort: cheapest offer first
    void sortByPrice(vector<Offer> &offers)
    {
        for (int i = 1; i < (int)offers.size(); i++)
        {
            Offer current = offers[i];
            int j = i - 1;
            while (j >= 0 && offers[j].getPrice() > current.getPrice())
            {
                offers[j + 1] = offers[j];
                j--;
            }
            offers[j + 1] = current;
        }
    }

    void showOrder(Order *o)
    {
        int status = o->getStatus();
        cout << "\n--------------------------------\n";
        cout << "Order #" << o->getId() << " | " << o->getQty() << " x " << o->getProductName() << "\n";
        cout << "Shop: " << o->getShopName() << " | Total: Rs " << o->getTotal() << "\n";
        cout << "Deliver to: " << o->getAddress() << " (" << AREA_NAMES[o->getLocation()] << ")\n";
        if (o->getAgentId() != 0)
            cout << "Delivery agent ID: " << o->getAgentId() << "\n";
        cout << "Status: ";
        cout << (status >= PENDING ? "[X] Pending " : "[ ] Pending ");
        cout << (status >= ACCEPTED ? "[X] Accepted " : "[ ] Accepted ");
        cout << (status >= DELIVERED ? "[X] Delivered" : "[ ] Delivered");
        cout << "\nCurrent: " << o->getStatusText() << "\n";
    }

    void addToCart(int shopId, int productId, Shop *s, Product *p)
    {
        if (p->getStock() == 0)
        {
            cout << "Sorry, this product is out of stock.\n";
            return;
        }
        int alreadyInCart = cart.getQty(shopId, productId);
        int canAdd = p->getStock() - alreadyInCart;
        if (canAdd <= 0)
        {
            cout << "You already have all the available stock in your cart.\n";
            return;
        }
        cout << "You can add up to " << canAdd << " piece(s).\n";
        int qty = getNumber("Quantity: ", 1, canAdd);
        cart.addItem(shopId, productId, s->getName(), p->getName(), p->getPrice(), qty);
        cout << "Added to cart: " << qty << " x " << p->getName() << " from " << s->getName() << ".\n";
    }

public:
    Customer(int id, string name, ShopTable &shops, OrderList &orders, OrderQueue &pending)
        : User(id, name), shops(shops), orders(orders), pending(pending) {}

    void browseProducts()
    {
        for (int i = 0; i < shops.capacity(); i++)
        {
            Shop *s = shops.getAt(i);
            if (s == NULL)
                continue;
            cout << "\n[Shop " << s->getId() << "] " << s->getName() << " (" << s->getArea() << ")\n";
            HashTable<Product, PRODUCT_SIZE> &list = s->getProducts();
            for (int j = 0; j < list.capacity(); j++)
            {
                Product *p = list.getAt(j);
                if (p == NULL)
                    continue;
                cout << "  [ID " << p->getId() << "] " << p->getName() << " - Rs " << p->getPrice();
                if (p->getStock() > 0)
                    cout << " (stock: " << p->getStock() << ")\n";
                else
                    cout << " (OUT OF STOCK)\n";
            }
        }
    }

    void comparePrices()
    {
        string key = toLower(getText("Which product do you want to compare? (e.g. rice): "));
        vector<Offer> offers;

        for (int i = 0; i < shops.capacity(); i++)
        {
            Shop *s = shops.getAt(i);
            if (s == NULL)
                continue;
            HashTable<Product, PRODUCT_SIZE> &list = s->getProducts();
            for (int j = 0; j < list.capacity(); j++)
            {
                Product *p = list.getAt(j);
                if (p == NULL)
                    continue;
                if (toLower(p->getName()).find(key) != string::npos)
                {
                    Offer o(s->getId(), p->getId(), s->getName(), s->getArea(),
                            p->getName(), p->getPrice(), p->getStock());
                    offers.push_back(o);
                }
            }
        }

        if (offers.size() == 0)
        {
            cout << "No shop sells \"" << key << "\".\n";
            return;
        }

        sortByPrice(offers);

        cout << "\nNo.\tProduct\t\tShop\t\t\tArea\t\tPrice\tStock\n";
        cout << "---------------------------------------------------------------------------\n";
        for (int i = 0; i < (int)offers.size(); i++)
        {
            cout << i + 1 << "\t" << offers[i];
            if (i == 0)
                cout << " <- cheapest";
            cout << "\n";
        }

        int choice = getNumber("\nAdd one to cart? Enter its number (0 to go back): ", 0, offers.size());
        if (choice > 0)
        {
            int shopId = offers[choice - 1].getShopId();
            int productId = offers[choice - 1].getProductId();
            Shop *s = shops.find(shopId);
            Product *p = s->getProducts().find(productId);
            addToCart(shopId, productId, s, p);
        }
    }

    void addProduct()
    {
        browseProducts();
        int shopId = getNumber("\nEnter shop ID (0 to cancel): ", 0, 9999);
        if (shopId == 0)
            return;
        Shop *s = shops.find(shopId);
        if (s == NULL)
        {
            cout << "No shop with that ID.\n";
            return;
        }
        int productId = getNumber("Enter product ID (0 to cancel): ", 0, 9999);
        if (productId == 0)
            return;
        Product *p = s->getProducts().find(productId);
        if (p == NULL)
        {
            cout << "That shop has no product with that ID.\n";
            return;
        }
        addToCart(shopId, productId, s, p);
    }

    void viewCart()
    {
        cart.display();
    }

    void removeFromCart()
    {
        if (cart.isEmpty())
        {
            cout << "Your cart is empty.\n";
            return;
        }
        cart.display();
        int position = getNumber("\nEnter item number to remove (0 to go back): ", 0, cart.size());
        if (position == 0)
            return;
        if (cart.removeItem(position))
            cout << "Item removed from cart.\n";
        else
            cout << "Could not remove that item.\n";
    }

    void checkout()
    {
        if (cart.isEmpty())
        {
            cout << "Your cart is empty. Add some products first.\n";
            return;
        }
        cart.display();

        // check that every item is still available
        for (CartItem *c = cart.getHead(); c != NULL; c = c->next)
        {
            Shop *s = shops.find(c->shopId);
            Product *p = NULL;
            if (s != NULL)
                p = s->getProducts().find(c->productId);
            if (p == NULL || p->getStock() < c->qty)
            {
                cout << "\nSorry, " << c->productName << " from " << c->shopName
                     << " is not available in this quantity now.\n";
                cout << "Please remove it from your cart and try again.\n";
                return;
            }
        }

        if (cart.size() > pending.freeSpace())
        {
            cout << "Too many orders are waiting right now. Please try again later.\n";
            return;
        }

        cout << "\nAreas:\n";
        showAreas();
        int location = getNumber("Your area number: ", 0, AREA_COUNT - 1);
        string address = getText("Delivery address / landmark: ");
        int grandTotal = cart.total();
        cout << "Total to pay: Rs " << grandTotal << " (Cash on delivery)\n";
        int confirm = getNumber("Confirm order? 1 = Yes, 0 = Cancel: ", 0, 1);
        if (confirm == 0)
        {
            cout << "Order cancelled. Your items are still in the cart.\n";
            return;
        }

        cout << "\n";
        for (CartItem *c = cart.getHead(); c != NULL; c = c->next)
        {
            Shop *s = shops.find(c->shopId);
            Product *p = s->getProducts().find(c->productId);
            int lineTotal = c->qty * c->price;

            p->reduceStock(c->qty);
            Order o(id, name, c->shopName, c->productName, c->qty, lineTotal, address, location);
            int orderId = orders.add(o);      // linked list of all orders
            pending.enqueue(orderId);         // queue of pending orders
            cout << "Order #" << orderId << " placed: " << c->qty << " x " << c->productName
                 << " from " << c->shopName << "\n";
        }
        cart.clear();
        cout << "\nAll orders placed! Please keep Rs " << grandTotal << " ready for delivery.\n";
    }

    void trackOrders()
    {
        bool found = false;
        for (int i = 0; i < orders.size(); i++)
        {
            Order *o = orders.get(i);
            if (o->getCustomerId() == id)
            {
                showOrder(o);
                found = true;
            }
        }
        if (!found)
            cout << "You have no orders yet.\n";
    }

    void showMenu() override
    {
        while (true)
        {
            cout << "\n===== Customer Menu (" << name << ") =====\n";
            cout << "1. Browse products\n";
            cout << "2. Compare prices\n";
            cout << "3. Add product to cart\n";
            cout << "4. View cart\n";
            cout << "5. Remove item from cart\n";
            cout << "6. Checkout (place order)\n";
            cout << "7. Track my orders\n";
            cout << "0. Logout\n";
            int choice = getNumber("Choice: ", 0, 7);

            if (choice == 1)
                browseProducts();
            else if (choice == 2)
                comparePrices();
            else if (choice == 3)
                addProduct();
            else if (choice == 4)
                viewCart();
            else if (choice == 5)
                removeFromCart();
            else if (choice == 6)
                checkout();
            else if (choice == 7)
                trackOrders();
            else
                break;
        }
    }
};


