#ifndef CLIENT_H
#define CLIENT_H
#define ADDR_CLIENT "127.0.0.1"
void checkValidity(int validity, char sentence[]);
struct message printCommand(struct message cmd,char* argv);
struct message sortCommand(struct message cmd,int numbers[],int lenght);
#endif