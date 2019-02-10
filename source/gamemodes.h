
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//   if you delete other in playerkilledplayer then return true!
//   you don't want memory faults, do you?
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// maybe someone somewhen thinks of a nicer way how to do this.



#define GAMEMODE_LAST 5


/*
  the game mode thingy is still not perfect, because when writing
  a new gamemode you are kind of concurring with the simpel deathmatch mode.

  alrady now what will be changed to eliminate that problem, but i
  don't want to do it now.

  i'll probably change that when adding multkill etc score awards...
  ..hmm.... bring me to another idea... the game mode has to keep track of the scores...
  ... hmm... maybe that causes too much problems... maybe not... hmmm... better think
  about that ;)
*/



//gamemode base class ( = endless play)
class CGameMode{
	public:
		virtual void init(){;};		//called once when the game is started
		virtual void think(){;};	//called once a frame
		virtual void draw(){;};
		//called when a player stomps another player, after the p2p logic has run
		//returns true if the other was deleted
		virtual bool playerkilledplayer(CPlayer &inflictor, CPlayer &other){return false;};


		virtual char *getMenuString(char *buffer64){return "no limit";};
		virtual void toggleoptions(){;};
};



//Fraglimit
class CGM_DM_Fraglimit : public CGameMode{
	public:
		CGM_DM_Fraglimit(){
			fraglimit = 20;
			winner = NULL;
		};
		void init();
		void think();
		bool playerkilledplayer(CPlayer &inflictor, CPlayer &other);
		char *getMenuString(char *buffer64);
		void toggleoptions();
	private:
		CPlayer *winner;
		int fraglimit;
		
};

//Timelimit
class CGM_DM_Timelimit : public CGameMode{
	public:
		CGM_DM_Timelimit(){
			timelimit = 30;
			winner = NULL;
		};
		void init();
		void think();
		void draw();
		bool playerkilledplayer(CPlayer &inflictor, CPlayer &other);
		char *getMenuString(char *buffer64);
		void toggleoptions();
	private:
		CPlayer *winner;
		int timelimit;
		int timeleft;
		int framesleft_persecond;
};

//mariowar classic
class CGM_MarioWar : public CGameMode{
	public:
		CGM_MarioWar(){
			lives = 10;
			winner = NULL;
		};
		void init();
		void think();
		bool playerkilledplayer(CPlayer &inflictor, CPlayer &other);
		char *getMenuString(char *buffer64);
		void toggleoptions();
	private:
		CPlayer *winner;
		int lives;
};

//capture the chicken
class CGM_CaptureTheChicken : public CGameMode{
		public:
		CGM_CaptureTheChicken();
		void init();
		void think();
		void draw();
		bool playerkilledplayer(CPlayer &inflictor, CPlayer &other);
		char *getMenuString(char *buffer64);
		void toggleoptions();
	private:
		CPlayer *winner;
		CPlayer *chicken;
		int frames;
};