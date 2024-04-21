
#include "bidiarray.h"


template<> void bidiarray<bool>::display() {
	for(Uint8 i = 0 ; i < 8 ; i++) {
		for(Uint8 j = 0 ; j < 8 ; j++) {
			if (array[i][j]) { 
				std::cout<<"O ";
			}else
				std::cout<<"X ";
		}
		std::cout<<std::endl;
	}
}

template<> void bidiarray<Sint16>::display() {
	for(Uint8 i = 0 ; i < 8 ; i++) {
		for(Uint8 j = 0 ; j < 8 ; j++) {
			if (array[i][j] == 0) std::cout<<"r ";
			else if (array[i][j] == -1) std::cout<<"x ";
			else if (array[i][j] == 1) std::cout<<"b ";
			else std::cout<<"? ";
		}
		std::cout<<endl;
	}
}

template<> string bidiarray<bool>::serialize() {
	string r;
	for(Uint8 i = 0 ; i < 8 ; i++) {
		for(Uint8 j = 0 ; j < 8 ; j++) {
			if (array[i][j]) { 
				r+="O";
			}else
				r+="X";
		}
	}
	return r;
}

template<> string bidiarray<Sint16>::serialize() {
	string r;
	for(Uint8 i = 0 ; i < 8 ; i++) {
		for(Uint8 j = 0 ; j < 8 ; j++) {
			if (array[i][j] == 0) r+="r";
			else if (array[i][j] == -1) r+="x";
			else if (array[i][j] == 1) r+="b";
			else r+="?";
		}
	}
	return r;
}

template<> bidiarray<bool> bidiarray<bool>::deserialize(string r) {
	bidiarray<bool> ba;
	int x=0;
	int y=0;
	for(std::string::iterator it = r.begin(); it != r.end(); ++it) {
		switch(*it) {
			case 'O':
				ba.set(x,y,true);
				break;
			case 'X':
				ba.set(x,y,false);
				break;
			default:
				std::cout<<"ERROR in deserialize! (Line: "<<__LINE__<<", file: "<<__FILE__<<")"<<std::endl;
				exit(1);
		}
		y++;
		if(y>=8){
			y=0;
			x++;
		}
	}
	if(y!=0 && x!=8) {
		std::cout<<"ERROR in deserialize! (Line: "<<__LINE__<<", file: "<<__FILE__<<")"<<std::endl;
		exit(1);
	}
	return ba;
}

template<> bidiarray<Sint16> bidiarray<Sint16>::deserialize(string r) {
	bidiarray<Sint16> ba;
	int x=0;
	int y=0;
	for(std::string::iterator it = r.begin(); it != r.end(); ++it) {
		switch(*it) {
			case 'r':
				ba.set(x,y,0);
				break;
			case 'b':
				ba.set(x,y,1);
				break;
			case 'x':
				ba.set(x,y,-1);
				break;
			default:
				ba.set(x,y,42);
		}
		y++;
		if(y>=8){
			y=0;
			x++;
		}
	}
	if(y!=0 && x!=8) {
		std::cout<<"ERROR in deserialize! (Line: "<<__LINE__<<", file: "<<__FILE__<<")"<<std::endl;
		exit(1);
	}
	return ba;
}


