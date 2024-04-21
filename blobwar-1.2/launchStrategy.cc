#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "strategy.h"
#include "shmem.h"

//! Display a move on console
void saveBestMoveToConsole(movement& m)
{
	cout<<"SAVE MOVE: "<<(Uint32)m.ox<<","<<(Uint32)m.oy<<" to "<<(Uint32)m.nx<<","<<(Uint32)m.ny<<endl;
}

//! Save a move to the shared memory with blobwar
void saveBestMoveToShmem(movement& m)
{
#ifdef DEBUG
	saveBestMoveToConsole(m);
#endif
	shmem_set(m);
}

/** Main of launchStrategy
 * This executable is called automatically by blobwar to compute IA moves.
 * The args should be:
 * - blobs (serialized)
 * - holes (serialized)
 * - current player (an int)
 */
int main(int argc, char **argv) {
	
#ifdef DEBUG
	cout << "Starting launchStrategy" << endl;
#endif
	if(argc != 4) {
		printf("Usage: ./launchStrategy blobs holes current_player\n");
		printf("	blobs is a serialized bidiarray<Sint16> containing the blobs\n");
		printf("	holes is a serialized bidiarray<bool> containing the holes\n");
		printf("	current_player is an int indicating which player should play\n");
		return 1;
	}
	int i = 1;
	
	bidiarray<Sint16> blobs = bidiarray<Sint16>::deserialize(argv[i++]);
	//blobs.display();
	bidiarray<bool> holes = bidiarray<bool>::deserialize(argv[i++]);
	//holes.display();
	int cplayer = atoi(argv[i++]);
	//std::cout << "player: "<<cplayer<<std::endl;
	void (*func)(movement&) = saveBestMoveToShmem;
	
	shmem_init();
	func = saveBestMoveToShmem;
	
	Strategy strategy(blobs, holes, cplayer, func);
	strategy.computeBestMove();
	
	return 0;
}




