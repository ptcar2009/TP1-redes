#include <arpa/inet.h>
#include <stdio.h>

#include "common.h"

void print_trie(trie_node_p node) {
  if (node->word != NULL) printf("%s\n", node->word);
}

int main(int argc, char* argv[]) {
  struct sockaddr_storage storage;
  if (addrparse(argv[1], argv[2], &storage)) return 1;
  char addr_str[500];

  struct sockaddr* addr = (struct sockaddr*)(&storage);

  addrtostr(addr, addr_str);
  printf(addr_str);
  return 0;
}
