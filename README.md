# Local Shop Delivery System

![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)
![Database](https://img.shields.io/badge/database-MySQL%20(XAMPP)-orange)
![Status](https://img.shields.io/badge/status-Phase%20II%20(in%20development)-yellow)

A console-based C++ application that lets small shopkeepers in remote and hilly areas sell their products online, with customers ordering from nearby shops and delivery agents completing the delivery. Built as a Project-Based Learning (PBL) project using **Data Structures, OOP with C++ and DBMS (MySQL)**.

> **Course:** DSCPP &nbsp;|&nbsp; **Team ID:** DSCPP-III-2026-T113 &nbsp;|&nbsp; **Mentor:** Mr. Priyanshu Rawat
> Department of Computer Science & Engineering, Graphic Era (Deemed to be University), Dehradun — Academic Session 2026–27

---

## Table of Contents
1. [Problem Statement](#problem-statement)
2. [Features](#features)
3. [System Architecture](#system-architecture)
4. [Data Structures and OOP Used](#data-structures-and-oop-used)
5. [Database Design](#database-design)
6. [Project Structure](#project-structure)
7. [Getting Started](#getting-started)
8. [Testing](#testing)
9. [Project Status](#project-status)
10. [Team and Contributions](#team-and-contributions)
11. [Git Workflow](#git-workflow)
12. [Roadmap](#roadmap)
13. [References](#references)

---

## Problem Statement
Quick-delivery apps such as Blinkit and Zepto do not reach many remote and hilly areas. People without a vehicle have to walk a long way to the nearest store, and local shopkeepers have no way to sell online.

This project connects **shopkeepers, customers and delivery agents** in one system so that small towns get the same shopping convenience as cities, and local shops and delivery agents get a new source of income.

## Features

| User | What they can do |
|------|------------------|
| **Shopkeeper** | Add, update, delete and view products and stock |
| **Customer** | Browse products, compare the price of the same product across nearby shops (cheapest first), place orders, check order status |
| **Delivery Agent** | Register, view assigned orders, accept and complete deliveries |

**Scope decisions:** console application only, cash on delivery, no online payment, no real GPS (nearby areas are modelled as a small graph), one order per agent at a time.

## System Architecture

The system is built in layers:

```
Input Layer -> Processing Layer -> Core Logic -> Database Layer -> Output Layer
```

Major modules: **Shop Management**, **Order Management**, **Delivery Agent** and **Data Storage (MySQL)**.
Module dependency: `Shop -> Order -> Delivery Agent`, with all data saved in MySQL.

**Order life cycle**

```
PENDING  ->  ACCEPTED  ->  DELIVERED
(placed)     (agent took it)   (completed, cash collected)
```

## Data Structures and OOP Used

| Concept | Where it is used |
|---------|------------------|
| Hash map | Fast product and shop lookup by ID / name |
| Queue | Incoming orders are processed first-come, first-served |
| Linked list | Each order's status chain (pending -> accepted -> delivered) |
| Sorting | Price comparison, cheapest shop first |
| Graph + BFS / Dijkstra | Finding the nearest delivery agent (in progress) |
| OOP (classes, objects, inheritance) | `User` base class with `Shopkeeper`, `Customer` and `DeliveryAgent` |

## Database Design

Database name: `local_shop_db`. The database and all tables are **created automatically** by the program on the first run (`DatabaseSetup`).

| Table | Purpose | Key columns |
|-------|---------|-------------|
| `users` | Shopkeepers, customers and delivery agents | `user_id`, `name`, `phone` (unique), `role`, `area`, `is_available` |
| `products` | Products sold by each shop | `product_id`, `shop_id` (FK), `name`, `price`, `stock` |
| `orders` | Customer orders and their status | `order_id`, `customer_id`, `shop_id`, `product_id`, `quantity`, `agent_id`, `status`, `created_at` |

`orders.status` is one of `PENDING`, `ACCEPTED`, `DELIVERED`.

## Project Structure

```
Local-shop-delivery-system/
├── main.cpp                  # Entry point, DB setup, main menu (integration layer)
├── Makefile                  # Build and test commands
├── README.md
├── .gitignore
├── include/
│   ├── config.h              # Database host / user / password / name
│   ├── DBConnection.h        # MySQL connection handling
│   ├── DatabaseSetup.h       # Creates database, tables and sample data
│   ├── InputValidator.h      # Safe console input (no crashes on wrong input)
│   └── ...                   # Headers of the other modules (products, shops, customers, agents, orders)
├── src/
│   ├── DBConnection.cpp
│   ├── DatabaseSetup.cpp
│   └── ...                   # Implementation of the other modules
├── tests/
│   ├── test_validator.cpp    # Input validation tests (no database needed)
│   └── test_database_flow.cpp# Tables + full order flow test with sample data
└── docs/
    └── screenshots/          # Output screenshots
```

## Getting Started

### Prerequisites
- A C++17 compiler (g++ / MinGW)
- [XAMPP](https://www.apachefriends.org/) (used to run MySQL)
- MySQL Connector/C++ (legacy JDBC-style API, version 8.x)
- Git, and VS Code (recommended)

### Setup
1. **Clone the repository**
   ```bash
   git clone https://github.com/shauryabisht75-ui/Local-shop-delivery-system.git
   cd Local-shop-delivery-system
   ```
2. **Start MySQL** from the XAMPP Control Panel.
3. **Check the database settings** in `include/config.h`. XAMPP's default is user `root` with an empty password.
4. **Build**
   ```bash
   make CONNECTOR_DIR="path/to/mysql-connector-c++"
   ```
   On Linux, after installing `libmysqlcppconn-dev`, plain `make` is enough.
5. **Run**
   ```bash
   ./local_shop_delivery
   ```
   The first run creates `local_shop_db` with the `users`, `products` and `orders` tables and a small set of sample shops, products and agents.

### Common problems
| Problem | Fix |
|---------|-----|
| `Could not connect` | MySQL is not started in XAMPP, or the settings in `config.h` are wrong |
| Linker errors about `mysqlcppconn` | Wrong `CONNECTOR_DIR`; point it to the folder that contains the connector's `include` and `lib` folders |
| Program stops on wrong input | It should not. Input goes through `InputValidator`; please report the case as an issue |

## Testing

```bash
make test          # runs both test programs
```

| Test | What it checks |
|------|----------------|
| `test_validator` | Letters, negative numbers, zero, `nan`, wrong phone numbers and empty names are rejected and re-asked |
| `test_database_flow` | Tables exist, sample data loads, the same product exists in two shops, and one order moves `PENDING -> ACCEPTED -> DELIVERED` |

Phase-II module tests (sample shops and products):

| Test | Result |
|------|--------|
| Shop stock functions (add / update / delete / view) | Pass |
| Product lookup (hash map) | Pass |
| Customer order placing | Pass |
| Price comparison (cheapest shop first) | Pass |
| Delivery agent assignment (nearest agent) | Not tested yet (logic pending) |

## Project Status

**Overall progress: about 60% (Phase II).**

| Module | Status |
|--------|--------|
| Shop management | Completed |
| Data storage (MySQL) | Completed |
| Order management | In progress (order tracking pending) |
| Delivery agent | In progress (nearest-agent assignment and delivery completion pending) |
| Rating system | Planned for Phase III |

## Team and Contributions

| Member | Roll No. | Role | Contribution |
|--------|----------|------|--------------|
| **Shaurya Bisht** | 2510012399 | Team Lead | Repository and folder structure, order queue and linked list, MySQL tables, database connection and setup, input validation, integration and final testing |
| Mayank Ranjan | 2510011761 | Shop management | `Product` and `Shop` classes, hash-map lookup, add / update / delete / view stock |
| Raj Vardhan | 2510010483 | Customer module | `Customer` class, browse and place order, price comparison by sorting |
| Aradhay Singh | 2510011780 | Delivery agent module | `User` base class, `DeliveryAgent` class (inheritance), agent registration, viewing assigned orders |

## Git Workflow
- One module per member, one branch per member:
  `shaurya/order-queue`, `mayank/shop-database`, `raj/customer-module`, `aradhay/delivery-agent`
- Small, meaningful commits (for example `Add price comparison using sorting`)
- Branches are merged into `main` only **after review by the team lead**

## Roadmap

**Phase III**
- [ ] Order tracking screen for customers
- [ ] Nearest delivery agent using Dijkstra on the local-area graph
- [ ] Delivery completion (accepted -> delivered)
- [ ] Shop and delivery agent rating system
- [ ] Input validation and error handling in all modules
- [ ] Merge all branches into `main` after lead review
- [ ] Full flow test: shopkeeper -> customer -> delivery agent
- [ ] Final report and demo

**Future scope:** mobile app and real-time tracking, built on top of this console version.

## References
1. T. H. Cormen et al., *Introduction to Algorithms*
2. A. Silberschatz, H. Korth, S. Sudarshan, *Database System Concepts*
3. B. Stroustrup, *The C++ Programming Language*
4. Oracle, *MySQL Connector/C++ Developer Guide*
