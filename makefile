CC = gcc
CFLAGS = -Wall -ggdb
DEPS = client.h server.h
BIN = bin/
OUT = out/
CLIENTOBJ = $(BIN)client.o
SERVEROBJ = $(BIN)server.o

$(BIN)%.o: %.c $(DEPS)
		$(CC) $(CFLAGS) -c -o $@ $<

all: init client server

client: $(CLIENTOBJ)
		gcc $(CFLAGS) -o $(OUT)$@ $^

server: $(SERVEROBJ)
		gcc $(CFLAGS) -o $(OUT)$@ $^

init:
		mkdir -p $(BIN)
		mkdir -p $(OUT)
		
clean:
		rm $(BIN)client.o $(BIN)server.o $(BIN)main.o $(OUT)client $(OUT)server