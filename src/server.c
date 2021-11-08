#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

trie_node_p new_trie() {
  trie_node_p ret = malloc(sizeof(trie_node));
  ret->flags = 0;
  for (int i = 0; i < 26; ++i) ret->children[i] = NULL;
  ret->word = NULL;
  ret->n = 0;
  return ret;
}

int has_prefix(const char* word, const char* prefix) {
  return !strncmp(word, prefix, strlen(prefix));
}

int check_message_prefix(const char* message) {
  if (has_prefix(message, ADD_POKEMON_PREFIX)) return 0;
  if (has_prefix(message, REMOVE_POKEMON_PREFIX)) return 1;
  if (has_prefix(message, EXCHANGE_POKEMON_PREFIX)) return 2;
  if (has_prefix(message, LIST_POKEMON_PREFIX)) return 3;
  if (has_prefix(message, KILL_PREFIX)) return 4;
  return 5;
}

trie_node_p find_in_trie(const trie_node_p trie, const char* word) {
  trie_node_p cur = trie;

  for (const char* cur_char = word; *cur_char != '\0'; ++cur_char) {
    int cur_i = *cur_char - 'a';
    if (cur->children[cur_i] == NULL) cur->children[cur_i] = new_trie();
    cur = cur->children[cur_i];
  }

  return cur;
}

int insert_into_dictionary(trie_node_p trie, const char* word) {
  trie_node_p cur = find_in_trie(trie, word);
  cur->flags = cur->flags | IS_EOW_MASK;
  return 0;
}

int insert_into_existing(trie_node_p trie, const char* word) {
  trie_node_p cur = find_in_trie(trie, word);
  DEBUG("word: %s", word);

  // more than maximum capacity
  if (trie->n >= 50) return 2;

  // is not in dictionary
  if (!(cur->flags & IS_EOW_MASK)) return -1;

  // already in pokedex
  if (cur->flags & IS_IN_TRIE) return 1;

  cur->flags = cur->flags | IS_IN_TRIE;

  cur->word = malloc(100);
  strcpy(cur->word, word);
  trie->n++;

  return 0;
}

void traverse(trie_node_p trie, int sock) {
  if (trie->flags & IS_IN_TRIE) {
    if (!send(sock, trie->word, strlen(trie->word), 0))
      ERROR("unable to send response");
    if (!send(sock, " ", 1, 0)) ERROR("unable to send response");
  }

  for (int i = 0; i < 26; ++i) {
    if (trie->children[i] != NULL) {
      traverse(trie->children[i], sock);
    }
  }
}

int remove_from_trie(trie_node_p trie, const char* word) {
  trie_node_p cur = find_in_trie(trie, word);
  if (!(cur->flags & IS_EOW_MASK)) return -1;
  if (!(cur->flags & IS_IN_TRIE)) return 1;

  cur->flags &= ~IS_IN_TRIE;
  trie->n--;
  return 0;
}

int is_message_valid(const char* msg) {
  DEBUG("checking message validity");
  while (*msg != '\0') {
    if (*msg != ' ' && ! isalnum(*msg)) return 0;
    msg++;
  }
  return 1;
}

trie_node_p t;
int sock;

void send_message(const int socket, const char* msg) {
  if (!send(socket, msg, strlen(msg), 0)) {
    ERROR("unable to send response");
  }
}

void process_add(int socket, char* msg) {
  char* curw = strtok(msg, " ");
  char response[BUFSZ];
  // while there is a space before a new line
  DEBUG("entering add pokemon procedure");
  curw = strtok(NULL, " ");
  while (curw) {
    switch (insert_into_existing(t, curw)) {
      case 0:
        INFO("adding pokemon %s to trie", curw);
        strcat(response, curw);
        strcat(response, " \0");
        break;
      case 1:
        INFO("pokemon %s already exists, responding", curw);
        sprintf(response, "%s already exists", curw);
        send_message(socket, response);
        return;
      case -1:
        INFO("pokemon %s does not exist", curw);
        sprintf(response, "%s does not exist", curw);
        send_message(socket, response);
        return;
    }
    curw = strtok(NULL, " ");
  }

  strcat(response, "added\0");
  send_message(socket, response);
}

void process_remove(int socket, char* msg) {
  char* curw = strtok(msg, " ");
  char response[BUFSZ];
  DEBUG("entering remove pokemon procedure");
  curw = strtok(NULL, " ");
  while (curw) {
    if (remove_from_trie(t, curw)) {
      INFO("pokemon %s does not exist, responding", curw);
      sprintf(response, "%s does not exist", curw);
      send_message(socket, response);
      return;
    }
    INFO("removing pokemon %s from trie", curw);
    strcat(response, curw);
    strcat(response, " ");
    curw = strtok(NULL, " ");
  }
  strcat(response, "removed\0");
  send_message(socket, response);
}

