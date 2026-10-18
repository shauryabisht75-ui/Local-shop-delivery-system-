// ============================================================
// Local Shop Delivery System  |  Team ID: DSCPP-III-2026-T113
// File    : price_comparison.cpp
// Module  : Customer module
// Feature : Price comparison - same product from different shops,
//           shown with the CHEAPEST shop first.
// DSA used: Array of structures + Bubble Sort
// ============================================================

#include <iostream>
#include <string>
using namespace std;

// One product of one shop
struct Product {
    int id;
    string shopName;
    string name;
    int price;
    int stock;
};

// All products of all shops (sample data, same as database.sql)
Product products[6] = {
    {1, "Negi General Store", "Rice", 60, 50},
    {2, "Negi General Store", "Sugar", 45, 30},
    {3, "Negi General Store", "Tea", 120, 20},
    {4, "Devi Kirana", "Rice", 55, 40},
    {5, "Devi Kirana", "Sugar", 48, 25},
    {6, "Devi Kirana", "Salt", 20, 60}
};
int total = 6;   // number of products


// Show all products
void showAll() {
    cout << "\n--- All Products ---" << endl;
    for (int i = 0; i < total; i++) {
        cout << products[i].id << ". "
             << products[i].name << " | "
             << products[i].shopName << " | Rs "
             << products[i].price << " | Stock: "
             << products[i].stock << endl;
    }
}


// Bubble sort: lowest price comes first
// Compare two neighbours, swap them if the left one is costlier
void sortByPrice(Product list[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (list[j].price > list[j + 1].price) {
                Product temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}


// Compare prices of one product in different shops
void comparePrices() {
    string wanted;
    cout << "Enter product name (Rice / Sugar / Tea / Salt): ";
    getline(cin, wanted);

    // Step 1: copy all shops that sell this product into a new list
    Product found[6];
    int count = 0;
    for (int i = 0; i < total; i++) {
        if (products[i].name == wanted && products[i].stock > 0) {
            found[count] = products[i];
            count++;
        }
    }

    if (count == 0) {
        cout << "No shop has " << wanted << " in stock." << endl;
        return;
    }

    // Step 2: sort that list by price
    sortByPrice(found, count);

    // Step 3: print it (first one is the cheapest)
    cout << "\n--- Price comparison for " << wanted << " ---" << endl;
    for (int i = 0; i < count; i++) {
        cout << i + 1 << ". " << found[i].shopName
             << " - Rs " << found[i].price;
        if (i == 0) {
            cout << "  (Cheapest)";
        }
        cout << endl;
    }
}


int main() {
    int choice;

    do {
        cout << "\n===== Customer Menu =====" << endl;
        cout << "1. View all products" << endl;
        cout << "2. Compare prices" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        // If the user types a letter, clear the error and ask again
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        } else {
            cin.ignore(1000, '\n');   // remove the Enter key
        }

        if (choice == 1) {
            showAll();
        } else if (choice == 2) {
            comparePrices();
        } else if (choice == 3) {
            cout << "Thank you!" << endl;
        } else {
            cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 3);

    return 0;
}
