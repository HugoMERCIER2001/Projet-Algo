#include "board.h"
#include <fstream>
#include <dirent.h>
#include <sstream>

tile::tile(SDL_Rect pos, SDL_Surface* bg, SDL_Surface* focused, Uint8 lx, Uint8 ly) {
	position = pos;
	bg_not_focused = bg;
	bg_focused = focused;
	x = lx;
	y = ly;
}


tile::~tile() {
}

Uint8 tile::get_x() {
	return x;
}

Uint8 tile::get_y() {
	return y;
}

void tile::display() {
	if (game->bwboard->is_active()) {
		//if we seem focused, check first before displaying
		if (is_in(game->mousepointer->get_x(), game->mousepointer->get_y())) {
			SDL_BlitSurface(bg_focused, NULL, game->screen, &position);
			game->bwboard->set_focused_tile(x, y);
		} else 
			SDL_BlitSurface(bg_not_focused, NULL, game->screen, &position);
	} else
		SDL_BlitSurface(bg_not_focused, NULL, game->screen, &position);
}

bool tile::is_in(Sint32 x, Sint32 y) {
	//first test whether we are in the SDL_Rect
	if ((x>=position.x)&&(x<position.x + position.w)&&(y>=position.y)&&(y<position.y + position.h)) {
		//ok we are, now we need a more fine grain test
		Sint32 lx = x - position.x;
		Sint32 ly = y - position.y;
		//test against the four lines delimiting the shape
		if (ly >= (position.h*lx/ position.w) + (position.h /2)) return false;
		if (ly <= - (position.h*lx/ position.w) + (position.h /2)) return false;
		if (ly <= (position.h*lx/position.w) - (position.h / 2)) return false;
		if (ly >= - (position.h*lx/ position.w) + 1.5*position.h) return false;
		return true;
	}
	return false;
}


void tile::flip(SDL_Surface* s) {
	//if we are a hole return (holes don't highlight as they aren't candidates
	//for destinations)
	if (bg_focused == bg_not_focused) return;
	//not a hole, change surface
	bg_not_focused = s;
	return;
}

void board::reset() {
	//misc inits
	active = false;
	fx = 255;
	fy = 255;
	selected_tile = NULL;

	//reinit blobs 
	for(Uint8 i = 0 ; i < 8 ; i ++) 
		for(Uint8 j = 0 ; j < 8 ; j++) 
			if (blobs[i][j] != NULL) {
				delete blobs[i][j];
				blobs[i][j] = NULL;
			};

	for(Uint16 i = 0 ; i < 4 ; i++)
		scores[i] = 0;
}

board::board() {

	//misc inits
	active = false;
	fx = 255;
	fy = 255;
	selected_tile = NULL;

	//init blobs 
	for(Uint8 i = 0 ; i < 8 ; i ++) 
		for(Uint8 j = 0 ; j < 8 ; j++) 
				blobs[i][j] = NULL;


	for(Uint16 i = 0 ; i < 4 ; i++)
		scores[i] = 0;
	players_names[0] = "red";
	players_names[1] = "blue";
	players_names[2] = "green";
	players_names[3] = "yellow";

	position.x = 70;
	position.w = 700;
	position.y = 100;
	position.h = 400;

	ulrect.x = position.x;
	ulrect.w = 336;
	ulrect.y = 320;
	ulrect.h = 300;

	urrect.x = position.x+336;
	urrect.w = 336;
	urrect.y = 320;
	urrect.h = 300;

	//start by loading images
	underleft = game->alpha_image_load("underleft.png");
	underright = game->alpha_image_load("underright.png");

	tilegrass = game->alpha_image_load("tilegrass.png");
	tilegrasshigh = game->alpha_image_load("tilegrasshigh.png");
	h0 = game->alpha_image_load("tilegrass3.png");
	h1 = game->alpha_image_load("tilegrass2.png");
	h2 = game->alpha_image_load("tilegrass1.png");
	tilehole = game->alpha_image_load("tilehole.png");

	blobs_surfaces.push_back(game->alpha_image_load("redblob.png"));
	blobs_surfaces.push_back(game->alpha_image_load("blueblob.png"));
	blobs_surfaces.push_back(game->alpha_image_load("greenblob.png"));
	blobs_surfaces.push_back(game->alpha_image_load("yellowblob.png"));

	//load the list of all maps
	string dirname = game->rename("boards/");
	struct dirent *entry;
	DIR* d = opendir(dirname.c_str());
	do {
		entry = readdir(d);
		if (entry != NULL) {
			//skip hiddend files
			if (entry->d_name[0] != '.') {
				string s(entry->d_name);
				maps.push_back(s);
			}
		}
	} while(entry!=NULL);
	
	closedir(d);

#ifdef DEBUG
	cout<<"list of all maps:"<<endl;
	for(vector<string>::iterator it = maps.begin(); it != maps.end(); it++) {
		cout << "name: "<<(*it)<<endl;
	}
#endif

	//load map
	load_map(maps[0]);
	map_number = 0;

	//build tiles
	SDL_Rect r;
	r.w = 84;
	r.h = 40;
	for(Uint8 i = 0 ; i < 8 ; i++) {
		for(Uint8 j = 0 ; j < 8 ; j++) {
			r.x = position.x + i*(r.w/2) +j*(r.w/2);
			r.y = position.y+position.h/2 +j*(r.h/2) -i*(r.h/2);
			if (holes.get(i, j)) { 
				tiles[i*8+j] = new tile(r, tilehole, tilehole, i, j);
			} else tiles[i*8+j] = new tile(r, tilegrass, tilegrasshigh, i, j);
		}
	}
}

