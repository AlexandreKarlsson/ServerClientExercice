#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
// ################### : change with socket for linux of Mac
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"

int main() {
    int validity=404;
    boolean known_cmd=TRUE;
    // variable to see if the server is full
    boolean server_full=TRUE;

    char buffer[BUF_SIZE];
    struct message msg;

    // ################### : Suppress for linux or Mac
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    int socket_Client = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr_Server;
    addr_Server.sin_addr.s_addr = inet_addr(ADDR_CLIENT);
    addr_Server.sin_family = AF_INET;
    addr_Server.sin_port = htons(PORT);

    validity=connect(socket_Client, (struct sockaddr*)&addr_Server, sizeof(addr_Server));
    checkValidity(validity, "Connecting");

    struct message connection_msg;

    while (server_full)
    {
        recv(socket_Client, (char*)&connection_msg, sizeof(struct message), 0);
        if(strcmp(connection_msg.command, COMMAND_CONNECTION)==0)
            {
                struct connection_command_payload connection_payload;
                memcpy(&connection_payload, connection_msg.buf, sizeof(struct connection_command_payload));
                if(connection_payload.connection_code==SERV_FREE)
                {
                    printf("Server FREE \n");
                    server_full=FALSE;
                }
                else if(connection_payload.connection_code==SERV_FULL_Q)
                {
                    printf("In Queue \n");
                }
                else if(connection_payload.connection_code==SERV_FULL_EXIT)
                {
                    printf("Try later \n");
                    closesocket(socket_Client);
                    WSACleanup();
                    exit(1);
                }
                else if(connection_payload.connection_code==SERV_FULL_RETRY)
                {
                    printf("Try connection \n");
                    closesocket(socket_Client);
                    socket_Client = socket(AF_INET, SOCK_STREAM, 0);
                    validity=connect(socket_Client, (struct sockaddr*)&addr_Server, sizeof(addr_Server));
                }
            }
    }
    while (1) {
        printf("Enter a command (or 'close' to exit): ");
        scanf("%s", buffer);

        if (strcmp(buffer, COMMAND_CLOSE) == 0) {
            closesocket(socket_Client);
            WSACleanup();
            break;
        }
        if (strcmp(buffer, COMMAND_PRINT) == 0) {
            printf("Print Command detected \n");
            char order[MAX_PRINT_CHARS];
            printf("Enter the order: ");
            scanf(" %[^\n]", order);
            msg = printCommand(msg, order);
            known_cmd=TRUE;
        }
        else if (strcmp(buffer, COMMAND_SORT) == 0) 
        {
            printf("Sort Command detected\n");
            int numbers[MAX_NUMBERS];
            int count = 0;
            printf("Enter the numbers (example: 3 1 2): ");
            char input[BUF_SIZE];
            scanf(" %[^\n]", input);

            // Split the input into tokens
            char* token = strtok(input, " ");
            while (token != NULL && count < MAX_NUMBERS) {
                int valid = 1;
                for (int i = 0; token[i] != '\0'; i++) {
                    if (!isdigit(token[i]) && (token[0] != '-')) {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    sscanf(token, "%d", &numbers[count]);
                    count++;
                } else {
                    printf("Wrong entry: %s\n", token);
                    closesocket(socket_Client);
                    WSACleanup();
                    break;
                }
                token = strtok(NULL, " ");
            }

            msg = sortCommand(msg, numbers, count);
            known_cmd = TRUE;
        }
        
        else {
            printf("Unknown command!\n");
            known_cmd=FALSE;
        }
        // SEND THE COMMAND
        if(known_cmd)
        {
            send(socket_Client, (char*) &msg, sizeof(struct message), 0);
            // ACK ///////////////////////////////////////////////
            struct message return_msg;
            recv(socket_Client, (char*)&return_msg, sizeof(struct message), 0);

            if(strcmp(return_msg.command, COMMAND_RETURN)==0)
            {
                struct return_command_payload ack_payload;
                memcpy(&ack_payload, return_msg.buf, sizeof(struct return_command_payload));
                if(ack_payload.return_code==RET_SUCCESS)
                {
                    printf("Ack : success \n");
                }
                else if(ack_payload.return_code==RET_ERROR)
                {
                    printf("Ack : ERROR \n");
                }
                else{
                    printf("Communication error \n");
                }
            }
        
        }
        else
        {
            closesocket(socket_Client);
            WSACleanup();
        }
    } 
    closesocket(socket_Client);
    // ################### : Suppress for linux or Mac
    WSACleanup();

    return 0;
}