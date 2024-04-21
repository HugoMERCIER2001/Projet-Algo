#ifndef GIMAGE
#define GIMAGE


class image;

#include "widget.h"
#include "blobwar.h"


class image : public widget {
	private:
		//!real image
		SDL_Surface *surface;
	public:
		//!build a black image
		image(Uint32 w, Uint32 h);
		//!build image from file
		image(const char* filename);
		//!build image from file (activate or not transparency)
		image(const char* filename, bool transparent);
		//!build image from SDL_Surface*
		image(SDL_Surface* existing_surface);
		//!free memory
		~image();
		//!display on screen
		void display();
		//!convert name
		static char* rename(char* name);
		//!get current surface
		SDL_Surface* get_surface();
		//!draw a box on our image 
		void draw_box(SDL_Rect box, SDL_Color color);
};

#endif
