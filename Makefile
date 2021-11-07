IDIR =inc
CC=gcc
LOG_LEVEL ?= 1
CFLAGS=-I$(IDIR) -DLOG_LEVEL=$(LOG_LEVEL) -Wall -lpthread

SDIR=src
ODIR=obj

_COMMON_OBJ = common.o
COMMON_OBJ=$(patsubst %,$(ODIR)/%,$(_COMMON_OBJ))

_CLIENT_OBJ = client.o
CLIENT_OBJ=$(patsubst %,$(ODIR)/%,$(_CLIENT_OBJ))

_SERVER_OBJ = server.o
SERVER_OBJ=$(patsubst %,$(ODIR)/%,$(_SERVER_OBJ))

all: client server

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(COMMON_OBJ) $(SERVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

client: $(COMMON_OBJ) $(CLIENT_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

main: $(COMMON_OBJ) $(ODIR)/main.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(ODIR)/*.o
	rm -f client server main