board::~board() {
	for(Uint8 i = 0 ; i < 64 ; i++) {
		if (tiles[i] != NULL) delete tiles[i];
	}
	cout<<"all tiles deleted"<<endl;
	SDL_FreeSurface(underleft);
	cout<<"board deleted"<<endl;
}

void board::load_map(string filename) {
	char line[9];
	string realname = game->rename("boards/"+filename);
	ifstream infile;
	infile.open(realname.c_str(), ios::in);
#ifdef DEBUG
	cout<<"loading map : "<<realname<<endl;
#endif

	for(Uint8 i = 0 ; i < 8 ; i++) {
		infile.getline(line, 9, '\n');
		for(Uint8 j = 0 ; j < 8 ; j++) {
			bool value = (line[j] == 'x')?true:false;
			holes.set(i, j, value);
		}
	}
	infile.close();

	game->set_map_label(filename);
}

void board::display() {

	SDL_BlitSurface(underleft, NULL, game->screen, &ulrect);
	SDL_BlitSurface(underright, NULL, game->screen, &urrect);
	for(Uint8 i = 0 ; i < 64 ; i++) {
		if (tiles[i] != NULL) {
			tiles[i]->display();
		}
	}
}

void board::focus() {
}

void board::unfocus() {
}

void board::click() {
	if (fx != 255) {
		//if a tile was selected try move or change selection
		if (selected_tile != NULL) {
			//if target is a blob change selection
			if (blobs[fx][fy] != NULL) {
				//first check for authorization
				if (game->bwrules->authorize_selection(fx, fy)) {
					unselect_tile(selected_tile->get_x(), selected_tile->get_y());
					selected_tile = tiles[8*fx+fy];
					select_tile(fx, fy);
				}
			}
			//else try move
			else game->bwrules->set_move(selected_tile->get_x(), selected_tile->get_y(), fx, fy);
		} else {
			//if not try select
			if (game->bwrules->authorize_selection(fx, fy)) {
				selected_tile = tiles[8*fx+fy];
				//highlight neighbours
				select_tile(fx, fy);
			} 
		}
	}
}

const char* board::get_next_board() {
	Uint32 n = map_number +1;
	n = (n >= maps.size())?0:n;
	return maps[n].c_str();
}

const char* board::get_previous_board() {
	Uint32 n = map_number;
	n = (n == 0)?(maps.size()-1):(n-1);
	return maps[n].c_str();
}

void board::next_board() {
	map_number++;
	map_number = (map_number >= maps.size())?0:map_number;
	switch_map(maps[map_number]);
}

void board::previous_board() {
	map_number = (map_number == 0)?(maps.size()-1):(map_number-1);
	switch_map(maps[map_number]);
}


/**change the board to a new map
 * this method is used in the map selection screen
 * \param filename : name of new map*/
