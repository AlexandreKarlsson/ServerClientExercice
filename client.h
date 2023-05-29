#ifndef CLIENT_H
#define CLIENT_H
#define ADDR_CLIENT "127.0.0.1"
void checkValidity(int validity, char *sentence);
void printNumbers(int numbers[], int length);
struct message printCommand(struct message cmd, char *order);
struct message sortCommand(struct message cmd, int numbers[], int length);
#endif