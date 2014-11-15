#include "allegro.h"
#include "sys.hpp"
#include "formulas.hpp"
#include "math.h"

const int WIDTH  = 640;
const int HEIGHT = 480;

// timing
volatile long int g_timer;
long int old_timer;

void ttimer(  void )
{
   g_timer++;
}
END_OF_FUNCTION(ttimer);



g_system::g_system()
{

   // init timer
   LOCK_VARIABLE( g_timer );
   LOCK_FUNCTION( ttimer ); 
   install_int( ttimer, 1 );
   // timer is ready and running;

   // prepare palette
   int r,g,b;
   get_palette_range( backup_pal, 0, 16 );
   for( int i=16; i<256; i++ )
   {
      hsv_to_rgb( float(i-16)*1.5, 1.0, 1.0, &r, &g, &b );
      backup_pal[i].r = char( (r>>2) + 0.5 );
      backup_pal[i].g = char( (g>>2) + 0.5 );
      backup_pal[i].b = char( (b>>2) + 0.5 );
   }
   for( int i=16; i>=0; i-- )
   {
      shift( true );
      set_color( 16, &default_palette[i] );
   }            

   current_pal = 0;
   // palette is ready

   // init variables
   quit = false;   
   shifting = false;
   shift_up = false;
   help = false;
   formula = 1;
   max_itter = 256;
   left  = -2.0;
   right =  2.0;
   lower = -1.5;
   upper =  1.5;
   update = true;
   dirt = create_bitmap( 11, 11 );
   help_map = create_bitmap( 340, 120 );
   startx = starty = destx = desty = -1;
   // all variables are set
}

g_system::~g_system()
{
   destroy_bitmap( dirt );
   destroy_bitmap( help_map );
}

void g_system::reset_pal( void )
{
   set_palette( default_palette );
   for( int i=16; i>=0; i-- )
   {
      shift( true );
      set_color( 16, &default_palette[i] );
   }            

   current_pal = 0;
}

void g_system::shift( bool up )
{
   PALETTE temp1;
   RGB temp2;
   get_palette( temp1 );
   if( up )
   {
      temp2 = temp1[255];
      for( int i=255; i > 16; i-- )
         temp1[i] = temp1[i-1];
      temp1[16] = temp2;
   }else
   {
      temp2 = temp1[16];
      for( int i=16; i < 255; i++ )
         temp1[i] = temp1[i+1];
      temp1[255] = temp2;
   }   
   set_palette( temp1 );
}

void g_system::change_pal( void )
{
   PALETTE temp;
   get_palette( temp );
   if( current_pal == 0 )
   {
      fade_from_range( temp, backup_pal, 2, 16, 255 );
      current_pal = 1;
   }else
   {
      fade_from_range( temp, default_palette, 2, 16, 255 );
      for( int i=16; i>=0; i-- )
      {
         shift( true );
         set_color( 16, &default_palette[i] );
      }            
      current_pal = 0;
   }
}


/* keylist
 * =======
 * - ESC    quit / abort calculation
 * - 1-6    change formula
 * - r      reset everything to default
 * - SPACE  redraw
 * - c      show palette
 * - p      switch palette
 * - s      palette shifting
 * - <      switch shifting direction
 * - +/-    increase/decrease Itterations
 * - h      toggle help
 */

