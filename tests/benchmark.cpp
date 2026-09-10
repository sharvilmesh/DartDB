#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

void runClient(int clientId, int requestsPerClient) {

    SOCKET clientSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Client " << clientId
                  << ": socket creation failed\n";
        return;
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

        std::cout << "Client " << clientId
                  << ": connection failed\n";

        closesocket(clientSocket);
        return;
    }

    char buffer[1024];

    for (int i = 0; i < requestsPerClient; i++) {

        std::string command =
            "SET client" +
            std::to_string(clientId) +
            "_key" +
            std::to_string(i) +
            " value";

        send(
            clientSocket,
            command.c_str(),
            static_cast<int>(command.length()),
            0
        );

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytesReceived <= 0) {
            break;
        }
    }

    closesocket(clientSocket);
}


int main() {

    WSADATA wsaData;

    if (WSAStartup(
        MAKEWORD(2, 2),
        &wsaData
    ) != 0) {

        std::cout << "WSAStartup failed\n";
        return 1;
    }

    const int clientCount = 10;
    const int requestsPerClient = 100;

    const int totalRequests =
        clientCount * requestsPerClient;

    std::vector<std::thread> clients;

    auto start =
        std::chrono::high_resolution_clock::now();

    for (int i = 0; i < clientCount; i++) {

        clients.emplace_back(
            runClient,
            i,
            requestsPerClient
        );
    }

    for (auto& client : clients) {
        client.join();
    }

    auto end =
        std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed =
        end - start;

    double requestsPerSecond =
        totalRequests / elapsed.count();

    std::cout << "\n";
    std::cout << "DartDB Concurrent Benchmark\n";
    std::cout << "---------------------------\n";
    std::cout << "Clients: " << clientCount << "\n";
    std::cout << "Requests/client: "
              << requestsPerClient << "\n";
    std::cout << "Total requests: "
              << totalRequests << "\n";
    std::cout << "Time: "
              << elapsed.count()
              << " seconds\n";
    std::cout << "Requests/sec: "
              << requestsPerSecond
              << "\n";

    WSACleanup();

    return 0;
}