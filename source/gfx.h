/*----------------------------------------------+
| gfx							Florian Hufsky	|
|												|
| start:		14.12.2003						|
| last changes:	22.12.2003						|
+----------------------------------------------*/
/*--------------------------------------------------------------+
| gfx															|
|																|
| a stripped down version of tfree/free  for a pixelate article	|
|																|
| gfx is a mini graphics library containing						|
|  + initialisation of SDL										|
|  + a class for simple Sprites									|
|  + a class for animated Sprites								|
|																|
| have a lot of fun!											|
|					© 2003 Florian Hufsky <fhufsky@phorus.at>	|
+--------------------------------------------------------------*/

#ifndef __GFX_H__
#define __GFX_H__


#include "SDL.h"
#include "SFont.h"


bool gfx_init(int w, int h, bool fullscreen);
void gfx_setresolution(int w, int h, bool fullscreen);


class gfxSprite{
	public:
		gfxSprite();
		~gfxSprite();

		bool init(const char *filename, Uint8 r, Uint8 g, Uint8 b);	//color keyed
		bool init(const char *filename);							//non color keyed

		bool draw(int x, int y);


		int getWidth(){return m_picture->w;};
		int getHeight(){return m_picture->h;};

		SDL_Surface *getSurface(){return m_picture;};
	private:
		SDL_Surface *m_picture;
		SDL_Rect m_bltrect;
};


class gfxFont{
	public:
		~gfxFont(){if(m_font)SFont_FreeFont(m_font);};

		bool init(const char *filename);
		void draw(int x, int y, const char *s);
		void drawf(int x, int y, char *s, ...);

		void drawCentered(int y, const char *text);


		int getHeight(){return SFont_TextHeight(m_font);};
		int getWidth(const char *text){return SFont_TextWidth(m_font, text);};

	private:
		SFont_Font *m_font;
};

#endif

