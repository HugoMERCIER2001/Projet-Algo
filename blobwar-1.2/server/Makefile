
CFLAGS=-Wall -Werror -g -I/home/perms/wagnerf/libs/include/SDL -DDEBUG

blobwar_server: 	server.o rules.o
	g++ server.o rules.o -lSDL_net `sdl-config --libs` -o blobwar_server $(CFLAGS) -L/home/perms/wagnerf/libs/lib

server.o: 	server.cc 
	g++ -c server.cc -o server.o `sdl-config --cflags` $(CFLAGS)

rules.o: 	rules.cc 
	g++ -c rules.cc -o rules.o `sdl-config --cflags` $(CFLAGS)

clean: 	
	rm -f *.o *~ core blobwar_server
