#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

bool sendCommand(
    SOCKET socket,
    const std::string& command,
    const std::string& expected
) {
    send(
        socket,
        command.c_str(),
        static_cast<int>(command.length()),
        0
    );

    char buffer[1024];

    int bytesReceived = recv(
        socket,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (bytesReceived <= 0) {
        std::cout << "[FAIL] " << command
                  << " -> No response\n";
        return false;
    }

    buffer[bytesReceived] = '\0';

    std::string response(buffer);

    if (response == expected) {
        std::cout << "[PASS] "
                  << command
                  << " -> "
                  << response
                  << "\n";
        return true;
    }

    std::cout << "[FAIL] "
              << command
              << " -> Expected: "
              << expected
              << ", Got: "
              << response
              << "\n";

    return false;
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

    SOCKET clientSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (clientSocket == INVALID_SOCKET) {

        std::cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr =
        inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(6379);

    if (connect(
        clientSocket,
        (sockaddr*)&serverAddress,
        sizeof(serverAddress)
    ) == SOCKET_ERROR) {

        std::cout << "Could not connect to DartDB server.\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    int passed = 0;
    int total = 0;

    auto test = [&](const std::string& command,
                    const std::string& expected) {

        total++;

        if (sendCommand(
            clientSocket,
            command,
            expected
        )) {
            passed++;
        }
    };

    std::cout << "\nDartDB Test Suite\n";
    std::cout << "-----------------\n";

    test("PING", "PONG");

    test(
        "SET testKey Hello World",
        "OK"
    );

    test(
        "GET testKey",
        "Hello World"
    );

    test(
        "EXISTS testKey",
        "YES"
    );

    test(
        "DEL testKey",
        "OK"
    );

    test(
        "GET testKey",
        "(nil)"
    );

    test(
        "EXISTS testKey",
        "NO"
    );

    test(
        "INFO",
        "DartDB Server\nPort: 6379\nStatus: Running"
    );

    std::cout << "\n-----------------\n";
    std::cout << "Tests passed: "
              << passed
              << "/"
              << total
              << "\n";

    if (passed == total) {
        std::cout << "All tests passed!\n";
    }
    else {
        std::cout << "Some tests failed.\n";
    }

    closesocket(clientSocket);
    WSACleanup();

    return (passed == total) ? 0 : 1;
}