#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int closeConnection(int closingSocket);

int main() {
    // Create IP socket address
    sockaddr_in socketAddress{};
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(9000);
    socketAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Create server socket
    int serverSocket = socket(
        socketAddress.sin_family,
        SOCK_STREAM,
        0
    );
    if (serverSocket == -1) {
        std::perror("socket create");
        return 1;
    }

    // Bind socket
    int bindResult = bind(
        serverSocket,
        reinterpret_cast<const sockaddr*>(&socketAddress),
        sizeof(socketAddress)
    );
    if (bindResult == -1) {
        std::perror("socket bind");
        closeConnection(serverSocket);
        return 1;
    }

    // Set socket to listen
    int listenResult = listen(
        serverSocket,
        5
    );
    if (listenResult == -1) {
        std::perror("socket listen");
        closeConnection(serverSocket);
        return 1;
    }

    // Accept client connection
    int clientSocket = accept(
        serverSocket,
        NULL,
        NULL
    );
    if (clientSocket == -1) {
        std::perror("client connection accept");
        closeConnection(serverSocket);
        return 1;
    }

    // Set message buffer
    char messageBuffer[512];

    // Receive message from client
    ssize_t messageSize = recv(
        clientSocket,
        messageBuffer,
        sizeof(messageBuffer),
        0
    );
    if (messageSize == -1) {
        std::perror("message receive");
        closeConnection(clientSocket);
        closeConnection(serverSocket);
        return 1;
    }
    else if (messageSize == 0) {
        // Close server and client connection
        int clientCloseResult = closeConnection(clientSocket);
        int serverCloseResult = closeConnection(serverSocket);

        return clientCloseResult || serverCloseResult;
    }

    // Process message received
    for (int i = 0; i < messageSize; i++) {
        messageBuffer[i] += 1;
    }

    // Send message to client
    ssize_t sentSize = send(
        clientSocket,
        messageBuffer,
        messageSize,
        0
    );
    if (sentSize == -1) {
        std::perror("message send");
        closeConnection(clientSocket);
        closeConnection(serverSocket);
        return 1;
    }

    // Close server and client connection
    int clientCloseResult = closeConnection(clientSocket);
    int serverCloseResult = closeConnection(serverSocket);

    return clientCloseResult || serverCloseResult;
}

int closeConnection(int closingSocket) {
    int closeResult = close(
        closingSocket
    );
    if (closeResult == -1) {
        std::perror("socket close");
        return 1;
    }
    
    return 0;
}