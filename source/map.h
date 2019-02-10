
#ifndef _MAP_H
#define _MAP_H



enum TileType{tile_nonsolid = 0, tile_solid = 1, tile_solid_on_top = 2};


class CMap{
	public:
		void loadTileSet(const char *tilesetfile, const char *tilesetbmp);
		void saveTileSet(const char *tilesetfile);
		void clearTileSet();

		void clearMap();
		void loadMap(const char *file);
		void saveMap(const char *file);

		void predraw(gfxSprite &background, gfxSprite &mapspr);

		~CMap();

		TileType map(int x, int y){			//return the tiletype at the specific position (map coordinates)
			return tileset[mapdata[x][y]];
		}

	private:
		int			mapdata[MAPWIDTH][MAPHEIGHT];		//0 to TILESTESIZE-1: tiles, TILESETSIZE: no tile

		TileType	tileset[TILESETSIZE+1];
		SDL_Surface	*tilesetsurface;
		SDL_Rect	tilebltrect;
		SDL_Rect bltrect;


		void draw(SDL_Surface *targetsurf);


		friend int editor_edit();
		friend int editor_tiles();
};



#endif

