class g_system 
{
public:
   g_system(); 
   ~g_system();
   int update_gfx( BITMAP* );
   int update_logic( void );
   bool quit;
   int current_pal;
   g_system* self;
   friend int mandel( BITMAP*, g_system*);
     
private:
   int formula;
   int max_itter;
   double left;
   double right;
   double lower;
   double upper;   
   bool update;
   bool shifting;
   bool shift_up;
   bool help;
   PALETTE backup_pal;
   BITMAP* dirt;
   BITMAP* help_map;
   int startx, starty, destx, desty;


   void reset_pal( void );
   void change_pal( void );
   void shift( bool );
};
 

#define MANDEL2   1
#define MANDEL3   2 
#define MANDEL4   3
#define MANDEL5   4
#define MANDEL6   5
#define MANDEL7   6
