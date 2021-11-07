#include "common.h"

#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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

int addrtostr(const struct sockaddr* addr, char* dst) {
  int version;
  char addrstr[INET6_ADDRSTRLEN + 1] = "";
  uint16_t port;

  if (addr->sa_family == AF_INET) {
    version = 4;
    struct sockaddr_in* addr4 = (struct sockaddr_in*)(addr);
    if (!inet_ntop(addr->sa_family, &addr4->sin_addr, addrstr,
                   INET6_ADDRSTRLEN + 1))
      return -1;
    port = ntohs(addr4->sin_port);
  } else if (addr->sa_family == AF_INET6) {
    version = 6;
    struct sockaddr_in6* addr6 = (struct sockaddr_in6*)(addr);
    if (!inet_ntop(addr->sa_family, &addr6->sin6_addr, addrstr,
                   INET6_ADDRSTRLEN + 1))
      return -1;
    port = ntohs(addr6->sin6_port);

  } else
    return -1;

  sprintf(dst, "IPv%d %s:%hu", version, addrstr, port);
  return 0;
}

char *time_stamp(){

char *timestamp = (char *)malloc(sizeof(char) * 16);
time_t ltime;
ltime=time(NULL);
struct tm *tm;
tm=localtime(&ltime);

sprintf(timestamp,"%04d/%02d/%02d %02d:%02d:%02d", tm->tm_year+1900, tm->tm_mon,
    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
return timestamp;
}

#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3


#if (LOG_LEVEL==LOG_LEVEL_INFO)
void INFO(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [INFO] ", time_stamp());
  vprintf(format, args);
  printf("\n");
  va_end(args);
}
void DEBUG(char* format, ...) {
}
void FATAL(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [FATAL] ", time_stamp());
  vprintf(format, args);
  printf(" ERROR: %s\n", strerror(errno));
  va_end(args);
  exit(1);
}

void ERROR(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [ERROR] ", time_stamp());
  vprintf(format, args);
  printf(" ERROR: %s\n", strerror(errno));
  va_end(args);
}

#elif (LOG_LEVEL==LOG_LEVEL_DEBUG)
void INFO(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [INFO] ", time_stamp());
  vprintf(format, args);
  printf("\n");
  va_end(args);
}
void DEBUG(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [DEBUG] ", time_stamp());
  vprintf(format, args);
  printf("\n");
  va_end(args);
}
void FATAL(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [FATAL] ", time_stamp());
  vprintf(format, args);
  printf(" ERROR: %s\n", strerror(errno));
  va_end(args);
  exit(1);
}

void ERROR(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [ERROR] ", time_stamp());
  vprintf(format, args);
  printf(" ERROR: %s\n", strerror(errno));
  va_end(args);
}

#elif (LOG_LEVEL==LOG_LEVEL_ERROR)
void INFO(char* format, ...) {
}
void DEBUG(char* format, ...) {
}
void FATAL(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [FATAL] ", time_stamp());
  vprintf(format, args);
  printf(" ERROR: %s\n", strerror(errno));
  va_end(args);
  exit(1);
}

void ERROR(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [ERROR] ", time_stamp());
  vprintf(format, args);
  printf(" ERROR: %s\n", strerror(errno));
  va_end(args);
}
#elif (LOG_LEVEL==LOG_LEVEL_FATAL)
void INFO(char* format, ...) {
}
void DEBUG(char* format, ...) {
}
void FATAL(char* format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s [FATAL] ", time_stamp());
  vprintf(format, args);
  va_end(args);
  printf(" ERROR: %s\n", strerror(errno));
  exit(1);
}
void ERROR(char* format, ...) {
}
#endif
