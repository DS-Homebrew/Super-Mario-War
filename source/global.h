
#ifndef _GLOBAL_H
#define _GLOBAL_H

//----------------------------------------------------------------
// this file contains stuff that's needed by every part of smw
//----------------------------------------------------------------


#include "SDL_mixer.h"
#include <stdlib.h>	//srand/rand (maybe replace with an own version)



#include "gfx.h"


struct SAward{
	char			name[64];
	Mix_Chunk		*sound;
	gfxFont			*font;
};

//------------- global definitions -------------


//----------------------------------------------------------------
// DON'T CHANGE THE ORDER xxx_R xxx_L xxx_R xxx_L !!!
//----------------------------------------------------------------
// because this is used to determine the direction the player was facing in the last frame
// if the last sprite was facing right (spr & 0x1) is 0
// because the last bit of every PGFX_[action]_R is 0
// the last bit of PGFX_[action]_L is 1
// IF YOU CHANGE THE ORDER OF THE PGFX_L/R STUFF THIS WON'T WORK!

// see CPlayer::draw() on how this is used

#define PGFX_STANDING_R	0
#define PGFX_STANDING_L	1
#define PGFX_RUNNING_R	2
#define PGFX_RUNNING_L	3
#define PGFX_JUMPING_R	4
#define PGFX_JUMPING_L	5
#define PGFX_STOPPING_R	6
#define PGFX_STOPPING_L	7
#define PGFX_DEADFLYING	8
#define PGFX_DEAD		9
//--------------------------
#define PGFX_LAST		10

#define PSOUND_LAST		9

#define PH				12		//Player width
#define PW				14		//Player height



#define VELMOVING		4.0f		//velocity (speed) for moving left, right
#define	VELMOVING_CHICKEN 2.9f		//speed of the chicken in the gamemode capturethechicken
#define VELMOVINGFRICTION 0.2f
#define VELMOVINGADD	0.6f
#define VELJUMP			9.0f		//velocity for jumping
#define VELPUSHBACK		5.0f

#define	GRAVITATION		0.40f		//gravitation - 1 pixel per frame (too fast, will be changed somewhen (playerx,y -> float, grav: 0.xx))

#define MAXVELY			20



#define MAPWIDTH		20			//width of the map
#define MAPHEIGHT		15			//height of the map
#define TILESETSIZE		96			//8*12 Tiles á 32 pixel in a 256*384 bmp
#define TILESIZE		16			//size of the tiles, should be dynamically read


#define SCREENCRUNCH	2.0f
#define CRUNCHSTART		10.0f

#define MAXEYECANDY		64
#define CORPSESTAY		200


#define WAITTIME		16		//delay between frames (default:16)
#define AWARDTIME		150


//------------- data structures / selfmade include files -------------
#include "map.h"
#include "player.h"
#include "eyecandy.h"
#include "gamemodes.h"


//------------- global variables / etc -------------
//i know that using so much global variables is ugly, this will change somewhen

//gfx stuff
extern Uint8			*keystates;
extern SDL_Surface		*screen;		//for gfx


extern CMap				map;
extern CEyecandyContainer eyecandy;
extern CPlayer			*list_players[4];
extern int				list_players_cnt;
extern bool				showscoreboard;
extern SScore			score[4];
extern int				score_cnt;


extern SDL_Rect			y_rect;


extern SAward			awards[PSOUND_LAST];
extern Mix_Chunk	*stoprow;
extern Mix_Chunk	*mip;
extern Mix_Chunk	*jump;
extern Mix_Chunk	*excellent;

extern	gfxSprite		spr_chicken;


void crunch_screen();


//----------------- game options all parts of the game need -----------
enum gs{GS_MENU, GS_GAME, GS_QUIT};


struct gv{
	int			players;
	int			cpu;
	bool		showfps;
	bool		sound;
	bool		NUMmousecontrol;
	bool		fullscreen;
	gs			gamestate;
	int			screenResize;
	CGameMode	*gamemode;
};

extern gv game_values;

#endif

