// ============================================================
// LocalKart - web server (backend)
// Uses the classes from core.h (your original project code):
//   HashTable, Shop, Product, OrderList, OrderQueue, Cart,
//   DeliveryManager (Dijkstra), Offer
// Compile: g++ -std=c++11 server.cpp -o localkart
// Run    : ./localkart   then open http://localhost:8080
// ============================================================
#include "core.h"
#include <map>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// ---------- global data (same objects that main() used) ----------
ShopTable shops;
OrderList orders;
OrderQueue pending;
DeliveryManager delivery(orders);

// one cart per customer name (Cart is your linked list class)
struct CartHolder { Cart cart; };
map<string, CartHolder *> carts;
map<string, int> customerIds;

int customerIdOf(const string &name)
{
    string key = toLower(name);
    if (customerIds.count(key) == 0)
    {
        int n = (int)customerIds.size() + 1;
        customerIds[key] = n;
    }
    return customerIds[key];
}

Cart &cartOf(const string &name)
{
    string key = toLower(name);
    if (carts.count(key) == 0)
        carts[key] = new CartHolder();
    return carts[key]->cart;
}

// ---------- small helpers ----------
string esc(const string &s)   // escape text for JSON
{
    string r;
    for (size_t i = 0; i < s.size(); i++)
    {
        char c = s[i];
        if (c == '"' || c == '\\') { r += '\\'; r += c; }
        else if (c == '\n') r += "\\n";
        else if ((unsigned char)c < 32) r += ' ';
        else r += c;
    }
    return r;
}

string q(const string &s) { return "\"" + esc(s) + "\""; }

string urlDecode(const string &s)
{
    string r;
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == '+') r += ' ';
        else if (s[i] == '%' && i + 2 < s.size())
        {
            r += (char)strtol(s.substr(i + 1, 2).c_str(), NULL, 16);
            i += 2;
        }
        else r += s[i];
    }
    return r;
}

map<string, string> parseParams(const string &s)
{
    map<string, string> m;
    size_t i = 0;
    while (i < s.size())
    {
        size_t amp = s.find('&', i);
        if (amp == string::npos) amp = s.size();
        string pair = s.substr(i, amp - i);
        size_t eq = pair.find('=');
        if (eq != string::npos)
            m[urlDecode(pair.substr(0, eq))] = urlDecode(pair.substr(eq + 1));
        i = amp + 1;
    }
    return m;
}

int num(map<string, string> &p, const string &key, int def = 0)
{
    if (p.count(key) == 0 || p[key].empty()) return def;
    for (size_t i = 0; i < p[key].size(); i++)
        if (!isdigit((unsigned char)p[key][i])) return def;
    if (p[key].size() > 7) return def;
    return atoi(p[key].c_str());
}

string str(map<string, string> &p, const string &key)
{
    return p.count(key) ? p[key] : "";
}

string ok(const string &msg) { return "{\"ok\":true,\"message\":" + q(msg) + "}"; }
string fail(const string &msg) { return "{\"ok\":false,\"message\":" + q(msg) + "}"; }

string orderJson(Order *o)
{
    string s = "{";
    s += "\"id\":" + to_string(o->getId());
    s += ",\"customer\":" + q(o->getCustomerName());
    s += ",\"shop\":" + q(o->getShopName());
    s += ",\"product\":" + q(o->getProductName());
    s += ",\"qty\":" + to_string(o->getQty());
    s += ",\"total\":" + to_string(o->getTotal());
    s += ",\"address\":" + q(o->getAddress());
    s += ",\"area\":" + q(AREA_NAMES[o->getLocation()]);
    s += ",\"agentId\":" + to_string(o->getAgentId());
    s += ",\"status\":" + to_string(o->getStatus());
    s += ",\"statusText\":" + q(o->getStatusText());
    return s + "}";
}

// ---------- API handlers ----------
string apiAreas()
{
    string s = "[";
    for (int i = 0; i < AREA_COUNT; i++)
    {
        if (i) s += ",";
        s += "{\"id\":" + to_string(i) + ",\"name\":" + q(AREA_NAMES[i]) + "}";
    }
    return s + "]";
}

