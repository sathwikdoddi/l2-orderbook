#pragma once
#include <deque>
#include <map>
#include <unordered_map>

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
};

class OrderBook {
public:
    void add_order(const Order& order);
    void cancel_order(int id);
    void print_book();

private:
    std::map<double, std::deque<Order>, std::greater<>> bids;
    std::map<double, std::deque<Order>> asks;
    std::unordered_map<int, Order> order_map;

    void match_order(const Order& order);
};

