#include <stdio.h>      // For printf() and fprintf
#include <sys/socket.h> // For socket(), bind(), and connect()
#include <arpa/inet.h>  // For sockaddr_in and inet_ntoa()
#include <stdlib.h>     // For atoi() and exit()
#include <string.h>     // For memset()
#include <unistd.h>     // For close()
#include <pthread.h>    // For POSIX threads

// For logging
void log_print(char *fmt,...);
#define LOG_PRINT(...) log_print(__VA_ARGS__ )

#define MAXPENDING 5    // Maximum outstanding connection requests
#define RCVBUFSIZE 32   // Size of receive buffer

void DieWithError (char *errorMessage);  // Error handling function
void HandleTCPClient (int clntSocket);   // TCP client handling function

int CreateTCPServerSocket (unsigned short port); // Create socket function
int AcceptTCPConnection (int servSock);          // Client connect function

void *ThreadMain (void *args); // Main program of a Thread

// Structure of arguments to pass to client thread
struct ThreadArgs {
    int clntSock;   // Client socket descriptor
};

int main (int argc, char *argv[]) {
    int servSock;                   // Socket descriptor for server
    int clntSock;                   // Socket descriptor for client
    unsigned short echoServPort;    // Server port
    pthread_t threadID;             // Thread ID
    struct ThreadArgs *threadArgs;  // Pointer to thread Args
    char jumlahThread;              // jumlah thread

    if (argc != 2) {     // Test for correct number of arguments
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);                   // First arg:  local port
    servSock = CreateTCPServerSocket(echoServPort); // Create socket for incoming connections
    jumlahThread = 0;

    for (;;) {
        clntSock = AcceptTCPConnection(servSock);

        if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == 0) {
            DieWithError("malloc() failed");
        }
        threadArgs -> clntSock = clntSock;
        jumlahThread += 1;

        // Create client thread
        if (pthread_create(&threadID, NULL, ThreadMain, (void*) threadArgs) != 0) {
            DieWithError("phtread_create() failed");
        }
    }
}

int AcceptTCPConnection(int servSock) {
    int clntSock;                       // Client socket descriptor
    struct sockaddr_in echoClntAddr;    // CLient address
    unsigned int clntLen;               // Data client address length

    clntLen = sizeof(echoClntAddr);

    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0) {
        DieWithError("accept() failed");
    }

    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    return clntSock;
}

int CreateTCPServerSocket(unsigned short port) {
    int sock;
    struct sockaddr_in echoServAddr;
    
    // Create socket for incoming connections
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        DieWithError("socket() failed");
    }

    // Construct local address structure
    memset(&echoServAddr, 0, sizeof(echoServAddr));     // Zero out structure
    echoServAddr.sin_family = AF_INET;                  // Internet address family
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);   // Any incoming interface
    echoServAddr.sin_port = htons(port);                // Local port

    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
        DieWithError("bind() failed");
    }

    if (listen(sock, MAXPENDING) < 0) {
        DieWithError("listen() failed");
    }

    return sock;
}

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

void HandleTCPClient(int clntSocket) {
    char echoBuffer[RCVBUFSIZE];
    char simpanpesan[RCVBUFSIZE];
    
    int recvMsgSize;  /* Size of received message */

    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 1) {
        DieWithError("recv() failed");
    }

    while (recvMsgSize > 0) {      /* zero indicates end of transmission */
        /*echo message back to client */
        /* Print receiving data to Monitor */
        echoBuffer[recvMsgSize] = '\0';
        sprintf(simpanpesan, "c: %d, s: %s", clntSocket, echoBuffer);
        log_print(simpanpesan);
        printf("Data yang diterima: %s\n", echoBuffer);
        printf("Data telah tersimpan\n"); 

        int a=4;
        while (a<20){
            if (a==clntSocket){
                send(a, echoBuffer, recvMsgSize, 0);
            }
            a=a+1;
        }
        /* See if there is more data to receive */
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
            DieWithError("recv() failed");
        }
    }

    close(clntSocket);    /* Close client socket */
    printf("Client %d has been closed\n\n", clntSocket);
}

void *ThreadMain(void *threadArgs)
{
    int clntSock;                   /* Socket descriptor for client connection */

    /* Guarantees that thread resources are deallocated upon return */
    pthread_detach(pthread_self()); 

    /* Extract socket file descriptor from argument */
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    free(threadArgs);              /* Deallocate memory for argument */
    printf("clntSockID %d\n", clntSock);
    HandleTCPClient(clntSock);
    return (NULL);
}
