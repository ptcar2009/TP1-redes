// Client side C/C++ program to demonstrate Socket programming
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"

int addrparse(const char* addrstr, const char* portstr,
              struct sockaddr_storage* storage) {
  if (addrstr == NULL || portstr == NULL) return -1;

  uint16_t port = (uint16_t)atoi(portstr);
  if (!port) return -1;
  port = htons(port);

  struct in_addr inaddr4;
  if (inet_pton(AF_INET, addrstr, &inaddr4)) {
    struct sockaddr_in* addr4 = (struct sockaddr_in*)storage;
    addr4->sin_family = AF_INET;
    addr4->sin_port = port;
    addr4->sin_addr = inaddr4;
    return 0;
  }
  struct in6_addr inaddr6;
  if (inet_pton(AF_INET6, addrstr, &inaddr6)) {
    struct sockaddr_in6* addr6 = (struct sockaddr_in6*)storage;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_port = port;
    memcpy(&addr6->sin6_addr, &inaddr6, sizeof(inaddr6));
    return 0;
  }

  return -1;
}
int read_message_from_server(int socket, char* buf) {
  int count = 1;
  unsigned total = 0;
  while (count && (!strchr(buf, '\n'))) {
    count = recv(socket, buf + total, BUFSZ - total, 0);
    if (count == -1) return -1;
    total += count;
  }
  return total;
}

int send_message_to_server(int socket, char* msg) {
  DEBUG("sending message to server");
  return send(socket, msg, strlen(msg), 0);
}

int main(int argc, char const* argv[]) {
  struct sockaddr_storage storage;

  // parse addr and port from CLI
  if (addrparse(argv[1], argv[2], &storage)) return 1;

  char addr_str[BUFSZ];

  struct sockaddr* addr = (struct sockaddr*)(&storage);

  addrtostr(addr, addr_str);

  int sock = 0;

  if ((sock = socket(storage.ss_family, SOCK_STREAM, 0)) < 0) {
    FATAL("socket creation failed");
  }
  if (connect(sock, addr, sizeof(storage)) < 0) {
    FATAL("socket connection failed");
  }
  char buf[BUFSZ] = {0};
  while (1) {
    printf("> ");
    if (!fgets(buf, BUFSZ, stdin))
      sprintf(buf, "%s", "kill\n");

    if (send_message_to_server(sock, buf) == -1) {
      break;
    }

    if (!strcmp(buf, "kill\n")) break;

    memset(buf, 0, BUFSZ);


    if (read_message_from_server(sock, buf) == -1) {
      break;
    }

    printf("< ");

    printf(buf);
  }

  close(sock);

  return 0;
}
