#ifndef GFONT
#define GFONT
#include"common.h"
#include <SDL_ttf.h>

/**this class handles each chars of a font individually,
 * necessary since height differs from char to char*/
class character {
	public:
		//! image builded 
		SDL_Surface *img;
		//! width of char
		int x;
		//! height of char
		int y;
		//!constructor from font FILE positioned correctly
		character(TTF_Font* font, SDL_Color clrFg, SDL_Color clrBg, char i);
		//!destructor
		~character();
};

class font {
	private:
		//!height of all chars
		int y;
		//!pointers on all chars
		character* chars[96];
	public:
		//!constructor (constructs also all characters)
		font(string filename, SDL_Color foreground, SDL_Color background, int fontsize);
		//!destructor
		~font();
		//!main use of this class
		SDL_Surface *render_text(string text);
};

#endif
