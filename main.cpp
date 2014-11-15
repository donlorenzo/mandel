#include "allegro.h"
#include "math.h"
#include "sys.hpp"
  
using namespace std;







int main( int argc, char *argv[] )
{
   allegro_init();
   install_keyboard();
   install_mouse();
   install_timer();

   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0 ) )
      if( set_gfx_mode( GFX_AUTODETECT, 640, 480, 0, 0 ) )
      {
         set_gfx_mode( GFX_TEXT, 0, 0, 0, 0);
         allegro_message("ERROR setting gfx mode! aborting...");
         return 1;
      }

          
   BITMAP* buffer = create_bitmap( SCREEN_W, SCREEN_H );

   g_system *sys;
	sys = new g_system;
   
   sys->self = sys;

	do
	{
	   sys->update_logic();
	   sys->update_gfx( buffer );
   } while( !sys->quit );

   destroy_bitmap( buffer ); 
   allegro_exit();
   return 0;
}
END_OF_MAIN() 
