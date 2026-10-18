#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int closeConnection(int closingSocket);

int main() {
    constexpr std::size_t BUFFER_SIZE = 256;

    // Create IP socket address
    sockaddr_in socketAddress{};
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(9000);
    socketAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Create server socket
    int serverSocket = socket(socketAddress.sin_family, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        std::perror("socket creation");
        return 1;
    }

    // Bind socket
    int bindResult = bind(serverSocket, reinterpret_cast<const sockaddr*>(&socketAddress), sizeof(socketAddress));
    if (bindResult == -1) {
        std::perror("socket bind");
        closeConnection(serverSocket);
        return 1;
    }

    // Set message buffer
    char messageBuffer[BUFFER_SIZE];

    // Create IP socket address for client connection
    sockaddr_in clientSocketAddress{};
    socklen_t clientSocketAddressSize = sizeof(clientSocketAddress);

    // Receive message from client
    std::size_t messageSize = recvfrom(serverSocket, messageBuffer, BUFFER_SIZE, 0, reinterpret_cast<sockaddr*>(&clientSocketAddress), &clientSocketAddressSize);
    if (messageSize == -1) {
        std::perror("message receive");
        closeConnection(serverSocket);
        return 1;
    }

    // Process message received
    constexpr char humorousMessage[] = " - Are you a TCP packet? Because I want to keep checking if you received my message.";
    constexpr std::size_t humorousMessageSize = sizeof(humorousMessage);

    std::size_t messageConcatenationIndex = messageSize + humorousMessageSize <= BUFFER_SIZE
        ? messageSize
        : BUFFER_SIZE - humorousMessageSize;

    for (std::size_t i = 0; i < humorousMessageSize; i++) {
        messageBuffer[i + messageConcatenationIndex] = humorousMessage[i];
    }

    // Send message to client
    std::size_t sentSize = sendto(serverSocket, messageBuffer, messageConcatenationIndex + humorousMessageSize, 0, reinterpret_cast<sockaddr*>(&clientSocketAddress), clientSocketAddressSize);
    if (sentSize == -1) {
        std::perror("message send");
        closeConnection(serverSocket);
        return 1;
    }

    // Close server connection
    return closeConnection(serverSocket);
}

int closeConnection(int closingSocket) {
    if (close(closingSocket) == -1) {
        std::perror("socket close");
        return 1;
    }
    
    return 0;
}