#include "rules.h"
#include "blobwar.h"
#include <stdlib.h>
#include <pthread.h>
#include "strategy.h"
#include <sys/time.h>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "shmem.h"

Sint32 estimate(bidiarray<Sint16> &blobs) {
	Sint32 count = 0;
	for(Uint8 i = 0 ; i < 8 ; i++) {
		for(Uint8 j = 0 ; j < 8 ; j++) {
			if (blobs.get(i, j) == 0) count++;
			if (blobs.get(i, j) == 1) count--;
		}
	}
	return count;
}

void rules::display_position() {
	for(Uint8 i = 0 ; i < 8 ; i++) {
		for(Uint8 j = 0 ; j < 8 ; j++) {
			if (holes.get(i, j)) { 
				cout<<"o ";
				continue;
			}
			if (blobs.get(i, j) == 0) cout<<"r ";
			if (blobs.get(i, j) == -1) cout<<"x ";
			if (blobs.get(i, j) == 1) cout<<"b ";
		}
		cout<<endl;
	}
	return;
}

player::player(Uint16 id, Uint8 type) {
	player_type = type;
	player_id = id;
}

player::~player() {}

bool player::is_human() {
	return (player_type == 0);
}

bool player::is_computer() {
	return (player_type == 1);
}

rules::rules(Uint16 type, board *b, Uint32 local_player_id) {
	

	gametype = type;

	if (type == GAME4PMATCH) 
		number_of_players = 4;
	else number_of_players = 2;
	turn_number = 0;
	b->copy_board(holes); //copy data from the map in holes array

	colors = new string[number_of_players];
	colors[0] = "Red";
	colors[1] = "Blue";
	if (number_of_players == 4) {
		colors[2] = "Green";
		colors[3] = "Yellow";
	}

	//we start, the game is not finished yet
	finished = false;

	//initially no one has blobs...
	for(Uint8 i = 0 ; i < 8 ; i++)
		for(Uint8 j = 0 ; j < 8 ; j++)
			blobs.set(i, j, -1);

	//...only one blob in each corner
	if (number_of_players == 2) {
		blobs.set(0, 0, 0);
		blobs.set(7, 0, 0);
		blobs.set(0, 7, 1);
		blobs.set(7, 7, 1);
	} else {
		blobs.set(0, 0, 0);
		blobs.set(7, 0, 1);
		blobs.set(0, 7, 2);
		blobs.set(7, 7, 3);
	}
	//put the blobs in place on the board
	game->bwboard->init(number_of_players);
	

	//now check for the type of game
	if (type == GAME1P) {
		players.push_back(new player(0, 0));
		players.push_back(new player(1, 1));
	}

	if (type == GAME2P) {
		players.push_back(new player(0, 0));
		players.push_back(new player(1, 0));
	}
	if (type == GAME2PMATCH ){
		players.push_back(new player(0, 1));
		players.push_back(new player(1, 1));
	}

	if (type == GAME4PMATCH ){
        players.push_back(new player(0, 1));
        players.push_back(new player(1, 1));
        players.push_back(new player(2, 1));
        players.push_back(new player(3, 1));
	}

	if (type == NETGAME) {
#ifdef DEBUG
	cout<<"starting network game ; local player is number "<<local_player_id<<endl;
#endif
		if (local_player_id == 0) {
			players.push_back(new player(0, game->bwnet->get_player_type()));
			players.push_back(new player(0, 2));
		} else {
			players.push_back(new player(0, 2));
			players.push_back(new player(0, game->bwnet->get_player_type()));
		}
	}

	if (type == GAME4P) {
		players.push_back(new player(0, 0));
		players.push_back(new player(1, 0));
		players.push_back(new player(2, 0));
		players.push_back(new player(3, 0));
	}

	//start
	game->bwrules = this;
	next_turn();
}

rules::~rules() {
	for(vector<player*>::iterator it = players.begin(); it != players.end() ; it++) {
		delete (*it);
	}
}

bool rules::authorize_selection(Uint8 x, Uint8 y) {
	//if it's not one blob of current player refuse
	if (blobs.get(x, y) != (Sint16)CURRENT_PLAYER) return false;
	//if current player is not human, refuse
	if (!players[CURRENT_PLAYER]->is_human()) return false;
#ifdef DEBUG
	cout<<"player "<<CURRENT_PLAYER<<" selected blob at "<<(Uint32)x<<","<<(Uint32)y<<endl;
#endif
	return true;
}

