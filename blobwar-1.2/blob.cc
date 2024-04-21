#include"blob.h"
#include"blobwar.h"

blob::blob(Uint8 x, Uint8 y, Uint16 p, SDL_Surface *s) {
#ifdef DEBUG
	cout<<"creating a new blob at "<<(Uint32) x<<","<<(Uint32) y << "(p: "<<(Uint32)p<<")"<<endl;
#endif
	//set the player owning this blob
	player = p;
	//set surface
	surface = s;
	//compute the position on screen
	position.w = s->w;
	position.h = s->h;
	move(x,y);

	//7frames in the animation
	r.w = s->w / 7;
	r.h = s->h;
	r.y = 0;
	anim_start = game->frame;
}

blob::~blob() { }

void blob::display() {
	//compute image to display
#ifdef ANIMATION
	r.x = 40 * (((game->frame / ANIMATIONSPEED) - anim_start) % 7);
#else
	r.x = 0;
#endif
	SDL_BlitSurface(surface, &r, game->screen, &position);
}

void blob::move(Uint8 x, Uint8 y) {
	position.x = 70 + (x+y) * (84/2) + (84- (surface->w)/7)/2;
	position.y = 290 + (y-x) * (40/2);
}

void blob::change_owner(Uint16 p, SDL_Surface* s) {
	player = p;
	surface = s;
}

Uint16 blob::get_owner() {
	return player;
}
