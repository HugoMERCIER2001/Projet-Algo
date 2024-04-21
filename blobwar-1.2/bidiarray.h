#ifndef BIDIARRAY
#define BIDIARRAY
#include "common.h"

/** bidiarray class
 * A 2D array the size of a blobwar map (8x8).
 */
template<class element_type> class bidiarray {
	private:
		element_type array[8][8];
	public:
		bidiarray() {}
		~bidiarray() {}
		void set(unsigned int x, unsigned int y, element_type value) {
			array[x][y] = value;
		}
		element_type get(unsigned int x, unsigned int y) const {
			return array[x][y];
		}
		
		void display() {
			for(Uint8 i = 0 ; i < 8 ; i++) {
				for(Uint8 j = 0 ; j < 8 ; j++) {
					std::cout<<array[i][j];
				}
				std::cout<<std::endl;
			}
			return;
		}
		
		string serialize() {
			std::cout<<"NOT IMPLEMENTED for this specialization"<<std::endl;
			return "";
		}
		
		static bidiarray deserialize(string r) {
			std::cout<<"NOT IMPLEMENTED for this specialization"<<std::endl;
			return bidiarray();
		}
};

template<> void bidiarray<bool>::display();

template<> void bidiarray<Sint16>::display();

template<> string bidiarray<bool>::serialize();

template<> string bidiarray<Sint16>::serialize();

template<> bidiarray<bool> bidiarray<bool>::deserialize(string r);

template<> bidiarray<Sint16> bidiarray<Sint16>::deserialize(string r);



#endif 
