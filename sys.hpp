#include "allegro.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class g_system 
{
public:
   g_system(); 
   ~g_system();
   BITMAP* buffer;   
   int update_gfx( BITMAP* );
   int update_logic( void );
   bool quit;
   int current_pal;
   g_system* self;
   friend int mandel_guessing( BITMAP*, g_system*);
   friend int mandel_no_guessing( BITMAP*, g_system*);
   friend int julia_no_guessing( BITMAP*, BITMAP*, g_system* );
private: 
   int formula;
   int max_itter;
   int shift_delay;
   int current_res;
   int history_pos;
   double left;
   double right;
   double lower;
   double upper;   
	bool julia;
   bool show_palette;
   bool update;
   bool shifting;
   bool shift_dir;
   bool info;
   bool help;
   bool guessing;
   PALETTE backup_pal;
   BITMAP* dirt;
   BITMAP* info_map;
   BITMAP* help_map;
   BITMAP* julia_map;
   int startx, starty, destx, desty;
	double newton_abstand;
	int newton_color1, newton_color2, newton_color3;

   void reset_pal( void );
   void change_pal( void );
   void shift( bool ); 
   void screenshot( void );
   void save( void );
   void load( void );
   int load_file( string filename );
	void add_history( void );
	void clear_history( int first );
};
 
#define NEWTON	 -1
#define MANDEL2   1
#define MANDEL3   2 
#define MANDEL4   3
#define MANDEL5   4
#define MANDEL6   5
#define MANDEL7   6
 
#define UP        true
#define DOWN      false
