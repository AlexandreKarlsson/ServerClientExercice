# Server-Client Communication in C
*Italic text* This is a README file for a C code that demonstrates server-client communication. The code consists of different pairs of servers and clients, each with unique functionality. The clients (clientv1, clientv2, clientv3) are designed to interact with their respective servers (server, servermulti, servermutlichat).

# Code Structure
The code is organized as follows:

- clientv1.c: Client implementation for server.
- clientv2.c: Client implementation for servermulti.
- clientv3.c: Client implementation for servermutlichat.
- server.c: Server implementation for clientv1.
- servermulti.c: Server implementation for clientv2.
- servermutlichat.c: Server implementation for clientv3.
- message.h: Header file for the message implementation and information.
- client.h: Header file for the clients.
- message.h: Header file for the servers.

# Server-Client Pairings
- clientv1 - server: This pairing allows the client (clientv1) to send a single command to the server (server). The server will process the command and provide a response.
- clientv2 - servermulti: The client (clientv2) communicates with the server (servermulti). Multiple clients can connect to the server simultaneously, and the server handles each client's requests in a separate thread. The server also includes a queue (with a capacity of 1) to handle situations when it is already serving the maximum number of clients. If the queue is full, the server rejects new client connections.
- clientv3 - servermutlichat: The client (clientv3) interacts with the server (servermutlichat). The server prompts the client to provide a name and stores it to represent the client. Unlike the previous pairing, this server does not include a queue.

# Platform Compatibility
The code is currently designed to run on Windows. However, you can adapt the code to work on Linux and macOS platforms. Please make the necessary modifications to the code, including changing libraries and compilation commands, to ensure compatibility with your target platform.

# Compilation
To compile the code and generate the executable files, you can use the following commands:

## For Windows:
- gcc server.c -o server.exe -lws2_32
- gcc client.c -o client.exe -lws2_32
- gcc clientv1.c -o clientv1.exe -lws2_32
- gcc clientv2.c -o clientv2.exe -lws2_32
- gcc clientv3.c -o clientv3.exe -lws2_32
- gcc servermulti.c -o servermulti.exe -lws2_32 -lpthread
- gcc servermultichat.c -o servermultichat.exe -lws2_32 -lpthread


## For Linux or macOS:
Ensure that you have the necessary libraries and dependencies installed for successful compilation on your platform.

# Usage
To run the compiled executables, you can use the following commands:

## For Windows:

- clientv1.exe
- clientv2.exe
- clientv3.exe
- server.exe
- servermulti.exe
- servermutlichat.exe

## For Linux or macOS:

- ./clientv1
- ./clientv2
- ./clientv3
- ./server
- ./servermulti
- ./servermutlichat

Please note that the usage and functionality of each client and server are specific to their respective pairings. Follow the prompts and instructions provided by each program to interact with them effectively.

# Sources
- Windows lib for socket:
 https://stackoverflow.com/questions/67726142/how-can-i-use-sys-socket-h-on-windows
 
- copy a struct in a buffer:
 https://stackoverflow.com/questions/5589136/copy-struct-to-buffer
 
- compare string:
 https://stackoverflow.com/questions/8222495/how-to-compare-strings-in-an-if-statement
 
- separate a string in arguments:
 https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
 
- get the next token with strtok:
 https://stackoverflow.com/questions/3889992/how-does-strtok-split-the-string-into-tokens-in-c
 
- bubblesort:
 https://www.geeksforgeeks.org/bubble-sort-in-c/

