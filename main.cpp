#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void handleRequest(int clientSocket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // Read data from the socket
    ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
    if (bytesRead < 0) {
        perror("Error reading from socket");
        return;
    }

    // Analyze the HTTP request
    std::istringstream request(buffer);
    std::string requestLine;
    getline(request, requestLine);

    // Output the contents of the HTTP request to the console
    std::cout << "Received HTTP Request:\n" << requestLine << std::endl;

    // Send a response to the client
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nHello, World!";
    send(clientSocket, response.c_str(), response.length(), 0);

    // Close the socket
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    socklen_t clientLength;
    struct sockaddr_in serverAddress, clientAddress;

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error opening socket");
        return 1;
    }

    // Initialize the server address structure
    memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error on binding");
        return 1;
    }

    // Listen for incoming connections
    listen(serverSocket, 5);
    std::cout << "Server listening on port " << PORT << "...\n";

    clientLength = sizeof(clientAddress);

    while (true) {
        // Accept incoming connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLength);
        if (clientSocket < 0) {
            perror("Error on accept");
            return 1;
        }

        // Process the request
        handleRequest(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}