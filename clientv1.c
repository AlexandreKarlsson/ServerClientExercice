#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
// ################### : change with socket for linux of Mac
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"

int main(int argc, char *argv[]) {
    // Variable to see if connection goes well
    int validity=404;
    // Variable to see if the command is known
    boolean known_cmd=TRUE;
    char buffer[BUF_SIZE];
    struct message msg;
    /*
    char command[COMMAND_STR_LENGTH];
    char buf[BUF_SIZE];
    */
    //printf("Start %s \n",argv[0]);
    
    // Variable to stock information for implementation of Winsock     ################### : Suppress for linux or Mac
    WSADATA wsaData;
    // Initialise Winsock with version 2.2 and put it in wsaData       ################### : Suppress for linux or Mac
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Initialise socket_Client
    // Create a socket on the client side : AF_INET= adress familly of the IP IPV4, SOCK_STREAM is for TCP and 0 is for the protocol=> chose automaticaly
    int socket_Client = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr_Server;
    // Set the IP adress of ther server in addr_Server and inet_addr convert a string into an IP adress
    addr_Server.sin_addr.s_addr = inet_addr(ADDR_CLIENT);
    // IPV4
    addr_Server.sin_family = AF_INET;
    // Set the port number in the structure, htons convert the host byte number order into a network order
    addr_Server.sin_port = htons(PORT);

    // Establish connection with the server : (struct sockaddr*)&addr_Server) pointer to addr_Server => struct sockaddr* to match function signature.
    // sizeof(addr_Server) is the size of the address structure
    validity=connect(socket_Client, (struct sockaddr*)&addr_Server, sizeof(addr_Server));
    checkValidity(validity, "Connecting");

    ////////////////////////////////////////////////////////////////////////
    // CONNECTION OK => COMMAND TREATMENT
    /*
    for (size_t i = 0; i < argc; i++)
    {
        printf("argv %i: %s \n",i,argv[i]);
    }
    */
    // argv[1]=class argv[2]=command argv[3]=order
    ////////////////////////////////////////////////////////////////////////
    if (argc < 3) 
    {
        printf("Missing input arguments!\n");
        closesocket(socket_Client);
        WSACleanup();
        exit(1);
    }
    char *order = argv[2];
    if (strcmp(argv[1], COMMAND_PRINT) == 0) 
    {
        if(argc>3)
        {
            printf("Excess input arguments! \n");
            closesocket(socket_Client);
            WSACleanup();
            exit(1);
        }
        // Create the message as a PRINT message
        msg=printCommand(msg,order);
    }
    else if (strcmp(argv[1], COMMAND_SORT)==0)
    {
        int numbers[MAX_NUMBERS];
        int count=0;
        // Go trough all arguments
        for (int i = 2; i < argc; i++) {
            char *number_str = argv[i];
            // Verify if char is an int
            for (int j = 0; number_str[j] != '\0'; j++) 
            {
                if (!isdigit(number_str[j]) && number_str[0] != '-') 
                {
                    printf("Wrong entry! \n");
                    closesocket(socket_Client);
                    WSACleanup();
                    exit(1);  
                }
            }
            // convert arg in int and put it in numbers
            numbers[count++] = atoi(number_str);
        }
        // Create the message as a PRINT message
        msg=sortCommand(msg,numbers,count);
    }
    else
    {
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
    // Close the connection with the server
    closesocket(socket_Client);
    // Close Winsock                         ################### : Suppress for linux or Mac
    WSACleanup();
    return 0;
}