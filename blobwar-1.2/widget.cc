#include "widget.h"
#include "blobwar.h"

widget::widget(){
	game->register_widget(this);
	hidden = false;
}

widget::~widget(){
	game->unregister_widget(this);
}

void widget::display(){}

void widget::hide(){
	hidden = true;
}

void widget::show(){
	hidden = false;
}

void widget::set_position(SDL_Rect new_position) {
	position = new_position;
}

void widget::set_fullscreen() {
	position.x = 0;
	position.y = 0;
	position.w = game->screen->w;
	position.h = game->screen->h;
}

bool widget::is_hidden() {
	return hidden;
}
