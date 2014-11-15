#include "allegro.h" 
#include "math.h"
#include <deque>
#include "sys.hpp"

void formula( double &re, double old_re, double &im, double old_im, int expo )
{
  
   double temp = im;
/*
//old school
   switch( expo )
   {
      case 2:{ im = 2*re*im + old_im; re = re*re - temp*temp + old_re; break; }
      case 3:{ im = 3*re*re*im - im*im*im + old_im; re = re*re*re - 3*re*temp*temp + old_re; break; }
      case 4:{ im = 4*re*re*re*im  - 4*re*im*im*im; re = re*re*re*re - 6*re*re*temp*temp + temp*temp*temp*temp + old_re; break; }
      case 5:{ im = 5*re*re*re*re*im - 10*re*re*im*im*im + im*im*im*im*im + old_im; re = re*re*re*re*re - 10*re*re*re*temp*temp + 5*re*temp*temp*temp*temp + old_re; break; }
      case 6:{ im = 6*re*re*re*re*re*im - 20*re*re*re*im*im*im + 6*re*im*im*im*im*im + old_im; re = re*re*re*re*re*re - 15*re*re*re*re*temp*temp + 15*re*re*temp*temp*temp*temp - temp*temp*temp*temp*temp*temp + old_re; break; }
      case 7:{ im = 7*re*re*re*re*re*re*im - 35*re*re*re*re*im*im*im + 21*re*re*im*im*im*im*im - im*im*im*im*im*im*im + old_im; re = re*re*re*re*re*re*re - 21*re*re*re*re*re*temp*temp + 35*re*re*re*temp*temp*temp*temp - 7*re*temp*temp*temp*temp*temp*temp + old_re; break; }
   }
*/

//optimized
   switch( expo )
   {
      case 2:{ im = 2*re*im + old_im; re = re*re - temp*temp + old_re; break; }
      case 3:{ im = im*( 3*re*re - im*im) + old_im; re = re*( re*re - 3*temp*temp ) + old_re; break; }
      case 4:{ im = 4*re*im*(re*re - im*im) + old_im; re = re*re*re*re - 6*re*re*temp*temp + temp*temp*temp*temp + old_re; break; }
      case 5:{ im = im*( 5*re*re*( re*re - 2*im*im ) + im*im*im*im ) + old_im; re = re*( re*re*re*re - ( 5*temp*temp*( 2*re*re - temp*temp ) ) ) + old_re; break; }
      case 6:{ im = re*im*( re*re*( 6*re*re - 20*im*im ) + 6*im*im*im*im ) + old_im; re = re*re*re*re*( re*re - 15*temp*temp ) + temp*temp*temp*temp*( 15*re*re - temp*temp ) + old_re; break; }
      case 7:{ im = im*( 7*re*re*( re*re*( re*re - 5*im*im ) + 3*im*im*im*im ) - im*im*im*im*im*im ) + old_im; re = re*( re*re*re*re*re*re - ( 7*temp*temp*( re*re*( 3*re*re - 5*temp*temp ) + temp*temp*temp*temp ) ) ) + old_re; break; }
   }


}




int mandel( BITMAP* bmp, g_system* system)
{   
   double re, old_re, im, old_im, temp, scale_x, scale_y;
   double y_val[SCREEN_H];
   int it;
   scale_x = double(system->right - system->left)/double(SCREEN_W);
   scale_y = double(system->upper - system->lower)/double(SCREEN_H);

   for( int i=0; i<SCREEN_H; i++ )
   {
      y_val[i] = system->upper - i*scale_y;
   }
   for( int i=0; i<SCREEN_W; i++ )
   {
      if( key[KEY_ESC] )
      {
         while( keypressed() );
         break;
      }
      old_re = system->left + i*scale_x;
      textprintf_ex( screen, font, 10, SCREEN_H-10, 15, 0, "%3.3f %", 100.0*i/SCREEN_W );
      for( int j=0; j<SCREEN_H; j++)
      {
         re = old_re;
         im = old_im = y_val[j];
         it=0;

         while( ( re*re + im*im < 4.0 ) && ( it < system->max_itter ) )
         {
            formula( re, old_re, im, old_im, system->formula + 1 );
            
            clear_keybuf();
            if( key[KEY_ESC] )
            {
               while( key[KEY_ESC] );
               return 0;
            }
            
            it++;
         }
         if( it == system->max_itter )
            it = -16;
         acquire_bitmap( bmp );
         putpixel( bmp, i, j, 16 + it%240 );
         release_bitmap( bmp );
      }
   }

   return 0;
}

