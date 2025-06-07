#include "orderbook.hpp"
#include <chrono>

int main() {
    OrderBook book;

    auto now = std::chrono::high_resolution_clock::now();

    book.add_order({1, 101.0, 10, true, OrderType::LIMIT, now});
    book.add_order({2, 100.0, 5, false, OrderType::LIMIT, std::chrono::high_resolution_clock::now()});
    book.add_order({3, 99.0, 20, false, OrderType::LIMIT, std::chrono::high_resolution_clock::now()});
    book.add_order({4, 0.0, 15, true, OrderType::MARKET, std::chrono::high_resolution_clock::now()});

    book.print_book();
    book.print_trade_tape();

    return 0;
}