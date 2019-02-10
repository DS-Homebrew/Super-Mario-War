
//this system is pretty cool ;)
//it can be easily changed to handle every object in the game

//eyecandy base class
class CEyecandy{
	public:
		virtual void draw(){printf("CEyecandy::draw() - NO!\n");};
		virtual void update(){printf("CEyecandy::update() - NO!\n");};
		virtual ~CEyecandy(){;};
	protected:
		int index;
	
	friend class CEyecandyContainer;
};





//actual eyecandy implementations
class EC_Corpse : public CEyecandy{
	public:
		EC_Corpse(gfxSprite *nspr, float nx, float ny);
		~EC_Corpse(){;};
		void draw();
		void update();

	private:
		gfxSprite *spr;
		float x, y;
		float vely;
		int timeleft;
};

class EC_Cloud : public CEyecandy{
	public:
		EC_Cloud(gfxSprite *nspr, float nx, float ny, float nvelx);
		~EC_Cloud(){;};
		void draw();
		void update();

	private:
		gfxSprite *spr;
		float x, y;
		float velx;
		int w;
};

class EC_GravText : public CEyecandy{
	public:
		EC_GravText(gfxFont *font, int nx, int ny, const char *text, float nvely);
		~EC_GravText();
		void draw();
		void update();

	private:
		gfxFont *font;
		float x, y;
		float vely;
		char *text;
};

class EC_GameOverMario : public CEyecandy{
	public:
		EC_GameOverMario(gfxSprite *nspr, int nx, int ny, float nvely);
		void draw();
		void update();

	private:
		gfxSprite *spr;
		float x, y;
		float vely;
};




//eyecandy container
class CEyecandyContainer{
	public:
		CEyecandyContainer();
		~CEyecandyContainer();

		void add(CEyecandy *ec);

		void update(){
			for(int i = 0; i<list_end; i++)
				list[i]->update();
		};
		void draw(){
			for(int i = 0; i<list_end; i++)
				list[i]->draw();
		};

		void clean();

	private:
		CEyecandy *list[MAXEYECANDY];
		int		 list_end;

		void remove(int i);
		friend class EC_Corpse;
		friend class EC_GravText;
		friend class EC_GameOverMario;
};

