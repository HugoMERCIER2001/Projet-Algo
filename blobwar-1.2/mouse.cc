#include"blobwar.h"

mouse::mouse()
{
	//first load all different mouse images
	image = game->alpha_image_load("mouse.png");

	active = SDL_TRUE;
	xdec = 0;
	ydec = 0;
	SDL_WarpMouse(0, 0);
	mousepos.x = 0;
	mousepos.y = 0;
	mousepos.w = image->w;
	mousepos.h = image->h;

#ifdef SOFT_MOUSE
	SDL_ShowCursor(0);
#else
	SDL_ShowCursor(1);
#endif
}

mouse::~mouse()
{
#ifdef DEBUG
	cout << "Shutting down mouse" << endl;
#endif
	SDL_FreeSurface(image);
}


/**this moves the mouse, draw new mouse, updates coordinates*/
void mouse::move(SDL_Event * event)
{
	//update mouse coordinates
	mousepos.x = event->motion.x;
	mousepos.y = event->motion.y;
#ifdef SOFT_MOUSE
	//display mouse
	game->display2update = 1;
#endif
	recompute_focuses();
}

void mouse::move(Sint32 x, Sint32 y) {
	//update mouse coordinates
	mousepos.x = x;
	mousepos.y = y;
#ifdef SOFT_MOUSE
	//display mouse
	game->display2update = 1;
#endif
	recompute_focuses();
}

/**this does'nt move the mouse but displays it*/
void mouse::display() {
#ifdef SOFT_MOUSE
	SDL_BlitSurface(image, NULL, game->screen, &mousepos);
#endif
}

Sint16 mouse::get_x() {
	return mousepos.x;
}

Sint16 mouse::get_y() {
	return mousepos.y;
}

void mouse::register_rollover(rollover* r) {
		rollovers.push_back(r);
}

void mouse::unregister_rollover(rollover* r) {
		rollovers.remove(r);
}

/**this method iterates on the list of all widgets affected by mouse position (rollovers)
 * when the mouse is above a rollover it calls rollover::focus and rollover::unfocus else */
void mouse::recompute_focuses() {
	for(list<rollover*>::iterator it = rollovers.begin() ; it != rollovers.end() ; it++) {
		if ((*it)->is_in(mousepos.x, mousepos.y)) {
			(*it)->focus() ;
			game->display2update = 1;
		} else (*it)->unfocus() ;
	}
}

void mouse::click() {
	for(list<rollover*>::reverse_iterator it = rollovers.rbegin() ; it != rollovers.rend() ; it++) {
		if ((!(*it)->is_hidden())&&((*it)->is_in(mousepos.x, mousepos.y))) {
			//rollover is not hidden and mouse is over it so click 
			(*it)->click();
			return;
		}
	}
}
