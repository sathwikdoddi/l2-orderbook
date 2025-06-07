#include "orderbook.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>

#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

OrderBook::OrderBook()
    : bids([](double a, double b) { return a > b; }),
      asks([](double a, double b) { return a < b; }),
      start_time(std::chrono::steady_clock::now()) {}

void OrderBook::add_order(const Order& order) {
    order_count++;
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

            auto now = std::chrono::high_resolution_clock::now();
            auto buy_wait = std::chrono::duration_cast<std::chrono::microseconds>(
                now - (temp.is_buy ? temp.timestamp : resting.timestamp));
            auto sell_wait = std::chrono::duration_cast<std::chrono::microseconds>(
                now - (temp.is_buy ? resting.timestamp : temp.timestamp));
            long long latency = std::max(buy_wait.count(), sell_wait.count());
            total_fill_latency_us += latency;
            max_fill_latency_us = std::max(max_fill_latency_us, latency);

            match_count++;
            total_quantity_matched += trade_qty;

            trade_tape.push_back({
                .buy_order_id = temp.is_buy ? temp.id : resting.id,
                .sell_order_id = temp.is_buy ? resting.id : temp.id,
                .price = resting.price,
                .quantity = trade_qty,
                .timestamp = now,
                .buy_queue_time = buy_wait,
                .sell_queue_time = sell_wait
            });

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
    std::cout << "\n" << CYAN << "====== ORDER BOOK ======" << RESET << "\n";

    std::cout << GREEN << "---- BIDS ----" << RESET << "\n";
    if (bids.empty()) std::cout << "  [empty]\n";
    for (auto& [price, queue] : bids) {
        std::cout << "  $" << price << ": ";
        for (const auto& order : queue)
            std::cout << "[" << order.id << ", qty " << order.quantity << "] ";
        std::cout << "\n";
    }

    std::cout << RED << "---- ASKS ----" << RESET << "\n";
    if (asks.empty()) std::cout << "  [empty]\n";
    for (auto& [price, queue] : asks) {
        std::cout << "  $" << price << ": ";
        for (const auto& order : queue)
            std::cout << "[" << order.id << ", qty " << order.quantity << "] ";
        std::cout << "\n";
    }
}

void OrderBook::print_trade_tape() {
    if (trade_tape.empty()) return;

    std::cout << "\n" << YELLOW << "====== TRADE TAPE ======" << RESET << "\n";
    for (const auto& trade : trade_tape) {
        std::cout << "  🟢 BUY " << trade.buy_order_id
                  << " | 🔴 SELL " << trade.sell_order_id
                  << " | @ $" << trade.price
                  << " | Qty: " << trade.quantity
                  << " | ⏱️  Buy Wait: " << trade.buy_queue_time.count() << "µs"
                  << ", Sell Wait: " << trade.sell_queue_time.count() << "µs\n";
    }
}

void OrderBook::print_stats() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto elapsed_ms = duration_cast<milliseconds>(now - start_time).count();
    double seconds = elapsed_ms / 1000.0;

    std::cout << CYAN << "\n====== ENGINE STATS ======" << RESET << "\n";
    std::cout << "  Orders received:      " << order_count << "\n";
    std::cout << "  Total matches:        " << match_count << "\n";
    std::cout << "  Quantity matched:     " << total_quantity_matched << "\n";
    std::cout << "  Max fill latency:     " << max_fill_latency_us << " µs\n";
    if (match_count > 0)
        std::cout << "  Avg fill latency:     " << (total_fill_latency_us / match_count) << " µs\n";
    std::cout << "  Time elapsed:         " << seconds << " sec\n";
    std::cout << "  Orders/sec (avg):     " << static_cast<int>(order_count / seconds) << "\n";
}