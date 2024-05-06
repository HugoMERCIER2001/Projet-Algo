#include "strategy.h"
#include <cmath>
#include <vector>
#include <algorithm>


void Strategy::applyMove (const movement& mv) {
    //On part du principe que le mouvement est valide. COMPLEXITE ASSEZ FAIBLE PAS BESOIN D'AMELIORER.
    //On réalise le mouvement peut importe le player.
        int distance = std::sqrt((mv.nx - mv.ox) * (mv.nx - mv.ox) + (mv.ny - mv.oy) * (mv.ny - mv.oy));
        std::cout << "distance = " << distance << std::endl;
        if(distance >= 2){ //Si on fait un mouvement de plus de 2 cases, on retire le blob sur l'ancienne case et on le met sur la nouvelle case.
            int old_value = (int) _blobs.get(mv.ox, mv.oy);
            std::cout << "old value vaut : " << old_value << std::endl; 
            _blobs.set(mv.ox, mv.oy, -1);
            for(int i = std::max(0, mv.nx - 1); i <= std::min(7, mv.nx + 1); i++){
                for(int j = std::max(0, mv.ny - 1); j <= std::min(7, mv.ny + 1); j++){
                    if(_blobs.get(i, j) != -1){
                        _blobs.set(i, j, old_value);
                    }
                }
            }
            //Changement des blobs autour de la nouvelle position.

        }

        if(distance < 2){
            int old_value = (int) _blobs.get(mv.ox, mv.oy);
            std::cout << "old value vaut : " << old_value << std::endl;
            _blobs.set(mv.nx, mv.ny, old_value);
            for(int i = std::max(0, mv.nx - 1); i <= std::min(7, mv.nx + 1); i++){
                for(int j = std::max(0, mv.ny - 1); j <= std::min(7, mv.ny + 1); j++){
                    if(_blobs.get(i, j) != -1){
                        _blobs.set(i, j, old_value);
                    }
                }
            }
        }
}

Sint32 Strategy::estimateCurrentScore () const {
    //On cherche à évaluer le score actuel du joueur à qui c'est le tour. AMELIORABLE AVEC UN FOR PARRALELLE.
    int joueur = (int) _current_player;
    int score = 0;
    for (Uint8 i = 0; i < 8; i++){ //On parcourt toutes les abcisses.
        for(Uint8 j = 0; j < 8; j++){//On parcourt toutes les ordonnées.
            if(_blobs.get(i, j) == joueur){
                score += 1;
            }
        }
    }
    Sint32 valeurSint32 = static_cast<Sint32>(score); //On transtype le score qui est un int en type Sint32.
    return valeurSint32;
}

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves) const {
    //On commence par faire tous les mouvements de une case.
    movement mv = movement(0, 0, 0, 0);
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {   
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                    for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                    std::cout << "salut " << mv.nx << mv.ny << _holes.get(mv.nx, mv.ny) << std::endl;
                        if (_holes.get(mv.nx, mv.ny)){
                            continue;
                        };
                        if (_blobs.get(mv.nx, mv.ny) == -1){
                            valid_moves.push_back(mv);
                        }
                    }
                    }   
            }
        }
    }
    return valid_moves;
}

void Strategy::computeBestMove (int type_strategy) {
    // Le paramètre type strategy sert à definir le type de stratégy que nous allons utiliser : si type_strategy == 0 : algo glouton.
    std::vector<movement> valid_moves;
    movement mv = movement(0, 0, 0, 0);
    Sint32 score = estimateCurrentScore();
    if(type_strategy == 0){
        //ALGO GLOUTON.
        //Reviens à regarder tous les mouvements possible, pour chaque mouvement estimer le score si on réalise le mouvement, puis on sauvegarde celui avec le meilleur score.
        vector<movement>& liste_mv = computeValidMoves (valid_moves);
        for(const movement& mv_boucle : liste_mv){
            std::cout << "la taille de la liste_mv est : " << liste_mv.size() << std::endl;
            Strategy new_strat = Strategy(*this);
            new_strat.applyMove(mv_boucle);
            Sint32 score_local = new_strat.estimateCurrentScore();
            if((int) score_local > (int) score){
                std::cout << "la valeur du score calculée vaut : " << score_local << std::endl;
                mv = mv_boucle;
                score = score_local;
            }
            //Pas besoin de detruire la copie faite dans la boucle de strategy, se fait tt seul avec le destructor.

        }
    }
     _saveBestMove(mv);
     return;
}