string productJson(Product *p)
{
    return "{\"id\":" + to_string(p->getId()) + ",\"name\":" + q(p->getName()) +
           ",\"price\":" + to_string(p->getPrice()) + ",\"stock\":" + to_string(p->getStock()) + "}";
}

string shopJson(Shop *s)
{
    string r = "{\"id\":" + to_string(s->getId()) + ",\"name\":" + q(s->getName()) +
               ",\"owner\":" + q(s->getOwner()) + ",\"area\":" + q(s->getArea()) + ",\"products\":[";
    bool first = true;
    HashTable<Product, PRODUCT_SIZE> &list = s->getProducts();
    for (int j = 0; j < list.capacity(); j++)
    {
        Product *p = list.getAt(j);
        if (p == NULL) continue;
        if (!first) r += ",";
        r += productJson(p);
        first = false;
    }
    return r + "]}";
}

string apiShops()
{
    string r = "[";
    bool first = true;
    for (int i = 0; i < shops.capacity(); i++)
    {
        Shop *s = shops.getAt(i);
        if (s == NULL) continue;
        if (!first) r += ",";
        r += shopJson(s);
        first = false;
    }
    return r + "]";
}

// price comparison: same insertion sort idea as Customer::sortByPrice
string apiCompare(map<string, string> &p)
{
    string key = toLower(str(p, "q"));
    vector<Offer> offers;
    for (int i = 0; i < shops.capacity(); i++)
    {
        Shop *s = shops.getAt(i);
        if (s == NULL) continue;
        HashTable<Product, PRODUCT_SIZE> &list = s->getProducts();
        for (int j = 0; j < list.capacity(); j++)
        {
            Product *pr = list.getAt(j);
            if (pr == NULL) continue;
            if (!key.empty() && toLower(pr->getName()).find(key) != string::npos)
                offers.push_back(Offer(s->getId(), pr->getId(), s->getName(), s->getArea(),
                                       pr->getName(), pr->getPrice(), pr->getStock()));
        }
    }
    for (int i = 1; i < (int)offers.size(); i++)   // insertion sort, cheapest first
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
    string r = "[";
    for (int i = 0; i < (int)offers.size(); i++)
    {
        if (i) r += ",";
        Shop *s = shops.find(offers[i].getShopId());
        Product *pr = s->getProducts().find(offers[i].getProductId());
        r += "{\"shopId\":" + to_string(s->getId()) + ",\"shop\":" + q(s->getName()) +
             ",\"area\":" + q(s->getArea()) + ",\"product\":" + productJson(pr) + "}";
    }
    return r + "]";
}

string apiCart(map<string, string> &p)
{
    string name = str(p, "customer");
    if (name.empty()) return "{\"items\":[],\"total\":0}";
    Cart &cart = cartOf(name);
    string r = "{\"items\":[";
    int n = 1;
    for (CartItem *c = cart.getHead(); c != NULL; c = c->next, n++)
    {
        if (n > 1) r += ",";
        r += "{\"no\":" + to_string(n) + ",\"shopId\":" + to_string(c->shopId) +
             ",\"shop\":" + q(c->shopName) + ",\"product\":" + q(c->productName) +
             ",\"price\":" + to_string(c->price) + ",\"qty\":" + to_string(c->qty) +
             ",\"subtotal\":" + to_string(c->price * c->qty) + "}";
    }
    return r + "],\"total\":" + to_string(cart.total()) + "}";
}

string apiCartAdd(map<string, string> &p)
{
    string name = str(p, "customer");
    if (name.empty()) return fail("Enter your name first.");
    int shopId = num(p, "shopId"), productId = num(p, "productId"), qty = num(p, "qty");
    Shop *s = shops.find(shopId);
    if (s == NULL) return fail("Shop not found.");
    Product *pr = s->getProducts().find(productId);
    if (pr == NULL) return fail("Product not found.");
    if (pr->getStock() == 0) return fail("Sorry, this product is out of stock.");
    Cart &cart = cartOf(name);
    int canAdd = pr->getStock() - cart.getQty(shopId, productId);
    if (canAdd <= 0) return fail("You already have all the available stock in your cart.");
    if (qty < 1 || qty > canAdd) return fail("Quantity must be between 1 and " + to_string(canAdd) + ".");
    cart.addItem(shopId, productId, s->getName(), pr->getName(), pr->getPrice(), qty);
    return ok("Added " + to_string(qty) + " x " + pr->getName() + " from " + s->getName() + ".");
}

