#pragma once
#include <deque>
#include <map>
#include <unordered_map>
#include <functional>

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
    OrderBook();

    void add_order(const Order& order);
    void cancel_order(int id);
    void print_book();

private:
    using BookSide = std::map<double, std::deque<Order>, std::function<bool(double, double)>>;
    BookSide bids;
    BookSide asks;
    std::unordered_map<int, Order> order_map;

    void match_order(const Order& order);
};

