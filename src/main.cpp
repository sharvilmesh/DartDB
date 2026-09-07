#include <iostream>
#include <unordered_map>
#include <string>

int main() {
    std::unordered_map<std::string, std::string> db;

    db["name"] = "Sharvil";

    std::cout << "name = " << db["name"] << std::endl;

    return 0;
}