string apiCartRemove(map<string, string> &p)
{
    string name = str(p, "customer");
    if (name.empty()) return fail("Enter your name first.");
    if (cartOf(name).removeItem(num(p, "position"))) return ok("Item removed from cart.");
    return fail("Could not remove that item.");
}

string apiCheckout(map<string, string> &p)
{
    string name = str(p, "customer");
    if (name.empty()) return fail("Enter your name first.");
    Cart &cart = cartOf(name);
    if (cart.isEmpty()) return fail("Your cart is empty. Add some products first.");

    int area = num(p, "area", -1);
    string address = str(p, "address");
    if (area < 0 || area >= AREA_COUNT) return fail("Please choose your area.");
    bool blank = true;
    for (size_t i = 0; i < address.size(); i++) if (address[i] != ' ') blank = false;
    if (blank) return fail("Please enter the delivery address / landmark.");

    for (CartItem *c = cart.getHead(); c != NULL; c = c->next)   // stock check
    {
        Shop *s = shops.find(c->shopId);
        Product *pr = s ? s->getProducts().find(c->productId) : NULL;
        if (pr == NULL || pr->getStock() < c->qty)
            return fail(c->productName + " from " + c->shopName +
                        " is not available in this quantity now. Remove it from the cart and try again.");
    }
    if (cart.size() > pending.freeSpace())
        return fail("Too many orders are waiting right now. Please try again later.");

    int customerId = customerIdOf(name);
    int grandTotal = cart.total();
    string ids;
    for (CartItem *c = cart.getHead(); c != NULL; c = c->next)
    {
        Shop *s = shops.find(c->shopId);
        Product *pr = s->getProducts().find(c->productId);
        pr->reduceStock(c->qty);
        Order o(customerId, name, c->shopName, c->productName, c->qty, c->qty * c->price, address, area);
        int orderId = orders.add(o);     // linked list
        pending.enqueue(orderId);        // queue
        ids += (ids.empty() ? "#" : ", #") + to_string(orderId);
    }
    cart.clear();
    return ok("Order(s) " + ids + " placed! Please keep Rs " + to_string(grandTotal) + " ready (Cash on delivery).");
}

string apiOrders(map<string, string> &p)
{
    string name = toLower(str(p, "customer"));
    string r = "[";
    bool first = true;
    for (int i = 0; i < orders.size(); i++)
    {
        Order *o = orders.get(i);
        if (toLower(o->getCustomerName()) != name) continue;
        if (!first) r += ",";
        r += orderJson(o);
        first = false;
    }
    return r + "]";
}

// ----- shopkeeper -----
string apiShopOrders(map<string, string> &p)
{
    Shop *s = shops.find(num(p, "shopId"));
    if (s == NULL) return "[]";
    string r = "[";
    bool first = true;
    for (int i = 0; i < orders.size(); i++)
    {
        Order *o = orders.get(i);
        if (o->getShopName() != s->getName()) continue;
        if (!first) r += ",";
        r += orderJson(o);
        first = false;
    }
    return r + "]";
}

// Accept the oldest pending order of THIS shop (queue stays first-come-first-served)
string apiShopAccept(map<string, string> &p)
{
    Shop *s = shops.find(num(p, "shopId"));
    if (s == NULL) return fail("Shop not found.");
    int total = pending.size();
    int acceptedId = -1;
    for (int k = 0; k < total; k++)
    {
        int id = pending.dequeue();
        Order *o = orders.findById(id);
        if (acceptedId == -1 && o != NULL && o->getShopName() == s->getName())
        {
            o->setStatus(ACCEPTED);
            acceptedId = id;
        }
        else
            pending.enqueue(id);         // put the others back in the same order
    }
    if (acceptedId == -1) return fail("No pending orders for your shop.");
    Order *o = orders.findById(acceptedId);
    return ok("Order #" + to_string(acceptedId) + " (" + to_string(o->getQty()) + " x " +
              o->getProductName() + ") accepted.");
}

