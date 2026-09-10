#include <iostream>
#include <winsock2.h>
#include <string>
#include <unordered_map>
#include <sstream>
#include <thread>
#include <mutex>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

std::unordered_map<std::string, std::string> db;
std::mutex dbMutex;


// Append a database operation to the log file.
// This function is called while dbMutex is already locked.
void appendLog(const std::string& command) {

    std::ofstream file("dartdb.log", std::ios::app);

    if (file.is_open()) {
        file << command << "\n";
    }
}


// Load the database when the server starts.
void loadDatabase() {

    std::lock_guard<std::mutex> lock(dbMutex);

    // Load existing snapshot
    std::ifstream databaseFile("dartdb.db");

    std::string key;
    std::string value;

    while (databaseFile >> key) {

        std::getline(databaseFile, value);

        if (!value.empty() && value[0] == ' ') {
            value.erase(0, 1);
        }

        db[key] = value;
    }

    databaseFile.close();


    // Replay changes from the log
    std::ifstream logFile("dartdb.log");

    std::string command;

    while (std::getline(logFile, command)) {

        std::stringstream ss(command);

        std::string operation;
        std::string logKey;
        std::string logValue;

        ss >> operation;

        if (operation == "SET") {

            ss >> logKey;

            std::getline(ss, logValue);

            if (!logValue.empty() && logValue[0] == ' ') {
                logValue.erase(0, 1);
            }

            db[logKey] = logValue;
        }

        else if (operation == "DEL") {

            ss >> logKey;

            db.erase(logKey);
        }
    }

    logFile.close();
}


void handleClient(SOCKET clientSocket) {

    std::cout << "Client connected!\n";

    char buffer[1024];

    while (true) {

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytesReceived <= 0) {

            std::cout << "Client disconnected.\n";
            break;
        }

        buffer[bytesReceived] = '\0';

        std::string request(buffer);

        std::cout << "Received: " << request << "\n";

        std::stringstream ss(request);

        std::string command;
        std::string key;
        std::string value;

        ss >> command >> key;

        std::getline(ss, value);

        if (!value.empty() && value[0] == ' ') {
            value.erase(0, 1);
        }

        std::string response;


        // SET
        if (command == "SET") {

            if (key.empty() || value.empty()) {

                response = "ERROR: SET requires key and value";
            }

            else {

                std::lock_guard<std::mutex> lock(dbMutex);

                db[key] = value;

                appendLog(
                    "SET " + key + " " + value
                );

                response = "OK";
            }
        }


        // GET
        else if (command == "GET") {

            if (key.empty()) {

                response = "ERROR: GET requires a key";
            }

            else {

                std::lock_guard<std::mutex> lock(dbMutex);

                if (db.count(key)) {

                    response = db[key];
                }

                else {

                    response = "(nil)";
                }
            }
        }


        // DEL
        else if (command == "DEL") {

            if (key.empty()) {

                response = "ERROR: DEL requires a key";
            }

            else {

                std::lock_guard<std::mutex> lock(dbMutex);

                if (db.erase(key)) {

                    appendLog(
                        "DEL " + key
                    );

                    response = "OK";
                }

                else {

                    response = "(nil)";
                }
            }
        }


        // EXISTS
        else if (command == "EXISTS") {

            if (key.empty()) {

                response = "ERROR: EXISTS requires a key";
            }

            else {

                std::lock_guard<std::mutex> lock(dbMutex);

                if (db.count(key)) {

                    response = "YES";
                }

                else {

                    response = "NO";
                }
            }
        }


        // PING
        else if (command == "PING") {

            response = "PONG";
        }


        // INFO
        else if (command == "INFO") {

            response =
                "DartDB Server\n"
                "Port: 6379\n"
                "Status: Running";
        }


        // Unknown command
        else {

            response = "Unknown command";
        }


        send(
            clientSocket,
            response.c_str(),
            response.length(),
            0
        );
    }

    closesocket(clientSocket);
}


int main() {

    loadDatabase();

    std::cout << "DartDB database loaded.\n";


    WSADATA wsaData;

    if (WSAStartup(
        MAKEWORD(2, 2),
        &wsaData
    ) != 0) {

        std::cout << "WSAStartup failed\n";
        return 1;
    }


    SOCKET serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

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


    if (listen(
        serverSocket,
        10
    ) == SOCKET_ERROR) {

        std::cout << "Listen failed\n";

        closesocket(serverSocket);

        WSACleanup();

        return 1;
    }


    std::cout
        << "DartDB server is listening on port 6379...\n";


    while (true) {

        SOCKET clientSocket = accept(
            serverSocket,
            nullptr,
            nullptr
        );

        if (clientSocket == INVALID_SOCKET) {

            std::cout
                << "Client connection failed\n";

            continue;
        }


        std::thread clientThread(
            handleClient,
            clientSocket
        );

        clientThread.detach();
    }


    closesocket(serverSocket);

    WSACleanup();

    return 0;
}