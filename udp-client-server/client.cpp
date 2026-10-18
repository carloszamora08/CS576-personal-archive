#include <cstdio>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int closeConnection(int closingSocket);

int main() {
    constexpr std::size_t BUFFER_SIZE = 257;

    // Create server IP socket address
    sockaddr_in serverSocketAddress{};
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(9000);
    serverSocketAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t serverSocketAddressSize = sizeof(serverSocketAddress);

    // Create client socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        std::perror("socket creation");
        return 1;
    }

    // Prepare message
    char messageBuffer[BUFFER_SIZE] = "Hello world, this is a TCP toy server";

    // Send message to server
    ssize_t sentSize = sendto(clientSocket, messageBuffer, std::strlen(messageBuffer), 0, reinterpret_cast<const sockaddr*>(&serverSocketAddress), serverSocketAddressSize);
    if (sentSize == -1) {
        std::perror("message send");
        closeConnection(clientSocket);
        return 1;
    }

    // Receive message from server
    ssize_t receivedSize = recv(clientSocket, messageBuffer, sizeof(messageBuffer) - 1, 0);
    if (receivedSize == -1) {
        std::perror("message receive");
        closeConnection(clientSocket);
        return 1;
    }

    // Process received server response
    messageBuffer[receivedSize] = '\0';
    std::cout << "Message: " << messageBuffer << "\n";

    // Close connection
    return closeConnection(clientSocket);
}

int closeConnection(int closingSocket) {
    if (close(closingSocket) == -1) {
        std::perror("socket close");
        return 1;
    }
    
    return 0;
}