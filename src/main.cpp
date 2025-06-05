#include "orderbook.hpp"

int main() {
    OrderBook book;

    book.add_order({1, 101.0, 10, true, OrderType::LIMIT});
    book.add_order({2, 100.0, 5, false, OrderType::LIMIT});
    book.add_order({3, 99.0, 20, false, OrderType::LIMIT});
    book.add_order({4, 101.0, 15, true, OrderType::MARKET});

    book.print_book();
}

