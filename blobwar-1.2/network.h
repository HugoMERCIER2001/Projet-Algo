#ifndef GNET
#define GNET

#include"common.h"
#include<SDL_net.h>

class network;

#include"blobwar.h"


/**network class
 * helpfull class to send/parse messages, initiate connection, ...
 * */
class network {
	private:
		//!send buffer for outgoing messages
		char send_buffer[100];
		//!receive buffer for incoming messages
		char receive_buffer[100];
		//!socket used to communication to server
		TCPsocket sock;
		//!set used to detect incoming messages
		SDLNet_SocketSet set;
		//!are we or not already connected to server ?
		bool connected;

		//!name of server
		char servername[100];
		//!port of server
		Sint16 serverport;
		//!name of local team
		char teamname[100];
		//!is local player human (0) or AI (1) ?
		Uint32 playertype;

	public:
		//!constructor (init SDLnet)
		network();
		//!destructor (closes connections)
		~network();
		//!connect to server (returns map name if succeeded, NULL if failure)
		char* connect();
		//!inform the server of a map change
		void change_map(const char* mapname);
		//!send message
		void send();
		//!send message given in argument
		void send(const char* msg);
		//!handle incoming messages
		void handle();
		//!parse messages and execute corresponding actions
		void parse(char* msg);
		//!send move to the server
		void do_move(Uint8 ox, Uint8 oy, Uint8 nx, Uint8 ny);
		//!return the type of player used in network games
		Uint32 get_player_type();	
		//!go through all received messages and execute them all
		void execute_messages(Uint32 startposition, Uint32 size);
		//!we received a message, display it
		void message(char* msg);
};

#endif
