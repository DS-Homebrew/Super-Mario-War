
#include "global.h"
//#include <math.h>



CPlayer::CPlayer(int padNumber, int nkl, int nkr, int nkj, gfxSprite nsprites[10], SScore *nscore, bool cpu){

	findspawnpoint();

	padInputNumber = padNumber;

	velx	= 0.0f;
	vely	= 0.0f;

	xpressed = 0;

	//cpu stuff
	is_cpu	= cpu;
	cl		= false;
	cr		= false;
	cj		= false;	

	//control keys
	kl = nkl;
	kr = nkr;
	kj = nkj;

	//jump stuff
	inair		= true;
	lockjump	= true;

	score = nscore;
	score->score = 0;

	for(int i=0;i<PGFX_LAST;i++)
		sprites[i] = &nsprites[i];

	spr = PGFX_STANDING_R;
	
	sprswitch = 0;

	killsinrow = 0;
	killsinrowinair = 0;	
}








void CPlayer::move(){

	int lrn = 0;	//move left-right-no: -1.. left 0 no 1 ... right

	if(is_cpu){
		cpu_think();

		if(cr) lrn = 1;
		if(cl) lrn = -1;
	} else {
		if(! (keystates[kr] && keystates[kl])){
			if(keystates[kr])
				lrn = 1;		
			else if(keystates[kl])
				lrn = -1;
		}
	}


	if((keystates[kj] || cj)){	//jump pressed?
		if(!inair && !lockjump){	//only if on the ground and the jump key was released somewhen after it was pressed the last time
			vely = -VELJUMP;
			lockjump = true;

			if(game_values.sound) 
				Mix_PlayChannel(1, jump, 0);
		}
	}
	else{
		lockjump = false;		//the jump key is not pressed: the player may jump again if he is on the ground
	}
	


	if(lrn == 1){
		velx += VELMOVINGADD;		//move right
		xpressed = 1;		//player graphic is facing right
		if(velx > VELMOVING)
			velx = VELMOVING;
	}
	else if(lrn == -1){
		velx -= VELMOVINGADD;		//move left
		xpressed = -1;		//player graphic is facing left
		if(velx < -VELMOVING)
			velx = -VELMOVING;
	}
	else{
		xpressed = 0;
		if(velx > 0){
			velx -= VELMOVINGFRICTION;
			if(velx < 0)	//too much fricition - where moving in the other direction
				velx = 0;
		}
		else if(velx < 0){
			velx += VELMOVINGFRICTION;
			if(velx > 0)	//too much fricition - where moving in the other direction
				velx = 0;
		}
	}
	

	oldy = (int)y;
	oldx = (int)x;

	collision_detection_map();
}








//Search and return in the list_players[] array the player, which is the nearest. 

CPlayer *CPlayer::getNearestPlayer(){
	CPlayer *target = 0;
	int distance_player_pow2;
	int nearest = 320*320+240*240;
	int t1, t2;


	for(int i=0;i<list_players_cnt;i++){
		if(list_players[i] != this){	//don't calculate the distance to this player ( = 0 )
			t1 = int(list_players[i]->x - x);
			t2 = int(list_players[i]->y - y);
			distance_player_pow2 = t1*t1 + t2*t2;    // a^2 = b^2 + c^2 :)

			if (distance_player_pow2 < nearest){
				target = list_players[i];
				nearest = distance_player_pow2;
			}
		}
	}
	
	return target;
}



/*
if is_cpu is true, cpu_think will be used. Fist he search the nearest player, than if "target" is lower or at the same y-axis-level, he runs 
to the target; if the target is a little higher, he stands still and wait. If the target is very high, he jumps to get "maybe" higher 
than the target. If target is very near (on x-axis), he jumps. If target is very near (x-axis) and higher(y-axis) he "runs away".

  */
void CPlayer::cpu_think(){
	CPlayer *target = getNearestPlayer();		//search the nearest Player

	if(target == NULL)
		return;

	//-----------    what will computer do --------------
	if(target->x > x)						//if true target is on the right side
		if(target->y >= y){					//if true target is lower or at the same level, run right
			cl=false;
			cr=true;
		}
		else								
			if(target->x - x < 90 &&		//else if target is near but higher, run away (left)
				target->x - x > -90){
				cl=true;
				cr=false;
			}
			else{							//else target is not very near, stand still, do nothing
				cl=false;
				cr=false;
			}
	else if(target->x < x)					//if true target is on the left side,  (rest equal right)
		if(target->y >= y){
			cl=true;
			cr=false;
		}
		else
			if(target->x - x < 90 && 
				target->x - x > -90){
				cl=false;
				cr=true;
			}
			else{
				cl=false;
				cr=false;
			}
	else {
		cr=false;
		cl=false;
	}

	if (target->y <= y &&					//jump if target is higher or at the sam level and
		target->x - x < 45 &&				//target is very near
		target->x - x > -45 || 
		y - target->y > 70){				//or if target is high
		cj=true;
	}
	else cj=false;
}




