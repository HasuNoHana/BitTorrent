main: main.c source/socket.c source/user.c source/client.c source/queue.c source/metadata.c source/interface.c
	gcc -o bittorrent main.c source/socket.c source/user.c source/client.c source/queue.c source/metadata.c source/interface.c -I. -lpthread -lssl -lcrypto

.PHONY: clean

clean:
	rm bittorrent
