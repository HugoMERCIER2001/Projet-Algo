#include"test_display.h"

void (*_saveBestMove)(movement&){

};

int main(){
    bidiarray<int> bidi = bidiarray<int>();
    bidi.set(0, 0, 1);
    bidi.display();
    std::cout << "---------" << std::endl;
    Uint8 oldx = 0;
    Uint8 oldy = 0;
    Uint8 newx = 2;
    Uint8 newy = 1;
    movement mv = movement(oldx, oldy, newx, newy);
    bidiarray<bool> holes = bidiarray<bool>();
    holes.display();
    Uint16 current_player = 1;
    return 0;
}