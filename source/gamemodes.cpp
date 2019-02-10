
#include "global.h"
extern	gfxFont			font[3];

#include <string.h>



void removeplayer(CPlayer *other){
	int i,j;

	for(i=0;i<list_players_cnt;i++){
		if(list_players[i] == other){
			other->spawnGameOverMario();

			delete other;
			
			for(j=i+1;j<list_players_cnt;j++){
				list_players[j-1] = list_players[j];
			}

			list_players_cnt--;
			list_players[list_players_cnt] = NULL;
		}
	}	
}

void removeplayersbutone(CPlayer *stay){
	int i;

	for(i=0;i<list_players_cnt;i++){
		if(list_players[i] == stay){
			list_players[0] = stay;
		}
		else{
			list_players[i]->spawnGameOverMario();
			delete list_players[i];
			list_players[i] = NULL;
		}
		
	}	
	list_players_cnt = 1;
}



//fraglimit:
void CGM_DM_Fraglimit::init(){
	winner = NULL;
}
void CGM_DM_Fraglimit::think(){
	if(winner) winner->spawnTextWinner();
}

bool CGM_DM_Fraglimit::playerkilledplayer(CPlayer &inflictor, CPlayer &other){
	if(inflictor.score->score >= fraglimit){
		winner = &inflictor;
		removeplayersbutone(winner);

		showscoreboard = true;

		return true;	//we removed a player
	}
	return false;		//we have not removed a player
}

char * CGM_DM_Fraglimit::getMenuString(char *buffer64){
	sprintf(buffer64, "fraglimit %d", fraglimit);
	return buffer64;
}

void CGM_DM_Fraglimit::toggleoptions(){
	fraglimit += 20;
	if(fraglimit > 100)
		fraglimit = 20;
}





//timelimit
void CGM_DM_Timelimit::init(){
	winner = NULL;
	timeleft = timelimit;
	framesleft_persecond = 1000 / WAITTIME;
}
void CGM_DM_Timelimit::think(){
	if(winner) winner->spawnTextWinner();

	if(timeleft > 0){
		if(framesleft_persecond > 0){
			framesleft_persecond--;
		}			
		else{
			framesleft_persecond = 1000 / WAITTIME;
			timeleft--;
		}
	}
	else if(timeleft == 0){		//the game ends
		showscoreboard = true;
		timeleft--;
	}
}

bool CGM_DM_Timelimit::playerkilledplayer(CPlayer &inflictor, CPlayer &other){
	//game over -> don't count kills
	if(timeleft == -1)
		inflictor.score->score --;

	return false;		//we have not removed a player
}

void CGM_DM_Timelimit::draw(){
	char buffer[64];

	if(timeleft > 0)
		sprintf(buffer, "%d seconds left", timeleft);
	else
		strcpy(buffer, "game over");

	font[0].draw(320-font[0].getWidth(buffer), 0, buffer);
}

	
char * CGM_DM_Timelimit::getMenuString(char *buffer64){
	sprintf(buffer64, "timelimit %d seconds", timelimit);
	return buffer64;
}

void CGM_DM_Timelimit::toggleoptions(){
	timelimit += 30;
	if(timelimit > 240)
		timelimit = 30;
}



//mariowar (x lives - counting down)
void CGM_MarioWar::init(){
	winner = NULL;

	for(int i=0;i<list_players_cnt;i++){
		list_players[i]->score->score = lives;
	}
}
void CGM_MarioWar::think(){
	if(winner) winner->spawnTextWinner();
}

bool CGM_MarioWar::playerkilledplayer(CPlayer &inflictor, CPlayer &other){

	(other.score->score)--;
	(inflictor.score->score)--;	//no kills are counted

	if(other.score->score == 0){
		removeplayer(&other);

		if(list_players_cnt == 1){
			winner = &inflictor;
			showscoreboard = true;
		}

		return true;	//we removed a player
	}
	return false;		//we have not removed a player
}

char * CGM_MarioWar::getMenuString(char *buffer64){
	sprintf(buffer64, "MarioWar Classic %d", lives);
	return buffer64;
}

void CGM_MarioWar::toggleoptions(){
	lives += 10;
	if(lives > 50)
		lives = 10;
}









//capture the chicken
//one player is the chicken
//if he is stomped the attacker becomes the chicken.
//get points for being the chicken
void CGM_CaptureTheChicken::init(){
	winner = NULL;
	chicken = NULL;
}
void CGM_CaptureTheChicken::think(){
	if(winner)
		winner->spawnTextWinner();
	else{

		if(chicken){
			if( chicken->velx > VELMOVING_CHICKEN )
				chicken->velx = VELMOVING_CHICKEN;
			else if(chicken->velx < -VELMOVING_CHICKEN)
				chicken->velx = -VELMOVING_CHICKEN;

			(chicken->score->score)++;

			if(chicken->score->score >= frames){
				winner = chicken;
				showscoreboard = true;
				removeplayersbutone(winner);
			}
		}	
	}
}

void CGM_CaptureTheChicken::draw(){
	if(chicken){
		int cx = int(chicken->x);
		int cy = int(chicken->y);
		spr_chicken.draw(cx - 1,   cy   + y_rect.h - 4);

		if(cx < 0)	//if the player is in the -both-sides-of-the-screen- area
			spr_chicken.draw(320-(PW- (cx+PW)%320) - 1,   cy + y_rect.h- 4);
	}
}

bool CGM_CaptureTheChicken::playerkilledplayer(CPlayer &inflictor, CPlayer &other){
	if(!chicken){
		chicken = &inflictor;
		eyecandy.add(new EC_GravText(awards[PSOUND_LAST-1].font, int(inflictor.x) + (PW>>1), int(inflictor.y)+PH, "CHICKEN!", -VELJUMP*1.5));
	}
	else{
		if(&other == chicken){
			chicken = &inflictor;
			eyecandy.add(new EC_GravText(awards[PSOUND_LAST-1].font, int(inflictor.x) + (PW>>1), int(inflictor.y)+PH, "CHICKEN!", -VELJUMP*1.5));	
		}
	}
	return false;		//we have not removed a player
}


CGM_CaptureTheChicken::CGM_CaptureTheChicken(){
	frames = 4000;
}

char * CGM_CaptureTheChicken::getMenuString(char *buffer64){
	sprintf(buffer64, "GetTheChicken %d", frames);
	return buffer64;
}

void CGM_CaptureTheChicken::toggleoptions(){
	frames += 1000;
	if(frames > 10000)
		frames = 1000;
}
