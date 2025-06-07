# 🏛️ OrderBook Engine

A high-performance Level-2 order book and matching engine written in modern C++. Supports LIMIT, MARKET, and CANCEL orders, trade tape logging, and real-time performance benchmarking.

## 🚀 Features

- ✅ Full L2 price-time priority matching
- ✅ Support for LIMIT, MARKET, CANCEL orders
- ✅ Real-time socket-based client/server architecture
- ✅ Trade tape with timestamped fill latency
- ✅ Performance logging (orders/sec, match rate, latency)
- ✅ Clean architecture, easy to extend to L3 or multi-threaded

---

## ⚙️ Build Instructions

```bash
git clone https://github.com/your-username/orderbook-engine.git
cd orderbook-engine
mkdir build && cd build
cmake ..
make
```

### 🔌 Start the Matching Engine (Server)

From the `build/` directory:

```bash
./server
```

Expected output:
`Server listening on port 8080...`

### 🧑‍💻 Connect via Client and Submit Orders
From the `build/` directory:

```bash
./client
```

You can now submit orders in this format:
```
LIMIT 101.0 10 1 1      # price qty is_buy(1=buy, 0=sell) id
MARKET 0 5 1 2          # qty is_buy id (price is ignored for MARKET)
CANCEL 1               # cancel by order ID
```

### 📊 Run the Bulk Performance Test
To benchmark engine throughput and latency:
`./engine`

Example output:
```
✅ Bulk order test complete
Total orders:     100000
Elapsed time:     1348136 µs
Avg per order:    13.4814 µs

====== ENGINE STATS ======
Orders received:      100000
Total matches:        118001
Quantity matched:     664470
Max fill latency:     786176 µs
Avg fill latency:     2485 µs
Orders/sec (avg):     74183
```
