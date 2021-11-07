#ifndef COMMON_H

#define COMMON_H
#include <arpa/inet.h>
#define ADD_POKEMON_PREFIX "add\0"
#define REMOVE_POKEMON_PREFIX "remove\0"
#define EXCHANGE_POKEMON_PREFIX "exchange\0"
#define LIST_POKEMON_PREFIX "list\0"
#define KILL_PREFIX "kill\0"
#define BUFSZ 500

int check_message_prefix(const char* message);

char** parse_pokemons(const char* message);

int get_number_of_blocks(const char* message);

#define IS_EOW_MASK 1
#define IS_IN_TRIE 2

typedef struct trie_node trie_node;
typedef trie_node* trie_node_p;

struct trie_node {
  int n;
  char flags;
  trie_node_p children[26];
  char* word;
};

typedef void (*trie_callback)(trie_node_p);


int addrparse(const char* addrstr, const char* portstr,
              struct sockaddr_storage* storage);
int addrtostr(const struct sockaddr* addr, char* str);

void FATAL(char*, ...);
void ERROR(char*, ...);
void INFO(char*, ...);
void DEBUG(char*, ...);

#endif
