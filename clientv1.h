#ifndef CLIENTV1_H
#define CLIENTV1_H
void checkValidity(int validity, char sentence[]);
struct message printCommand(struct message cmd,char* argv);
#endif