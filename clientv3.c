#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
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
void printNumbers(int numbers[], int length) {
    printf("Lenght: %i \n",length);
    printf("Numbers:");
    for (int i = 0; i < length; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
}

struct message printCommand(struct message cmd,char *order){
    //printf("Print command detected \n");
    //printf("order : %s \n", order);
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
    //printf("cmd_payload.len : %i \n", cmd_payload.len);
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

int main() {
    int validity=404;
    char buffer[BUF_SIZE];
    boolean known_cmd=TRUE;
    struct message msg;
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //SOCKET socket_Client;
    int socket_Client = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr_Server;
    addr_Server.sin_addr.s_addr = inet_addr(ADDR_CLIENT);
    addr_Server.sin_family = AF_INET;
    addr_Server.sin_port = htons(PORT);

    validity=connect(socket_Client, (struct sockaddr*)&addr_Server, sizeof(addr_Server));
    checkValidity(validity, "Connecting");
    char req[30];
    char name[MAX_NAME_SIZE];
    recv(socket_Client, req, 30, 0);
    printf("%s",req);
    scanf("%s",name);
    send(socket_Client, name, sizeof(name), 0);

    while (1) {
        // Lecture de la commande depuis l'entrée utilisateur
        printf("Enter a command (or 'close' to exit): ");
        scanf("%s", buffer);

        if (strcmp(buffer, COMMAND_CLOSE) == 0) {
            // Close the client
            closesocket(socket_Client);
            WSACleanup();
            break;
        }
         // Traitement des autres commandes
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
            printf("number received: %s\n", input);

            // split the input into tokens
            char* token = strtok(input, " ");
            while (token != NULL && count < MAX_NUMBERS) {
                int valid = 1;
                for (int i = 0; token[i] != '\0'; i++) {
                    if (!isdigit(token[i]) && (token[i] != '-')) {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    sscanf(token, "%d", &numbers[count]);  // Convertir chaque nombre en entier
                    count++;
                } else {
                    printf("Wrong entry: %s\n", token);
                    closesocket(socket_Client);
                    WSACleanup();
                    exit(1);
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

    // Fermer la connexion avec le serveur
    closesocket(socket_Client);
    // Terminer l'utilisation de Winsock
    WSACleanup();

    return 0;
}