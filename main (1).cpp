#include "Shop.h"

static bool readLine(const string &prompt, string &out) {
    cout << prompt;
    return (bool)getline(cin, out);
}
static bool readInt(const string &prompt, int &out) {
    string s;
    if (!readLine(prompt, s)) return false;
    if (!isNumber(s)) { cout << "Error: enter a whole number\n"; return false; }
    out = stoi(s); return true;
}
static bool readPrice(const string &prompt, double &out) {
    string s;
    if (!readLine(prompt, s)) return false;
    if (!isPrice(s)) { cout << "Error: enter a valid price\n"; return false; }
    out = stod(s); return true;
}

static void menu() {
    cout << "\n===== SHOP MANAGEMENT =====\n"
         << " 1 Add product      2 Update product   3 Delete product   4 View stock\n"
         << " 5 Search name      6 Sort by price    7 Sort by qty      8 Low stock\n"
         << " 9 Place order     10 View desk       11 Accept next     12 Pack next\n"
         << "13 Reject next     14 Hand to dispatch 15 Save            16 Load\n"
         << " 0 Exit\n";
}

int main() {
    Shop shop(1, "Sharma General Store", "Civil Lines");
    const string file = "inventory.txt";
    shop.load(file);
    int nextOrderId = 1;
    string line, err;

    while (true) {
        menu();
        int ch;
        if (!readInt("Choice: ", ch)) { if (!cin) break; continue; }
        int pid, qty; double price; string s;

        switch (ch) {
        case 1:
            if (!readInt("Product id: ", pid)) break;
            if (!readLine("Name: ", s)) break;
            if (!readPrice("Price: ", price)) break;
            if (!readInt("Quantity: ", qty)) break;
            cout << (shop.addStock(pid, s, price, qty, err) ? "Added.\n" : "Error: " + err + "\n");
            break;
        case 2:
            if (!readInt("Product id: ", pid)) break;
            if (!readPrice("New price: ", price)) break;
            if (!readInt("New quantity: ", qty)) break;
            cout << (shop.updateStock(pid, price, qty, err) ? "Updated.\n" : "Error: " + err + "\n");
            break;
        case 3:
            if (!readInt("Product id: ", pid)) break;
            cout << (shop.deleteStock(pid, err) ? "Deleted.\n" : "Error: " + err + "\n");
            break;
        case 4: shop.viewStock(); break;
        case 5:
            if (!readLine("Search text: ", s)) break;
            if (StockNode *n = shop.findByName(s))
                cout << "Exact match: [" << n->productId << "] " << n->name << " Rs." << n->price << " qty " << n->qty << "\n";
            else if (shop.searchPartial(s) == 0) cout << "No product found.\n";
            break;
        case 6: shop.viewSortedByPrice(true); break;
        case 7: shop.viewSortedByQty(); break;
        case 8:
            if (!readInt("Threshold: ", qty)) break;
            shop.lowStock(qty); break;
        case 9:
            if (!readInt("Product id: ", pid)) break;
            if (!readInt("Quantity: ", qty)) break;
            if (shop.receiveOrder(nextOrderId, pid, qty, err)) cout << "Order #" << nextOrderId++ << " placed.\n";
            else cout << "Error: " << err << "\n";
            break;
        case 10: shop.desk.display(); break;
        case 11: cout << (shop.acceptNext(err) ? "Accepted.\n" : "Error: " + err + "\n"); break;
        case 12: cout << (shop.packNext(err) ? "Packed.\n" : "Error: " + err + "\n"); break;
        case 13: cout << (shop.rejectNext(err) ? "Rejected, stock released.\n" : "Error: " + err + "\n"); break;
        case 14: {
            int oid;
            if (shop.handToDispatch(oid, err)) cout << "Order #" << oid << " handed to dispatch.\n";
            else cout << "Error: " << err << "\n";
            break;
        }
        case 15: cout << (shop.save(file) ? "Saved.\n" : "Error: could not save\n"); break;
        case 16: cout << (shop.load(file) ? "Loaded.\n" : "Error: file not found\n"); break;
        case 0:
            shop.save(file);
            cout << "Saved. Bye.\n";
            return 0;
        default: cout << "Invalid choice.\n";
        }
    }
    return 0;
}
