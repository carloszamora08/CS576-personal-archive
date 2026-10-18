#include <cstdio>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int closeConnection(int closingSocket);

int main() {
    // Create server IP socket address
    sockaddr_in serverSocketAddress{};
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(9000);
    serverSocketAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Create client socket
    int clientSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );
    if (clientSocket == -1) {
        std::perror("socket create");
        return 1;
    }

    // Connect to server
    int connectResult = connect(
        clientSocket,
        reinterpret_cast<const sockaddr*>(&serverSocketAddress),
        sizeof(serverSocketAddress)
    );
    if (connectResult == -1) {
        std::perror("server connection");
        closeConnection(clientSocket);
        return 1;
    }

    // Prepare message
    char messageBuffer[513] = "Hello world, this is a TCP toy server";

    // Send message to server
    ssize_t sentSize = send(
        clientSocket,
        messageBuffer,
        std::strlen(messageBuffer),
        0
    );
    if (sentSize == -1) {
        std::perror("message send");
        closeConnection(clientSocket);
        return 1;
    }

    // Receive message from server
    ssize_t receivedSize = recv(
        clientSocket,
        messageBuffer,
        sizeof(messageBuffer) - 1,
        0
    );
    if (receivedSize == -1) {
        std::perror("message receive");
        closeConnection(clientSocket);
        return 1;
    }
    else if (receivedSize == 0) {
        // Close connection
        return closeConnection(clientSocket);
    }

    // Process received server response
    messageBuffer[receivedSize] = '\0';
    std::cout << "Message: " << messageBuffer << "\n";

    // Close connection
    return closeConnection(clientSocket);
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