#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"
#include "server.h"

int main() {
    int validity = 404;
    boolean cmd_not_done = TRUE;
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
    addr_Server.sin_addr.s_addr = inet_addr(ADDR_SERVER);;
    addr_Server.sin_family = AF_INET;
    addr_Server.sin_port = htons(PORT);

    validity= bind(socket_Server, (struct sockaddr *)&addr_Server, sizeof(addr_Server));
    //printf("Server binding\n");
    checkValidity(validity, "Binding");

    validity=listen(socket_Server, 3);
    //printf("Server Listening\n");
    checkValidity(validity, "Listening");
    while (TRUE)
    {
        boolean ack_bool = TRUE;
        socket_Client = accept(socket_Server, (struct sockaddr*)&addr_Client, &clientAddrLen);
        validity = recv(socket_Client, (char*)&msg, sizeof(struct message), 0);
        checkValidity(validity, "Receive");
        
        if (strcmp(msg.command, COMMAND_PRINT) == 0) {
            if(cmd_not_done=TRUE)
            {
                printf("Command: %s \n", msg.command);
                struct print_command_payload* msg_payload = (struct print_command_payload*)msg.buf;
                printf("Output: %s \n", msg_payload->string_to_print);
                //printf("msg_payload.len : %i \n", msg_payload->len);
                cmd_not_done=FALSE;
            }
        }
        else if (strcmp(msg.command, COMMAND_SORT)==0){
            printf("Command: %s \n", msg.command);
            struct sort_command_payload* msg_payload = (struct sort_command_payload*)msg.buf;
            //printNumbers(msg_payload->numbers,msg_payload->len);
            bubbleSort(msg_payload->numbers,msg_payload->len);
        }
        else{
            //printf("Unknown command : %s!\n",msg.command);
            boolean ack_bool=FALSE;
        }
        // ACK TO CLIENT //////////////////////
        struct return_command_payload ack_payload;
        if(ack_bool==TRUE){ack_payload.return_code = RET_SUCCESS;}
        else{ack_payload.return_code = RET_ERROR;}
        struct message ack;
        strcpy(ack.command, COMMAND_RETURN);
        memcpy(ack.buf, &ack_payload, sizeof(struct return_command_payload));
        send(socket_Client, (char*)&ack, sizeof(struct message), 0);
    }
    
    printf("Commande : %s \n", buffer);

    

    // Close Client and server
    closesocket(socket_Server);

    // Close Winsock
    WSACleanup();
    return 0;
}
