#include <nds.h>
#include <stdio.h>		//atexit?
#include <stdlib.h>		//atexit?
 #include <string.h>
#include "global.h"
#include "SDL_image.h"
#include <gbfs.h>

extern const GBFS_FILE data_gbfs;
extern const int data_gbfs_size;

CMap::~CMap(){
	if(tilesetsurface != NULL){
		SDL_FreeSurface(tilesetsurface);
		tilesetsurface = NULL;
	}
}


void CMap::loadTileSet(const char *tilesetfile, const char *tilesetbmp){
	FILE *tsf;
	int i;
	SDL_RWops *rw;
	clearTileSet();

	//1. load tileset file
	fprintf(stderr,"loading tile set from %s ... ", tilesetfile);
	unsigned int length = 0;
	uint8* ftemp;
	ftemp = (uint8*)gbfs_get_obj(&data_gbfs, (char *)tilesetfile, &length);
	/*tsf = fopen(tilesetfile, "rb");
	if(!tsf){
		printf("\n ERROR: couldn't open tileset file.\n");
		return;
	}
	*/
	uint8* a=ftemp;
	for(i = 0; i < TILESETSIZE; i++){
		//fread(&(tileset[i]), sizeof(TileType), 1, tsf);
			a+=sizeof(TileType);
			memcpy(&(tileset[i]),a,sizeof(TileType));
	}
	tileset[TILESETSIZE] = tile_nonsolid;	//this is the no tile selected tile

	//fclose(tsf);
	fprintf(stderr,"done loading map\n");
	
	

	//2. load tileset graphics
	fprintf(stderr,"loading tile set suface from %s ... ", tilesetbmp);

	ftemp = (uint8*)gbfs_get_obj(&data_gbfs, (char *)tilesetbmp, &length);
	rw = SDL_RWFromMem((void*)ftemp, length);
    // Load the BMP file into a surface
	//m_picture = SDL_LoadBMP(filename);
	tilesetsurface = IMG_LoadBMP_RW(rw);
	SDL_FreeRW(rw);
	
	/*f(tilesetsurface != NULL){
		SDL_FreeSurface(tilesetsurface);
		tilesetsurface = NULL;
	}
	

	//tilesetsurface = SDL_LoadBMP(tilesetbmp);
	tilesetsurface = IMG_Load(tilesetbmp);

	*/
    if (!tilesetsurface){
		fprintf(stderr,"\n ERROR: Couldn't load %s: %s\n", tilesetbmp, SDL_GetError());
        return;
    }

	if( SDL_SetColorKey(tilesetsurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(tilesetsurface->format, 255, 0, 255)) < 0){
		printf("\n ERROR: Couldn't set ColorKey + RLE: %s\n", SDL_GetError());
		return;
	}

	SDL_Surface *temp;
	if( (temp = SDL_DisplayFormat(tilesetsurface)) == NULL){
		printf("\n ERROR: couldn't convert to display format: %s\n", SDL_GetError());
		return;
	}
	
	SDL_FreeSurface(tilesetsurface);
	tilesetsurface = temp;

	printf("done\n");
}




void CMap::saveTileSet(const char *tilesetfile){
	FILE *mapfile;
	int i;

	printf("saving tileset %s ... ", tilesetfile);

	mapfile = fopen(tilesetfile, "wb");
	if(mapfile == NULL){
		printf("\n ERROR: couldn't open tileset file\n");
		return;
	}

	//save tileset
	for(i = 0; i < TILESETSIZE; i++){
		fwrite(&(tileset[i]), sizeof(TileType), 1, mapfile);
	}
	

	fclose(mapfile);

	printf("done\n");
}


void CMap::clearTileSet(){
	int i;
	for(i=0; i<TILESETSIZE; i++){
		tileset[i] = tile_nonsolid;
	}
	tilebltrect.w = TILESIZE;
	tilebltrect.h = TILESIZE;
}

void CMap::clearMap(){
	int i, j;

	for(j = 0; j < MAPHEIGHT; j++){
		for(i = 0; i < MAPWIDTH; i++){
			//reset tile
			mapdata[i][j] = TILESETSIZE;	//no tile selected
		}
	}
	bltrect.w = TILESIZE;
	bltrect.h = TILESIZE;
}


void CMap::loadMap(const char *file){
	FILE *mapfile;
	int i, j;
	
	printf("loading map %s ... ", file);

	//clear map
	clearMap();
	unsigned int length = 0;
	uint8* ftemp;
	ftemp = (uint8*)gbfs_get_obj(&data_gbfs, (char *)file, &length);	
	/*mapfile = fopen(file, "rb");
	if(!mapfile){
		printf("\n ERROR: couldn't open map\n");
		return;
	}
	
	*/
	//2. load map data
	uint8* a=ftemp;
	for(j = 0; j < MAPHEIGHT; j++){
		for(i = 0; i < MAPWIDTH; i++){
			//fread(&(mapdata[i][j]), sizeof(int), 1, mapfile);
			//&mapdata[i][j]=(int)ftemp++;
			a+=sizeof(int);
			memcpy(&mapdata[i][j],a,sizeof(int));

		}
	}
	//fclose(mapfile);
	
	
	fprintf(stderr,"done\n");
}




void CMap::saveMap(const char *file){
	FILE *mapfile;
	int i, j;

	printf("saving map %s ... ", file);

	mapfile = fopen(file, "wb");
	if(mapfile == NULL){
		printf("\n ERROR: couldn't save map\n");
		return;
	}
	
	//save map
	for(j = 0; j < MAPHEIGHT; j++){
		for(i = 0; i < MAPWIDTH; i++){
			//write index in tileset array
			fwrite(&(mapdata[i][j]), sizeof(int), 1, mapfile);
		}
	}

	fclose(mapfile);

	printf("done\n");
}




void CMap::draw(SDL_Surface *targetsurf){
	int i, j, ts;
	

	//draw left to right full vertical
	bltrect.x = 0;
	for(i = 0; i < MAPWIDTH; i++){
		bltrect.y = -TILESIZE;	//this is okay, see

		for(j = 0; j < MAPHEIGHT; j++){
			bltrect.y += TILESIZE;	//				here

			ts = mapdata[i][j];
			if(ts == TILESETSIZE)
				continue;

			
			tilebltrect.x = (ts%12)*TILESIZE;
			tilebltrect.y = (ts/12)*TILESIZE;
			

			SDL_BlitSurface(tilesetsurface, &tilebltrect, targetsurf, &bltrect);
		}

		bltrect.x += TILESIZE;
	}

	bltrect.x = 0; bltrect.y = 0; bltrect.w = 320, bltrect.h = 240;
}


void CMap::predraw(gfxSprite &background, gfxSprite &mapspr){
	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = 320;
	r.h = 240;

	printf("predrawing map... ");

	SDL_BlitSurface(background.getSurface(), NULL, mapspr.getSurface(), &r);
	
	draw(mapspr.getSurface());

	printf("done!\n");
}