int g_system::update_logic( void )
{
   clear_keybuf();
   if( key[KEY_ESC] )
   {
      quit = true;
      return 0;
   }
   if( key[KEY_1] && ( formula != MANDEL2 ) )
   {
      formula = MANDEL2;
      update = true;
   }
   if( key[KEY_2] && ( formula != MANDEL3 ) )
   {
      formula = MANDEL3;
      update = true;
   }
   if( key[KEY_3] && ( formula != MANDEL4 ) )
   {
      formula = MANDEL4;
      update = true;
   }
   if( key[KEY_4] && ( formula != MANDEL5 ) )
   {
      formula = MANDEL5;
      update = true;
   }
   if( key[KEY_5] && ( formula != MANDEL6 ) )
   {
      formula = MANDEL6;
      update = true;
   }
   if( key[KEY_6] && ( formula != MANDEL7 ) )
   {
      formula = MANDEL7;
      update = true;
   }

   if( key[KEY_LCONTROL] )
   {
      if( mouse_b & 1 )
      {
         if( startx == -1 )
         {
            startx = mouse_x;
            starty = mouse_y;
         }else
         {
            rect( screen, startx, starty, mouse_x, starty + (mouse_x-startx)*SCREEN_H/SCREEN_W * ( starty - mouse_y ) / ( abs( starty - mouse_y ) + 1 ) * ( startx - mouse_x ) / ( abs( startx - mouse_x ) + 1 ), 4 );
         }
      }else if( startx != -1 )
      {
         int x1, y1;
         int x2 = mouse_x;
//         int y2 = int((float((x2-startx)*SCREEN_H)/float(SCREEN_W))+0.5+starty);
         int y2 = starty + (mouse_x-startx)*SCREEN_H/SCREEN_W * ( starty - mouse_y ) / ( abs( starty - mouse_y ) + 1 ) * ( startx - mouse_x ) / ( abs( startx - mouse_x ) + 1 );
         x1 = (startx + x2 - abs( startx-x2 )) /2;
         y1 = (starty + y2 - abs( starty-y2 )) /2;
         x2 = (startx + x2 + abs( startx-x2 )) /2;
         y2 = (starty + y2 + abs( starty-y2 )) /2;
         double temp;
         temp = left;
         left  = left + x1 * double(right - left)/double(SCREEN_W);
         right = temp + x2 * double(right - temp)/double(SCREEN_W);

         temp = upper;
         upper = upper - y1 * double(upper - lower)/double(SCREEN_H);
         lower = temp  - y2 * double(temp  - lower)/double(SCREEN_H);
         startx = -1;
         update = true;
      }

   }else
    if( mouse_b & 1 )
   {
      startx = mouse_x;
      starty = mouse_y;      

      double width = (right - left);
      right = left + (double)startx*width/double(SCREEN_W) + (width/4.0);
      left  = left + (double)startx*width/double(SCREEN_W) - (width/4.0);

      double height = (upper - lower);
      lower = upper - (double)starty*height/double(SCREEN_H) - (height/4.0);
      upper = upper - (double)starty*height/double(SCREEN_H) + (height/4.0);

      startx = -1;
//      zoom *= 2;
      update = true;
   }
   if( mouse_b & 2 )
   {
      startx = mouse_x;
      starty = mouse_y;

      double width = (right - left);
      right = left + (double)startx*width/double(SCREEN_W) + width;
      left  = left + (double)startx*width/double(SCREEN_W) - width;

      double height = (upper - lower);
      lower = upper - (double)starty*height/double(SCREEN_H) - height;
      upper = upper - (double)starty*height/double(SCREEN_H) + height;

      startx = -1;
//      zoom /= 2;
      update = true;
   }
   if( key[KEY_PLUS_PAD] )
   {
      while( key[KEY_PLUS_PAD] )
      {
         max_itter++;
         textprintf_ex( screen, font, 10, 10, 15, 0, "it: %6d", max_itter );
         rest(10);
      }
      update = true;
   }
   if( key[KEY_MINUS_PAD] )
   {
      while( key[KEY_MINUS_PAD] )
      {
         max_itter--;
         textprintf_ex( screen, font, 10, 10, 15, 0, "it: %6d", max_itter );
         rest(10);
      }
      update = true;
   }
   if( key[KEY_R] )
   {
      reset_pal();
      left  = -2.0;
      right =  2.0;
      lower = -1.5;
      upper =  1.5;
      max_itter = 256;
      update = true;
      shifting = false;
   }
   if( key[KEY_P] )
   {
      change_pal();
   }
   if( key[KEY_S] )
   {
      shifting = !shifting;
      while( key[KEY_S] );
   }
   if( key[KEY_BACKSLASH2] )
   {
      shift_up = !shift_up;
      while( key[KEY_BACKSLASH2] );
   }
   if( key[KEY_C] )
   {
      for( int i=0 ; i< 16; i++ )
         for( int j=0; j< 16; j++ )
            rectfill( screen, j*SCREEN_W/16, i*SCREEN_H/16, (j+1)*SCREEN_W/16 -1, (i+1)*SCREEN_H/16 - 1, j+(i*16) );
      while( key[KEY_C] );
      while( !key[KEY_C] );
      while( key[KEY_C] );
   }
   if( key[KEY_H] )
   {
      help = !help;
      while( key[KEY_H] );
   }
   if( key[KEY_SPACE] )
   {
      update = true;
   }
   
//   while( keypressed() );
   return 0;
}


