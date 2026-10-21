// Local Shop Delivery System
// Made by: Mayank Ranjan (2510011761)
// My part: Product, Shop (hash table), stock functions, search, rating system

#include <iostream>
#include <string>
using namespace std;

const int SIZE = 50;   // size of hash table

// ---------- Rating class ----------
class Rating {
public:
    int totalStars;
    int totalCount;

    Rating() {
        totalStars = 0;
        totalCount = 0;
    }

    void addRating(int stars) {
        totalStars = totalStars + stars;
        totalCount++;
    }

    double getAverage() {
        if (totalCount == 0) {
            return 0;
        }
        return (double) totalStars / totalCount;
    }

    void display() {
        if (totalCount == 0) {
            cout << "No ratings yet";
        } else {
            cout << getAverage() << "/5 (" << totalCount << " ratings)";
        }
    }
};

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
// Hash table: position = id % SIZE
// If the position is full, we go to the next position (linear probing)
// state[i]: 0 = empty, 1 = filled, 2 = deleted
class Shop {
public:
    int shopId;
    string shopName;
    string ownerName;
    Rating rating;

    Product products[SIZE];
    int state[SIZE];
    int count;

    Shop() {
        shopId = 0;
        shopName = "";
        ownerName = "";
        count = 0;
        for (int i = 0; i < SIZE; i++) {
            state[i] = 0;
        }
    }

    void setShop(int id, string name, string owner) {
        shopId = id;
        shopName = name;
        ownerName = owner;
    }

    int hashFunction(int id) {
        return id % SIZE;
    }

    // returns position of product, -1 if not found
    int findIndex(int id) {
        int i = hashFunction(id);
        for (int k = 0; k < SIZE; k++) {
            if (state[i] == 0) {
                return -1;
            }
            if (state[i] == 1 && products[i].id == id) {
                return i;
            }
            i = (i + 1) % SIZE;
        }
        return -1;
    }

    void addProduct(int id, string name, double price, int qty) {
        if (findIndex(id) != -1) {
            cout << "Product ID already exists!" << endl;
            return;
        }
        if (count == SIZE) {
            cout << "Shop is full!" << endl;
            return;
        }

        int i = hashFunction(id);
        while (state[i] == 1) {
            i = (i + 1) % SIZE;
        }

        products[i].id = id;
        products[i].name = name;
        products[i].price = price;
        products[i].quantity = qty;
        state[i] = 1;
        count++;
        cout << "Product added." << endl;
    }

    void updateProduct(int id, string name, double price, int qty) {
        int i = findIndex(id);
        if (i == -1) {
            cout << "Product not found!" << endl;
            return;
        }
        products[i].name = name;
        products[i].price = price;
        products[i].quantity = qty;
        cout << "Product updated." << endl;
    }

    void deleteProduct(int id) {
        int i = findIndex(id);
        if (i == -1) {
            cout << "Product not found!" << endl;
            return;
        }
        state[i] = 2;
        count--;
        cout << "Product deleted." << endl;
    }

    void searchById(int id) {
        int i = findIndex(id);
        if (i == -1) {
            cout << "Product not found!" << endl;
            return;
        }
        cout << "ID\tName\tPrice\tQty" << endl;
        products[i].display();
    }

    void searchByName(string name) {
        bool found = false;
        for (int i = 0; i < SIZE; i++) {
            if (state[i] == 1 && products[i].name == name) {
                if (found == false) {
                    cout << "ID\tName\tPrice\tQty" << endl;
                }
                products[i].display();
                found = true;
            }
        }
        if (found == false) {
            cout << "Product not found!" << endl;
        }
    }

    void viewStock() {
        cout << "\n--- Stock of " << shopName << " ---" << endl;
        cout << "Rating: ";
        rating.display();
        cout << endl;
        if (count == 0) {
            cout << "No products in stock." << endl;
            return;
        }
        cout << "ID\tName\tPrice\tQty" << endl;
        for (int i = 0; i < SIZE; i++) {
            if (state[i] == 1) {
                products[i].display();
            }
        }
    }
};

// ---------- Agent class (only for testing rating) ----------
// Real DeliveryAgent class is made by Aradhay
class Agent {
public:
    int id;
    string name;
    Rating rating;

    void setAgent(int i, string n) {
        id = i;
        name = n;
    }
};

