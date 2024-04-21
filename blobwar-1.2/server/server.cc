#include<SDL.h>
#include<SDL_net.h>
#include<iostream>
#include<string.h>
#include"rules.h"

#define MAXPLAYERS 2

//misc info
int number_of_players;
bool game_started;
char player_names[MAXPLAYERS][100];
char mapname[100];
rules* bwrules;

//network global variables
char send_buffer[100];
char receive_buffer[100];
TCPsocket player_sockets[MAXPLAYERS];
SDLNet_SocketSet set;

//end the game (disconnect everyone, ....)
void end_game() {
	cout<<"ending game"<<endl;
	//disconnect everyone
	for(int playerid = 0 ; playerid < number_of_players ; playerid++) {
		SDLNet_TCP_DelSocket(set, player_sockets[playerid]);
		SDLNet_TCP_Close(player_sockets[playerid]);
		player_sockets[playerid] = NULL;
	}
	//put things back in place for next game
	number_of_players = 0;
	delete bwrules;
}

void disconnect(int playerid) {
#ifdef DEBUG
	cerr<<"disconnecting player "<<playerid<<endl;
#endif
	SDLNet_TCP_DelSocket(set, player_sockets[playerid]);
	SDLNet_TCP_Close(player_sockets[playerid]);
	player_sockets[playerid] = NULL;
}

void change_map(char* msg) {
#ifdef DEBUG
	cerr<<"changing map"<<endl;
#endif
	//put back \n at end of string
	strcat (msg, "\n");
	//broadcast map change to everyone
	for(int i = 0 ; i < number_of_players ; i++) {
		SDLNet_TCP_Send(player_sockets[i], (void*) msg, strlen(msg));
	}
}

void start_game() {
#ifdef DEBUG
	cerr<<"starting game"<<endl;
#endif
	//create new game
	bwrules = new rules(mapname);
	cout<<"starting game on map :"<<mapname<<endl;
	//send info to everyone
	for(int i = 0 ; i < number_of_players ; i++) {
		char tmp[5];
		//send to each player it's ID together with start signal
		strcpy(send_buffer, "/start ");
		sprintf(tmp, "%d", i);
		strcat(send_buffer, tmp);
		strcat(send_buffer, "\n");
		SDLNet_TCP_Send(player_sockets[i], (void*) send_buffer, strlen(send_buffer));
		cout<<"player "<<i<<" : "<<player_names[i]<<endl;
	}
	game_started = true;
	return;
}

//check if move is OK and broadcast to everyone
void do_move(char* msg, int player) {
#ifdef DEBUG
	cerr<<"player: "<<player<<" suggested a move : "<<msg<<endl;
#endif

	if (player != bwrules->get_current_player()) {
#ifdef DEBUG
		cerr<<"player: "<<player<<" tried to play while not on his turn"<<endl;
#endif
		return;
	}

	int ox, oy, nx, ny;
	sscanf(msg+9, "(%d,%d) to (%d,%d)", &ox, &oy, &nx, &ny);
	
	if (bwrules->set_move(ox, oy, nx, ny)) {
		//move accepted
		strcat(msg,"\n");
		//broadcast move to everyone
		for(int i = 0 ; i < number_of_players ; i++) {
			SDLNet_TCP_Send(player_sockets[i], (void*) msg, strlen(msg));
		}
		if (!game_started) {
			//game is now ended, reset server
			end_game();	
		}
	} else {
		//move refused
#ifdef DEBUG
		cerr<<"player: "<<player<<" sent an invalid move !"<<endl;
#endif
	}
}

void broadcast_message(char* msg, int player) {
	char finalmsg[200];
	strcpy(finalmsg, player_names[player]);
	strcat(finalmsg, ": ");
	strcat(finalmsg, msg);
	strcat(finalmsg, "\n");

	for(int i = 0 ; i < number_of_players ; i++) {
		if (i==player) continue;
		SDLNet_TCP_Send(player_sockets[i], finalmsg, strlen(finalmsg));
	}
}

//msg should be a NULL terminated string
void process_message(char* msg, int player) {
#ifdef DEBUG
	cerr<<"player: "<<player<<" sent :\""<<msg<<"\""<<endl;
#endif
	//nothing
	if (strlen(msg) == 0) return;

	//name change
	if (strncmp(msg, "/name", 5)==0) {
		//update player name
		strncpy(player_names[player], msg+6, 100);
#ifdef DEBUG
		cerr<<"player: "<<player<<" changed name to \""<<player_names[player]<<"\""<<endl;
#endif
		return;
	}

	//change map
	if (strncmp(msg, "/map", 4)==0) {
		if (!game_started) {
			strncpy(mapname, msg+5, 100);
			change_map(msg);
		} else {
			cerr<<"warning: player "<<player<<"tried changing map in an already started game"<<endl;
		}
		return;
	}

	//start game
	if (strncmp(msg, "/start", 6)==0) {
		if (!game_started) {
			if (number_of_players == 2)
				start_game();
			else {
				cerr<<"too few players, game not started"<<endl;
			}
		} else {
			cerr<<"warning: player "<<player<<"tried starting a game already started"<<endl;
		}
		return;
	}

	//do move
	if (strncmp(msg, "/do_move", 8)==0) {
		do_move(msg,player);
		return;
	}

	//not a command then it's a message to broadcast
	broadcast_message(msg, player);
}