string apiProductAdd(map<string, string> &p)
{
    Shop *s = shops.find(num(p, "shopId"));
    if (s == NULL) return fail("Shop not found.");
    int pid = num(p, "id"), price = num(p, "price"), qty = num(p, "stock", -1);
    string name = str(p, "name");
    if (pid < 1 || pid > 9999) return fail("Product ID must be 1-9999.");
    if (name.empty()) return fail("Product name cannot be empty.");
    if (price < 1 || price > 100000) return fail("Price must be 1-100000.");
    if (qty < 0 || qty > 100000) return fail("Quantity must be 0-100000.");
    if (s->getProducts().find(pid) != NULL) return fail("Product with this ID already exists!");
    if (!s->getProducts().insert(Product(pid, name, price, qty))) return fail("Shop is full, cannot add more products!");
    return ok("Product added successfully.");
}

string apiProductUpdate(map<string, string> &p)
{
    Shop *s = shops.find(num(p, "shopId"));
    if (s == NULL) return fail("Shop not found.");
    Product *pr = s->getProducts().find(num(p, "id"));
    if (pr == NULL) return fail("Product not found!");
    int price = num(p, "price"), qty = num(p, "stock", -1);
    string name = str(p, "name");
    if (name.empty()) return fail("Product name cannot be empty.");
    if (price < 1 || price > 100000) return fail("Price must be 1-100000.");
    if (qty < 0 || qty > 100000) return fail("Quantity must be 0-100000.");
    pr->setDetails(name, price, qty);
    return ok("Product updated successfully.");
}

string apiProductDelete(map<string, string> &p)
{
    Shop *s = shops.find(num(p, "shopId"));
    if (s == NULL) return fail("Shop not found.");
    if (!s->getProducts().remove(num(p, "id"))) return fail("Product not found!");
    return ok("Product deleted successfully.");
}

// ----- delivery agent -----
string apiAgentAdd(map<string, string> &p)
{
    string name = str(p, "name");
    int area = num(p, "area", -1);
    if (name.empty()) return fail("Agent name cannot be empty.");
    if (area < 0 || area >= AREA_COUNT) return fail("Choose a valid area.");
    int id = delivery.addAgent(name, area);
    return ok("Agent registered. Agent ID: " + to_string(id));
}

string apiAllOrders()
{
    string r = "[";
    for (int i = 0; i < orders.size(); i++)
    {
        if (i) r += ",";
        r += orderJson(orders.get(i));
    }
    return r + "]";
}

string apiAssign(map<string, string> &p)
{
    string agentName;
    int dist = 0;
    int code = delivery.assignNearest(num(p, "orderId"), agentName, dist);
    if (code == 1) return fail("Register an agent first.");
    if (code == 2) return fail("That order is not waiting for an agent (shopkeeper must accept it first).");
    if (code == 3) return fail("No available agent can reach this location.");
    return ok("Assigned to " + agentName + " (" + to_string(dist) + " km from the customer, found with Dijkstra).");
}

string apiComplete(map<string, string> &p)
{
    int code = delivery.finishDelivery(num(p, "orderId"));
    if (code == 1) return fail("Order not found.");
    if (code == 2) return fail("This order is not out for delivery.");
    return ok("Order marked Delivered. The agent is available again.");
}

string route(const string &method, const string &path, map<string, string> &p)
{
    if (path == "/api/areas") return apiAreas();
    if (path == "/api/shops") return apiShops();
    if (path == "/api/compare") return apiCompare(p);
    if (path == "/api/cart") return apiCart(p);
    if (path == "/api/cart/add") return apiCartAdd(p);
    if (path == "/api/cart/remove") return apiCartRemove(p);
    if (path == "/api/checkout") return apiCheckout(p);
    if (path == "/api/orders") return apiOrders(p);
    if (path == "/api/shop/orders") return apiShopOrders(p);
    if (path == "/api/shop/accept") return apiShopAccept(p);
    if (path == "/api/shop/product/add") return apiProductAdd(p);
    if (path == "/api/shop/product/update") return apiProductUpdate(p);
    if (path == "/api/shop/product/delete") return apiProductDelete(p);
    if (path == "/api/agents") return delivery.agentsJson();
    if (path == "/api/agents/add") return apiAgentAdd(p);
    if (path == "/api/delivery/orders") return apiAllOrders();
    if (path == "/api/delivery/assign") return apiAssign(p);
    if (path == "/api/delivery/complete") return apiComplete(p);
    return fail("Unknown API.");
}

