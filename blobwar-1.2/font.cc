#include"font.h"
#include"blobwar.h"
#include <SDL_ttf.h>

/**constructs the different letters images.
 * \param font : font FILE positioned on current char
 * \param c1 : color in 16bits (game depth)
 * \param c2 : color in 16bits (game depth) 
 * \param y : height of char (at beginning of FILE) */

character::character(TTF_Font* font, SDL_Color clrFg, SDL_Color clrBg, char letter) {
         //j'ai toujours dit que programmer ça forgeait le charactère
         //bon j'ai honte elle est mauvaise celle-là
        char text[2];
	text[0]=(letter+32);
	text[1]='\0';
	TTF_SizeText(font, text, &x, &y);
	img = TTF_RenderText_Solid(font, text, clrFg);
}

/**frees surface, blah blah blah*/
character::~character() {
	SDL_FreeSurface(img);
}

/**constructs all chars*/
font::font(string filename, SDL_Color foreground, SDL_Color background, int fontsize) {
	//first : correct file name
	string realname = game->rename(filename);
	
	// Load the Font
	TTF_Font *f = TTF_OpenFont(realname.c_str(),fontsize);
#ifdef DEBUG
	cout<<"loading font: "<<realname.c_str()<<endl;
#endif

	//now read all chars
	for (Uint32 i = 0; i < 96 ; i++)
	{
		chars[i] = new character(f, foreground, background,(char) i);
	}
	

	TTF_CloseFont(f);
}

/**frees all mem*/
font::~font() {
	Uint32 i;
	for(i=0 ; i < 96; i++)
		delete chars[i];
}

/**this method renders the text using the color allocated
 * in the font*/
SDL_Surface *font::render_text(string text) {
	SDL_Surface* final_img = NULL;
	Uint32 final_x = 0;
	y = chars[1]->y;
	//first go through all letters to calculate size
	for(Uint32 i = 0; i < text.size() ; i++) {
		final_x += chars[text[i]-32]->x;
	}
	//now create rendered string and fill it
	final_img = SDL_CreateRGBSurface(SDL_SWSURFACE, final_x, y, game->screen->format->BitsPerPixel, game->screen->format->Rmask, game->screen->format->Gmask, game->screen->format->Bmask, game->screen->format->Amask);
	SDL_Rect src, dest;
	src.x = 0;
	src.y = 0;
	src.h = y;
	dest.x = 0;
	dest.y = 0;
	dest.w = 0;
	dest.h = y;
	
	for(Uint32 i = 0 ; i < text.size() ; i++) {
		src.w = chars[text[i]-32]->x;
		dest.w = src.w;

		SDL_BlitSurface(chars[text[i]-32]->img, &src, final_img, &dest);
		dest.x += chars[text[i]-32]->x;
	}

	//now set colorkey as the usual color
	// was 0,254,0
	SDL_SetColorKey(final_img, SDL_SRCCOLORKEY, SDL_MapRGB(final_img->format, 0, 0, 0));

	return final_img;
};