void board::switch_map(string filename) {
	load_map(filename);
	//modify tiles
	for(Uint8 i = 0 ; i < 8 ; i++) {
		for(Uint8 j = 0 ; j < 8 ; j++) {
			if (holes.get(i, j)) { 
				tiles[i*8+j]->set_tile(tilehole, tilehole);
			} else tiles[i*8+j]->set_tile(tilegrass, tilegrasshigh);
		}
	}
	game->display2update = true;
	//recompute map number
	for(Uint32 i = 0 ; i < maps.size(); i++) {
		if (filename == maps[i]) {
			map_number = i;
			break;
		}
	}
}

void tile::set_tile(SDL_Surface *t, SDL_Surface *t2) {
	bg_not_focused = t;
	bg_focused = t2;
}

void board::create_blob(Uint8 x, Uint8 y, Uint16 player) {
	//update score
	scores[player]++;	
	string s;
	for(Uint16 i = 0 ; i < number_of_players ; i++) 
		if (scores[i] != 0) {
			stringstream o;
			o << scores[i];
			s = s + players_names[i] + ": " + o.str() + " ";
		}
	game->set_scores_label(s);
	//create blob
	blobs[x][y] = new blob(x, y, player, blobs_surfaces[player]);
	game->display2update = true;
	//after a move selection is put back to none
	selected_tile = NULL;
}

void board::move_blob(Uint8 ox, Uint8 oy, Uint8 x, Uint8 y) {
	blobs[ox][oy]->move(x, y);
	blobs[x][y] = blobs[ox][oy];
	blobs[ox][oy] = NULL;
	game->display2update = true;
	selected_tile = NULL;
}

void board::activate() {
	active = true;
}

bool board::is_active() {
	return active;
}

void board::set_focused_tile(Uint8 x, Uint8 y) {
	fx = x;
	fy = y;
}

void board::change_blob_owner(Uint8 x, Uint8 y, Uint16 player) {
	//update score
	scores[player]++;	
	scores[blobs[x][y]->get_owner()]--;
	string s;
	for(Uint16 i = 0 ; i < number_of_players ; i++) 
		if (scores[i] != 0) {
			stringstream o;
			o << scores[i];
			s = s + players_names[i] + ": " + o.str() + " ";
		}
	game->set_scores_label(s);

	//now change owner
	blobs[x][y]->change_owner(player, blobs_surfaces[player]);
}

void board::select_tile(Uint8 x, Uint8 y) {
	//highlight selected tile
	tiles[8*x+y]->flip(h0);

	//highlight tiles neighbours
	for(Sint8 i = -2 ; i < 3 ; i++)
		for(Sint8 j = -2 ; j < 3 ; j++) {
			if (x+i < 0) continue;
			if (y+j < 0) continue;
			if (x+i > 7) continue;
			if (y+j > 7) continue;
			if ((i==0)&&(j==0)) continue;
			if ((i==2)||(i==-2)||(j==2)||(j==-2)) tiles[8*(x+i)+y+j]->flip(h2);
			else tiles[8*(x+i)+y+j]->flip(h1);
		}

	game->display2update = 1;
}

void board::unselect_tile(Uint8 x, Uint8 y) {
	//all tiles go back to non-selected image
	for(Sint8 i = -2 ; i < 3 ; i++)
		for(Sint8 j = -2 ; j < 3 ; j++) {
			if (x+i < 0) continue;
			if (y+j < 0) continue;
			if (x+i > 7) continue;
			if (y+j > 7) continue;
			tiles[8*(x+i)+y+j]->flip(tilegrass);
		}
}

/**
 * puts the initial blobs on the board
 * at the same time updates the private "number_of_players" field
 */
void board::init(unsigned int num) {
	number_of_players = num;
	if (number_of_players == 2) {
		create_blob(0,0,0);
		create_blob(7,0,0);
		create_blob(0,7,1);
		create_blob(7,7,1);
	} else {
		//4 players
		create_blob(0,0,0);
		create_blob(7,0,1);
		create_blob(0,7,2);
		create_blob(7,7,3);
	}
}

void board::copy_board(bidiarray<bool> &destination) {
	destination = holes;
}
