#include <nds.h>

#include "gfx.h"
#include <stdarg.h>		//atexit?
#include <stdlib.h>		//atexit?
#include "SDL_image.h"
#include <gbfs.h>

//better than setting in the IDE
#ifdef _WIN32
	#ifndef _XBOX
		#pragma comment(lib, "SDL.lib")
		#pragma comment(lib, "SDLmain.lib")
	#endif
#endif


extern SDL_Surface *screen;

#define GFX_BPP		8 
#define GFX_FLAGS	SDL_SWSURFACE
extern const GBFS_FILE data_gbfs;
extern const int data_gbfs_size;

//gfx_init
bool gfx_init(int w, int h, bool fullscreen){
	printf("init SDL\n");
	if( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO ) < 0 ) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return false;
    }
 
    // Clean up on exit
    //atexit(SDL_Quit);
    
    if(fullscreen)
		screen = SDL_SetVideoMode(w, h, GFX_BPP, GFX_FLAGS | SDL_FULLSCREEN);
	else
		screen = SDL_SetVideoMode(w, h, GFX_BPP, GFX_FLAGS);

    if ( screen == NULL ) {
        printf("Couldn't set video mode %dx%d: %s\n", w, h, SDL_GetError());
		return false;
    }
    printf(" running @ %dx%d %dbpp (done)\n", w,h,screen->format->BitsPerPixel);	
	return true;
}

void gfx_setresolution(int w, int h, bool fullscreen){
	Uint32 flags = GFX_FLAGS;
	if(fullscreen)
		flags |= SDL_FULLSCREEN;
	screen = SDL_SetVideoMode(w, h, GFX_BPP, flags);
}





//gfxSprite

gfxSprite::gfxSprite(){
	m_bltrect.x = 0;
	m_bltrect.y = 0;
	m_bltrect.w = 0;
	m_bltrect.h = 0;
	m_picture = NULL;
	WAIT_CR &= ~0x80;

}

gfxSprite::~gfxSprite(){
	//free the allocated BMP surface 
	if(m_picture){
		SDL_FreeSurface(m_picture);
		m_picture = NULL;
	}
}

bool gfxSprite::init(const char *filename, Uint8 r, Uint8 g, Uint8 b){
	fprintf(stderr,"loading sprite %s ... ", filename);
	SDL_RWops *rw;
	uint8* ftemp;
	unsigned int length = 0;	
	if(m_picture != NULL){
		printf("sprite already loaded ... deleting ...");
		
		SDL_FreeSurface(m_picture);
		m_picture = NULL;
		
		printf("ok ...");
	}
	
	ftemp = (uint8*)gbfs_get_obj(&data_gbfs, (char *)filename, &length);
	rw = SDL_RWFromMem((void*)ftemp, length);
    // Load the BMP file into a surface
	//m_picture = SDL_LoadBMP(filename);
	m_picture = IMG_LoadBMP_RW(rw);
	SDL_FreeRW(rw);

    if (m_picture == NULL) {
		printf("\n ERROR: Couldn't load %s: %s\n", filename, SDL_GetError());
        return false;
    }

	if( SDL_SetColorKey(m_picture, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(m_picture->format, r, g, b)) < 0){
		printf("\n ERROR: Couldn't set ColorKey + RLE for %s: %s\n", filename, SDL_GetError());
		return false;
	}

	SDL_Surface *temp;
	if( (temp = SDL_DisplayFormat(m_picture)) == NULL){
		printf("\n ERROR: couldn't convert %s to display format: %s\n", filename, SDL_GetError());
		return false;
	}
	SDL_FreeSurface(m_picture);
	m_picture = temp;


	m_bltrect.w = m_picture->w;
	m_bltrect.h = m_picture->h;

	printf("done\n");
	return true;
}

bool gfxSprite::init(const char *filename) {
	printf("loading sprite %s ... ", filename);
	SDL_RWops *rw;
	uint8* ftemp;
	unsigned int length = 0;
	if(m_picture != NULL){
		printf("sprite already loaded - deleting ...");
		
		SDL_FreeSurface(m_picture);
		m_picture = NULL;
		
		printf("ok ...");
	}

    // Load the BMP file into a surface
	//m_picture = SDL_LoadBMP(filename);
	//m_picture = IMG_Load(filename);
	ftemp = (uint8*)gbfs_get_obj(&data_gbfs, (char *)filename, &length);
	rw = SDL_RWFromMem((void*)ftemp, length);
    // Load the BMP file into a surface
	//m_picture = SDL_LoadBMP(filename);
	m_picture = IMG_LoadBMP_RW(rw);
	SDL_FreeRW(rw);
    if (!m_picture) {
		printf("\n ERROR: Couldn't load %s: %s\n", filename, SDL_GetError());
        return false;
    }

	SDL_Surface *temp;
	if( (temp = SDL_DisplayFormat(m_picture)) == NULL){
		printf("\n ERROR: couldn't convert %s to display format: %s\n", filename, SDL_GetError());
		return false;
	}
	SDL_FreeSurface(m_picture);
	m_picture = temp;


	m_bltrect.w = m_picture->w;
	m_bltrect.h = m_picture->h;

	printf("done\n");
	return true;
}



bool gfxSprite::draw(int x, int y){
	m_bltrect.x = x;
	m_bltrect.y = y;

	// Blit onto the screen surface
	if(SDL_BlitSurface(m_picture, NULL, screen, &m_bltrect) < 0){
		fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
		return false;
	}
	return true;
}




// gfxFont
bool gfxFont::init(const char *filename){
	fprintf(stderr,"loading font %s ... ", filename);
	SDL_RWops *rw;
	uint8* ftemp;
	unsigned int length = 0;	
	ftemp = (uint8*)gbfs_get_obj(&data_gbfs, (char *)filename, &length);
	rw = SDL_RWFromMem((void*)ftemp, length);
	
	//SDL_Surface *fontsurf = SDL_LoadBMP(filename);
	SDL_Surface *fontsurf = IMG_LoadBMP_RW(rw);
	if(!fontsurf){
	 	fprintf(stderr,"\n ERROR: couldn't load file %s: %s\n", filename, SDL_GetError());
		SDL_Delay(9000);
		return false;
	}
	
	m_font = SFont_InitFont(fontsurf);
	if(!m_font) {
		printf("\n ERROR: an error occured while loading the font.\n");
		return false;
	}
	SDL_FreeRW(rw);
	fprintf(stderr,"done\n");
	
	return true;
}

void gfxFont::draw(int x, int y, const char *s){
	if(y >= 0)
		SFont_Write(screen, m_font, x, y, s);
}

void gfxFont::drawCentered(int y, const char *text){
	if(y >= 0)
		SFont_WriteCenter(screen, m_font, y, text);
};


void gfxFont::drawf(int x, int y, char *s, ...){
	char buffer[256];

	va_list zeiger;
	va_start(zeiger, s);
	vsprintf(buffer, s, zeiger);
	va_end(zeiger);

	draw(x,y,buffer);
}

