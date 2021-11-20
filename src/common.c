#include "common.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int addrtostr(const struct sockaddr* addr, char* dst) {
  int version;
  char addrstr[INET6_ADDRSTRLEN + 1] = "";
  uint16_t port;

  if (addr->sa_family == AF_INET) {
    version = 4;
    struct sockaddr_in* addr4 = (struct sockaddr_in*)(addr);
    if (!inet_ntop(addr->sa_family, &addr4->sin_addr, addrstr,
                   INET6_ADDRSTRLEN + 1)) {
      return -1;
    }
    port = ntohs(addr4->sin_port);
  } else if (addr->sa_family == AF_INET6) {
    version = 6;
    struct sockaddr_in6* addr6 = (struct sockaddr_in6*)(addr);
    if (!inet_ntop(addr->sa_family, &addr6->sin6_addr, addrstr,
                   INET6_ADDRSTRLEN + 1)) {
      INFO("bad things");
      return -1;
    }
    port = ntohs(addr6->sin6_port);

  } else {
    INFO("unable to find sa_family. sa_family: %d", addr->sa_family);
    return -1;
  }

  sprintf(dst, "IPv%d %s:%hu", version, addrstr, port);
  return 0;
}

char* time_stamp() {
  char* timestamp = (char*)malloc(BUFSZ);
  time_t ltime;
  ltime = time(NULL);
  struct tm* tm;
  tm = localtime(&ltime);

  sprintf(timestamp, "%04d/%02d/%02d %02d:%02d:%02d", tm->tm_year + 1900,
          tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  return timestamp;
}

#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

#if (LOG_LEVEL >= LOG_LEVEL_INFO)
void INFO(char* format, ...) {
  va_list args;
  va_start(args, format);
  char* ts = time_stamp();
  printf("%s [INFO] ",ts );
  free(ts);
  vprintf(format, args);
  printf("\n");
  va_end(args);
}
#else
void INFO(char* format, ...) {};
#endif

#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
void DEBUG(char* format, ...) {
  va_list args;
  va_start(args, format);
  char* ts = time_stamp();
  printf("%s [DEBUG] ",ts );
  free(ts);
  vprintf(format, args);
  printf("\n");
  va_end(args);
}
#else
void DEBUG(char* format, ...){}
#endif


#if (LOG_LEVEL >= LOG_LEVEL_ERROR)
void ERROR(char* format, ...) {
  va_list args;
  va_start(args, format);
  char* ts = time_stamp();
  printf("%s [ERROR] ", ts);
  free(ts);
  vprintf(format, args);
  printf(" ERROR: %s\n", strerror(errno));
  va_end(args);
}
#else
void ERROR(char* format, ...) {};
#endif

#if (LOG_LEVEL >= LOG_LEVEL_FATAL)
void FATAL(char* format, ...) {
  va_list args;
  va_start(args, format);
  char* ts = time_stamp();
  printf("%s [FATAL] ",ts );
  free(ts);
  vprintf(format, args);
  va_end(args);
  printf(" ERROR: %s\n", strerror(errno));
  exit(1);
}
#else
void FATAL(char* format, ...) {exit(1)};
#endif