// ---------- tiny HTTP server ----------
string readFile(const string &name, bool &found)
{
    ifstream f(name.c_str(), ios::binary);
    if (!f) { found = false; return ""; }
    stringstream ss;
    ss << f.rdbuf();
    found = true;
    return ss.str();
}

string contentType(const string &path)
{
    if (path.size() >= 5 && path.substr(path.size() - 5) == ".html") return "text/html; charset=utf-8";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".css") return "text/css";
    if (path.size() >= 3 && path.substr(path.size() - 3) == ".js") return "application/javascript";
    return "text/plain";
}

void sendResponse(int client, int code, const string &type, const string &body)
{
    string status = (code == 200) ? "200 OK" : (code == 404 ? "404 Not Found" : "400 Bad Request");
    string head = "HTTP/1.1 " + status + "\r\nContent-Type: " + type +
                  "\r\nContent-Length: " + to_string(body.size()) +
                  "\r\nConnection: close\r\n\r\n";
    string all = head + body;
    size_t sent = 0;
    while (sent < all.size())
    {
        ssize_t n = write(client, all.c_str() + sent, all.size() - sent);
        if (n <= 0) break;
        sent += n;
    }
}

void handleClient(int client)
{
    string req;
    char buf[4096];
    size_t headerEnd = string::npos;
    while (headerEnd == string::npos && req.size() < 65536)
    {
        ssize_t n = read(client, buf, sizeof(buf));
        if (n <= 0) break;
        req.append(buf, n);
        headerEnd = req.find("\r\n\r\n");
    }
    if (headerEnd == string::npos) return;

    string headers = req.substr(0, headerEnd);
    string body = req.substr(headerEnd + 4);
    size_t cl = headers.find("Content-Length:");
    if (cl != string::npos)
    {
        size_t want = atoi(headers.c_str() + cl + 15);
        while (body.size() < want && body.size() < 65536)
        {
            ssize_t n = read(client, buf, sizeof(buf));
            if (n <= 0) break;
            body.append(buf, n);
        }
    }

    istringstream first(headers.substr(0, headers.find("\r\n")));
    string method, target, version;
    first >> method >> target >> version;

    string path = target, query;
    size_t qm = target.find('?');
    if (qm != string::npos) { path = target.substr(0, qm); query = target.substr(qm + 1); }

    if (path.find("/api/") == 0)
    {
        map<string, string> params = parseParams(query);
        map<string, string> bodyParams = parseParams(body);
        for (map<string, string>::iterator it = bodyParams.begin(); it != bodyParams.end(); ++it)
            params[it->first] = it->second;
        sendResponse(client, 200, "application/json", route(method, path, params));
        return;
    }

    if (path == "/") path = "/index.html";
    if (path.find("..") != string::npos) { sendResponse(client, 400, "text/plain", "Bad path"); return; }
    bool found;
    string content = readFile("frontend" + path, found);
    if (!found) { sendResponse(client, 404, "text/plain", "Not found"); return; }
    sendResponse(client, 200, contentType(path), content);
}

int main(int argc, char **argv)
{
    int port = (argc > 1) ? atoi(argv[1]) : 8080;

    loadSampleData(shops);           // same sample data as the console version
    delivery.loadDefaultMap();
    delivery.addAgent("Rohit", 1);
    delivery.addAgent("Imran", 4);

    int server = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(server, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        cout << "Could not use port " << port << ". Try: ./localkart 9000\n";
        return 1;
    }
    listen(server, 20);
    cout << "LocalKart running at http://localhost:" << port << "  (Ctrl+C to stop)\n";

    while (true)                      // one request at a time keeps the data safe
    {
        int client = accept(server, NULL, NULL);
        if (client < 0) continue;
        handleClient(client);
        close(client);
    }
}
