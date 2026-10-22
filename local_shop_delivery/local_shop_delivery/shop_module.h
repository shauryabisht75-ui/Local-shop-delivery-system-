// ============================================================
// shop_module.h  -  HashTable, Product, Shop, Shopkeeper
// Part of: Shop Management (Mayank Ranjan, 2510011761)
// Team ID: DSCPP-III-2026-T113
//
// DSA used: hash table (index = id % SIZE, linear probing)
// state[i] : 0 = empty, 1 = filled, 2 = deleted
// ============================================================
#ifndef SHOP_MODULE_H
#define SHOP_MODULE_H

#include <iostream>
#include <string>
#include "common_input.h"
#include "order_module.h"
#include "user_agent_module.h"
using namespace std;

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

#endif
