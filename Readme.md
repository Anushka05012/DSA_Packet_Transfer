# 💻 Computer Network Simulation in C

This project simulates a basic computer network using **C**. It allows users to add/remove computers (nodes), add communication routes (edges with weights), find the shortest path using **Dijkstra’s Algorithm**, simulate **data transfer** using threads, and **visualize the network** using Graphviz.

---

## 📌 Features

- ➕ Add and remove computers (nodes)
- 🔗 Add routes between computers with custom weights (in milliseconds)
- 🔄 Dijkstra’s algorithm for finding the shortest path
- 📁 File-based simulated data transfer using threads
- 📊 Export network to Graphviz `.dot` format
- 💾 Persistent network using `network.txt` (auto-save and load)

---

## 🚀 How It Works

Each computer is simulated as a node with its own text file (`computer_X.txt`) representing its data. Data transfer is simulated by copying content from one file to another in **packets**, following the shortest route.

---

## 📂 File Structure

📁 project/
├── main.c # Source code
├── network.txt # Persistent network structure (auto-created)
├── computer_0.txt # Files created per computer (auto-created)
├── network.dot # Graphviz export file
└── README.md # This file

---
## 💽 Menu Options
1. Add Computer          --> Adds a new computer node
2. Remove Computer       --> Removes an existing computer node
3. Add Route             --> Adds a bidirectional route between two computers
4. Transfer File         --> Simulates file transfer between computers
5. Export Network        --> Exports current network as Graphviz .dot file
6. Exit                  --> Saves and exits

---
## 🖼️ Network Visualization (Graphviz)
After exporting (Option 5), use:

```txt
dot -Tpng network.dot -o network.png
```

---
## 🛠️ Future Improvements
GUI using GTK or WinAPI

Real network communication using sockets

Packet loss & retransmission simulation

Dynamic traffic analysis
