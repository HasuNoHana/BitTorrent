run: src/main.c src/file_handler.c src/socket.c src/client_service.c src/filelist_handler.c
	gcc -o run src/main.c src/file_handler.c src/socket.c src/client_service.c src/filelist_handler.c -I. -lpthread
	
.PHONY: clean

clean:
	rm run 
	rm -r files/*