int g_system::update_gfx( BITMAP* bmp )
{
//   clear( bmp );
//   textprintf_ex(screen, font, 10,100,3,-1,"servus");
   int old_x, old_y;

   if( update )
   {
      old_timer = g_timer;
      switch( formula )
      {
         case 1:{ mandel( bmp, self); break; }
         case 2:{ mandel( bmp, self); break; }
         case 3:{ mandel( bmp, self); break; }
         case 4:{ mandel( bmp, self); break; }
         case 5:{ mandel( bmp, self); break; }         
         case 6:{ mandel( bmp, self); break; }
      }
	   old_timer = g_timer - old_timer;

      // prepare help message
      blit( bmp, help_map, 20, 50, 0, 0, help_map->w, help_map->h );
      // help message ready

      
	   if( old_timer > 100 )
         textprintf_centre_ex( bmp, font, SCREEN_W/2, 10, 15, 0, "%d ms", old_timer );
 
      update = false;
   }

   // palette shifting
   if( shifting )
      shift( shift_up );
      
   textprintf_ex( bmp, font, 10, 10, 15, 0, "it: %6d", max_itter );

   textprintf_ex( bmp, font, SCREEN_W - 160,  5, 15, 0, "mouse x : %1.8f", left  + (double)mouse_x*(right-left) /double(SCREEN_W) );
   textprintf_ex( bmp, font, SCREEN_W - 160, 14, 15, 0, "mouse y : %1.8f", upper - (double)mouse_y*(upper-lower)/double(SCREEN_H) );

   textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H - 45, 15, 0, "zoom : %15.0f", double(4.0/( fabs( right - left ) ) + 0.5 ) );
   textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H - 35, 15, 0, "left : %1.12f", left );
   textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H - 26, 15, 0, "right: %1.12f", right );
   textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H - 17, 15, 0, "upper: %1.12f", upper );
   textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H -  8, 15, 0, "lower: %1.12f", lower );

   if( !help )
      blit( help_map, bmp, 0, 0, 20, 50, help_map->w, help_map->h );

   // display help
   if( help )
   {
      textprintf_ex( bmp, font, 20,  50, 15,  0, "HELP" );
      textprintf_ex( bmp, font, 20,  56, 15, -1, "====" );
      textprintf_ex( bmp, font, 20,  66, 15,  0, "keys:" );
      textprintf_ex( bmp, font, 20,  72, 15, -1, "-----" );
      textprintf_ex( bmp, font, 20,  82, 15,  0, " * ESC    quit / abort calculation" );      
      textprintf_ex( bmp, font, 20,  92, 15,  0, " * 1-6    change formula" );
      textprintf_ex( bmp, font, 20, 102, 15,  0, " * r      reset everything to default" );
      textprintf_ex( bmp, font, 20, 112, 15,  0, " * SPACE  redraw" );
      textprintf_ex( bmp, font, 20, 122, 15,  0, " * +/-    increase/decrease Itterations" );
      textprintf_ex( bmp, font, 20, 132, 15,  0, " * c      show palette" );
      textprintf_ex( bmp, font, 20, 142, 15,  0, " * p      switch palette" );
      textprintf_ex( bmp, font, 20, 152, 15,  0, " * s      palette shifting" );
      textprintf_ex( bmp, font, 20, 162, 15,  0, " * h      toggle help" );
   }

   blit( bmp, dirt, mouse_x-5, mouse_y-5, 0, 0, 11, 11 );   
   hline( bmp, mouse_x-5, mouse_y, mouse_x+5, g_timer%16 );
   vline( bmp, mouse_x, mouse_y-5, mouse_y+5, g_timer%16 ); //8*(g_timer%3)-1*(g_timer%2) );
   old_x = mouse_x-5;
   old_y = mouse_y-5;

   blit( bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
   blit( dirt, bmp, 0, 0, old_x, old_y, 11, 11 );


   return 0;
}

 
