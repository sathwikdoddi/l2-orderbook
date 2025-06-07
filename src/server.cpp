#include "orderbook.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>

#define PORT 8080

#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

Order parse_order(const std::string& line) {
    std::istringstream iss(line);
    std::string type;
    double price = 0.0;
    int qty = 0, is_buy = 0, id = 0;

    Order order;
    auto now = std::chrono::high_resolution_clock::now();
    order.timestamp = now;

    if (!(iss >> type)) {
        throw std::invalid_argument("Missing order type.");
    }

    if (type == "LIMIT" || type == "MARKET") {
        if (!(iss >> price >> qty >> is_buy >> id)) {
            throw std::invalid_argument("Expected format: LIMIT <price> <qty> <is_buy 0|1> <id>");
        }
        order = {id, price, qty, (bool)is_buy, (type == "LIMIT" ? OrderType::LIMIT : OrderType::MARKET), now};
    } else if (type == "CANCEL") {
        if (!(iss >> id)) {
            throw std::invalid_argument("Expected format: CANCEL <id>");
        }
        order = {id, 0, 0, false, OrderType::CANCEL, now};
    } else {
        throw std::invalid_argument("Unknown order type.");
    }

    return order;
}

int main() {
    OrderBook book;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address;
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << CYAN << "[SERVER] Listening on port " << PORT << "...\n" << RESET;

    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    std::cout << GREEN << "[SERVER] Client connected.\n" << RESET;

    char buffer[1024] = {0};

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(new_socket, buffer, 1024);
        if (valread <= 0) {
            std::cout << RED << "[SERVER] Client disconnected.\n" << RESET;
            break;
        }

        std::string msg(buffer);
        if (msg.empty() || msg == "\n") continue;

        try {
            Order order = parse_order(msg);
            std::cout << YELLOW << "\n[ORDER RECEIVED] " << msg << RESET << "\n";
            book.add_order(order);
            book.print_book();
            book.print_trade_tape();
        } catch (const std::exception& e) {
            std::cout << RED << "\n[ERROR] Invalid order format: " << e.what() << RESET << "\n";
            std::cout << CYAN << "Examples:\n"
                      << "  LIMIT 101.0 10 1 1     # price qty is_buy id\n"
                      << "  MARKET 0 15 1 2        # qty is_buy id (price = 0 ignored)\n"
                      << "  CANCEL 1               # cancel by id\n" << RESET;
        }
    }

    close(server_fd);
    return 0;
}