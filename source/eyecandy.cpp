
#include <string.h>

#include "global.h"

//this system is pretty cool ;)


//------------------------------------------------------------------------------
// class cloud
//------------------------------------------------------------------------------
EC_Cloud::EC_Cloud(gfxSprite *nspr, float nx, float ny, float nvelx){
	spr = nspr;
	x = nx;
	y = ny;
	velx = nvelx;
	w = spr->getWidth();
}

void EC_Cloud::draw(){
	spr->draw((int)x, (int)y +y_rect.h);
	if((x+w) > 320)
		spr->draw(0-(w-(int)(x+w)%320),   (int)y  + y_rect.h);
	else if(x < 0)
		spr->draw(320-(w-(int)(x+w)%320),   (int)y + y_rect.h);
}

void EC_Cloud::update(){
	x += velx;
	if((x+w) > 320)
		x = (float) ( 0-(w-(int)(x+w)%320) );
	else if( x < 0){
		x = (float) ( 320-(w-(int)(x+w)%320) );
	}
}



//------------------------------------------------------------------------------
// class corpse
//------------------------------------------------------------------------------
EC_Corpse::EC_Corpse(gfxSprite *nspr, float nx, float ny){
	spr = nspr;
	x = nx;
	y = ny;
	vely = 1.0f;
	timeleft = CORPSESTAY;
}

void EC_Corpse::draw(){
	spr->draw((int)x, (int)y-31 + y_rect.h);	
}

void EC_Corpse::update(){
	if(vely != 0.0f){
		int ty = (int) ( (y+vely) / TILESIZE );
		int tx = (int) ( x / TILESIZE );
		int tx2 = (int) ( (x+31) / TILESIZE );


		if(map.map(tx, ty) == tile_solid_on_top || map.map(tx2, ty) == tile_solid_on_top){	//on ground on tile solid_on_top
			if((y - vely)/TILESIZE < ty){	//only if we were above the tile in the previous frame
				y		= (float) (ty*TILESIZE -1);
				vely	= 0.0f;
				return;
			}
		}

		if(map.map(tx, ty) == tile_solid || map.map(tx2, ty) == tile_solid){	//on ground
			y		= (float) (ty*TILESIZE -1);
			vely	= 0.0f;
		}
		else{	//falling (in air)
			y		+= vely;
			vely	+= GRAVITATION;

			if(vely >= MAXVELY)		//if the speed is higher than this we might fall through a tile
				vely = MAXVELY;
		}
	}
	else{
		if(timeleft > 0)
			timeleft--;
		else
			eyecandy.remove(index);
	}
}



//------------------------------------------------------------------------------
// class EC_GravText
//------------------------------------------------------------------------------

EC_GravText::EC_GravText(gfxFont *nfont, int nx, int ny, const char *ntext, float nvely){
	font = nfont;
	x = (float)(nx - (font->getWidth(ntext) / 2));
	y = (float)ny;
	
	text = new char[strlen(ntext)+1];	//someone should test if we got the memory...
	strcpy(text, ntext);
	
	vely = nvely;
}

void EC_GravText::update(){
	y += vely;
	vely += GRAVITATION;
	if(vely >= MAXVELY)
		vely = MAXVELY;
	else if(vely <= -MAXVELY)
		vely = -MAXVELY;

	if(y > 480)
		eyecandy.remove(index);
}

void EC_GravText::draw(){
	font->draw((int)x, (int)y + y_rect.h, text);
}

EC_GravText::~EC_GravText(){
	delete text;
	text = NULL;
}


//------------------------------------------------------------------------------
// class EC_GameOverMario
//------------------------------------------------------------------------------

EC_GameOverMario::EC_GameOverMario(gfxSprite *nspr, int nx, int ny, float nvely){
	spr = nspr;
	x = (float)nx;
	y = (float)ny;
	vely = nvely;
}

void EC_GameOverMario::update(){
	y += vely;
	vely += GRAVITATION;
	if(vely >= MAXVELY)
		vely = MAXVELY;
	if(y > 480)
		eyecandy.remove(index);
}

void EC_GameOverMario::draw(){
	spr->draw((int)x, (int)y-31 + y_rect.h);	
}




//------------------------------------------------------------------------------
// class eyecandy_container
//------------------------------------------------------------------------------
CEyecandyContainer::CEyecandyContainer(){
	for(int i = 0; i < MAXEYECANDY; i++)
		list[i] = NULL;
	list_end = 0;
}

CEyecandyContainer::~CEyecandyContainer(){
	clean();
}

void CEyecandyContainer::clean(){
	for(int i=0; i < list_end; i++){
		delete list[i];
		list[i] = NULL;
	}
	list_end = 0;
}

void CEyecandyContainer::add(CEyecandy *ec){
	if(list_end < MAXEYECANDY){
		list[list_end] = ec;
		ec->index = list_end;	//save index for removing
		list_end++;
	}
	else{
		delete ec;	//otherwise memory leak!
		//printf("eyecandy list full!\n");
	}
}

void CEyecandyContainer::remove(int i){	
	delete list[i];
	list_end--;

	if(i != list_end){		//if we didn't remove the last element we move the last elemnt in the new free place in the eyecandy list
		list[i] = list[list_end];
		list[i]->index = i;	//save new index (for removing)
	}
}

