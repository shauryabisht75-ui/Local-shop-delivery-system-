# Shop Delivery and Management System

![Backend](https://img.shields.io/badge/backend-C%2B%2B11-blue)
![Frontend](https://img.shields.io/badge/frontend-HTML%20%7C%20CSS%20%7C%20JavaScript-orange)
![Storage](https://img.shields.io/badge/storage-text%20files-lightgrey)
![Phase III](https://img.shields.io/badge/Phase%20III-completed-brightgreen)
![Tests](https://img.shields.io/badge/tests-165%20API%20%2B%20114%20browser%20checks-brightgreen)

A hyperlocal quick-delivery website where every order is fulfilled by a **neighbourhood shop** instead of a company-owned store. Customers order from nearby shops, shopkeepers get an online sales channel, and delivery agents earn by delivering. The whole backend is written in **C++** using **Data Structures and OOP**, with a small web server built in, so you do not need Apache, MySQL, Node or any library to run it.

> **Course:** DSCPP &nbsp;|&nbsp; **Team ID:** DSCPP-III-2026-T113 &nbsp;|&nbsp; **Mentor:** Mr. Priyanshu Rawat
> Department of Computer Science & Engineering, Graphic Era (Deemed to be University), Dehradun, Academic Session 2026–27
>
> *Earlier name used in the Phase-I and Phase-II documents: "Local Shop Delivery System".*

![Customer home page](docs/screenshots/customer-home.png)

---

## Table of Contents
1. [Problem Statement](#problem-statement)
2. [Features](#features)
3. [How an Order Works](#how-an-order-works)
4. [System Architecture](#system-architecture)
5. [Data Structures and OOP Used](#data-structures-and-oop-used)
6. [Data Storage](#data-storage)
7. [Project Structure](#project-structure)
8. [Getting Started](#getting-started)
9. [Demo Logins](#demo-logins)
10. [Five-Minute Demo](#five-minute-demo)
11. [Live Tracking and Maps](#live-tracking-and-maps)
12. [Testing](#testing)
13. [Screenshots](#screenshots)
14. [Project Phases](#project-phases)
    - [Phase I: Planning and Design](#phase-i-planning-and-design)
    - [Phase II: Core Module Development](#phase-ii-core-module-development)
    - [Phase III: Final Version](#phase-iii-final-version)
15. [Team and Contributions](#team-and-contributions)
16. [Git Workflow](#git-workflow)
17. [Limitations and Future Scope](#limitations-and-future-scope)
18. [References](#references)

---

## Problem Statement
Apps such as Blinkit and Zepto need their own stores and a large delivery network to work. Small neighbourhood shops have no online channel, and customers cannot easily get home delivery from them.

This project connects **shopkeepers, customers, delivery agents and an admin** on one platform. Shops get online sales, customers get doorstep delivery, and delivery agents get a new source of income.

## Features

| User | What they can do |
|------|------------------|
| **Customer** | Pick an area, search and browse products, **compare the price of the same product across nearby shops**, fill the cart, choose how the shop is picked (**nearest / cheapest / best overall**), pay by cash on delivery, UPI or card (simulated), **track the delivery live on a map**, see the full bill, cancel before packing |
| **Shopkeeper** | Create the shop, set price and stock for each product, handle the order desk **first-come-first-served** (accept or reject, then pack), see earnings after commission |
| **Delivery agent** | Register, get the nearest order automatically, confirm pickup and delivery, see earnings |
| **Admin** | Live operations map, money settlement (who gets what), all orders, users and shops, add products to the catalog, map settings |

**Scope decisions:** no real GPS (the town is a small graph of areas and the rider is simulated), payments are simulated, and each delivery agent handles one order at a time.

## How an Order Works

1. **Area.** The town is modelled as a graph of 8 areas joined by roads with fixed lengths in km. The customer picks an area. Only shops within 8 km can serve it.
2. **Cart and shop choice.** The system keeps only the shops that have **every** item in stock and ranks them as *nearest*, *cheapest items* or *best overall* (items + delivery fee).
3. **Bill.** `items total + delivery fee (Rs 20 + Rs 5 per km) + small-cart fee (Rs 15 if items are below Rs 100) + platform fee (Rs 5)`.
4. **Placing the order.** Stock is **reserved immediately**, so two customers cannot buy the last unit. The order joins the shop's first-come-first-served queue as `PLACED`.
5. **Shop desk.** Only the oldest new order can be accepted or rejected. *Reject* sends the order to the next best shop (a UPI or card payment is refunded if no shop is left). *Pack* makes it `PACKED`.
6. **Delivery agent.** Dijkstra's algorithm runs from the shop's area and the **nearest free agent** gets the order. If every agent is busy, the order waits in the dispatch queue until one is free.
7. **Delivery.** The agent confirms pickup, the rider travels shop to customer, and the agent confirms delivery.
8. **Money.** The shop gets the item total minus 10% commission, the agent gets 80% of the delivery fee, and the platform keeps the rest.

**Order status flow** (any other jump is refused):

```
PLACED -> ACCEPTED -> PACKED -> AGENT_ASSIGNED -> PICKED_UP -> DELIVERED
   |
   +--> CANCELLED (by customer, before packing)   +--> REJECTED (no shop can fulfil)
```

**Worked example** (3 km order): items Rs 31 + Rs 43 = **Rs 74**, delivery Rs 20 + 3 x Rs 5 = **Rs 35**, small-cart fee **Rs 15**, platform fee **Rs 5**, so the customer pays **Rs 129**.
The shop gets 74 x 90% = **Rs 66.60**, the agent gets 35 x 80% = **Rs 28**, and the platform keeps **Rs 34.40**.

## System Architecture

```
Browser  (frontend/: HTML + CSS + JavaScript, Leaflet / OpenStreetMap or Google Maps)
   |   JSON over HTTP, forms and buttons as input, pages / order status / ETA / map as output
   v
backend/main.cpp  ->  small built-in web server (src/http.h)
   |-- src/api*.h     URL list: which URL calls which function
   |-- six C++ modules (Common, Shop, Order, Customer, Delivery Agent, Storage)
   `-- backend/data/*.txt   text-file storage, saved after every change
```

| Layer | What it does |
|-------|--------------|
| **Interface layer** | Web pages for each role: forms, buttons, tracking map |
| **Core processing** | Validates every input, then runs the module logic (hash tables, queues, linked list, sorting, Dijkstra) |
| **Storage layer** | Text files, saved after each change and loaded when the server starts |

The API answers `{ "ok": true, ... }` or `{ "ok": false, "error": ... }` with the codes 400 (bad input), 401 (not logged in), 403 (wrong role), 404 (not found) and 409 (rule broken, for example an invalid status change). Login uses a token per session.

## Data Structures and OOP Used

| Concept | Where it is used | File |
|---------|------------------|------|
| **Linked list** | All orders, kept in one chain with their status | `order.h`, `types.h` |
| **Linked FIFO queue** | Each shop's order desk and the agent dispatch queue | `types.h`, `agent.h` |
| **Chained hash table** | Shop inventory (key = shop id + product id) and product names, O(1) on average | `shop.h` |
| **Graph (adjacency matrix) + Dijkstra** | 8-area map, nearest free agent and the delivery route | `common.h`, `agent.h` |
| **Sorting** | Bubble sort for ranking shops and price comparison, insertion sort to keep the catalog in name order | `order.h`, `customer.h`, `shop.h` |
| **Arrays** | Users, shops, products, payments (fixed size, simple indexes) | `types.h`, `common.h` |
| **OOP: classes, objects, inheritance, virtual functions** | `User` base class with `Customer`, `Shopkeeper`, `DeliveryAgent` and `Admin` | `types.h` |

## Data Storage
All data is stored as plain text files in `backend/data/` (created on the first run, with demo data):
`users`, `agents`, `products`, `shops`, `inventory`, `orders`, `order_items`, `payments` and `area_graph`.

To start again from the demo data, stop the server and delete the `backend/data/` folder.

## Project Structure

```
Local-shop-delivery-system-/
├── README.md
├── run.sh / run.bat              # compile (when needed) and start the server
├── backend/
│   ├── main.cpp                  # entry point: options, server loop
│   └── src/
│       ├── common.h              # MODULE 1: fee constants, validation, area graph, Dijkstra
│       ├── shop.h                # MODULE 2: catalog, inventory hash tables, shop desk
│       ├── order.h, types.h      # MODULE 3: queue, linked list, status flow, allocation, fees, ETA
│       ├── customer.h            # MODULE 4: cart, search, price comparison, checkout, cancel
│       ├── agent.h               # MODULE 5: dispatch, pickup / delivery, tracking simulation
│       ├── storage.h, seed.h     # MODULE 6: save / load text files, login, demo data
│       ├── http.h, json.h        # small web server and JSON writer
│       └── api*.h                # URL routes for each role
├── frontend/
│   ├── index.html
│   ├── css/                      # base.css, pages.css
│   ├── js/                       # one file per screen group (home, cart, orders, shop, agent, admin) + map.js
│   └── vendor/leaflet/           # Leaflet 1.9.4 (map library)
├── tests/                        # Python API tests, Node + Playwright browser tests
└── docs/
    ├── original_console_version.cpp   # our first console version, kept as it was
    ├── PROJECT_STATE.md               # decisions, API list and checklist
    ├── blinkit_blueprint_prompt.md    # design rules used for the project
    └── screenshots/
```

## Getting Started

### Prerequisites
- A C++ compiler (`g++`). Nothing else is needed to run the website.
- *Only for the tests:* Python 3, and Node.js with Playwright for the browser tests.

### Run
**Linux / macOS**
```bash
git clone https://github.com/shauryabisht75-ui/Local-shop-delivery-system-.git
cd Local-shop-delivery-system-
./run.sh            # compiles the first time (10-20 s), then starts the server
```

**Windows** (MinGW-w64 or MSYS2 `g++` on PATH)
```bat
run.bat
```

**By hand, any system**
```bash
cd backend
g++ -std=c++11 -O2 -o lsds main.cpp      # on Windows add  -lws2_32  at the end
./lsds
```

Then open **http://localhost:8080** in a browser. Always use this address; do not double-click `index.html`.

### Server options
Add them after `./run.sh`, for example `./run.sh --port 8090 --lan`.

| Option | Meaning |
|--------|---------|
| `--port N` | Web port (default 8080) |
| `--lan` | Let phones and laptops on the same Wi-Fi open the site: `http://<this-computer-ip>:8080` |
| `--sim-seconds-per-km S` | Demo rider speed in real seconds per km (default 12, so a 6 km ride takes 72 s) |
| `--realtime` | Rider speed like real life (4 minutes per km) |
| `--no-demo` | First start only: create just the admin, without demo shops, agents and customers |
| `--data FOLDER`, `--web FOLDER` | Where the text files and the website files are |

### Console version
Our first version was a console program. It is kept in `docs/original_console_version.cpp`:
```bash
g++ -std=c++11 -o lsds_console docs/original_console_version.cpp
./lsds_console
```

### Common problems
| Problem | Fix |
|---------|-----|
| `g++ was not found` | Install a compiler: `sudo apt install g++` (Ubuntu), `xcode-select --install` (macOS), MinGW-w64 or MSYS2 (Windows) |
| Page does not open | Use `http://localhost:8080`, not the `index.html` file. Check that the port is free, or use `--port` |
| Want fresh demo data | Stop the server and delete `backend/data/` |

## Demo Logins
Created automatically on the first start. All demo passwords are **`demo123`**, except the admin. The login window also has one-click demo buttons, and you can create your own accounts.

| Role | Phone | Who |
|------|-------|-----|
| Customer | 9200000001 | Rahul Mehta |
| Customer | 9200000002 | Priya Joshi |
| Shopkeeper | 9000000001 | Sharma General Store (Market Chowk) |
| Shopkeeper | 9000000002 / 03 / 04 | Gupta Kirana & Dairy, Singh Fresh Mart, Hill View Mart |
| Delivery agent | 9100000001 / 02 / 03 | Amit Kumar, Rohit Verma, Sanjay Rawat |
| Admin | 9999999999 | password **`admin123`** |

## Five-Minute Demo
Every browser tab keeps its own login, so one person can play all four roles in four tabs.

1. **Tab 1, customer:** choose *Temple Road*, add *Milk* and *Bread*, open the cart, pick *Nearest shop*, log in as 9200000001 and place the order. You land on the tracking page.
2. **Tab 2, shopkeeper:** log in as 9000000001. The new order is at the front of the queue: **Accept**, then **Pack**.
3. **Tab 1** now shows who will deliver (*Amit Kumar*), found with Dijkstra, and the rider on the map.
4. **Tab 3, delivery agent:** log in as 9100000001 and press **Confirm pickup**. The rider starts moving on Tab 1's map.
5. When the rider reaches the house, press **Confirm delivery**. Tab 1 shows *Delivered*.
6. **Tab 4, admin** (9999999999 / admin123): *Live map* shows every delivery, *Money* shows how the Rs 129 was split.

## Live Tracking and Maps
The map uses **OpenStreetMap (Leaflet) by default**, which is free and needs no key. If you add a **Google Maps key**, the same screens use Google Maps instead.

To use Google Maps, paste your key in `frontend/js/config.js`:
```js
GOOGLE_MAPS_API_KEY: "AIza..."
```
A small bar above every map shows which map is in use. If Google refuses the key, the site switches to OpenStreetMap by itself.

**How "real-time" works.** We do not use real GPS, so the **rider is simulated by the C++ server**. It moves the rider along the Dijkstra path 10 times a second. The customer's page asks the server every 2 seconds and glides the marker along the road, with ETA and distance left. Replacing the simulation with a phone's GPS would only change where the rider position comes from.

## Testing

```bash
bash tests/run_backend.sh     # needs Python 3
bash tests/run_ui.sh          # needs Node.js + Playwright (Chromium)
```

| Test | What it checks | Result |
|------|----------------|--------|
| Backend API tests (5 suites, 165 checks) | Valid status flow, correct fees, roles enforced, FIFO shop desk, no overselling, reject and refund, dispatch queue, data survives a restart | Pass |
| Browser tests (114 checks) | Every screen of all four roles, phone layout, Google Maps code path | Pass |
| Integration (end to end) | Full order from customer to shop to delivery agent, bill and money split correct | Pass |
| Input validation | Wrong input shows an error and the app keeps running | Pass |

## Screenshots

| Customer price comparison | Checkout |
|---|---|
| ![Compare prices](docs/screenshots/customer-compare-prices.png) | ![Checkout](docs/screenshots/customer-checkout.png) |

| Live tracking | Shop order desk |
|---|---|
| ![Live tracking](docs/screenshots/customer-live-tracking.png) | ![Shop order desk](docs/screenshots/shop-order-desk.png) |

| Delivery agent app | Admin money settlement |
|---|---|
| ![Agent app](docs/screenshots/agent-app.png) | ![Admin money](docs/screenshots/admin-money.png) |

*The map background in the test screenshots is a plain stand-in tile because the test machine had no internet. On a normal computer the map shows real tiles.*

## Project Phases

| Phase | Focus | Status |
|-------|-------|--------|
| **Phase I** | Proposal, problem definition, module design, technology and DSA plan | ✅ Completed |
| **Phase II** | Development of the main modules, database connection, first round of testing (about 60% of the project) | ✅ Completed |
| **Phase III** | Nearest-agent assignment (Dijkstra), live tracking, integration, full testing, final report and demo | ✅ Completed |

### Phase I: Planning and Design

Phase I was the planning phase. We submitted the proposal and finalised the problem, the modules, the technology stack and the data structure plan.

**Planned scope:** a console-based C++ application, cash on delivery, no real GPS (nearby areas modelled as a small graph), and one order per agent at a time.

**Planned architecture:** five layers, `Input Layer -> Processing Layer -> Core Logic -> Database Layer -> Output Layer`, with four modules: Shop Management, Order Management, Delivery Agent and Data Storage. The modules depend on each other as `Shop -> Order -> Delivery Agent`.

**Planned order life cycle:** `PENDING -> ACCEPTED -> DELIVERED`

**Data structure plan**

| Concept | Planned use |
|---------|-------------|
| Hash map | Fast product and shop lookup |
| Queue | Orders handled first-come, first-served |
| Linked list | Status chain of each order |
| Sorting | Price comparison, cheapest shop first |
| Graph search (BFS or Dijkstra) | Finding the nearest delivery agent |
| OOP (classes, objects, inheritance) | `User` base class with `Shopkeeper`, `Customer` and `DeliveryAgent` |

**Technology stack chosen:** C++, MySQL (XAMPP), VS Code, Git and GitHub.

**What was completed in Phase I**
- [x] Proposal submitted
- [x] Problem statement and objectives finalised
- [x] Requirement analysis done
- [x] System design done (modules, layers, order flow)
- [x] Technology stack chosen
- [x] Data structure plan finalised
- [x] Team roles and module split decided

**Feedback received in Phase I**

| Feedback | What we did |
|----------|-------------|
| Keep the scope realistic | Fixed the scope: no real GPS, no real payment gateway. An area graph with Dijkstra finds the nearest agent. Rider GPS, a payment gateway and a mobile app moved to future scope |
| Show where DSA is actually needed; divide work clearly | Each structure is tied to a feature (hash table for inventory, FIFO queue for the shop desk, linked list for orders, graph for the agent). One module and one Git branch per member, with review by the team lead |

### Phase II: Core Module Development

Phase II was about building the main modules, connecting the database and running the first round of tests. At the end of this phase the project was **about 60% complete**.

**Module status at the end of Phase II**

| Module | Status |
|--------|--------|
| Shop management (add, update, delete, view products and stock) | Completed |
| Data storage (MySQL tables and C++ connection) | Completed |
| Customer (browse, place order, price comparison by sorting) | Completed |
| Order management | In progress (order tracking pending) |
| Delivery agent | In progress (registration and viewing assigned orders done; nearest-agent assignment and delivery completion pending) |

**Phase II test results**

| Test | Result |
|------|--------|
| Shop stock functions (add / update / delete / view) | Pass |
| Product lookup (hash map) | Pass |
| Customer order placing | Pass |
| Price comparison (cheapest shop first) | Pass |
| Delivery agent assignment (nearest agent) | Not tested yet (logic pending) |

**Pending after Phase II and how Phase III finished it**

| Pending item | What we did in Phase III |
|--------------|--------------------------|
| Nearest-agent logic (BFS or Dijkstra not yet decided) | Dijkstra on the 8-area graph, chosen over BFS because the distances between areas are unequal. The pending test now passes |
| Input validation, integration and final testing | Validation in every module, all branches merged into `main`, full API and browser test suites added |

### Phase III: Final Version

Phase III finished the remaining work and turned the console program into a working website. The system was also extended beyond the Phase II plan:

- Nearest free agent found with Dijkstra, pickup and delivery steps, and delivery completion
- Order tracking with a live map (simulated rider)
- Web frontend, a built-in C++ web server and a JSON API
- Shop choice by nearest, cheapest or best overall, with a full bill and revenue split
- Admin role with a live map and money settlement
- Input validation in all modules, full integration and testing (165 API checks, 114 browser checks)

**What changed from the Phase II plan**

| | Phase II | Phase III (final) |
|---|----------|-------------------|
| Interface | Console application | Web app in the browser (the console version is kept in `docs/`) |
| Data storage | MySQL (XAMPP) | Text files, so nothing except a C++ compiler is needed to run it |
| Payment | Cash on delivery only | Cash on delivery, UPI or card (simulated) |
| Order status | `PENDING -> ACCEPTED -> DELIVERED` | `PLACED -> ACCEPTED -> PACKED -> AGENT_ASSIGNED -> PICKED_UP -> DELIVERED` |
| Users | Shopkeeper, customer, delivery agent | Same three, plus an admin |
| Rating system | Planned for Phase III | Moved to future scope |
| MySQL database | Used | Moved to future scope |

## Team and Contributions

| Member | Roll No. | Role | Contribution |
|--------|----------|------|--------------|
| **Shaurya Bisht** | 2510012399 | Team Lead | Repository and folder structure, order queue, linked-list status flow, shop allocation, fees, integration and final testing |
| Mayank Ranjan | 2510011761 | Shop management | `Product` and `Shop` classes, hash tables, stock add / update / delete / view |
| Raj Vardhan | 2510010483 | Customer module | `Customer` class, cart, browsing, price comparison by sorting, order tracking |
| Aradhay Singh | 2510011780 | Delivery agent module | `User` and `DeliveryAgent` classes, registration, Dijkstra nearest agent, delivery completion, tracking simulation |

## Git Workflow
- One module per member, one branch per member: `shaurya/order-queue`, `mayank/shop-database`, `raj/customer-module`, `aradhay/delivery-agent`
- Small, meaningful commits
- Branches are merged into `main` only **after review by the team lead**

## Limitations and Future Scope

**Current limitations**
- Google Maps with a real API key has not been tested (the OpenStreetMap path is tested in a real browser)
- The Windows build has not been tested (Linux was)
- Payments are simulated. UPI and card details are only checked for the right format and are never stored or sent anywhere
- Passwords use a simple hash, which is fine for learning. A real system would use bcrypt or argon2
- The server handles one request at a time, which is enough for a classroom demo
- No rating system yet

**Future scope**
- Rating system for shops and delivery agents
- Real rider GPS instead of the simulation
- A real payment gateway
- A MySQL database in place of text files
- A mobile app

## References
1. T. H. Cormen et al., *Introduction to Algorithms*
2. B. Stroustrup, *The C++ Programming Language*
3. Leaflet and OpenStreetMap documentation
