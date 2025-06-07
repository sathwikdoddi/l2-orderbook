#pragma once
#include <deque>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>

enum class OrderType {
    LIMIT,
    MARKET,
    CANCEL
};

struct Order {
    int id;
    double price;
    int quantity;
    bool is_buy;
    OrderType type;
    std::chrono::high_resolution_clock::time_point timestamp;
};

struct Trade {
    int buy_order_id;
    int sell_order_id;
    double price;
    int quantity;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::chrono::microseconds buy_queue_time;
    std::chrono::microseconds sell_queue_time;
};

class OrderBook {
public:
    OrderBook();
    void add_order(const Order& order);
    void cancel_order(int id);
    void print_book();
    void print_trade_tape(); // 🆕

private:
    using BookSide = std::map<double, std::deque<Order>, std::function<bool(double, double)>>;
    BookSide bids, asks;
    std::unordered_map<int, Order> order_map;
    std::vector<Trade> trade_tape; // 🆕

    void match_order(const Order& order);
};

