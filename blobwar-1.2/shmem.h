#ifndef __SHMEM_H
#define __SHMEM_H

#include "common.h"
#include "move.h"

/**
 * Share memory between 2 process (blobwar and launchStrategy).
 */

//! should be called first
//! if init_move is true, the move will be set to 0,0,0,0
void shmem_init(bool init_move=false);

//! return the last save move
movement shmem_get();

//! save a new move
void shmem_set(movement& m);


#endif