void CPlayer::die(){
	eyecandy.add(new EC_Corpse(sprites[PGFX_DEAD], x, y+PH));

	findspawnpoint();

	velx = 0;
	vely = -(VELJUMP/2);	//make the player jump up on respawn
	oldx = int(x);
	oldy = int(y);

	inair = true;
	lockjump = true;

	killsinrow = 0;
	killsinrowinair = 0;
}



//very slow and stupid brute force method (tm)
void CPlayer::findspawnpoint(){
	for(int tries = 0; tries < 32; tries++){
		x = (float)(rand()%320);
		y = (float)(rand()%(240-TILESIZE*2));	//don't spawn too low
		
		
		if(map.map(int(x)/TILESIZE,int(y)/TILESIZE) == tile_nonsolid && map.map((int(x)+PW)/TILESIZE, int(y)/TILESIZE) == tile_nonsolid	&& \
			map.map(int(x)/TILESIZE,(int(y)+PH)/TILESIZE) == tile_nonsolid && map.map((int(x)+PW)/TILESIZE,(int(y)+PH)/TILESIZE) == tile_nonsolid){
			//fprintf(stderr,"xy =%d , %d\n",x,y);
			return;
		}
	}
	
	x = 160;
	y = 120;
} 




void CPlayer::spawnTextWinner(){
	static int spawntext = 0;

	if(spawntext == 0){
		eyecandy.add(new EC_GravText(awards[PSOUND_LAST-1].font, int(x) + (PW>>1), int(y), "winner", -VELJUMP));
		spawntext = 20;	//spawn text every 20 frames
	}
	else{
		spawntext--;
	}
}



void CPlayer::spawnGameOverMario(){
	eyecandy.add(new EC_GameOverMario(sprites[PGFX_DEADFLYING],int(x),int(y)+PH, -VELJUMP*1.1f));
}






bool CPlayer::isstomping(CPlayer &o){
	if(int(oldy+PH) <= int(o.oldy) && int(y+PH) >= int(o.y)){
		y = o.y-PH;		//set new position to top of other player
		
		if(keystates[kj])
			vely = -VELJUMP;
		else
			vely = -VELJUMP/2;	//jump a little of the other players head

		score->score++;


		if(game_values.sound)
			Mix_PlayChannel(1, mip, 0);

		killsinrow++;
		if(inair){
			killsinrowinair++;
			if(killsinrowinair > 1){
				if(game_values.sound) 
					Mix_PlayChannel(1, excellent, 0);
					
				eyecandy.add(new EC_GravText(awards[PSOUND_LAST-1].font, int(o.x) + (PW>>1), int(o.y)+PH, "EXCELLENT", -VELJUMP*1.5));
			}
		}
			
		//if we have enough kills in a row -> spawn an award								
		if(killsinrow >= 2){						
			char	text[128];				//text to show
			
			int a = (killsinrow-2) >= PSOUND_LAST ? PSOUND_LAST -1 : (killsinrow-2);
			sprintf(text, "%d - %s", killsinrow, awards[a].name);

			if(game_values.sound) 
				Mix_PlayChannel(1, awards[a].sound, 0);
			
			//now add the eyecandy
			eyecandy.add(new EC_GravText(awards[a].font, int(o.x) + (PW>>1), int(o.y)+PH, text, -VELJUMP));
		}

		//if we stopped the other players run show another award
		if(o.killsinrow >= 2){
			int a = (o.killsinrow-2) >= PSOUND_LAST ? PSOUND_LAST -1 : (o.killsinrow-2);
			char text[128];
			sprintf(text, "%s stopped!",  awards[a].name);

			if(game_values.sound)
				Mix_PlayChannel(1, stoprow, 0);
			
			eyecandy.add(new EC_GravText(awards[a].font, int(o.x) + (PW>>1), int(o.y)+PH, text, -VELJUMP*1.3f));
		}
		

		
		crunch_screen();	//shake the screen 

		if ( ! game_values.gamemode->playerkilledplayer(*this, o) ){	//true if the player was deleted
			o.die();			//now kill the player (don't call this function earlier because we need the old position, etc.	
		}

		return true;
	}

	return false;
}


