// ============================================================
// Local Shop Delivery System (LocalKart)
// Team ID : DSCPP-III-2026-T113
// Course  : DSCPP, B.Tech CSE 3rd Semester, Graphic Era (Deemed to be University)
//
// main.cpp - joins all modules and shows the main menu
// Part of: Team Lead (Shaurya Bisht, 2510012399)
//
// Modules:
//   common_input.h       - safe input functions        (Raj Vardhan)
//   order_module.h       - Order, linked list, queue   (Shaurya Bisht)
//   user_agent_module.h  - User, DeliveryAgent, Dijkstra (Aradhay Singh)
//   shop_module.h        - Hash table, Product, Shop   (Mayank Ranjan)
//   customer_module.h    - Cart, Customer              (Raj Vardhan)
//
// Compile: g++ -std=c++11 main.cpp -o localkart
// ============================================================
#include <iostream>
#include <string>
#include "common_input.h"
#include "order_module.h"
#include "user_agent_module.h"
#include "shop_module.h"
#include "customer_module.h"
using namespace std;

// Customers are known by name, so their orders can be tracked after logout/login
const int MAX_CUSTOMERS = 50;
string customerNames[MAX_CUSTOMERS];
int customerCount = 0;

int getCustomerId(string name)
{
    string key = toLower(name);
    for (int i = 0; i < customerCount; i++)
    {
        if (toLower(customerNames[i]) == key)
            return i + 1;
    }
    if (customerCount == MAX_CUSTOMERS)
        return -1;
    customerNames[customerCount] = name;
    customerCount++;
    return customerCount;
}

void customerLogin(ShopTable &shops, OrderList &orders, OrderQueue &pending)
{
    string name = getText("Your name: ");
    int customerId = getCustomerId(name);
    if (customerId == -1)
    {
        cout << "Too many customers registered.\n";
        return;
    }
    Customer customer(customerId, name, shops, orders, pending);
    User *user = &customer;       // base class pointer, calls Customer::showMenu()
    user->showMenu();
}

void shopkeeperLogin(ShopTable &shops, OrderList &orders, OrderQueue &pending)
{
    cout << "\nAvailable shops:\n";
    for (int i = 0; i < shops.capacity(); i++)
    {
        Shop *s = shops.getAt(i);
        if (s != NULL)
            cout << "  " << s->getId() << ". " << s->getName() << " (" << s->getArea() << ")\n";
    }
    int shopId = getNumber("Enter your shop ID (0 to go back): ", 0, 9999);
    if (shopId == 0)
        return;
    Shop *s = shops.find(shopId);
    if (s == NULL)
    {
        cout << "No shop with that ID.\n";
        return;
    }
    Shopkeeper keeper(shopId, s->getOwner(), *s, orders, pending);
    User *user = &keeper;         // base class pointer, calls Shopkeeper::showMenu()
    user->showMenu();
}

int main()
{
    ShopTable shops;
    OrderList orders;
    OrderQueue pending;
    DeliveryManager delivery(orders);

    loadSampleData(shops);
    delivery.loadDefaultMap();
    delivery.addAgent("Rohit", 1);       // sample agents
    delivery.addAgent("Imran", 4);

    cout << "Welcome to LocalKart - Local Shop Delivery System\n";

    while (true)
    {
        cout << "\n===== MAIN MENU =====\n";
        cout << "1. Customer\n";
        cout << "2. Shopkeeper\n";
        cout << "3. Delivery agent\n";
        cout << "0. Exit\n";
        int choice = getNumber("Enter choice: ", 0, 3);

        if (choice == 1)
            customerLogin(shops, orders, pending);
        else if (choice == 2)
            shopkeeperLogin(shops, orders, pending);
        else if (choice == 3)
            delivery.menu();
        else
            break;
    }

    cout << "Thank you for using LocalKart!\n";
    return 0;
}
