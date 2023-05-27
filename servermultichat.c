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
char name_tab[MAX_CLIENTS][MAX_NAME_SIZE];

void checkValidity(int validity, char sentence[]) {
    if (validity < 0) {
        printf("ERROR: %s \n", sentence);
    }
}

void printNumbers(int numbers[], int length) {
    printf("Output:");
    for (int i = 0; i < length; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
}

void bubbleSort(int numbers[], int length) {
    for (int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (numbers[j] > numbers[j + 1]) {
                int temp = numbers[j];
                numbers[j] = numbers[j + 1];
                numbers[j + 1] = temp;
            }
        }
    }
    printNumbers(numbers, length);
}

void addName(char name[]) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (name_tab[i][0] == '\0') {
            strcpy(name_tab[i], name);
            break;
        }
    }
    printf("Add Name : %s \n", name);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        printf("Name : %s \n", name_tab[i]);
    }
    pthread_mutex_unlock(&clients_mutex);
}

void deleteName(char name[]) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(name_tab[i], name) == 0) {
            name_tab[i][0] = '\0';
            break;
        }
    }
    num_clients--;
    printf("Close client, number of client : %i \n", num_clients);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        printf("Name : %s \n", name_tab[i]);
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* threadClient(void* socket) {
    SOCKET client_socket = *(SOCKET*)socket;
    boolean ack_bool = TRUE;
    boolean connection = TRUE;
    struct message msg;
    // REGISTER THE CLIENT
    char req[] = "Enter your name :";
    char name[MAX_NAME_SIZE];
    send(client_socket, req, sizeof(req), 0);
    recv(client_socket, name, MAX_NAME_SIZE, 0);
    addName(name);
    while (connection) {
        int validity = 404;
        validity = recv(client_socket, (char*)&msg, sizeof(struct message), 0);
        checkValidity(validity, "Receive");
        if (validity < 0) {
            break;
        }
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
            connection = FALSE;
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
    deleteName(name);
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

    while (TRUE) {
        socket_Client = accept(socket_Server, (struct sockaddr*)&addr_Client, &clientAddrLen);

        pthread_mutex_lock(&clients_mutex);
        if (num_clients < MAX_CLIENTS) {
            num_clients++;
            printf("New client, number of client : %i \n", num_clients);
            pthread_mutex_unlock(&clients_mutex);
            pthread_create(&threads[num_clients - 1], NULL, threadClient, &socket_Client);
        }
        else {
            pthread_mutex_unlock(&clients_mutex);
            printf("Connection rejected.\n");
            closesocket(socket_Client);
        }
    }

    pthread_mutex_destroy(&clients_mutex);

    closesocket(socket_Server);
    WSACleanup();
    return 0;
}
