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
     struct message msg;


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
        validity = recv(socket_Client, (char*)&msg, sizeof(struct message), 0);
        checkValidity(validity, "Receive");
        printf("msg.command : %s \n", msg.command);
        if (strcmp(msg.command, COMMAND_PRINT) == 0) {
            printf("Print command!\n");
            struct print_command_payload* msg_payload = (struct print_command_payload*)msg.buf;
            printf("msg_payload.string_to_print : %s \n", msg_payload->string_to_print);
            printf("msg_payload.len : %i \n", msg_payload->len);
        }
        else if (strcmp(msg.command, COMMAND_SORT)==0){
            printf("Sort command!\n");
        }
        else{
            printf("Unknown command!\n");
        }
    }
    
    printf("Commande : %s \n", buffer);

    

    // Fermer socket serveur
    closesocket(socket_Server);

    // Terminer l'utilisation de Winsock
    WSACleanup();
    return 0;
}
