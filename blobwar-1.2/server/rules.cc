#include"rules.h"
#include<stdlib.h>
#include<iostream>
#include<sys/timex.h>

extern int game_started;

struct ntptimeval currenttime;
Uint32 starttime;

rules::rules(string mapname) {

	number_of_players = 2;
	turn_number = 0;
	parse_map(mapname);

	//we start, the game is not finished yet
	finished = false;

	//initially no one has blobs...
	for(Uint8 i = 0 ; i < 8 ; i++)
		for(Uint8 j = 0 ; j < 8 ; j++)
			blobs[i][j] = -1;

	//...only one blob in each corner
	blobs[0][0] = 0;
	blobs[7][0] = 0;
	blobs[0][7] = 1;
	blobs[7][7] = 1;
	
	//start
	next_turn();
	ntp_gettime(&currenttime);
	starttime = currenttime.time.tv_sec;
}

rules::~rules() {
	for(Uint8 i = 0 ; i < 8 ; i++) 
		delete [] holes[i];
	delete [] holes;
}

bool rules::set_move(Uint8 oldx, Uint8 oldy, Uint8 x, Uint8 y) {
	//check if move is valid, if yes grant access and modify game state
	//first, check we don't move on empty space
	if (holes[x][y]) {
#ifdef DEBUG
		cout<<"trying to move into a hole, refusing move"<<endl;
#endif
		return false;
	}
	//now check there is no one there
	if (blobs[x][y] != -1) {
#ifdef DEBUG
		cout<<"trying to move into someone, refusing move"<<endl;
#endif
		return false;
	}
	//check if we are trying to move someone from the other side
	if (blobs[oldx][oldy] != get_current_player()) {
#ifdef DEBUG
		cout<<"trying to move from a blob of the other side, refusing move"<<endl;
		cout<<"blob at "<<(Uint32)oldx<<" "<<(Uint32)oldy<<" is "<<(Uint32)blobs[oldx][oldy]<<" and you are "<<get_current_player()<<endl;
#endif
		return false;
	}
	//now check distance and type of move
	//we first compute the square of the distance
	Uint32 d = (oldx - x) * (oldx - x);
	Uint32 d2 = (oldy - y) * (oldy - y);
	d = (d>d2)?d:d2;
	if (d<=4) {
		//move accepted
#ifdef DEBUG
		cerr<<"move from ("<<(Uint32)oldx<<","<<(Uint32)oldy<<") to ("<<(Uint32)x<<","<<(Uint32)y<<") accepted"<<endl;
#endif
		ox = oldx;
		oy = oldy;
		nx = x;
		ny = y;
		do_move();
		ntp_gettime(&currenttime);
		float turntime = (currenttime.time.tv_sec - starttime);
		float period = (float) currenttime.time.tv_usec / 1000000;
		turntime += period;
		cout<<"at "<<turntime<<" :";
		cout<<"turn: "<<turn_number<<" player: "<<(turn_number % number_of_players)<< " moved from ("<<(Uint32)ox<<","<<(Uint32)oy<<") to ("<<(Uint32)nx<<","<<(Uint32)ny<<")"<<endl;
		return true;
	}
	
	//too far
#ifdef DEBUG
	cout<<"trying to move too far, refusing move"<<endl;
#endif
	return false;
}

void rules::do_move() {
#ifdef DEBUG
	cerr<<"player: "<<(turn_number % number_of_players)<<"moving from: "<<(Uint32)ox<<","<<(Uint32)oy<<"to "<<(Uint32)nx<<","<<(Uint32)ny<<endl;
#endif

	//first check if we need to create a new blob or to move an old one
	if (((ox - nx)*(ox - nx)<=1)&&((oy - ny)*(oy - ny)<=1)) {
		//it's a copy
		//update our info
		blobs[nx][ny] = (turn_number % number_of_players);
	} else {
		//it's a move
		//update rules info
		blobs[ox][oy] = -1;
		blobs[nx][ny] = (turn_number % number_of_players);
	}
	//now eventually change some neighbours colors
	Sint16 current_player = (turn_number % number_of_players);

	for(Sint8 i = -1 ; i < 2 ; i++)
		for(Sint8 j = -1 ; j < 2 ; j++) {
			if (nx+i < 0) continue;
			if (nx+i > 7) continue;
			if (ny+j < 0) continue;
			if (ny+j > 7) continue;
			if ((blobs[nx+i][ny+j]!=-1)&&(blobs[nx+i][ny+j]!=current_player)) {
				blobs[nx+i][ny+j] = current_player;
			}
		}

	//a player finished moving, go to next turn
	next_turn();
}

void rules::next_turn() {
	turn_number++;

	//first, check if only one player is left
	bool alive[number_of_players];
	for(Uint16 i = 0 ; i < number_of_players ; i++)
		alive[i] = false;
	for(Uint8 x = 0 ; x < 8 ; x++)
		for(Uint8 y = 0 ; y < 8 ; y++) 
			if (blobs[x][y] != -1)
				alive[blobs[x][y]] = true;

	Uint16 num = 0;
	for(Uint16 i = 0 ; i < number_of_players ; i++) 
		if (alive[i]) num++;

	bool not_finished = false;
	bool can_move[number_of_players];

	if (num != 1) {
	
		//first, check if someone can move (iterate on all empty spaces)
		for(Uint16 i = 0 ; i < number_of_players ; i++)
			can_move[i] = false;
		for(Sint16 x = 0 ; x < 8 ; x++) 
			for(Sint16 y = 0 ; y < 8 ; y++) {
				//if a hole we can't move in it, continue
				if(holes[x][y]) continue;
				//same if already occupied
				if(blobs[x][y] != -1) continue;
				//now this space is empty, check if someone can enter it
				//loop on all neighbours
				for(Sint16 i = -2 ; i <= 2 ; i++)
					for(Sint16 j = -2 ; j <= 2 ; j++) {
						if ((x+i < 0)||(y+j < 0)||(x+i > 7)||(y+j > 7)) continue;
						if (blobs[x+i][y+j] != -1)
							can_move[blobs[x+i][y+j]] = true;
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
		while(!can_move[turn_number % number_of_players]) {
			turn_number++;
			cout<<"player can't play, skipping his turn"<<endl;
		}
	}
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
			if(blobs[i][j] != -1) scores[blobs[i][j]]++;

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
			
	if (cnt == 1) {
		cout<<"winner: "<<(Uint32)(winner)<<endl;
	} else {
		cout<<"draw"<<endl;
	}
	cout<<"player 0: "<<scores[0]<<endl;
	cout<<"player 1: "<<scores[1]<<endl;

	//tell network to shutdown connections
	game_started = false;
}

void rules::parse_map(string mapname) {
	char line[9];
	string realname = ("../data/boards/"+mapname);
	ifstream infile;
	infile.open(realname.c_str(), ios::in);
#ifdef DEBUG
	cerr<<"loading map : "<<realname<<endl;
#endif

	//allocate holes
	holes = new bool*[8];
	for(Uint8 i = 0 ; i < 8 ; i++)
		holes[i] = new bool[8];

	for(Uint8 i = 0 ; i < 8 ; i++) {
		infile.getline(line, 9, '\n');
		for(Uint8 j = 0 ; j < 8 ; j++) {
			holes[i][j] = (line[j] == 'x')?true:false;
		}
	}
	infile.close();

}


Uint16 rules::get_current_player() {
	return (turn_number % number_of_players);
}