//this is no member of CPlayer, but can only be used with CPlayer
//and it belongs to the context p2p collision detection + response (CPlayer->isstomping, collisionhandler, ...)

//handles a collision between two players (is being called if o1, o2 collide)
void collisionhandler_p2p(CPlayer &o1, CPlayer &o2){

	//--- 1. stompin other player? ---
	if(o1.isstomping(o2))
		return;
	if(o2.isstomping(o1))
		return;

	//--- 2. push back (horizontal) ---
	if(o1.x < o2.x)				//o1 is left -> o1 pushback left, o2 pushback right
		_collisionhandler_p2p_pushback(o1, o2);
	else if(o2.x < o1.x)
		_collisionhandler_p2p_pushback(o2, o1);
}


#define ABS(x) (x>0?x:-x)

//calculates the new positions for both players when they are pushing each other
void _collisionhandler_p2p_pushback(CPlayer &o1, CPlayer &o2){
	//o1 is left to o2
	//  |o1||o2|
	//-----------

	if(o1.velx == 0){
		o2.x = o1.x + PW + 1;	//o2 reposition to the right side of o1, o1 stays
	}
	else if(o2.velx == 0){
		o1.x = o2.x - PW - 1;	//o1 repositipn to the left side of o2, o2 stays
	}
	else{						//both objects moving - calculate middle and set both objects
		float middle = o2.x + ((o1.x+PW)-o2.x)/2;		//no ABS needed (o1.x < o2.x -> o1.x+w > o2.x !)

		//printf("hlf:%f, o1x:%f, o2x:%f\n", hlf, o1.x, o2.x);
		o1.x = middle-PW-1;		//o1 is left
		o2.x = middle+1;		//o2 is right
	}

	const float absv1 = ( o1.velx > 0 ? o1.velx : 0 ) * 1.5f;	//o1 is on the left side (only positive velx counts)
	const float absv2 = ( o2.velx < 0 ? o2.velx : 0 ) * 1.5f;	//o2 right (only negative velx counts)
	o1.velx = absv2;
	o2.velx = absv1;

}













void CPlayer::draw(){
	//lockjump is true wenn we are in the air (even if we fell of an edge)
	if(inair){
		if(xpressed == 1)
			spr = PGFX_JUMPING_R;
		else if(xpressed == -1)
			spr = PGFX_JUMPING_L;
		else{
			if((spr & 0x1) == 0)		// ;) - this is used to determine the direction the player was facing in the last frame
				spr = PGFX_JUMPING_R;	// if the last sprite was facing right spr & 0x1 is 0
			else						// because the last bit of every PGFX_[action]_R is 0
				spr = PGFX_JUMPING_L;	// the last bit of PGFX_[action]_L is 1
		}								// IF YOU CHANGE THE ORDER OF THE PGFX_L/R STUFF THIS WON'T WOR
	}

	else{
		if(velx > 0){
			if(xpressed == -1)
				spr = PGFX_STOPPING_R;
			else{
				if(sprswitch == 0){
					if(spr == PGFX_STANDING_R)
						spr = PGFX_RUNNING_R;
					else
						spr = PGFX_STANDING_R;

					sprswitch = 4;
				}
				else
					sprswitch--;
			}
		}
		else if(velx < 0){
			if(xpressed == 1)
				spr = PGFX_STOPPING_L;
			else{
				if(sprswitch == 0){
					if(spr == PGFX_STANDING_L)
						spr = PGFX_RUNNING_L;
					else
						spr = PGFX_STANDING_L;

					sprswitch = 4;
				}
				else
					sprswitch--;
				
			}
		}
		else{	//standing
			if((spr & 0x1) == 0)
				spr = PGFX_STANDING_R;
			else
				spr = PGFX_STANDING_L;
		}
	}


	sprites[spr]->draw(int(x) - 1,   int(y)   + y_rect.h - 4);

	if(x < 0.0f)	//if the player is in the -both-sides-of-the-screen- area
		sprites[spr]->draw(320-(PW-(int)(x+PW)%320) - 1,   int(y) + y_rect.h - 4);
		
}






