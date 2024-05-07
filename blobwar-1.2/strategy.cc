#include "strategy.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <random> // Pour la génération de nombres aléatoires
#include <ctime> // Pour initialiser le générateur de nombres aléatoires
#include <bits/sigcontext.h>


int Strategy::getCurrentPlayer();
    return (int) _current_player;

void Strategy::setCurrentPlayer((int) value){
    Uint16 valeur_Uint16 = static_cast<Uint16>(value);
    _current_player = valeur_Uint16;
}

void Strategy::applyMove (const movement& mv) {
    //On part du principe que le mouvement est valide. COMPLEXITE ASSEZ FAIBLE PAS BESOIN D'AMELIORER.
    //On réalise le mouvement peut importe le player.
        int distance = std::sqrt((mv.nx - mv.ox) * (mv.nx - mv.ox) + (mv.ny - mv.oy) * (mv.ny - mv.oy));
        if(distance >= 2){ //Si on fait un mouvement de plus de 2 cases, on retire le blob sur l'ancienne case et on le met sur la nouvelle case.
            int old_value = (int) _blobs.get(mv.ox, mv.oy);
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
    //On peut améliorer ce code (chose que l'on va faire) en définissant le score pas comme une valeure absolue mais comme la différence du score du joueur avec celle de son adversaire.
    // Cependant cette méthode fonctionne seulement si les joueurs jouent en 1v1 et non pas en 1v1v1v1. C'est pourquoi nous allons faire la fonction à part estimateCurrentScoreDuel().
    int joueur = getCurrentPlayer();
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

Sint32 Strategy::estimateCurrentScoreDuel () const {
    //Algo qui calcul la valeur du score du joueur comme la différence des score absolu des joueurs (permet d'optimiser certains déplacements qui ont le même score absolu mais fait perdre du score à l'adversaire).
    int joueur = getCurrentPlayer();
    int score = 0;
    for (Uint8 i = 0; i < 8; i++){ //On parcourt toutes les abcisses.
        for(Uint8 j = 0; j < 8; j++){//On parcourt toutes les ordonnées.
            if(_blobs.get(i, j) == joueur){
                score += 1;
            }
            if(_blobs.get(i,j) == 1 - joueur){//On retire le score absolu du joueur adversaire au nôtre.
                score -= 1;
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

std::tuple<movement, int> Strategy::min_max(int profondeur, Strategy strat){
    if(profondeur == 0){
        movement a = movement(0, 0, 0, 0);
        int b = (int) strat.estimateCurrentScoreDuel();
        return std::make_tuple(a,b);
    }
    else{
        std::vector<movement> valid_moves;
        vector<movement>& liste_mv = strat.computeValidMoves(valid_moves);
        if(liste_mv.size() == 0){
            std::cout << "Il n'y a aucuns mouvements faisable" << std::endl;
            movement a = movement(0, 0, 0, 0);
            int b = (int) strat.estimateCurrentScoreDuel();
            return std::make_tuple(a , b);
        }
        int valeur_max = -2147483648;//On initialise la valeure max à la plus petite valeure représentrable par un int de 32bits. Cela revient à l'initialiser à la première valeure puisque elle sera forcément supérieure.
        movement best_mv = movement(0, 0, 0, 0);
        for(const movement& mv_boucle : liste_mv){
            Strategy new_strat = Strategy(strat);
            new_strat.applyMove(mv_boucle);
            new_strat._current_player = 1 - strat._current_player;
            auto result = min_max(profondeur - 1, new_strat);
            int valeur = - std::get<1>(result);
            if(valeur > valeur_max){
                valeur_max = valeur;
                best_mv = mv_boucle;
            }
        return std::make_tuple(best_mv, valeur_max);
    }
}
}

void Strategy::computeBestMove (int type_strategy) {
    // Le paramètre type strategy sert à definir le type de stratégy que nous allons utiliser : si type_strategy == 0 : algo glouton.
    std::vector<movement> valid_moves;
    std::vector<movement> best_moves;
    Sint32 score = estimateCurrentScoreDuel();
    movement mv = movement(0, 0, 0, 0);
    if(type_strategy == 0){
        //ALGO GLOUTON.
        //Reviens à regarder tous les mouvements possible, pour chaque mouvement estimer le score si on réalise le mouvement, puis on sauvegarde celui avec le meilleur score.
        //Une manière simple de rendre nôtre algorithme moin prévisible est que lorsque plusieurs mouvements sont égaux en terme d'amélioration du score, on ne prend pas le premier mais on en prend un aléatoirement.
        vector<movement>& liste_mv = computeValidMoves (valid_moves);
        if(liste_mv.size() == 0){
            std::cout << "Il n'y a aucuns mouvements faisable" << std::endl;
            return;
        }
        for(const movement& mv_boucle : liste_mv){
            std::cout << "la taille de la liste_mv est : " << liste_mv.size() << std::endl;
            Strategy new_strat = Strategy(*this);
            new_strat.applyMove(mv_boucle);
            Sint32 score_local = new_strat.estimateCurrentScoreDuel();
            if((int) score_local > (int) score){
                std::cout << "la valeur du score calculée vaut : " << score_local << std::endl;
                best_moves.clear();
                score = score_local;
                best_moves.push_back(mv_boucle);
            }
            if((int) score_local == score){
                best_moves.push_back(mv_boucle);
            }

            //Pas besoin de detruire la copie faite dans la boucle de strategy, se fait tt seul avec le destructor.
            // Initialiser le générateur de nombres aléatoires
            std::mt19937 rng(time(0));
            std::uniform_int_distribution<int> distribution(0, best_moves.size() - 1);
            // Choisir un index aléatoire dans le vecteur
            int index = distribution(rng);
            // Utiliser cet index pour accéder au mouvement aléatoire
            movement mv = best_moves[index];
            _saveBestMove(mv);
        }
    }
    if(type_strategy == 1){
        auto result = min_max(1, *this);
        movement mv = std::get<0>(result);

    }
     _saveBestMove(mv);
     return;
}

