// PART 2: main.cpp
// Menu, input checking and main function
// Made by: Mayank Ranjan (2510011761)

#include <iostream>
#include <string>
#include "shop.h"
using namespace std;

// ---------- functions to take safe input ----------
int getInt(string message) {
    int x;
    cout << message;
    while (!(cin >> x) || x < 0) {
        cout << "Invalid input! Enter a positive number: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    return x;
}

double getPrice(string message) {
    double x;
    cout << message;
    while (!(cin >> x) || x <= 0) {
        cout << "Invalid price! Enter a number greater than 0: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    return x;
}

// ---------- main ----------
int main() {
    Shop shop(1, "Sharma General Store", "Ramesh Sharma");

    // some sample products
    shop.addProduct(101, "Rice", 60, 50);
    shop.addProduct(102, "Atta", 230, 30);

    int choice;
    do {
        cout << "\n===== Shopkeeper Menu =====" << endl;
        cout << "1. Add product" << endl;
        cout << "2. Update product" << endl;
        cout << "3. Delete product" << endl;
        cout << "4. View stock" << endl;
        cout << "5. Search product" << endl;
        cout << "0. Exit" << endl;
        choice = getInt("Enter choice: ");

        if (choice == 1) {
            int id = getInt("Enter product ID: ");
            string name;
            cout << "Enter product name: ";
            cin >> name;
            double price = getPrice("Enter price: ");
            int qty = getInt("Enter quantity: ");
            shop.addProduct(id, name, price, qty);
        }
        else if (choice == 2) {
            int id = getInt("Enter product ID to update: ");
            string name;
            cout << "Enter new name: ";
            cin >> name;
            double price = getPrice("Enter new price: ");
            int qty = getInt("Enter new quantity: ");
            shop.updateProduct(id, name, price, qty);
        }
        else if (choice == 3) {
            int id = getInt("Enter product ID to delete: ");
            shop.deleteProduct(id);
        }
        else if (choice == 4) {
            shop.viewStock();
        }
        else if (choice == 5) {
            int id = getInt("Enter product ID to search: ");
            shop.searchProduct(id);
        }
        else if (choice != 0) {
            cout << "Wrong choice, try again." << endl;
        }

    } while (choice != 0);

    cout << "Thank you!" << endl;
    return 0;
}