void process_exchange(int socket, char* msg) {
  char* curw = strtok(msg, " ");
  char response[BUFSZ];
  DEBUG("entering exchange pokemon procedure");
  char *pokemon1, *pokemon2;
  pokemon1 = curw = strtok(NULL, " ");
  pokemon2 = curw = strtok(NULL, " ");
  trie_node_p poke1_n, poke2_n;
  poke1_n = find_in_trie(t, pokemon1);
  poke2_n = find_in_trie(t, pokemon2);

  if (!poke1_n) {
    INFO("pokemon %s does not exist, responding", pokemon1);
    sprintf(response, "%s does not exist", pokemon1);
    send_message(socket, response);
    return;
  }
  if (!poke2_n || !(poke2_n->flags & IS_EOW_MASK)) {
    INFO("pokemon %s does not exist, responding", pokemon2);
    sprintf(response, "%s does not exist", pokemon2);
    send_message(socket, response);
    return;
  }

  if (find_in_trie(t, pokemon2)->flags & IS_IN_TRIE) {
    INFO("pokemon %s already in trie, responding", pokemon2);
    sprintf(response, "%s already exists", pokemon2);
    send_message(socket, response);
    return;
  }
  if (remove_from_trie(t, pokemon1)) {
    INFO("pokemon %s does not exist, responding");
    sprintf(response, "%s does not exist", pokemon1);
    send_message(socket, response);
    return;
  }

  if (insert_into_existing(t, pokemon2)) {
    INFO("pokemon %s already exists, responding");
    sprintf(response, "%s already exists", pokemon1);
    send_message(socket, response);
    return;

  }
  sprintf(response, "%s exchanged", pokemon1);
  send_message(socket, response);
}

void process_list(int socket, char* msg) {
  if (!t->n)
    send_message(socket, "none\0");
  else
    traverse(t, socket);
}
void process_kill(int socket, char* msg) {
  close(socket);
  exit(0);
}

void process_invalid(int socket, char* msg) {
  char response[BUFSZ];
  INFO("received invalid message");
  sprintf(response, "invalid message");
  send_message(socket, response);
}

// list with message processing functions
message_processer processor[] = {process_add,  process_remove, process_exchange,
                                 process_list, process_kill,   process_invalid};

void process_message(int socket, char* msg) {
  char response[BUFSZ] = "\0";
  if (!is_message_valid(msg)) {
    INFO("received invalid message");
    sprintf(response, "invalid message");
    send_message(socket, response);
    return;
  }
  // uses the prefix as an index to know which processer to use
  processor[check_message_prefix(msg)](socket, msg);
}

void* process_request(void* data) {
  struct client_data* cdata = (struct client_data*)data;
  struct sockaddr* caddr = (struct sockaddr*)&cdata->storage;

  char caddr_str[BUFSZ];
  addrtostr(caddr, caddr_str);
  INFO("received connection from %s", caddr_str);
  char buf[BUFSZ] = {0};

  size_t count;
  int total = 0;

  // recovering message until either the receiving is 0 or there is a new line
  do {
    count = recv(cdata->socket, buf + total, BUFSZ - total, 0);
    if (count == 0 || strchr(buf, '\n')) break;
    total += count;
  } while (1);

  DEBUG("message received: %s", buf);

  // processing message until next new line
  char* msg = strtok(buf, "\n");
  while (msg) {
    process_message(cdata->socket, msg);
    msg = strtok(NULL, "\n");
  }
  close(cdata->socket);

  return NULL;
}

int server_protocol_parse(const char* protocolstr, const char* portstr,
                          struct sockaddr_storage* storage) {
  DEBUG("started server protocol parsing procedure");
  if (protocolstr == NULL || portstr == NULL) return -1;

  uint16_t port = (uint16_t)atoi(portstr);
  DEBUG("parsing port...");
  if (!port) return -1;
  DEBUG("port parsed as %hu", port);
  port = htons(port);

  if (!strcmp(protocolstr, "v4")) {
    struct sockaddr_in* addr4 = (struct sockaddr_in*)storage;
    addr4->sin_family = AF_INET;
    addr4->sin_port = port;
    addr4->sin_addr.s_addr = INADDR_ANY;
    return 0;
  }
  if (!strcmp(protocolstr, "v6")) {
    struct sockaddr_in6* addr6 = (struct sockaddr_in6*)storage;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_port = port;
    memcpy(&addr6->sin6_addr, &in6addr_any, sizeof(in6addr_any));
    return 0;
  }

  return -1;
}

int main(int argc, char* argv[]) {
  printf("%d", argc);
  t = new_trie();
  FILE* fp = fopen("pokemon.txt", "r");
  if (fp == NULL) FATAL("");
  char pokemon[BUFSZ];
  while (fscanf(fp, "%s", pokemon) != EOF) insert_into_dictionary(t, pokemon);

  struct sockaddr_storage storage;

  // parse protocol and port from CLI
  if (server_protocol_parse(argv[1], argv[2], &storage))
    FATAL("address translation failed");

  char addr_str[BUFSZ];
  struct sockaddr* addr = (struct sockaddr*)(&storage);
  addrtostr(addr, addr_str);

  int sock = 0;
  if ((sock = socket(storage.ss_family, SOCK_STREAM, 0)) < 0) {
    FATAL("socket creation failed");
  }
  DEBUG("socket created");

  if (bind(sock, addr, sizeof(storage))) {
    FATAL("bind failed");
  }

  DEBUG("bound to %s", addr_str);

  if (listen(sock, 10)) FATAL("listen failed");
  INFO("server is listening on %s", addr_str);

  while (1) {
    struct sockaddr_storage cstorage;

    socklen_t len;
    struct sockaddr* caddr = (struct sockaddr*)&cstorage;
    int csock = accept(sock, caddr, &len);
    if (csock == -1) {
      FATAL("failed to accept connection message");
    }

    struct client_data* cdata = malloc(sizeof(struct client_data));
    cdata->socket = csock;
    cdata->storage = cstorage;
    pthread_t tid;
    pthread_create(&tid, NULL, process_request, cdata);
  }
}

