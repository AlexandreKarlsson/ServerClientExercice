#ifndef CLIENT_H
#define CLIENT_H
void checkValidity(int validity, char sentence[]);
struct message printCommand(struct message cmd,char* argv);
#endif