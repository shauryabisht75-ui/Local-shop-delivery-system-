#include "order.h"

int main() {
    int choice = -1;

    while (choice != 0) {
        cout << "\n----- Order Menu -----\n";
        cout << "1. Place order\n";
        cout << "2. Accept next order\n";
        cout << "3. Deliver order\n";
        cout << "4. Track order\n";
        cout << "5. Show all orders\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cout << "Please enter a number\n";
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
        }
        else if (choice == 1)
            placeOrder();
        else if (choice == 2)
            acceptOrder();
        else if (choice == 3)
            deliverOrder();
        else if (choice == 4)
            trackOrder();
        else if (choice == 5)
            showOrders();
        else if (choice != 0)
            cout << "Wrong choice\n";
    }

    cout << "Thank you\n";
    return 0;
}
