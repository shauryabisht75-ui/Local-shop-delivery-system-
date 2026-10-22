// ============================================================
// user_agent_module.h  -  User base class, DeliveryAgent,
//                         DeliveryManager (graph + Dijkstra)
// Part of: Delivery Agent module (Aradhay Singh, 2510011780)
// Team ID: DSCPP-III-2026-T113
//
// Concepts used: classes, inheritance, virtual functions,
//                vector, priority_queue, weighted graph, Dijkstra
// ============================================================
#ifndef USER_AGENT_MODULE_H
#define USER_AGENT_MODULE_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>        // priority_queue for Dijkstra
#include "common_input.h"
#include "order_module.h"
using namespace std;

const int INF = 1000000000;

// ---------- Areas of the town (nodes of the graph) ----------
const int AREA_COUNT = 6;
const string AREA_NAMES[AREA_COUNT] = {
    "Main Market", "Bus Stand", "Temple Road",
    "School Road", "Hill Colony", "River Side"};

inline void showAreas()
{
    for (int i = 0; i < AREA_COUNT; i++)
        cout << "  " << i << ". " << AREA_NAMES[i] << "\n";
}

// ---------- Base class for every kind of user ----------
class User
{
protected:
    int id;
    string name;

public:
    User(int id, string name)
    {
        this->id = id;
        this->name = name;
    }

    int getId() const { return id; }
    string getName() const { return name; }

    virtual void display() const
    {
        cout << "ID: " << id << "\nName: " << name << "\n";
    }

    virtual void showMenu() {}      // each user type shows its own menu

    virtual ~User() {}
};

// ---------- Delivery agent ----------
class DeliveryAgent : public User
{
public:
    int location;
    bool available;

    DeliveryAgent(int id, string name, int location) : User(id, name)
    {
        this->location = location;
        available = true;
    }

    void display() const override
    {
        User::display();
        cout << "Location: " << AREA_NAMES[location]
             << "\nStatus: " << (available ? "Available" : "Busy") << "\n";
    }
};

// ---------- Handles agents, the town map and delivery ----------
class DeliveryManager
{
private:
    vector<vector<pair<int, int> > > graph;   // graph[u] = list of (v, distance)
    vector<DeliveryAgent> agents;
    OrderList &orders;
    int nextAgentId;

    // shortest distance from source to every area
    vector<int> dijkstra(int source)
    {
        vector<int> dist(AREA_COUNT, INF);
        priority_queue<pair<int, int>, vector<pair<int, int> >,
                       greater<pair<int, int> > > pq;

        dist[source] = 0;
        pq.push(make_pair(0, source));

        while (!pq.empty())
        {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();
            if (d != dist[u])
                continue;
            for (int i = 0; i < (int)graph[u].size(); i++)
            {
                int v = graph[u][i].first;
                int w = graph[u][i].second;
                if (dist[u] + w < dist[v])
                {
                    dist[v] = dist[u] + w;
                    pq.push(make_pair(dist[v], v));
                }
            }
        }
        return dist;
    }

    DeliveryAgent *findAgent(int agentId)
    {
        for (int i = 0; i < (int)agents.size(); i++)
        {
            if (agents[i].getId() == agentId)
                return &agents[i];
        }
        return NULL;
    }

public:
    DeliveryManager(OrderList &orders) : orders(orders), nextAgentId(1)
    {
        graph.resize(AREA_COUNT);
    }

    void addRoad(int u, int v, int distance)
    {
        graph[u].push_back(make_pair(v, distance));
        graph[v].push_back(make_pair(u, distance));
    }

    // fixed map of the town (distance in km)
    void loadDefaultMap()
    {
        addRoad(0, 1, 4);
        addRoad(0, 2, 2);
        addRoad(1, 3, 5);
        addRoad(2, 3, 3);
        addRoad(2, 4, 6);
        addRoad(3, 5, 4);
        addRoad(4, 5, 2);
    }

    int addAgent(string name, int location)
    {
        agents.push_back(DeliveryAgent(nextAgentId, name, location));
        nextAgentId++;
        return nextAgentId - 1;
    }

    void registerAgent()
    {
        string name = getText("Enter agent name: ");
        cout << "Areas:\n";
        showAreas();
        int location = getNumber("Enter agent's current area number: ", 0, AREA_COUNT - 1);
        int agentId = addAgent(name, location);
        cout << "Agent registered successfully. Agent ID: " << agentId << "\n";
    }

    void viewAgents() const
    {
        if (agents.empty())
        {
            cout << "No delivery agents registered.\n";
            return;
        }
        for (int i = 0; i < (int)agents.size(); i++)
        {
            cout << "\n--------------------\n";
            agents[i].display();
        }
    }

