#include <iostream>
#include <winsock2.h>
#include <string>

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

    std::cout << "Connected to DartDB server!\n";

    std::string command;

std::cout << "DartDB> ";
std::getline(std::cin, command);

send(
    clientSocket,
    command.c_str(),
    command.length(),
    0
);

std::cout << "Sent: " << command << "\n";

    char buffer[1024];

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (bytesReceived > 0) {

        buffer[bytesReceived] = '\0';

        std::cout << "Server response: "
                  << buffer
                  << "\n";
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}