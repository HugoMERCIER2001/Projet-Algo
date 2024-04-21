#ifndef GWIDGET
#define GWIDGET
#include"common.h"

/**a widget
 * is just basically something with a position
 * and that can be displayed 
 * */
class widget {
	protected:
		//! position on screen
		SDL_Rect position;
		//! is the widget hidden yes or no ?
		bool hidden;
	public:
		//! constructor 
		widget ();
		//! destructor 
		virtual ~widget();
		//!display ourselves
		virtual void display();
		//! prevent widget from being displayed
		void hide();
		//! allow widget to be displayed
		void show();
		//! is the widget hidden or not ?
		bool is_hidden();
		//! modify position
		void set_position(SDL_Rect new_position);
		//! modify position so our widget occupies all screen
		void set_fullscreen();
};

#endif