    // Accepted order -> find the nearest free agent with Dijkstra
    void assignAgent()
    {
        if (agents.empty())
        {
            cout << "Register an agent first.\n";
            return;
        }

        bool any = false;
        cout << "\nAccepted orders waiting for an agent:\n";
        for (int i = 0; i < orders.size(); i++)
        {
            Order *o = orders.get(i);
            if (o->getStatus() == ACCEPTED && o->getAgentId() == 0)
            {
                cout << "Order #" << o->getId() << " | " << o->getCustomerName()
                     << " | " << o->getQty() << " x " << o->getProductName()
                     << " | Area: " << AREA_NAMES[o->getLocation()] << "\n";
                any = true;
            }
        }
        if (!any)
        {
            cout << "None. (Orders must be accepted by the shopkeeper first.)\n";
            return;
        }

        int orderId = getNumber("Enter order ID to assign (0 to go back): ", 0, 999999);
        if (orderId == 0)
            return;
        Order *o = orders.findById(orderId);
        if (o == NULL || o->getStatus() != ACCEPTED || o->getAgentId() != 0)
        {
            cout << "That order is not waiting for an agent.\n";
            return;
        }

        vector<int> dist = dijkstra(o->getLocation());
        int best = -1;
        int bestDistance = INF;
        for (int i = 0; i < (int)agents.size(); i++)
        {
            if (agents[i].available && dist[agents[i].location] < bestDistance)
            {
                bestDistance = dist[agents[i].location];
                best = i;
            }
        }

        if (best == -1)
        {
            cout << "No available agent can reach this location.\n";
            return;
        }

        agents[best].available = false;      // one order per agent at a time
        o->setAgentId(agents[best].getId());

        cout << "\nDelivery assigned successfully!\n";
        cout << "Order ID: " << o->getId() << "\n";
        cout << "Customer: " << o->getCustomerName() << "\n";
        cout << "Agent: " << agents[best].getName() << " (ID " << agents[best].getId() << ")\n";
        cout << "Agent distance from customer: " << bestDistance << " km\n";
    }

    void viewDeliveries()
    {
        if (orders.size() == 0)
        {
            cout << "No orders available.\n";
            return;
        }
        for (int i = 0; i < orders.size(); i++)
        {
            Order *o = orders.get(i);
            cout << "\n--------------------\n";
            cout << "Order ID: " << o->getId() << "\n";
            cout << "Customer: " << o->getCustomerName() << "\n";
            cout << "Item: " << o->getQty() << " x " << o->getProductName()
                 << " (" << o->getShopName() << ")\n";
            cout << "Area: " << AREA_NAMES[o->getLocation()] << "\n";
            if (o->getAgentId() != 0)
                cout << "Agent ID: " << o->getAgentId() << "\n";
            else
                cout << "Agent: not assigned\n";
            cout << "Status: " << o->getStatusText() << "\n";
        }
    }

    // Accepted -> Delivered, agent becomes free again
    void completeDelivery()
    {
        int orderId = getNumber("Enter order ID (0 to go back): ", 0, 999999);
        if (orderId == 0)
            return;
        Order *o = orders.findById(orderId);
        if (o == NULL)
        {
            cout << "Order not found.\n";
            return;
        }
        if (o->getStatus() != ACCEPTED || o->getAgentId() == 0)
        {
            cout << "This order is not out for delivery.\n";
            return;
        }

        o->setStatus(DELIVERED);
        DeliveryAgent *agent = findAgent(o->getAgentId());
        if (agent != NULL)
        {
            agent->available = true;
            agent->location = o->getLocation();   // agent is now at the customer's area
        }
        cout << "Order " << o->getId() << " status updated to Delivered.\n";
        cout << "Delivery agent is now available.\n";
    }

    void menu()
    {
        while (true)
        {
            cout << "\n===== DELIVERY AGENT MENU =====\n";
            cout << "1. Register delivery agent\n";
            cout << "2. View delivery agents\n";
            cout << "3. Assign nearest agent to an order\n";
            cout << "4. View all orders\n";
            cout << "5. Complete delivery\n";
            cout << "0. Back\n";
            int choice = getNumber("Enter choice: ", 0, 5);

            if (choice == 1)
                registerAgent();
            else if (choice == 2)
                viewAgents();
            else if (choice == 3)
                assignAgent();
            else if (choice == 4)
                viewDeliveries();
            else if (choice == 5)
                completeDelivery();
            else
                break;
        }
    }
};

#endif