bool rules::valid_move() {
	//first, check if the move is on the board
	if ((ox < 0)||(ox > 8)||(nx < 0)||(nx > 8)||(oy < 0)||(oy > 8)||(ny < 0)||(ny > 8)) {
#ifdef DEBUG
		cout<<"trying to move outside of the board, refusing move"<<endl;
		return false;
#endif
	}
	//check source is not destination
	if ((ox == nx) && (oy == ny)) {
#ifdef DEBUG
		cout<<"moving on same spot, refusing move"<<endl;
		return false;
#endif
	}
	//now, check we don't move on empty space
	if (holes.get(nx, ny)) {
#ifdef DEBUG
		cout<<"trying to move into a hole, refusing move"<<endl;
#endif
		return false;
	}
	//check there is no one there
	if (blobs.get(nx, ny) != -1) {
#ifdef DEBUG
		cout<<"trying to move into someone, refusing move"<<endl;
#endif
		return false;
	}
	//check if we are trying to move someone from the other side
	if (blobs.get(ox, oy) != Sint32(CURRENT_PLAYER)) {
#ifdef DEBUG
		cout<<"trying to move from a blob of the other side, refusing move"<<endl;
#endif
		return false;
	}
	//check distance and type of move
	//we first compute the square of the distance
	Uint32 d = (ox - nx) * (ox - nx);
	Uint32 d2 = (oy - ny) * (oy - ny);
	d = (d>d2)?d:d2;
	if (d<=4) {
		//move accepted
		return true;
	}
	//too far
#ifdef DEBUG
	cout<<"trying to move too far, refusing move"<<endl;
#endif
	return false;
}

bool rules::set_move(Uint8 oldx, Uint8 oldy, Uint8 x, Uint8 y) {
		ox = oldx;
		oy = oldy;
		nx = x;
		ny = y;
		if (valid_move()) {
#ifdef DEBUG
			cout<<"move from ("<<(Uint32)oldx<<","<<(Uint32)oldy<<") to ("<<(Uint32)x<<","<<(Uint32)y<<") accepted"<<endl;
#endif
			if (gametype == NETGAME) {
				game->bwnet->do_move(ox, oy, nx, ny);
			} else {
				do_move();
			}
			return true;
		} else {
			return false;
		}
}

void rules::do_move() {
#ifdef DEBUG
	cout<<"player: "<<(CURRENT_PLAYER)<<" moving from: "<<(Uint32)ox<<","<<(Uint32)oy<<" to "<<(Uint32)nx<<","<<(Uint32)ny<<endl;
#endif
	if (!valid_move()) {
		
		cerr<<"invalid move from player "<<CURRENT_PLAYER<<", he loses!"<<endl;
		//WARNING: this code does not work well with 4P mode
		for(Uint8 i = 0 ; i < 8 ; i++)
			for(Uint8 j = 0 ; j < 8 ; j++)
				if(blobs.get(i, j) == Sint32(CURRENT_PLAYER))
					blobs.set(i, j, !CURRENT_PLAYER);
	} else {
		pthread_mutex_lock(&game->mutex);
		
		//we don't need selection anymore
		game->bwboard->unselect_tile(ox, oy);
		//first check if we need to create a new blob or to move an old one
		if (((ox - nx)*(ox - nx)<=1)&&((oy - ny)*(oy - ny)<=1)) {
			//it's a copy
			//notify local board of the copy
			game->bwboard->create_blob(nx, ny, CURRENT_PLAYER);
			//update our info
			blobs.set(nx ,ny, CURRENT_PLAYER);
		} else {
			//it's a move
			//notify local board of the move
			game->bwboard->move_blob(ox, oy, nx, ny);
			//update rules info
			blobs.set(ox ,oy, -1);
			blobs.set(nx ,ny, CURRENT_PLAYER);
		}
		//now eventually change some neighbours colors
		Sint16 current_player = CURRENT_PLAYER;

		for(Sint8 i = -1 ; i < 2 ; i++)
			for(Sint8 j = -1 ; j < 2 ; j++) {
				if (nx+i < 0) continue;
				if (nx+i > 7) continue;
				if (ny+j < 0) continue;
				if (ny+j > 7) continue;
				if ((blobs.get(nx+i, ny+j)!=-1)&&(blobs.get(nx+i, ny+j)!=current_player)) {
					game->bwboard->change_blob_owner(nx+i, ny+j, current_player);
					blobs.set(nx+i, ny+j, current_player);
				}
			}
		
		pthread_mutex_unlock(&game->mutex);
	}

	//a player finished moving, go to next turn
	next_turn();
}


void* timer(void*d)
{
#ifdef DEBUG
	cout<<"Timer start for "<<game->compute_time_IA<<"s"<<endl;
#endif
	sleep(game->compute_time_IA);
#ifdef DEBUG
	cout<<"Timer out. Now kill the IA (pid: "<< (*((pid_t*)d)) <<")"<<endl;
#endif
	kill( *((pid_t*)d), SIGTERM);
	return NULL;
}


