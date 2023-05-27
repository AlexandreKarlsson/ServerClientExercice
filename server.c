#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"
#include "server.h"

void checkValidity(int validity, char sentence[]){
    if(validity<0){printf("ERROR : %s \n",sentence);}
}

int main() {
    int validity = 404;
    // Client that will connect
    SOCKET socket_Client;
    struct sockaddr_in addr_Client;
    int clientAddrLen = sizeof(addr_Client);
    char buffer[BUF_SIZE];


    printf("Start server \n");
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //SOCKET socket_Server;
    int socket_Server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr_Server;
    addr_Server.sin_addr.s_addr = inet_addr("127.0.0.1");;
    addr_Server.sin_family = AF_INET;
    addr_Server.sin_port = htons(PORT);

    validity= bind(socket_Server, (struct sockaddr *)&addr_Server, sizeof(addr_Server));
    printf("Server binding\n");
    checkValidity(validity, "Binding");

    validity=listen(socket_Server, 3);
    printf("Server Listening\n");
    checkValidity(validity, "Listening");
    while (TRUE)
    {
        socket_Client = accept(socket_Server, (struct sockaddr*)&addr_Client, &clientAddrLen);
        validity = recv(socket_Client, buffer, sizeof(buffer), 0);
        printf("Commande : %s \n", buffer);
    }
    
    printf("Commande : %s \n", buffer);

    

    // Fermer socket serveur
    closesocket(socket_Server);

    // Terminer l'utilisation de Winsock
    WSACleanup();
    return 0;
}
