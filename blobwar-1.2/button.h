#ifndef GBUTTON
#define GBUTTON


class button;

#include "rollover.h"
#include "blobwar.h"


class button : public rollover {
	private:
		//!image when focused
		SDL_Surface *focused;
		//!image when not focused
		SDL_Surface *unfocused;
		//!pointer on current surface
		SDL_Surface *surface;

		//!number of the function to call in case button is clicked
		Uint32 callbacknumber;

	public:
		//!build a button with text
		button(SDL_Rect pos, string text);
		//!build a button from an image file
		button(const char* filename, SDL_Rect pos);
		//!free memory
		~button();
		//!display on screen
		void display();
		//!we got mouse focus
		void focus();
		//!we lost mouse focus
		void unfocus();
		//!set callback number
		void setcallback(Uint32 number);
		//!to execute when the button is clicked
		void click();
};

#endif
