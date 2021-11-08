#ifndef SERVER_H
#define SERVER_H
#include <arpa/inet.h>

struct client_data {
  int socket;
  struct sockaddr_storage storage;
};

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

int check_message_prefix(const char* message);
typedef void (*message_processer)(int, char*);

#endif
