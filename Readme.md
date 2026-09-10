# DartDB

> A Redis-inspired in-memory key-value database built from scratch in C++.

DartDB is a lightweight client–server database created to explore the fundamentals of real database and systems software: TCP networking, concurrency, synchronization, and persistence.

It supports multiple concurrent clients, protects shared data with mutexes, and survives server restarts using an append-only command log.

---

## Features

- In-memory key-value storage using `std::unordered_map`
- TCP client–server architecture (IPv4, loopback)
- Multiple concurrent clients using `std::thread`
- Thread-safe shared database using `std::mutex`
- Persistent storage across server restarts
- Append-only persistence log
- Values containing spaces
- Interactive command-line client
- Automated command tests
- Built-in `PING` and `INFO` commands
- Local performance benchmarking

---

## Supported Commands

| Command           | Description                     |
|-------------------|---------------------------------|
| `SET key value`   | Store a value                   |
| `GET key`         | Retrieve a value                |
| `DEL key`         | Delete a key                    |
| `EXISTS key`      | Check whether a key exists      |
| `PING`            | Check server connectivity       |
| `INFO`            | Display server information      |
| `EXIT`            | Close the client                |

### Example

```text
DartDB> SET name Sharvil Meshram
OK

DartDB> GET name
Sharvil Meshram

DartDB> EXISTS name
YES

DartDB> DEL name
OK

DartDB> GET name
(nil)
```

---

## Architecture

```text
┌──────────────────┐
│   Client 1       │
└────────┬─────────┘
         │
┌────────▼─────────┐
│                  │
│   DartDB         │
│   Server         │
│                  │
└───────┬──────────┘
        │
┌───────▼──────────┐
│ Shared Database  │
│ std::unordered_map│
└───────┬──────────┘
        │
┌───────▼───────┐
│ dartdb.log    │
│ Persistence   │
└───────────────┘
```

- Each connected client is handled by a separate thread.
- A mutex protects the shared in-memory database from concurrent access.
- All mutating commands are appended to `dartdb.log` for durability.

---

## Tech Stack

- C++17
- Winsock2 (Windows sockets)
- TCP/IP
- `std::thread`
- `std::mutex`
- `std::unordered_map`
- File I/O
- PowerShell / Windows

---

## Project Structure

```text
DartDB/
│
├── src/
│   ├── main.cpp
│   ├── server.cpp
│   └── client.cpp
│
├── tests/
│   ├── benchmark.cpp
│   └── test_commands.cpp
│
├── dartdb.db          # (optional, not committed)
├── dartdb.log         # (optional, not committed)
├── README.md
└── .gitignore
```

Recommended `.gitignore`:

```gitignore
*.db
*.log
*.exe
```

---

## Build

Make sure you have a modern C++ compiler installed (e.g., MinGW, MSVC with appropriate flags).

From the project root:

### Build server

```bash
g++ -std=c++17 src/server.cpp -o dartdb-server -pthread -lws2_32
```

### Build client

```bash
g++ -std=c++17 src/client.cpp -o dartdb-client -lws2_32
```

### Build tests

```bash
g++ -std=c++17 tests/test_commands.cpp -o test_commands -lws2_32
```

### Build benchmark

```bash
g++ -std=c++17 tests/benchmark.cpp -o benchmark -pthread -lws2_32
```

---

## Running DartDB

### 1. Start the server

```bash
.\dartdb-server.exe
```

The server listens on:

```text
127.0.0.1:6379
```

### 2. Start a client

Open another terminal:

```bash
.\dartdb-client.exe
```

You can open multiple client terminals at the same time; each will be handled in its own thread.

---

## Persistence

DartDB stores data in memory for fast access and records modifications in an append-only log.

For example:

```text
SET name Sharvil Meshram
```

is recorded as a log entry. When DartDB restarts, it:

1. Loads the stored state (if any).
2. Replays the log to rebuild the in-memory database.

This allows data to survive server restarts.

---

## Benchmark

Benchmarks were performed locally on a Windows machine.

### Sequential benchmark

- Requests: 100  
- Time: 0.110262 seconds  
- Requests/sec: **906.932**

### Concurrent benchmark

- Clients: 10  
- Requests/client: 100  
- Total requests: 1000  
- Time: 1.27661 seconds  
- Requests/sec: **783.326**

### Persistence optimization

The persistence system was optimized from rewriting the entire database file after every write to an append-only log.

- Baseline: **116.416 requests/sec**  
- After optimization: **906.932 requests/sec**  

≈ **7.8× improvement** in the local benchmark.

> Benchmark results depend on hardware, OS, compiler, and workload.

---

## Testing

DartDB includes an automated command test suite.

Current result:

- **8/8 tests passed**

The test suite covers:

- `PING`
- `SET`
- `GET`
- `EXISTS`
- `DEL`
- Missing keys
- Values containing spaces
- `INFO`

---

## What I Learned

Building DartDB helped me understand practical systems concepts including:

- TCP client–server communication
- Socket programming (Winsock2)
- Multithreading with `std::thread`
- Mutex-based synchronization
- Shared-state concurrency
- In-memory data structures (`std::unordered_map`)
- Persistence and recovery via append-only logs
- File I/O
- Performance benchmarking
- Automated testing
- Building and debugging C++ systems software

---

## Future Improvements

Planned improvements include:

- More robust request/response protocol (e.g., length-prefixed or RESP-like)
- Better persistence and recovery (e.g., snapshots + log compaction)
- Additional data structures (lists, sets, hashes)
- More extensive automated tests (including concurrency and crash recovery)
- Improved benchmarking (varying payload sizes, mixed workloads)
- Graceful server shutdown (signal handling, clean client disconnects)
- Memory usage analysis and optimization
- CI/CD integration (GitHub Actions for build + tests)

---

## Author

**Sharvil Meshram**  
GitHub: [@sharvilmesh](https://github.com/sharvilmesh)