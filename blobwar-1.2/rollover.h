#ifndef GROLLOVER
#define GROLLOVER

class rollover;

#include"widget.h"

/**a rollover
 * is a widget which will react to the mouse position
 * */
class rollover : public widget {
	public:
		//! constructor 
		rollover ();
		//! destructor 
		virtual ~rollover();
		//! what happens when clicked
		virtual void click();
		//! what happens when mouse is above rollover
		virtual void focus();
		//! what happens when mouse is not above rollover
		virtual void unfocus();
		//! are these coordinates inside the rollover ?
		bool is_in(Sint16 x, Sint16 y);
};


#endif