void CPlayer::collision_detection_map(){
	//printf("x:%f, y:%f\n", x, y);

	
	// a)
	if(y+vely < 0.0f){
		y += vely;
		x += velx;
		vely += GRAVITATION;
		if(vely > TILESIZE)
			vely = TILESIZE;
		//printf("hit top!\n");
		
		if(int(x)+PW > 639)
			x = 639-PW;
		else if(int(x) < 0)
			x = 0;

		return;
	}
	else if(y+vely+PH > 480.0f){
		y = 0.0f;
		oldy = 0;
		//printf("on ground of screen!\n");

		if(x+PW > 639)
			x = 639-PW;
		else if(x < 0)
			x = 0;

		return;
	}


	//-----------------------------------------------------------------
	//the whole player-can-be-in-the-area-on-both-sides-of-the-screen thingy works like this:
	//if the player is in this area x < 0.0f (and x is guaranteed to be x > 0.0f-w)
	//this way we can determin if we need to take care of this special case
	//-----------------------------------------------------------------

	int ty = int(y / TILESIZE);
	int ty2 = int((y+PH) / TILESIZE);
	int tx = -1;
	int txl = -1, txr = -1;


	x += velx;

	if(x < 0.0f-PW)				//out of -on-both-sides- area
		x = 320+x;					//x is negativ!
	else if(int(x)+PW > 319)		//entered -on-both-sides- area (mind: the screen ends at 639 !!!)
		x = 0 - (319-int(x));
		
	
	

	//-----------------------------------------------------------------
	//  x axis first (--)
	//-----------------------------------------------------------------
	if(velx > 0){		//moving right	
		tx = int(x+PW)/TILESIZE;	//no special case here when on both sides

		if(map.map(tx, ty) == tile_solid || map.map(tx, ty2) == tile_solid){	//collision on the right side.
			x = float(tx*TILESIZE -PW - 1);			//move to the edge of the tile (tile on the right -> mind the player width)
			velx = 0;
		}
	}
	else if(velx < 0){	//moving left
		if(x < 0.0f)
			tx = (320+int(x))/TILESIZE;	//test against tiles on the right side of the sceen. x is negativ!
		else
			tx = int(x)/TILESIZE;

		if(map.map(tx, ty) == tile_solid || map.map(tx, ty2) == tile_solid){	//collision on the right side.
			x = float((tx)*TILESIZE +TILESIZE);			//move to the edge of the tile
			velx = 0;
		}
	}


	//calculate txleft, txright for y-axis checking
	if( x == -15.0f){					//special case: player hit a tile on the left side while being over the edge of the screen on the right side	
		x = 305.0f;	
		txl = 19;
	}
	else if( x == 320.0f){				//special case if we hit a tile on the other side of the screen (tile right side, player left side)
		txl = 0;
		x = 0;
	}
	else if(x < 0.0f){
		txl = (320+int(x))/TILESIZE;	//x is negativ (i keep forgetting that and write 640-int(x))
	}
	else{
		txl = int(x)/TILESIZE;
	}
	txr = int(x+PW)/TILESIZE;



	
	//printf("x:%f, tx:%d, txl: %d,txr:%d\n", x, tx, txl, txr);


	//-----------------------------------------------------------------
	//  then y axis (|)
	//-----------------------------------------------------------------
	if(vely < 0){	//moving up
		ty = int(y+vely) / TILESIZE;
		if(map.map(txl, ty) == tile_solid || map.map(txr, ty) == tile_solid){
			y		= ty*TILESIZE +TILESIZE +1;
			vely	= 0;
		}
		else{
			y		+= vely;
			vely	+= GRAVITATION;
		}
		inair = true;
	}		 
	else{		//moving down / on ground
		//printf("test: down, vely:%d\n", vely);
		ty = (int(y+vely)+PH) / TILESIZE;

		if(map.map(txl, ty) == tile_solid_on_top || map.map(txr, ty) == tile_solid_on_top){	//on ground
			if((oldy+PH)/TILESIZE < ty){	//we were above the tile in the previous frame
				y = ty*TILESIZE -PH -1;
				vely = 1;
				
				inair			= false;
				killsinrowinair = 0;

				return;
			}
		}
		if(map.map(txl, ty) == tile_solid || map.map(txr, ty) == tile_solid){	//on ground
			y		= ty*TILESIZE -PH-1;
			vely	= 1;				//1 so we test against the ground again int the next frame (0 would test against the ground in the next+1 frame)

			inair			= false;
			killsinrowinair = 0;
		}
		else{	//falling (in air)
			y		+= vely;
			vely	+= GRAVITATION;

			if(vely >= TILESIZE)		//if the speed is higher than this we might fall through a tile
				vely = TILESIZE;

			inair = true;
		}
	}
	
}
