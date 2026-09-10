#include <iostream>
#include <winsock2.h>
#include <string>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

int main() {

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(6379);

    if (connect(
        clientSocket,
        (sockaddr*)&serverAddress,
        sizeof(serverAddress)
    ) == SOCKET_ERROR) {

        std::cout << "Connection failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    const int totalRequests = 100;

    char buffer[1024];

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < totalRequests; i++) {

        std::string command =
            "SET key" + std::to_string(i) +
            " value" + std::to_string(i);

        send(
            clientSocket,
            command.c_str(),
            command.length(),
            0
        );

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytesReceived <= 0) {
            std::cout << "Server disconnected.\n";
            break;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    double requestsPerSecond =
        totalRequests / elapsed.count();

    std::cout << "\nDartDB Benchmark\n";
    std::cout << "----------------\n";
    std::cout << "Requests: " << totalRequests << "\n";
    std::cout << "Time: " << elapsed.count() << " seconds\n";
    std::cout << "Requests/sec: "
              << requestsPerSecond << "\n";

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}