void serverloop(TCPsocket server_socket) {

	//create a set of all sockets to monitor
	set = SDLNet_AllocSocketSet(MAXPLAYERS+1);
	SDLNet_TCP_AddSocket(set, server_socket);

	//server loop
	while(true) {
		//loop each time something happens to a socket we are monitoring
		int count = SDLNet_CheckSockets(set, 1000);
		if (count == -1) {
			cerr<<"error with select: "<<SDL_GetError()<<endl;
			SDLNet_FreeSocketSet(set);
			SDLNet_TCP_Close(server_socket);
			SDLNet_Quit();
			exit(3);
		}
		//no changes, loop again
		if (count == 0) continue;

		if SDLNet_SocketReady(server_socket) {
			//new incoming connection
			if ((number_of_players >= MAXPLAYERS)||(game_started)) {
				//number of allowed players exceeded or game already started
				cerr<<"one extra connection asked, refusing"<<endl;
				TCPsocket tmp = SDLNet_TCP_Accept(server_socket);
				SDLNet_TCP_Send(tmp, (void*) "server full\n", 12);
				SDLNet_TCP_Close(tmp);
			} else {
				//accept new connection
				//give to this player the first free socket to be found
				int playerid;
				for(playerid = 0 ; playerid < number_of_players ; playerid++) {
					if (player_sockets[playerid] == NULL) break;
				}

				player_sockets[playerid] = SDLNet_TCP_Accept(server_socket);
				if (player_sockets[playerid] == NULL) {
					cerr<<"unable to accept client connection: "<<SDL_GetError()<<endl;
				}
				else {
		#ifdef DEBUG
					IPaddress *tmp;
					tmp = SDLNet_TCP_GetPeerAddress(player_sockets[playerid]);
					cerr<<"new player connection from : "<<SDLNet_ResolveIP(tmp)<<endl;
		#endif 
					char startmsg[100];
					strcpy(startmsg, "/welcome ");
					strcat(startmsg, mapname);
					strcat(startmsg, "\n");
					SDLNet_TCP_Send(player_sockets[playerid], (void*) startmsg, strlen(startmsg));
					//don't forget to add new socket to the set of sockets to monitor
					SDLNet_TCP_AddSocket(set, player_sockets[playerid]);
					number_of_players++;
				}
			}
		}

		//one of the players sent something
		for(int player = 0 ; player < number_of_players ; player++) {
			if (SDLNet_SocketReady(player_sockets[player])) {
				//get what player sent
				int size = SDLNet_TCP_Recv(player_sockets[player], (void*) receive_buffer, 100);
				if (size <=0) {
					cerr<<"warning: error receiving message from player: "<<SDL_GetError()<<endl;
					if (game_started) {
						cout<<"player "<<player<<" disconnected during game"<<endl;
						//disconnect everyone
						for(int i = 0 ; i < number_of_players ; i++) 
							disconnect(i);
						//end game
						game_started = false;
						number_of_players = 0;
						delete bwrules;
						number_of_players = 0;
					} else {
						cerr<<"player"<<player<<" disconnected before game"<<endl;
						//disconnect only the faulty player
						disconnect(player);
						number_of_players--;
					}
				} else {
					//go through all messages received
					Sint32 position = 0;
					Sint32 start = 0;
					while(position < size) {
						if (receive_buffer[position] == '\n') {
							receive_buffer[position] = 0;
							process_message(receive_buffer+start, player);
							start = position+1;
						}
						position++;
					}
					if (start != position) {
						cerr<<"warning: something is left in buffer"<<endl;
					}
				}
			}
		}


	}

	SDLNet_FreeSocketSet(set);
	return;
}


int main(int argc, char **argv) {

	IPaddress server_address;
	TCPsocket server_socket;
	number_of_players = 0;
	for(int i = 0; i < MAXPLAYERS ; i++) {
		char tmp[5];
		player_sockets[i] = NULL;
		strcpy(player_names[i], "player ");
		sprintf(tmp, "%d", i);
		strcat(player_names[i], tmp);
	}
	strcpy(mapname, "standard");
	game_started = false;

	if(argc != 2) {
		cerr<<"usage: "<<argv[0]<<" port_number"<<endl;
		exit(1);
	}
#ifdef DEBUG
	cerr<<"starting server on port"<<atoi(argv[1])<<endl;
#endif
	//starting SDLnet
	SDL_Init(0);
	SDLNet_Init();

	//create server socket
	SDLNet_ResolveHost(&server_address, NULL, atoi(argv[1]));
	server_socket = SDLNet_TCP_Open(&server_address);
	if (server_socket == NULL) {
		cerr<<"unable to create server socket: "<<SDL_GetError()<<endl;
		exit(2);
	}

	serverloop(server_socket);


	//closing connection
#ifdef DEBUG
	cerr<<"finished"<<endl;
#endif
	SDLNet_TCP_Close(server_socket);
	SDLNet_Quit();
	exit(0);
}
