
#include"common.h"
#include"rollover.h"

/**mouse class, for handling mouse graphics and display*/
class mouse {
	private:
		//!current mouse image
		SDL_Surface *image;
		//!x coordinate of pointer pixel in image
		Uint32 xdec;
		//!y coordinate of pointer pixel in image
		Uint32 ydec;
		//!display mouse or not ?
		SDL_bool active;
		//!list of all widgets reactive to mouse position
		list<rollover*> rollovers;
	public:
		//!constructor
		mouse();
		//!destructor
		~mouse();
		//!handles moving and displaying
		void move(SDL_Event *event);
		//!handles moving and displaying
		void move(Sint32 x, Sint32 y);
		//!doesn't move but redraws mouse
		void display();
		//!activate mouse handling
		void enable();
		//!deactivate mouse handling
		void disable();
		//!add a new rollover to monitor to our list
		void register_rollover(rollover* r);
		//!remove rollover to monitor from our list
		void unregister_rollover(rollover* r);
		//!update rollovers according to mouse position
		void recompute_focuses();
		//!click 
		void click();
		//!get current mouse x coordinate
		Sint16 get_x();
		//!get current mouse y coordinate
		Sint16 get_y();
		//!last mouse coordinates
		SDL_Rect mousepos;
};
