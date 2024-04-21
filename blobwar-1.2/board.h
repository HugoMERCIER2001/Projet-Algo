#ifndef GBOARD
#define GBOARD

class tile;
class board;

#include "rollover.h"
#include "blobwar.h"
#include "blob.h"
#include "bidiarray.h"

class tile {
	private:
		//!position on screen
		SDL_Rect position;
		//!background surface when mouse focus
		SDL_Surface *bg_focused;
		//!background surface when not focused
		SDL_Surface *bg_not_focused;

		//our local coordinates
		//!tile x
		Uint8 x;
		//!tile y
		Uint8 y;

		//!are this coordinates inside this tile ?
		bool is_in(Sint32 x, Sint32 y);
	public:
		tile(SDL_Rect pos, SDL_Surface *bg, SDL_Surface *focused, Uint8 lx, Uint8 ly);
		~tile();
		//!change the tile (used when changing the board's map)
		void set_tile(SDL_Surface* t, SDL_Surface* t2);
		//!display on screen
		void display();
		//!return x coordinate (in grid of tiles)
		Uint8 get_x();
		//!return y coordinate (in grid of tiles)
		Uint8 get_y();
		//!change the not focused surface (if not a hole)
		void flip(SDL_Surface* s);
};

class board : public rollover {
	private:
		
		//!image under the board (left)
		SDL_Surface *underleft;
		SDL_Rect ulrect;
		//!image under the board (right)
		SDL_Surface *underright;
		SDL_Rect urrect;

		//!current map displayed
		Uint16 map_number;
		//!all the maps we have
		vector<string>maps;

		//!where are the holes in the board
		bidiarray<bool> holes; 
		//!all tiles (NULL is none)
		tile* tiles[64];
		//!tile for land (not focused)
		SDL_Surface* tilegrass;
		//!tile for land (focused)
		SDL_Surface* tilegrasshigh;
		//!tile for land at distance 1 from selected tile
		SDL_Surface* h1;
		//!tile for land at distance 2 from selected tile
		SDL_Surface* h2;
		//!tile for selected tile
		SDL_Surface* h0;
		//!tile for hole
		SDL_Surface* tilehole;

		//!array of blobs in game
		blob* blobs[8][8];

		//!vector of surfaces needed for the blobs
		vector<SDL_Surface*>blobs_surfaces;

		//!do we react to external events ?
		bool active;

		//!x coordinate of tile currently under mouse
		Uint8 fx;
		//!y coordinate of tile currently under mouse
		Uint8 fy;

		//!tile with blob selected for move
		tile* selected_tile;

		//!current number of blobs (scores) for each player
		Uint32 scores[4];

		//!names of different players (colors)
		string players_names[4];

		//!number of players in the game
		unsigned int number_of_players;
	public:
		//!build a board (reading the first map in map dir)
		board();
		//!free memory
		~board();
		//!load a new board from text file
		void load_map(string filename);
		//!display on screen
		void display();
		//!we got mouse focus
		void focus();
		//!we lost mouse focus
		void unfocus();
		//!change board to next one in the list
		void next_board();
		//!change board to previous one in the list
		void previous_board();
		//!returns name of next board
		const char* get_next_board();
		//!returns name of previous board
		const char* get_previous_board();
		//!change the map
		void switch_map(string filename);
		//!to execute when the button is clicked
		void click();
		//!add a new blob
		void create_blob(Uint8 x, Uint8 y, Uint16 player);
		//!move a blob
		void move_blob(Uint8 ox, Uint8 oy, Uint8 nx, Uint8 ny);
		//!enable the board to react to mouse events
		void activate();
		//!are we activated ?
		bool is_active();
		//!set the tile currently focused
		void set_focused_tile(Uint8 x, Uint8 y);
		//!change the player owning a blob (and color)
		void change_blob_owner(Uint8 x, Uint8 y, Uint16 player);
		//!we selected a tile, highlight neighbours
		void select_tile(Uint8 x, Uint8 y);
		//!no tile is selected anymore (remove highlight)
		void unselect_tile(Uint8 x, Uint8 y);
		//!remove blobs and prepare for next game
		void reset();
		//!put the initial blobs in place
		void init(unsigned int number_of_players);
		//!copy map holes into given array
		void copy_board(bidiarray<bool> &destination);
};

#endif
