#ifndef GRULES
#define GRULES

#include"common.h"

class rules;

#include "board.h"
#include "bidiarray.h"
//if we include strategy.h, there is a recursive inclusion, so:
class Strategy;
class Timer;

#define GAME1P 1
#define GAME2P 2
#define NETGAME 3
#define GAME4P 4
#define GAME2PMATCH 5
#define GAME4PMATCH 6

#define CURRENT_PLAYER (turn_number % number_of_players)

#include "move.h"
    

/**player class
 * all different type of players (human, ia, networked)*/
class player {
	private:
		//!our id 
		Uint16 player_id;
		//!are we local human (0), local IA (1), network player (2)
		Uint8 player_type;
	public:
		//!
		player(Uint16 id, Uint8 type);
		//!is this player human and playing locally ?
		bool is_human();
		//!is this player a local computer ?
		bool is_computer();
		~player();
};

/**rules class
 * all game behaviour is controlled here
 * */
class rules {
	private:
		//!type of game played (1P, 2P or network)
		Uint16 gametype;
		//!position of all blobs (-1 : no blob, else : player number)
		bidiarray<Sint16> blobs;
		//!all players we can ask to play
		vector<player*> players;

		//!correspondance between player numbers and colors
		string *colors;


		//!go to next turn (also check if game is not finished)
		void next_turn();

		//!end the game (someone won)
		void end();
		//
		//!checks if chosen move is valid
		bool valid_move();

	public:

		//!is the game finished ?
		bool finished;
		//!pointer on the board
		bidiarray<bool> holes;
		//!old x position
		Uint8 ox;
		//!old y position
		Uint8 oy;
		//!new x position
		Uint8 nx;
		//!new y position
		Uint8 ny;
		//!number of players should only be two for starting
		Uint16 number_of_players;
		//!current turn (number of turns elapsed since game beginning)
		Uint32 turn_number;

		//!start a new game
		rules(Uint16 type, board *b, Uint32 local_player_id);
		~rules();
		//!ask the rules whether we have right to select a blob
		bool authorize_selection(Uint8 x, Uint8 y);
		//!tell the rules we do a move
		bool set_move(Uint8 oldx, Uint8 oldy, Uint8 x, Uint8 y);

		//!compute the computer move (current player)
		void compute_move();

		//!play the move set by player or computed
		void do_move();

		//!update local interface to display current scores
		void set_scores();

		//!print current board to console
		void display_position();

};

		//!launcher for the computations
		void* launch_computations(void* data);

#endif
