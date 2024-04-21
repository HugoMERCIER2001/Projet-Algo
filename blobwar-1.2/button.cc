#include "button.h"

button::button(SDL_Rect pos, string text) {

	callbacknumber = 0;
	position = pos;

	//render text
	SDL_Surface *text_surface = game->bigfont->render_text(text);
	//allocate surfaces
	
	focused = SDL_CreateRGBSurface(SDL_SWSURFACE,
			pos.w,
			pos.h,
			game->screen->format->BitsPerPixel,
			game->screen->format->Rmask,
			game->screen->format->Gmask,
			game->screen->format->Bmask,
			game->screen->format->Amask);
	
	unfocused = SDL_CreateRGBSurface(SDL_SWSURFACE,
			pos.w,
			pos.h,
			game->screen->format->BitsPerPixel,
			game->screen->format->Rmask,
			game->screen->format->Gmask,
			game->screen->format->Bmask,
			game->screen->format->Amask);

	//fill the different backgrounds
	if ((pos.w > 4)&&(pos.h > 4)) {
		SDL_Rect r = pos;
		r.x = 2;
		r.w -= 4;
		r.y = 2;
		r.h -= 4;
		SDL_FillRect(focused, NULL, SDL_MapRGB(focused->format, 106, 91, 255));
		SDL_FillRect(unfocused, NULL, SDL_MapRGB(focused->format, 106, 91, 255));
		SDL_FillRect(unfocused, &r, SDL_MapRGB(focused->format, 50, 40, 143));
		SDL_FillRect(focused, &r, SDL_MapRGB(focused->format, 18, 146, 255));
	} else {
		SDL_FillRect(unfocused, NULL, SDL_MapRGB(focused->format, 50, 40, 143));
		SDL_FillRect(focused, NULL, SDL_MapRGB(focused->format, 18, 146, 255));
	}

	//blit text on background
	SDL_Rect dest;
	dest.x = (position.w - text_surface->w)/2;
	dest.y = (position.h - text_surface->h)/2;
	dest.w = text_surface->w;
	dest.h = text_surface->h;
	dest.x = (dest.x < 0)?0:dest.x;
	dest.y = (dest.y < 0)?0:dest.y;


	SDL_BlitSurface(text_surface, NULL, focused, &dest);
	SDL_BlitSurface(text_surface, NULL, unfocused, &dest);

	SDL_FreeSurface(text_surface);
	
	if (is_in(game->mousepointer->get_x(), game->mousepointer->get_y())) {
		surface = focused;
	} else surface = unfocused;
}

button::~button() {
	SDL_FreeSurface(focused);
	SDL_FreeSurface(unfocused);
}

void button::display() {
	SDL_BlitSurface(surface, NULL, game->screen, &position);
}

void button::focus() {
	surface = focused;
}

void button::unfocus() {
	surface = unfocused;
}

void button::setcallback(Uint32 number) {
	callbacknumber = number;
}

void button::click() {
	if (callbacknumber != 0) game->execute(callbacknumber);
}

button::button(const char* filename, SDL_Rect pos) {

	callbacknumber = 0;
	position = pos;

	//load image
	SDL_Surface *img_surface = game->alpha_image_load(filename);
	//allocate surfaces
	
	focused = SDL_CreateRGBSurface(SDL_SWSURFACE,
			pos.w,
			pos.h,
			game->screen->format->BitsPerPixel,
			game->screen->format->Rmask,
			game->screen->format->Gmask,
			game->screen->format->Bmask,
			game->screen->format->Amask);
	
	unfocused = SDL_CreateRGBSurface(SDL_SWSURFACE,
			pos.w,
			pos.h,
			game->screen->format->BitsPerPixel,
			game->screen->format->Rmask,
			game->screen->format->Gmask,
			game->screen->format->Bmask,
			game->screen->format->Amask);

	//fill the different backgrounds
	if ((pos.w > 4)&&(pos.h > 4)) {
		SDL_Rect r = pos;
		r.x = 2;
		r.w -= 4;
		r.y = 2;
		r.h -= 4;
		SDL_FillRect(focused, NULL, SDL_MapRGB(focused->format, 106, 91, 255));
		SDL_FillRect(unfocused, NULL, SDL_MapRGB(focused->format, 106, 91, 255));
		SDL_FillRect(unfocused, &r, SDL_MapRGB(focused->format, 50, 40, 143));
		SDL_FillRect(focused, &r, SDL_MapRGB(focused->format, 18, 146, 255));
	} else {
		SDL_FillRect(unfocused, NULL, SDL_MapRGB(focused->format, 50, 40, 143));
		SDL_FillRect(focused, NULL, SDL_MapRGB(focused->format, 18, 146, 255));
	}

	//blit image on background
	SDL_Rect dest;
	dest.x = (position.w - img_surface->w)/2;
	dest.y = (position.h - img_surface->h)/2;
	dest.w = img_surface->w;
	dest.h = img_surface->h;
	dest.x = (dest.x < 0)?0:dest.x;
	dest.y = (dest.y < 0)?0:dest.y;


	SDL_BlitSurface(img_surface, NULL, focused, &dest);
	SDL_BlitSurface(img_surface, NULL, unfocused, &dest);

	SDL_FreeSurface(img_surface);
	
	if (is_in(game->mousepointer->get_x(), game->mousepointer->get_y())) {
		surface = focused;
	} else surface = unfocused;
}