// ---------- input functions ----------
// takes a number between low and high
int getInt(string message, int low, int high) {
    int x;
    cout << message;
    while (!(cin >> x) || x < low || x > high) {
        cout << "Invalid input! Try again: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');
    return x;
}

double getPrice() {
    double x;
    cout << "Enter price: ";
    while (!(cin >> x) || x <= 0) {
        cout << "Invalid price! Try again: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');
    return x;
}

string getName(string message) {
    string s;
    cout << message;
    getline(cin, s);
    while (s == "") {
        cout << "Name cannot be empty. Try again: ";
        getline(cin, s);
    }
    return s;
}

// ---------- shopkeeper menu ----------
void shopkeeperMenu(Shop shops[], int index) {
    int choice;
    do {
        cout << "\n===== " << shops[index].shopName << " =====" << endl;
        cout << "1. Add product" << endl;
        cout << "2. Update product" << endl;
        cout << "3. Delete product" << endl;
        cout << "4. View stock" << endl;
        cout << "5. Search by ID" << endl;
        cout << "6. Search by name" << endl;
        cout << "0. Back" << endl;
        choice = getInt("Enter choice: ", 0, 6);

        if (choice == 1) {
            int id = getInt("Enter product ID: ", 1, 100000);
            string name = getName("Enter product name: ");
            double price = getPrice();
            int qty = getInt("Enter quantity: ", 0, 100000);
            shops[index].addProduct(id, name, price, qty);
        }
        else if (choice == 2) {
            int id = getInt("Enter product ID to update: ", 1, 100000);
            string name = getName("Enter new name: ");
            double price = getPrice();
            int qty = getInt("Enter new quantity: ", 0, 100000);
            shops[index].updateProduct(id, name, price, qty);
        }
        else if (choice == 3) {
            int id = getInt("Enter product ID to delete: ", 1, 100000);
            shops[index].deleteProduct(id);
        }
        else if (choice == 4) {
            shops[index].viewStock();
        }
        else if (choice == 5) {
            int id = getInt("Enter product ID: ", 1, 100000);
            shops[index].searchById(id);
        }
        else if (choice == 6) {
            string name = getName("Enter product name: ");
            shops[index].searchByName(name);
        }
    } while (choice != 0);
}

// ---------- show ratings ----------
void showShopRatings(Shop shops[], int n) {
    cout << "\n--- Shop ratings ---" << endl;
    for (int i = 0; i < n; i++) {
        cout << shops[i].shopId << ". " << shops[i].shopName << " : ";
        shops[i].rating.display();
        cout << endl;
    }
}

void showAgentRatings(Agent agents[], int n) {
    cout << "\n--- Agent ratings ---" << endl;
    for (int i = 0; i < n; i++) {
        cout << agents[i].id << ". " << agents[i].name << " : ";
        agents[i].rating.display();
        cout << endl;
    }
}

// ---------- main ----------
int main() {
    const int N = 3;
    Shop shops[N];
    Agent agents[N];

    shops[0].setShop(1, "Sharma General Store", "Ramesh Sharma");
    shops[1].setShop(2, "Hill Mart", "Suresh Negi");
    shops[2].setShop(3, "Negi Kirana", "Dinesh Negi");

    agents[0].setAgent(1, "Rohit");
    agents[1].setAgent(2, "Amit");
    agents[2].setAgent(3, "Vikas");

    // sample products
    shops[0].addProduct(101, "Rice", 60, 50);
    shops[0].addProduct(102, "Atta", 230, 30);
    shops[1].addProduct(101, "Rice", 55, 40);

    int choice;
    do {
        cout << "\n===== Local Shop Delivery System =====" << endl;
        cout << "1. Shopkeeper login" << endl;
        cout << "2. View shop ratings" << endl;
        cout << "3. Rate a shop" << endl;
        cout << "4. View agent ratings" << endl;
        cout << "5. Rate an agent" << endl;
        cout << "0. Exit" << endl;
        choice = getInt("Enter choice: ", 0, 5);

        if (choice == 1) {
            showShopRatings(shops, N);
            int num = getInt("Enter shop number: ", 1, N);
            shopkeeperMenu(shops, num - 1);
        }
        else if (choice == 2) {
            showShopRatings(shops, N);
        }
        else if (choice == 3) {
            showShopRatings(shops, N);
            int num = getInt("Enter shop number: ", 1, N);
            int stars = getInt("Enter stars (1 to 5): ", 1, 5);
            shops[num - 1].rating.addRating(stars);
            cout << "Thank you for rating!" << endl;
        }
        else if (choice == 4) {
            showAgentRatings(agents, N);
        }
        else if (choice == 5) {
            showAgentRatings(agents, N);
            int num = getInt("Enter agent number: ", 1, N);
            int stars = getInt("Enter stars (1 to 5): ", 1, 5);
            agents[num - 1].rating.addRating(stars);
            cout << "Thank you for rating!" << endl;
        }
    } while (choice != 0);

    cout << "Thank you!" << endl;
    return 0;
}
