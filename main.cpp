#include "allegro.h"
#include "math.h"
#include "sys.hpp"
#include <sys/types.h>
#include <sys/stat.h>



  
using namespace std;



int main( int argc, char *argv[] )
{
   allegro_init();
   install_keyboard();
   install_mouse();
   install_timer();

//   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0 ) )

   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0 ) )
      if( set_gfx_mode( GFX_AUTODETECT, 640, 480, 0, 0 ) )
      {
         set_gfx_mode( GFX_TEXT, 0, 0, 0, 0);
         allegro_message("ERROR setting gfx mode! aborting...");
         return 1;
      }

	textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2, 15, 0, "LOADING..." );

	g_system *sys;
	sys = new g_system;
   
   sys->self = sys;

	do
	{
	    rest(1);
	   sys->update_logic();
	   sys->update_gfx( sys->buffer );
    }while( !sys->quit );
	
	delete( sys );
	
   allegro_exit();
   return 0;
}
END_OF_MAIN() 
