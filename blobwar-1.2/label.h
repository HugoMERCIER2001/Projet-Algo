#ifndef GLABEL
#define GLABEL


class label;

#include "widget.h"
#include "blobwar.h"


class label : public widget {
	private:
		//!real image
		SDL_Surface *surface;
		//!string displayed
		string text;
		//!font used
		font* f;
	public:
		//!build a label from string
		label(SDL_Rect r, string s, font* f);
		//!free memory
		~label();
		//!display on screen
		void display();
		//!change the text displayed
		void set_text(string s);
};

#endif
