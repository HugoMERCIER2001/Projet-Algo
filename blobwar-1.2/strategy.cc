#include "strategy.h"
#include <cmath>


void Strategy::applyMove (const movement& mv) {
    //On part du principe que le mouvement est valide.
    //On réalise le mouvement peut importe le player.
        int distance = std::sqrt((mv.nx - mv.ox) * (mv.nx - mv.ox) + (mv.nx - mv.ox) * (mv.nx - mv.ox));
        std::cout << "distance = " << distance << std::endl;
        if(distance >= 2){
            int old_value = (int) _blobs.get(mv.ox, mv.oy);
            std::cout << "old value vaut : " << old_value << std::endl;
        }
}

Sint32 Strategy::estimateCurrentScore () const {
        // To be completed...
    return 0;
}

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves) const {
        // To be completed...
    return valid_moves;
}

void Strategy::computeBestMove () {
    // To be improved...

    //The following code finds a valid move.
    movement mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) goto end_choice;
                    }
                }
            }
        }
    }

end_choice:
     _saveBestMove(mv);
     return;
}

