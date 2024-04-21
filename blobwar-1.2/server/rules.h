#ifndef GRULES
#define GRULES

#include<SDL.h>
#include<string>
#include<fstream>

#define GAME1P 1
#define GAME2P 2
#define NETGAME 3

using namespace std;

/**rules class
 * all game behaviour is controlled here
 * */
class rules {
	private:
		//!position of all blobs (-1 : no blob, else : player number)
		Sint16 blobs[8][8];

		//!is the game finished ?
		bool finished;

		//!go to next turn (also check if game is not finished)
		void next_turn();

		//!end the game (someone won)
		void end();

	public:

		//!pointer on the board
		bool **holes;
		//!old x position
		Uint8 ox;
		//!old y position
		Uint8 oy;
		//!new x position
		Uint8 nx;
		//!new y position
		Uint8 ny;
		//!number of players should only be two for now
		Uint16 number_of_players;
		//!current turn (number of turns elapsed since game beginning)
		Uint32 turn_number;

		//!start a new game
		rules(string mapname);
		~rules();
		//!play move given 
		void do_move();
		//!generate holes array from map file
		void parse_map(string mapname);
		//!return the player from which we expect a move
		Uint16 get_current_player();
		//!check if move is ok (returns true if yes) also, if ok update local game state
		bool set_move(Uint8 oldx, Uint8 oldy, Uint8 x, Uint8 y);
};

#endif
