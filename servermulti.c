#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"
#include "server.h"
#include <pthread.h>

pthread_mutex_t clients_mutex;
int num_clients = 0;
boolean queueIsFull = FALSE;
SOCKET socket_Client_Q;


void* threadClient(void* socket) {
    SOCKET client_socket = *(SOCKET*)socket;
    boolean ack_bool = TRUE;
    boolean connection = TRUE;
    struct message msg;
    while(connection){
        int validity = 404;
        validity = recv(client_socket, (char*)&msg, sizeof(struct message), 0);
        checkValidity(validity, "Receive");
        if(validity<0){
            printf("Client deconnection \n");
            break;}
        if (strcmp(msg.command, COMMAND_PRINT) == 0) {
            printf("Command: %s \n", msg.command);
            struct print_command_payload* msg_payload = (struct print_command_payload*)msg.buf;
            printf("Output: %s \n", msg_payload->string_to_print);
        }
        else if (strcmp(msg.command, COMMAND_SORT) == 0) {
            printf("Command: %s \n", msg.command);
            struct sort_command_payload* msg_payload = (struct sort_command_payload*)msg.buf;
            //printNumbers(msg_payload->numbers, msg_payload->len);
            bubbleSort(msg_payload->numbers, msg_payload->len);
        }
        else if (strcmp(msg.command, COMMAND_CLOSE) == 0) {
            connection=FALSE;
            break;
        }
        else {
            ack_bool = FALSE;
        }

        struct return_command_payload ack_payload;
        if (ack_bool == TRUE) {
            ack_payload.return_code = RET_SUCCESS;
        }
        else {
            ack_payload.return_code = RET_ERROR;
        }
        
        struct message ack;
        strcpy(ack.command, COMMAND_RETURN);
        memcpy(ack.buf, &ack_payload, sizeof(struct return_command_payload));
        send(client_socket, (char*)&ack, sizeof(struct message), 0);
    }
   
    pthread_mutex_lock(&clients_mutex);
    num_clients--;
    printf("Close client, number of client : %i \n",num_clients);
    //////////////////////////////////////////////////////////////////////////
    struct message connection_msg;
    strcpy(connection_msg.command, COMMAND_CONNECTION);
    struct connection_command_payload connection_payload;
    connection_payload.connection_code = SERV_FULL_RETRY;
    memcpy(connection_msg.buf, &connection_payload, sizeof(struct connection_command_payload));
    send(socket_Client_Q, (char*)&connection_msg, sizeof(struct message), 0);
    queueIsFull=FALSE;
    //////////////////////////////////////////////////////////////////////////
    pthread_mutex_unlock(&clients_mutex);
    closesocket(client_socket);
    return NULL;
}

int main() {
    pthread_mutex_init(&clients_mutex, NULL);
    pthread_t threads[MAX_CLIENTS];
    int validity = 404;
    char buffer[BUF_SIZE];
    struct message msg;

    printf("Start server\n");

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    int socket_Server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr_Server;
    addr_Server.sin_addr.s_addr = inet_addr(ADDR_SERVER);
    addr_Server.sin_family = AF_INET;
    addr_Server.sin_port = htons(PORT);

    validity = bind(socket_Server, (struct sockaddr*)&addr_Server, sizeof(addr_Server));
    printf("Server binding\n");
    checkValidity(validity, "Binding");

    validity = listen(socket_Server, 3);
    printf("Server Listening\n");
    checkValidity(validity, "Listening");
    
    SOCKET socket_Client;
    struct sockaddr_in addr_Client;
    int clientAddrLen = sizeof(addr_Client);

    struct message connection_msg;
    strcpy(connection_msg.command, COMMAND_CONNECTION);
    struct connection_command_payload connection_payload;

    while (TRUE) {
        socket_Client = accept(socket_Server, (struct sockaddr*)&addr_Client, &clientAddrLen);
        pthread_mutex_lock(&clients_mutex);
        if (num_clients < MAX_CLIENTS) 
        {
            num_clients++;
            printf("New client, number of client : %i \n",num_clients);
            pthread_mutex_unlock(&clients_mutex);
            connection_payload.connection_code = SERV_FREE;
            memcpy(connection_msg.buf, &connection_payload, sizeof(struct connection_command_payload));
            send(socket_Client, (char*)&connection_msg, sizeof(struct message), 0);
            pthread_create(&threads[num_clients - 1], NULL, threadClient, &socket_Client);
        }
        else if (queueIsFull)
        {
            pthread_mutex_unlock(&clients_mutex);

            printf("Connection rejected.\n");
            connection_payload.connection_code = SERV_FULL_EXIT;
            memcpy(connection_msg.buf, &connection_payload, sizeof(struct connection_command_payload));
            send(socket_Client, (char*)&connection_msg, sizeof(struct message), 0);
            closesocket(socket_Client);
        }
        else
        {
            queueIsFull=TRUE;
            printf("Client in queue.\n");
            socket_Client_Q=socket_Client;
            pthread_mutex_unlock(&clients_mutex);

            connection_payload.connection_code = SERV_FULL_Q;
            memcpy(connection_msg.buf, &connection_payload, sizeof(struct connection_command_payload));
            send(socket_Client, (char*)&connection_msg, sizeof(struct message), 0);
        }
    }

    pthread_mutex_destroy(&clients_mutex);

    closesocket(socket_Server);
    WSACleanup();
    return 0;
}
