#include <iostream>
#include <winsock2.h>
#include <string>
#include <unordered_map>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

int main() {

    std::unordered_map<std::string, std::string> db;

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(6379);

    if (bind(
        serverSocket,
        (sockaddr*)&serverAddress,
        sizeof(serverAddress)
    ) == SOCKET_ERROR) {

        std::cout << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {

        std::cout << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "DartDB server is listening on port 6379...\n";

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Client connection failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client connected!\n";

    char buffer[1024];

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (bytesReceived > 0) {

        buffer[bytesReceived] = '\0';

        std::string request(buffer);

        std::cout << "Received: " << request << "\n";

        std::stringstream ss(request);

        std::string command;
        std::string key;
        std::string value;

        ss >> command >> key >> value;

        if (command == "SET") {

            db[key] = value;

            std::cout << "Stored: "
                      << key
                      << " = "
                      << value
                      << "\n";

            std::string response = "OK";

            send(
                clientSocket,
                response.c_str(),
                response.length(),
                0
            );
        }

        else {
            std::string response = "Unknown command";

            send(
                clientSocket,
                response.c_str(),
                response.length(),
                0
            );
        }
    }

    closesocket(clientSocket);
    closesocket(serverSocket);

    WSACleanup();

    return 0;
}