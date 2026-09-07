#include <iostream>
#include <unordered_map>
#include <string>

int main() {
    std::unordered_map<std::string, std::string> db;

    std::string command, key, value;

    std::cout << "DartDB> ";

    while (std::cin >> command) {

        if (command == "SET") {
            std::cin >> key >> value;
            db[key] = value;
            std::cout << "OK\n";
        }

        else if (command == "GET") {
            std::cin >> key;

            if (db.count(key))
                std::cout << db[key] << "\n";
            else
                std::cout << "(nil)\n";
        }

        else if (command == "DEL") {
            std::cin >> key;
            db.erase(key);
            std::cout << "OK\n";
        }

        else if (command == "EXISTS") {
            std::cin >> key;

            if (db.count(key))
                std::cout << "YES\n";
            else
                std::cout << "NO\n";
        }

        else {
            std::cout << "Unknown command\n";
        }

        std::cout << "DartDB> ";
    }

    return 0;
}