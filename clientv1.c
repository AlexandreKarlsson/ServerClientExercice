#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"



void checkValidity(int validity, char *sentence){
    if(validity<0){printf("ERROR : %s \n",sentence);}
}

int main(int argc, char *argv[]) {

    int validity=404;

    char buffer[BUF_SIZE];

    printf("Start %s \n",argv[0]);
    for (size_t i = 0; i < argc; i++)
    {
        printf("argv %i: %s \n",i,argv[i]);
    }
    
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //SOCKET socket_Client;
    int socket_Client = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr_Server;
    addr_Server.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_Server.sin_family = AF_INET;
    addr_Server.sin_port = htons(PORT);

    validity=connect(socket_Client, (struct sockaddr*)&addr_Server, sizeof(addr_Server));
    printf("Client connecting \n");
    checkValidity(validity, "Connecting");

    const char* command = "print Hello World";

    validity=send(socket_Client, command, strlen(command), 0);
    printf("Client sending \n");
    checkValidity(validity, "Sending");

    // Fermer la connexion avec le serveur
    closesocket(socket_Client);

    // Terminer l'utilisation de Winsock
    WSACleanup();

    return 0;

}