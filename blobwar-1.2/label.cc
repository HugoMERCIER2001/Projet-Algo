#include "label.h"

/**this constructor creates a label given 
 * \param r : the screen position
 * \param s : the text to display
 * \param f : the font to use
 * */
label::label(SDL_Rect r, string s, font* fo) {
	position = r;
	f = fo;
	surface = NULL;
	set_text(s);
}

void label::set_text(string s) {

	text = s;

	if (surface != NULL) {
		SDL_FreeSurface(surface);
	}

	surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
			position.w,
			position.h,
			game->screen->format->BitsPerPixel,
			game->screen->format->Rmask,
			game->screen->format->Gmask,
			game->screen->format->Bmask,
			game->screen->format->Amask);
	

	//fill background
	if ((position.w > 4)&&(position.h > 4)) {
		SDL_Rect r = position;
		r.x = 2;
		r.w -= 4;
		r.y = 2;
		r.h -= 4;
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 106, 91, 255));
		SDL_FillRect(surface, &r, SDL_MapRGB(surface->format, 50, 40, 143));
	} else {
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 50, 40, 143));
	}
	
	//create text if needed
	if(text.size() != 0) {
		SDL_Surface *text_surface = f->render_text(text);
	
		//blit text on background
		SDL_Rect dest;
		dest.x = (position.w - text_surface->w)/2;
		dest.y = (position.h - text_surface->h)/2;
		dest.w = text_surface->w;
		dest.h = text_surface->h;
		dest.x = (dest.x < 0)?0:dest.x;
		dest.y = (dest.y < 0)?0:dest.y;
	
		SDL_BlitSurface(text_surface, NULL, surface, &dest);
	
		SDL_FreeSurface(text_surface);
	}

	game->display2update = 1;
}

label::~label() {
	SDL_FreeSurface(surface);
}

void label::display() {
	SDL_BlitSurface(surface, NULL, game->screen, &position);
}

