#
# Makefile for Example
#
LIBS	= -lsocket -lnsl
CC = gcc
CFLAGS = -I.
DEPS = inet.h 
OBJ = chat_server.o chat_client.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: ChatServer ChatClient
	
ChatServer: chat_server.o
	$(CC) -o ChatServer chat_server.o -I.

ChatClient: chat_client.o 
	$(CC) -o ChatClient chat_client.o -I.

clean:
	rm *.o \
	ChatServer ChatClient