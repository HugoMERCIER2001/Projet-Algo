#ifndef __MOVE_H
#define __MOVE_H
#include<SDL.h>


/** Move class
 */
struct movement
{
    movement(){}
    
    movement(const movement & mv)
            :ox(mv.ox), oy(mv.oy), nx(mv.nx), ny(mv.ny) {}
    movement(const Uint8 oldx, const Uint8 oldy, const Uint8 newx, const Uint8 newy)
            : ox(oldx),oy(oldy),nx(newx),ny(newy){}
    
    movement& operator=(const movement& mv)
        {ox=mv.ox;oy=mv.oy;nx=mv.nx;ny=mv.ny; return *this;}
        
    
    Uint8 ox;
    Uint8 oy;
    Uint8 nx;
    Uint8 ny;
};

#endif
