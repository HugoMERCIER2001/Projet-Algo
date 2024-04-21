#ifndef __COMMON_H
#define __COMMON_H

#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<SDL.h>
#include<SDL_timer.h>
#ifdef SOUND
#include<SDL_mixer.h>
#endif
#include<SDL_image.h>
#include<vector>
#include<list>
#include<utility>
#include "move.h"

using namespace std;

//LIGHT should be defined in case the game is too heavy for your machine
#define LIGHT

//1 animated frame every three frames
#define ANIMATIONSPEED 3

#ifndef LIGHT
//blobs are animated
#define ANIMATION
//mouse handled by SDL
#define SOFT_MOUSE
#endif


#endif