#include "orderbook.hpp"
#include <iostream>
#include <algorithm>

OrderBook::OrderBook()
    : bids([](double a, double b) { return a > b; }),
      asks([](double a, double b) { return a < b; }) {}

void OrderBook::add_order(const Order& order) {
    if (order.type == OrderType::CANCEL) {
        cancel_order(order.id);
        return;
    }

    if (order.type == OrderType::MARKET) {
        match_order(order);
        return;
    }

    auto* book = order.is_buy ? &bids : &asks;
    (*book)[order.price].push_back(order);
    order_map[order.id] = order;
    match_order(order);
}

void OrderBook::cancel_order(int id) {
    if (order_map.count(id)) {
        order_map.erase(id);
        std::cout << "Canceled order: " << id << "\n";
    }
}

void OrderBook::match_order(const Order& incoming) {
    Order temp = incoming;
    auto* book = temp.is_buy ? &asks : &bids;

    for (auto it = book->begin(); it != book->end() && temp.quantity > 0;) {
        auto& queue = it->second;

        while (!queue.empty() && temp.quantity > 0) {
            Order& resting = queue.front();

            if (incoming.type == OrderType::LIMIT) {
                bool price_cross = (temp.is_buy && temp.price >= resting.price) ||
                                   (!temp.is_buy && temp.price <= resting.price);
                if (!price_cross) return;
            }

            int trade_qty = std::min(temp.quantity, resting.quantity);
            std::cout << "Match: " << temp.id << " with " << resting.id
                      << " qty " << trade_qty << " @ " << resting.price << "\n";

            temp.quantity -= trade_qty;
            resting.quantity -= trade_qty;

            if (resting.quantity == 0) {
                queue.pop_front();
                order_map.erase(resting.id);
            }
        }

        if (queue.empty()) {
            it = book->erase(it);
        } else {
            ++it;
        }
    }
}

void OrderBook::print_book() {
    std::cout << "\n---- BIDS ----\n";
    for (auto& [price, queue] : bids) {
        std::cout << price << ": ";
        for (const auto& order : queue)
            std::cout << "(" << order.id << ", " << order.quantity << ") ";
        std::cout << "\n";
    }

    std::cout << "---- ASKS ----\n";
    for (auto& [price, queue] : asks) {
        std::cout << price << ": ";
        for (const auto& order : queue)
            std::cout << "(" << order.id << ", " << order.quantity << ") ";
        std::cout << "\n";
    }
}