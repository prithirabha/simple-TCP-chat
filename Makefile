CC = gcc
CFLAGS = -Wall -Wextra -D_REENTRANT
LDFLAGS = -lpthread

SERVER_SRC = server.c
CLIENT_SRC = client.c

SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC) $(LDFLAGS)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC) $(LDFLAGS)

clean:
	rm -f $(SERVER) $(CLIENT)

.PHONY: all clean
