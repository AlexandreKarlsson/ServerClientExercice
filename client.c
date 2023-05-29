#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"

struct message printCommand(struct message cmd,char *order){
    //printf("Print command detected \n");
    //printf("order : %s \n", order);
    // PAYLOAD /////////////////////////////////////////////////////////////////////////
    struct print_command_payload cmd_payload;
    /*
    int len;
    char string_to_print[MAX_PRINT_CHARS];
    */
    // Copy the order in the cmd_payload with the max size as limit
    strncpy(cmd_payload.string_to_print, order, MAX_PRINT_CHARS);
    // Get the lenght of the order in len
    cmd_payload.len = strlen(cmd_payload.string_to_print);
    if(cmd_payload.len==MAX_PRINT_CHARS){printf("WARNING LIMIT MAX");}
    // Message /////////////////////////////////////////////////////////////////////////
    strcpy(cmd.command, COMMAND_PRINT);
    // copy the data of the payload inside the buffer
    memcpy(cmd.buf, &cmd_payload, sizeof(struct print_command_payload));
    return cmd;
}

struct message sortCommand(struct message cmd,int numbers[],int length){
    //printf("Sort command detected \n");
    // PAYLOAD /////////////////////////////////////////////////////////////////////////
    struct sort_command_payload cmd_payload;
    /*
    int len;
    int numbers[MAX_NUMBERS];
    */
    cmd_payload.len=length;
    for (int i = 0; i < length; i++) {
        cmd_payload.numbers[i] = numbers[i];
    }
    // Message /////////////////////////////////////////////////////////////////////////
    strcpy(cmd.command, COMMAND_SORT);
    // copy the data of the payload inside the buffer
    memcpy(cmd.buf, &cmd_payload, sizeof(struct sort_command_payload));
    return cmd;
}

void checkValidity(int validity, char *sentence){
    if(validity<0){
        printf("ERROR : %s \n",sentence);
        exit(1);
        }
}
void printNumbers(int numbers[], int length) {
    printf("Lenght: %i \n",length);
    printf("Numbers:");
    for (int i = 0; i < length; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
}
/*
int main() {

    int validity=404;
    char buffer[BUF_SIZE];

    printf("Start client \n");
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

*/