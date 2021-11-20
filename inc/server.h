#ifndef SERVER_H
#define SERVER_H
#include <arpa/inet.h>

struct client_data {
  int socket;
  struct sockaddr_storage storage;
  int thread_id;
};

#define IS_EOW_MASK 1
#define IS_IN_TRIE 2
#define MAX_N_POKEMON 40
#define ALPHABET_SIZE 26

#define POKEMON_NAME_MAX_SIZE 10

typedef struct trie_node trie_node;
typedef trie_node* trie_node_p;

struct trie_node {
  int n;
  char flags;
  trie_node_p children[ALPHABET_SIZE];
  char word[POKEMON_NAME_MAX_SIZE];
};

int check_message_prefix(const char* message);
typedef int (*message_processer)(int, char*);

#endif
