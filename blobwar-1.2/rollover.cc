#include "rollover.h"
#include "blobwar.h"

rollover::rollover(){
	game->mousepointer->register_rollover(this);
}

rollover::~rollover(){
	cout<<"deleting rollover"<<endl;
	game->mousepointer->unregister_rollover(this);
}

//TODO void rollover::display(){}
void rollover::click() {}

void rollover::focus() {}
void rollover::unfocus() {}

bool rollover::is_in(Sint16 x, Sint16 y) {
	if ((x>=position.x)&&(x<position.x + position.w)&&(y>=position.y)&&(y<position.y + position.h)) {
		return true;
	}
	return false;
}
