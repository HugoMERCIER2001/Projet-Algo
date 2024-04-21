#include "image.h"

/**builds a black image (usefull for backgrounds)
 * \par Uint32 w : width
 * \par Uint32 h : height */
image::image(Uint32 w, Uint32 h) {
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
				 w,
				 h,
				 game->screen->format->BitsPerPixel,
				 game->screen->format->Rmask,
				 game->screen->format->Gmask,
				 game->screen->format->Bmask,
				 game->screen->format->Amask);
	
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
}


/**load an image from a local filename
 * image is considered transparent
 * \par char* filename : the name of the image */
image::image(const char* filename) {
	surface = game->alpha_image_load(filename);
}

/**load an image from a local filename
 * \par char* filename : the name of the image 
 * \par bool transparent : does the image contain an alpha channel ?*/
image::image(const char* filename, bool transparent) {
	if (transparent) 
		surface = game->alpha_image_load(filename);
	else 
		surface = game->image_load(filename);
}

/**build the image using an already existant surface
 * \par SDL_Surface* existing surface : the existing surface
 * NO COPY, don't free the existing surface afterwards, don't register it twice to the engine
 * */
image::image(SDL_Surface* existing_surface) {
	surface = existing_surface;
}

/**free an image
 */
image::~image() {
	delete surface;
}


/**display image on screen at right position*/
void image::display() {
	SDL_BlitSurface(surface, NULL, game->screen, &position);
}

SDL_Surface* image::get_surface() {
	return surface;
}


/**draw a 1 pixel thin box on image
 * \par SDL_Rect box : box coordinates
 * \par SDL_Color color : line color
 * */
void image::draw_box(SDL_Rect box, SDL_Color color) {

	//direct pixels acces, lock image
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	switch(surface->format->BitsPerPixel) {
		case 16:
			{
			Uint16 col = SDL_MapRGB(surface->format, color.r, color.g, color.b);
			Uint16 *pixels = (Uint16*) surface->pixels;

			//draw horizontal lines
			for (Sint32 i = box.x; i < box.x + box.w; i++) {
				pixels[surface->pitch * box.y / 2 + i] = col;
				pixels[surface->pitch * (box.y + box.h - 1) / 2 + i] = col;
			}
			//draw vertical lines
			for (Sint32 j = box.y; j < box.y + box.h; j++) {
				pixels[surface->pitch * j / 2 + box.x] = col;
				pixels[surface->pitch * j / 2 + box.x + box.w - 1] = col;
			}
			}
			break;
		case 32:
			{
			Uint32 col = SDL_MapRGB(surface->format, color.r, color.g, color.b);
			Uint32 *pixels = (Uint32*) surface->pixels;

			//draw horizontal lines
			for (Sint32 i = box.x; i < box.x + box.w; i++) {
				pixels[surface->pitch * box.y / 4 + i] = col;
				pixels[surface->pitch * (box.y + box.h - 1) / 4 + i] = col;
			}
			//draw vertical lines
			for (Sint32 j = box.y; j < box.y + box.h; j++) {
				pixels[surface->pitch * j / 4 + box.x] = col;
				pixels[surface->pitch * j / 4 + box.x + box.w - 1] = col;
			}
			}
			break;
		default:
		cerr<<"warning : direct pixel access not implement for this bpp"<<endl;
	}

	//finished, unlock
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
	
	return;
}
