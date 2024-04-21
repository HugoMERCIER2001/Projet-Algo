#ifndef GBLOB
#define GBLOB

class blob;

#include "widget.h"

class blob : public widget {
	private:
		//!image
		SDL_Surface *surface;
		//!player owning this blob
		Uint16 player;
		//!we need a counter for the animation
		Uint32 anim_start;
		//!this rect is used for animating the image
		SDL_Rect r;
	public:
		//!build a blob 
		blob(Uint8 x, Uint8 y, Uint16 p, SDL_Surface *s);
		//!destructor (does nothing)
		~blob();
		//!display on screen
		void display();
		//!move the blob
		void move(Uint8 x, Uint8 y);
		//!change the player owning this blob
		void change_owner(Uint16 p, SDL_Surface* s);
		//!return current owner
		Uint16 get_owner();
};

#endif
