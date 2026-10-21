#ifndef CUSTOMER_MODULE_H
#define CUSTOMER_MODULE_H

#include <iostream>
#include <string>
#include <vector>
#include "common_input.h"
#include "order_module.h"
#include "user_agent_module.h"
#include "shop_module.h"
using namespace std;

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
                temp->qty += qty;
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

class Customer : public User
{
private:
    ShopTable &shops;
    OrderList &orders;
    OrderQueue &pending;
    Cart cart;

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
            int orderId = orders.add(o);
            pending.enqueue(orderId);
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

#endif