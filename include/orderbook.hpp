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
    void print_trade_tape();
    void print_stats();

private:
    using BookSide = std::map<double, std::deque<Order>, std::function<bool(double, double)>>;
    BookSide bids, asks;
    std::unordered_map<int, Order> order_map;
    std::vector<Trade> trade_tape;

    int order_count = 0;
    int match_count = 0;
    int total_quantity_matched = 0;

    std::chrono::steady_clock::time_point start_time;
    long long total_fill_latency_us = 0;
    long long max_fill_latency_us = 0;

    void match_order(const Order& order);
};

