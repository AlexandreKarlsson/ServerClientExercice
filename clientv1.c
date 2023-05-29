#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"

int main(int argc, char *argv[]) {
    int validity=404;
    char buffer[BUF_SIZE];
    boolean known_cmd=TRUE;
    struct message msg;
    /*
    char command[COMMAND_STR_LENGTH];
    char buf[BUF_SIZE];
    */
    //printf("Start %s \n",argv[0]);
    
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //SOCKET socket_Client;
    int socket_Client = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr_Server;
    addr_Server.sin_addr.s_addr = inet_addr(ADDR_CLIENT);
    addr_Server.sin_family = AF_INET;
    addr_Server.sin_port = htons(PORT);

    validity=connect(socket_Client, (struct sockaddr*)&addr_Server, sizeof(addr_Server));
    //printf("Client connecting \n");
    checkValidity(validity, "Connecting");

    // CONNECTION OK => COMMAND TREATMENT
    /*
    for (size_t i = 0; i < argc; i++)
    {
        printf("argv %i: %s \n",i,argv[i]);
    }
    */
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
        int numbers[MAX_NUMBERS];
        int count=0;
        // go trough all arguments
        for (int i = 2; i < argc; i++) {
            char *number_str = argv[i];
            // Verify if char is an int
            for (int j = 0; number_str[j] != '\0'; j++) {
                if (!isdigit(number_str[j]) && number_str[j] != '-') {
                    printf("Wrong entry! \n");
                    closesocket(socket_Client);
                    WSACleanup();
                    exit(1);  
                }
            }
            // convert arg in int and put it in numbers
            numbers[count++] = atoi(number_str);
        }

        //printNumbers(numbers,count);
        msg=sortCommand(msg,numbers,count);
    }
    else{
        printf("Unknown command!\n");
        known_cmd = FALSE; 
        closesocket(socket_Client);
        WSACleanup();
        exit(1);    
    }

    // SEND THE COMMAND
    if(known_cmd)
    {
        send(socket_Client, (char*) &msg, sizeof(struct message), 0);
        printf("Sent %s command!\n", argv[1]);
        
        // ACK ///////////////////////////////////////////////
        struct message return_msg;
        recv(socket_Client, (char*)&return_msg, sizeof(struct message), 0);

        if(strcmp(return_msg.command, COMMAND_RETURN)==0)
        {
            //printf("ACK received");
            //struct return_command_payload* ack_payload = (struct return_command_payload*)return_msg.buf;
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

    

    // Fermer la connexion avec le serveur
    closesocket(socket_Client);
    // Terminer l'utilisation de Winsock
    WSACleanup();

    return 0;
}