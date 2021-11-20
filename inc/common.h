#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>

#define ADD_POKEMON_PREFIX "add\0"
#define REMOVE_POKEMON_PREFIX "remove\0"
#define EXCHANGE_POKEMON_PREFIX "exchange\0"
#define LIST_POKEMON_PREFIX "list\0"
#define KILL_PREFIX "kill\0"
#define BUFSZ 500
#define POOLSZ 10

int addrparse(const char* addrstr, const char* portstr,
              struct sockaddr_storage* storage);
int addrtostr(const struct sockaddr* addr, char* str);

void FATAL(char*, ...);
void ERROR(char*, ...);
void INFO(char*, ...);
void DEBUG(char*, ...);

#endif
