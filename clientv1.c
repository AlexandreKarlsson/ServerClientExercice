#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"



void checkValidity(int validity, char *sentence){
    if(validity<0){
        printf("ERROR : %s \n",sentence);
        exit(1);
        }
}

struct message printCommand(struct message cmd,char *order){
    printf("Print command detected \n");
    printf("order : %s \n", order);
    // PAYLOAD /////////////////////////////////////////////////////////////////////////
    struct print_command_payload cmd_payload;
    /*
    int len;
    char string_to_print[MAX_PRINT_CHARS];
    */
    // copy the order in the cmd_payload with the max size as limit
    strncpy(cmd_payload.string_to_print, order, MAX_PRINT_CHARS);
    // get the lenght of the order in len
    cmd_payload.len = strlen(cmd_payload.string_to_print);
    printf("cmd_payload.len : %i \n", cmd_payload.len);
    if(cmd_payload.len==MAX_PRINT_CHARS){printf("WARING LIMIT MAX");}

    // Message /////////////////////////////////////////////////////////////////////////
    strcpy(cmd.command, COMMAND_PRINT);
    // copy the data of the payload inside the buffer
    memcpy(cmd.buf, &cmd_payload, sizeof(struct print_command_payload));

    return cmd;
}
struct message printCommand(struct message cmd,char *order){
    printf("Sort command detected \n");
    printf("order : %s \n", order);
    // PAYLOAD /////////////////////////////////////////////////////////////////////////
    struct sort_command_payload cmd_payload;
    /*
    int len;
    int numbers[MAX_NUMBERS];
    */
    // copy the order in the cmd_payload with the max size as limit
    strncpy(cmd_payload.string_to_print, order, MAX_PRINT_CHARS);
    // get the lenght of the order in len
    cmd_payload.len = strlen(cmd_payload.string_to_print);
    printf("cmd_payload.len : %i \n", cmd_payload.len);
    if(cmd_payload.len==MAX_PRINT_CHARS){printf("WARING LIMIT MAX");}

    // Message /////////////////////////////////////////////////////////////////////////
    strcpy(cmd.command, COMMAND_PRINT);
    // copy the data of the payload inside the buffer
    memcpy(cmd.buf, &cmd_payload, sizeof(struct print_command_payload));

    return cmd;
}

int main(int argc, char *argv[]) {
    int validity=404;
    char buffer[BUF_SIZE];
    struct message msg;
    /*
    char command[COMMAND_STR_LENGTH];
    char buf[BUF_SIZE];
    */
    printf("Start %s \n",argv[0]);
    
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

    // CONNECTION OK => COMMAND TREATMENT
    for (size_t i = 0; i < argc; i++)
    {
        printf("argv %i: %s \n",i,argv[i]);
    }
    // argv[1]=class argv[2]=command argv[3]=order
    if (argc < 3) {
        printf("Missing input arguments!\n");
        closesocket(socket_Client);
        WSACleanup();
        exit(1);
    }
    char *order = argv[2];
    
    if (strcmp(argv[1], COMMAND_PRINT) == 0) {
        if(argc>3){
            printf("Excess input arguments! \n");
            closesocket(socket_Client);
            WSACleanup();
            exit(1);
            }
        msg=printCommand(msg,order);
    }
    else if (strcmp(argv[1], COMMAND_SORT)==0){
        msg=sortCommand(msg,order);
    }
    else{
        printf("Unknown command!\n");
    }

    // SEND THE COMMAND
    send(socket_Client, (char*) &msg, sizeof(struct message), 0);
    printf("Sent %s command!\n", argv[1]);
    printf("Sent %s \n", (char*) &msg);


    // Fermer la connexion avec le serveur
    closesocket(socket_Client);
    // Terminer l'utilisation de Winsock
    WSACleanup();

    return 0;

}