void rules::compute_move() {

	shmem_init(true);
	
	string cplayer("0");
	cplayer[0] = '0'+(CURRENT_PLAYER);
	
#ifdef DEBUG
	printf("Now fork: %s %s %s %s\n", "./launchStrategy", blobs.serialize().c_str(), holes.serialize().c_str(), cplayer.c_str());
#endif
	int childPid = fork();
	if(childPid == 0) // Child process
	{
		execl("./launchStrategy", "./launchStrategy", blobs.serialize().c_str(), holes.serialize().c_str(), cplayer.c_str(), (char *)NULL);
	}
	
	// start timer
	pthread_t timerThread;
	pthread_create (&timerThread, NULL, timer, (void*)&childPid);
	
	int status;   
	while (wait(&status) != childPid)
		/* empty */;
	pthread_cancel(timerThread);
	
	movement m = shmem_get();
	ox = m.ox;
	oy = m.oy;
	nx = m.nx;
	ny = m.ny;
	
	
	
#ifdef DEBUG
	cout<<"computer computed move from: "<<(Uint32)ox<<","<<(Uint32)oy<<" to "<<(Uint32)nx<<","<<(Uint32)ny<<endl;
#endif

	if (gametype == NETGAME) {
		game->bwnet->do_move(ox, oy, nx, ny);
	} else {
		do_move();
	}
}

void rules::next_turn() {
	turn_number++;

	//first, check if only one player is left
	bool alive[number_of_players];
	for(Uint16 i = 0 ; i < number_of_players ; i++)
		alive[i] = false;
	for(Uint8 x = 0 ; x < 8 ; x++)
		for(Uint8 y = 0 ; y < 8 ; y++) 
			if (blobs.get(x, y) != -1)
				alive[blobs.get(x, y)] = true;

	Uint16 num = 0;
	for(Uint16 i = 0 ; i < number_of_players ; i++) 
		if (alive[i]) num++;

	bool not_finished = false;
	bool can_move[number_of_players];

// #ifdef DEBUG
// 	cout<<"turn finished ; remaining players : "<<num<<endl;
// #endif

	if (num != 1) {
	
		//first, check if someone can move (iterate on all empty spaces)
		for(Uint16 i = 0 ; i < number_of_players ; i++)
			can_move[i] = false;
		for(Sint16 x = 0 ; x < 8 ; x++) 
			for(Sint16 y = 0 ; y < 8 ; y++) {
				//if a hole we can't move in it, continue
				if(holes.get(x, y)) continue;
				//same if already occupied
				if(blobs.get(x, y) != -1) continue;
				//now this space is empty, check if someone can enter it
				//loop on all neighbours
				for(Sint16 i = -2 ; i <= 2 ; i++)
					for(Sint16 j = -2 ; j <= 2 ; j++) {
						if ((x+i < 0)||(y+j < 0)||(x+i > 7)||(y+j > 7)) continue;
						if (blobs.get(x+i, y+j) != -1)
							can_move[blobs.get(x+i, y+j)] = true;
					}
			}
		
		//now if no one can play any more, game is finished
		for(Uint16 i = 0 ; i < number_of_players ; i++) 
			not_finished = not_finished || can_move[i];
	
	}

	if ((!not_finished)||(num == 1)) {
		end();
	} else {
		//ok we can continue playing
		//loop until a player can play
		while(!can_move[CURRENT_PLAYER]) {
			turn_number++;
		}
	
		//let this player play
	
		game->set_main_label(colors[CURRENT_PLAYER]+" player's turn");
		if (players[CURRENT_PLAYER]->is_computer()) {
			//start computing move
			pthread_t thread;
			pthread_create (&thread, NULL, launch_computations, NULL);
		}
	}
}

void rules::set_scores() {
	Uint32 scores[4];
	scores[0] = 0;
	scores[1] = 0;
	scores[2] = 0;
	scores[3] = 0;
	for(Uint8 i = 0 ; i < 8 ; i++)
		for(Uint8 j = 0 ; j < 8 ; j++)
			if(blobs.get(i, j) != -1) scores[blobs.get(i, j)]++;
	game->set_scores(scores[0], scores[1]);
	return;
}

void rules::end() {
	//someone won
	//compute who
	Uint32 scores[4];
	scores[0] = 0;
	scores[1] = 0;
	scores[2] = 0;
	scores[3] = 0;
	for(Uint8 i = 0 ; i < 8 ; i++)
		for(Uint8 j = 0 ; j < 8 ; j++)
			if(blobs.get(i, j) != -1) scores[blobs.get(i, j)]++;

	Uint16 winner = 0;
	Uint32 max = 0;
	for(Uint8 i = 0 ; i < 4 ; i++) {
		if (max < scores[i]) {
			winner = i;
			max = scores[i];
		}
	}
	//check if not a draw
	Uint8 cnt = 0;
	for(Uint8 i = 0 ; i < 4 ; i++) {
		if (max == scores[i]) cnt++;
	}
			
	//forward the info to the interfaces
	if (cnt == 1) {
		//someone wins
#ifdef DEBUG
		cout<<colors[winner] + " player wins"<<endl;
#endif
		game->set_winner(winner+1);
	} else {
		//draw
#ifdef DEBUG
		cout<<"match ends in draw"<<endl;
#endif
		game->set_winner(0);
	}
	game->set_scores(scores[0], scores[1]);
}

void* launch_computations(void* data) {
	game->bwrules->compute_move();
	return  NULL;
}
