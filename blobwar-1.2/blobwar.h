
#ifndef GBLOBWAR
#define GBLOBWAR
//blobwar main class
#include"common.h"
#include"mouse.h"
#include"font.h"
#ifdef SOUND
#include"sound.h"
#endif
#include"widget.h"
#include"image.h"
#include"button.h"
#include"label.h"
#include"board.h"
#include"rules.h"
#include"network.h"

/** this is the main game class. All objects are stored in it. */
class blobwar {
	private:
		//!style of game chosen
		Uint8 gametype;

		//! list of all widgets we have to display
		list<widget*>widgets;

		//here are some widgets we use
		//! background image
		image *background;
		//! background logo
		image *background_logo;
		//! start 2 player game button
		button *twoplayersgamebutton;
		//! start 1 player game button
		button *oneplayergamebutton;
		//! start 2 player AI button
		button *twoplayerAIbutton;
		//! start 4 player AI button
		button *fourplayerAIbutton;
		//! start network game button
		button *networkgamebutton;
		//! quit the game button
		button *quitbutton;

		//!button to iterate on boards list
		button *nextboardbutton;
		//!button to iterate on boards list
		button *previousboardbutton;
		//!board name
		label *boardlabel;
		//!main label is used to indicate current player
		label *mainlabel;
		//!button to finaly start the game
		button *startgamebutton;

		//!label displaying current players scores
		label *scoreslabel;

		//!images for winning players (or disconnection error)
		image *winner_images[6];


		//!label displaying final scores (red player)
		label *winlabel1;
		//!label displaying final scores (blue player)
		label *winlabel2;

		//!label to display local player color in networked game
		label *local_player_label;

	public:
		//!you should lock this mutex when you modify blobs
		pthread_mutex_t mutex;
		
		//!button to click when winning to go back to mainmenu
		button *wingamebutton;

		//!game board
		board *bwboard;

		//!network engine
		network *bwnet;

		//!rules engine
		rules *bwrules;

		/// screen (use LOCK for a direct access)
		SDL_Surface *screen;
		/// last number of ticks
		Uint32 ticks;
		/// current frame playing (for animations)
		Uint32 frame;
		/// number of ticks between two frames
		Uint32 framerate;
		/// do we need to update display ? 1 -> yes 0-> no
		Uint8 display2update;
		//!mouse pointer
		mouse *mousepointer;
		//!big font
		font *bigfont;
		//!small font
		font *smallfont;
		//!The time that IA have to do their computations
		int compute_time_IA;
		/// constructor
		blobwar();
		/// destructor
		~blobwar();
		//! loads an image 
		SDL_Surface* image_load(string filename);
		//! loads an image with an alpha channel
		SDL_Surface* alpha_image_load(string filename);
		/// updates the screen
		void update();
		//! ends the game
		void quit();
		//! changes string filename using real datapath (install dir)
		string rename(string shortname);
		//! toggles fullscreen on/off
		void fullscreen();
		//! fills screen in black
		void black();
		//! handle game animations or player actions
		void handle();
		//! add a widget to widgets list
		void register_widget(widget* w);
		//! remove a widget from widgets list
		void unregister_widget(widget* w);
		//! execute an action numbered n
		void execute(Uint32 n);
		//! switch to board selection
		void board_selection();
		//! switch to board selection (choosing showed board)
		void board_selection(char* mapname);
		//! change the map label
		void set_map_label(string name);
		//! change the main label
		void set_main_label(string text);
		//! change the scores label (scores updated)
		void set_scores_label(string s);
		//! change label displaying who is local in a networked game
		void set_local_player_label(string color);
		//! someone wins, display winner
		void set_winner(Uint16 player);
		//! game ended update the scores to display
		void set_scores(Uint32 s1, Uint32 s2);
		//! launch the game (mask board selection screen)
		void start_game(Uint32 local_player_id);
};

extern blobwar *game;
#endif
