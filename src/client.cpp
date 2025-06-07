#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>

#define PORT 8080
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

bool validate_input(const std::string& input) {
    std::istringstream iss(input);
    std::string type;
    double price;
    int qty, is_buy, id;

    if (!(iss >> type)) return false;

    if (type == "LIMIT" || type == "MARKET") {
        if (!(iss >> price >> qty >> is_buy >> id)) return false;
        if ((is_buy != 0 && is_buy != 1) || qty <= 0 || id < 0) return false;
    } else if (type == "CANCEL") {
        if (!(iss >> id)) return false;
        if (id < 0) return false;
    } else {
        return false;
    }

    return true;
}

void print_help() {
    std::cout << CYAN << "\n[ORDER FORMAT GUIDE]" << RESET << "\n";
    std::cout << "  " << YELLOW << "LIMIT <price> <qty> <is_buy 0|1> <id>" << RESET << "\n";
    std::cout << "    → Place a limit order (0 = sell, 1 = buy)\n";
    std::cout << "  " << YELLOW << "MARKET 0 <qty> <is_buy 0|1> <id>" << RESET << "\n";
    std::cout << "    → Send a market order\n";
    std::cout << "  " << YELLOW << "CANCEL <id>" << RESET << "\n";
    std::cout << "    → Cancel order by ID\n";
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << RED << "Connection failed.\n" << RESET;
        return 1;
    }

    std::cout << GREEN << "Connected to server.\n" << RESET;
    print_help();

    std::string line;
    while (true) {
        std::cout << "\n> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        if (!validate_input(line)) {
            std::cout << RED << "[ERROR] Invalid format or values.\n" << RESET;
            print_help();
            continue;
        }

        line += '\n';
        send(sock, line.c_str(), line.size(), 0);
    }

    close(sock);
    return 0;
}