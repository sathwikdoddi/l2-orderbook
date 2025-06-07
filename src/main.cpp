#include "orderbook.hpp"
#include <chrono>
#include <random>
#include <iostream>

int main() {
    OrderBook book;
    const int NUM_ORDERS = 100000;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> price_dist(95.0, 105.0);
    std::uniform_int_distribution<> qty_dist(1, 20);
    std::bernoulli_distribution side_dist(0.5);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 1; i <= NUM_ORDERS; ++i) {
        double price = price_dist(gen);
        int quantity = qty_dist(gen);
        bool is_buy = side_dist(gen);

        Order order = {
            .id = i,
            .price = price,
            .quantity = quantity,
            .is_buy = is_buy,
            .type = OrderType::LIMIT,
            .timestamp = std::chrono::high_resolution_clock::now()
        };

        book.add_order(order);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    double avg_us_per_order = total_us / static_cast<double>(NUM_ORDERS);

    std::cout << "\n✅ Bulk order test complete\n";
    std::cout << "Total orders:     " << NUM_ORDERS << "\n";
    std::cout << "Elapsed time:     " << total_us << " µs\n";
    std::cout << "Avg per order:    " << avg_us_per_order << " µs\n";

    book.print_stats();
    return 0;
}