#include "network.h"
#include <stdio.h>

network::network() {
	SDLNet_Init();
	connected = false;
	//parse config file
	FILE *f = fopen("network.cfg", "r");
        int e;
        e=fscanf(f, "servername=%s\n", (char*) &servername);
	char sport[10];
	e=fscanf(f, "port=%s\n", sport);
	serverport = atoi(sport);
	e=fscanf(f, "teamname=%s\n", (char*) &teamname);
	e=fscanf(f, "AI=%d\n", &playertype);
        fclose(f);
	if (!e);
}

network::~network() {
	if (sock != NULL) 
		SDLNet_TCP_Close(sock);
	SDLNet_Quit();
}

char* network::connect() {
	IPaddress address;
	SDLNet_ResolveHost(&address, servername, serverport);
	sock = SDLNet_TCP_Open(&address);
	if (sock == NULL) {
		cerr<<"unable to connect to server: "<<SDL_GetError()<<endl;
		return NULL;
	}
	//now wait for connection welcome message
	bzero((void*)receive_buffer, 100);
	Uint32 size = SDLNet_TCP_Recv(sock, (void*) receive_buffer, 100);
	if (size <= 0) {
		cerr<<"no connection confirmation from server: "<<SDL_GetError()<<endl;
		SDLNet_TCP_Close(sock);
		return NULL;
	}
	//message(s) received, check it's a welcome message
	//also process remaining messages
	Uint32 position = 0;
	while (receive_buffer[position] != '\n') {
		position++;
	}
	receive_buffer[position] = 0;

	if (strncmp(receive_buffer, "/welcome", 8)==0) {
		connected = true;
		set = SDLNet_AllocSocketSet(1);
		SDLNet_TCP_AddSocket(set, sock);
		//go through remaining messages
		execute_messages(position+1, size);
		//send our name to server
		char name[100];
		memcpy(name, "/name ", 6);
		size_t team_name_capped_size = strlen(teamname);
		if (team_name_capped_size > 100-6-2) {
			team_name_capped_size = 100-6-2;
		}
		memcpy(name+6, teamname, team_name_capped_size);
		name[6+team_name_capped_size] = '\n';
		name[6+team_name_capped_size+1] = 0;

		send(name);
		//return our player number
		return (receive_buffer + 9);
	} else {
		cerr<<"error: received msg: \""<<receive_buffer<<"\" during connection"<<endl;
		SDLNet_TCP_Close(sock);
		return NULL;
	}	
}

void network::change_map(const char* mapname) {
	strcpy(send_buffer, "/map ");
	strncat(send_buffer, mapname, 100 - strlen(send_buffer));
	strncat(send_buffer, "\n", 100 - strlen(send_buffer));
	send();
}

void network::send(const char* msg) {
	strncpy(send_buffer, msg, 99);
    send_buffer[99] = 0;
	send();
	return;
}

void network::send() {
#ifdef DEBUG
	cout<<"sending: "<<send_buffer<<endl;
#endif
	SDLNet_TCP_Send(sock, send_buffer, strlen(send_buffer));
}

void network::handle() {
	if (!connected) return;

	SDLNet_CheckSockets(set, 1);

	if (SDLNet_SocketReady(sock)) {
		bzero((void*)receive_buffer, 100);
		int size = SDLNet_TCP_Recv(sock, receive_buffer, 100);
		if (size <= 0) {
			cerr<<"error receiving message from server (disconnected ?) "<<SDL_GetError()<<endl;
			connected = false;
			SDLNet_TCP_Close(sock);
			//first of all check if the game is not finished
			if (game->wingamebutton->is_hidden()) {
				//go to disconnection screen
				if (game->bwrules != NULL) {
					//if game already started display score
					game->bwrules->set_scores();
				}
				else game->set_scores(0, 0);
	
				//display disconnection screen
				game->set_winner(5);
			}

			return;
		}
		execute_messages(0, size);
	}
}

void network::execute_messages(Uint32 startposition, Uint32 size) {
	//iterate on all received messages
	Uint32 position = startposition; 
	Uint32 start = startposition;
	cout<<"executing on :\""<<(receive_buffer + startposition)<<"\""<<endl;
	while(position < size) {
		if (receive_buffer[position] == '\n') {
			//end of a message
			receive_buffer[position] = 0;
			parse(receive_buffer + start);
			start = position+1;
		}
		position++;
	}
	if (start != position) {
		cerr<<"execute messages: something is left!"<<endl;
		//TODO: copy what's left at start of next buffer
	}
}

void network::parse(char* msg) {
#ifdef DEBUG
	cout<<"we received: "<<msg<<endl;
#endif
	//map change
	if (strncmp(msg, "/map", 4) == 0) {
		//change local map
		game->bwboard->switch_map(msg+5);
		return;
	}
	//game start
	if (strncmp(msg, "/start", 6) == 0) {
		game->start_game(atoi(msg+7));
		return;
	}
	//movement
	if (strncmp(msg, "/do_move", 8) == 0) {
#ifdef DEBUG
		cout<<"move command received from server"<<endl;
#endif
		int ox, oy, nx, ny;
		sscanf(msg+9, "(%d,%d) to (%d,%d)", &ox, &oy, &nx, &ny);
		game->bwrules->ox = (Uint8) ox;
		game->bwrules->oy = (Uint8) oy;
		game->bwrules->nx = (Uint8) nx;
		game->bwrules->ny = (Uint8) ny;
		game->bwrules->do_move();
		return;
	}
	//not a command, then it's a message
	message(msg);
}

void network::do_move(Uint8 ox, Uint8 oy, Uint8 nx, Uint8 ny) {
	char tmp[40];
	strcpy(send_buffer, "/do_move ");
	snprintf(tmp, 40, "(%d,%d) to (%d,%d)\n", ox, oy, nx, ny);
	strncat(send_buffer, tmp, 100-strlen(send_buffer)-1);
	send();
}

Uint32 network::get_player_type() {
	return playertype;
}

void network::message(char* msg) {
	cout<<"message received: \""<<msg<<"\""<<endl;
}

//TODO: disconnect with free set
