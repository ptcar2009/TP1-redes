// Client side C/C++ program to demonstrate Socket programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"

int read_message_from_server(int socket, char *buf) {
  int count = 1;
  unsigned total = 0;
  while (count && (!strchr(buf, '\n'))) {
    count = recv(socket, buf + total, BUFSZ - total, 0);
    total += count;
  }
  return total;
}

int send_message_to_server(int socket, char *msg) {
  DEBUG("sending message to server");
  int count = send(socket, msg, strlen(msg) + 1, 0);
  if (count != strlen(msg) + 1) return -1;
  return count;
}

int main(int argc, char const *argv[]) {
  struct sockaddr_storage storage;

  // parse addr and port from CLI
  if (addrparse(argv[1], argv[2], &storage)) return 1;

  char addr_str[BUFSZ];

  struct sockaddr *addr = (struct sockaddr *)(&storage);

  addrtostr(addr, addr_str);

  int sock = 0;

  char buf[BUFSZ] = {0};
  while (1) {
    if ((sock = socket(storage.ss_family, SOCK_STREAM, 0)) < 0) {
      FATAL("socket creation failed");
    }
    if (connect(sock, addr, sizeof(storage)) < 0) {
      FATAL("socket connection failed");
    }
    printf("> ");
    fgets(buf, BUFSZ, stdin);
    send_message_to_server(sock, buf);

    if (!strcmp(buf, "kill\n")) break;

    memset(buf, 0, BUFSZ);

    read_message_from_server(sock, buf);

    printf("< ");

    puts(buf);
    close(sock);
  }

  return 0;
}
