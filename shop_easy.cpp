// Local Shop Delivery System - Shop Management
// Made by: Mayank Ranjan (2510011761)
// Part: Product class, Shop class (own hash table), add/update/delete/view stock

#include <iostream>
#include <string>
using namespace std;

const int SIZE = 50;   // size of the hash table

// ---------- Product class ----------
class Product {
public:
    int id;
    string name;
    double price;
    int quantity;

    Product() {
        id = 0;
        name = "";
        price = 0;
        quantity = 0;
    }

    void display() {
        cout << id << "\t" << name << "\t" << price << "\t" << quantity << endl;
    }
};

// ---------- Shop class ----------
// Hash table: index = id % SIZE
// If that place is already full, we check the next place (linear probing)
// state[i] : 0 = empty, 1 = filled, 2 = deleted
class Shop {
public:
    int shopId;
    string shopName;
    string ownerName;

    Product products[SIZE];
    int state[SIZE];
    int count;

    Shop(int id, string name, string owner) {
        shopId = id;
        shopName = name;
        ownerName = owner;
        count = 0;
        for (int i = 0; i < SIZE; i++) {
            state[i] = 0;
        }
    }

    // hash function
    int hashFunction(int id) {
        return id % SIZE;
    }

    // find the position of a product, returns -1 if not found
    int findIndex(int id) {
        int i = hashFunction(id);
        for (int k = 0; k < SIZE; k++) {
            if (state[i] == 0) {
                return -1;               // empty place, so product is not there
            }
            if (state[i] == 1 && products[i].id == id) {
                return i;                // found
            }
            i = (i + 1) % SIZE;          // check next place
        }
        return -1;
    }

    // add a new product
    void addProduct(int id, string name, double price, int qty) {
        if (findIndex(id) != -1) {
            cout << "Product with this ID already exists!" << endl;
            return;
        }
        if (count == SIZE) {
            cout << "Shop is full, cannot add more products!" << endl;
            return;
        }

        int i = hashFunction(id);
        while (state[i] == 1) {          // go ahead until a free place is found
            i = (i + 1) % SIZE;
        }

        products[i].id = id;
        products[i].name = name;
        products[i].price = price;
        products[i].quantity = qty;
        state[i] = 1;
        count++;
        cout << "Product added successfully." << endl;
    }

    // update an existing product
    void updateProduct(int id, string name, double price, int qty) {
        int i = findIndex(id);
        if (i == -1) {
            cout << "Product not found!" << endl;
            return;
        }
        products[i].name = name;
        products[i].price = price;
        products[i].quantity = qty;
        cout << "Product updated successfully." << endl;
    }

    // delete a product
    void deleteProduct(int id) {
        int i = findIndex(id);
        if (i == -1) {
            cout << "Product not found!" << endl;
            return;
        }
        state[i] = 2;                    // mark as deleted
        count--;
        cout << "Product deleted successfully." << endl;
    }

    // search a product by ID
    void searchProduct(int id) {
        int i = findIndex(id);
        if (i == -1) {
            cout << "Product not found!" << endl;
            return;
        }
        cout << "ID\tName\tPrice\tQty" << endl;
        products[i].display();
    }

    // view all products
    void viewStock() {
        if (count == 0) {
            cout << "No products in stock." << endl;
            return;
        }
        cout << "\n--- Stock of " << shopName << " ---" << endl;
        cout << "ID\tName\tPrice\tQty" << endl;
        for (int i = 0; i < SIZE; i++) {
            if (state[i] == 1) {
                products[i].display();
            }
        }
    }
};

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
