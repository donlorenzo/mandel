#include "allegro.h" 
#include "math.h"
#include "sys.hpp"

inline void formula( double &re, double old_re, double &im, double old_im, int expo );


int julia( double re, double old_re, double im, double old_im, int max_itter, int cur_formula )
{
    double temp;
    int it = 0;
    while( ( re*re + im*im < 4.0 ) && ( it < max_itter ) )
    {
	    temp = im;
		im = 2*re*im + old_im;
 		re = re*re - temp*temp + old_re;    
//		im = temp*re*re*temp + 2*old_im;
//		re = re*re*re - temp*temp*re + old_im*old_re;
//		formula( re, old_re, im, old_im, cur_formula );
        if( key[KEY_ESC] )
        	return -1;    
        it++;
    }
    if( it == max_itter )
        it = -16;

    return( 16 + (it%240) );
}    


void newton_formula( double &re, double old_re, double &im, double old_im )
{
/*
	f(z) = z^3 - 1
	=> re = re^3 - 3*re*im^2 - 1
		im = 3*re^2*im - im^3
	f'(z) = 3*z^2
	=> re = re^2 - im^2
		im = 2*re*im
	z1 = z0 - ( f(z0)/f'(z0) );
*/
	double a,b;
	a=re;
	b=im;

	re = (((a+b)*(a-b))/(3.0L*(a*a+b*b)*(a*a+b*b))) + 2.0L*a/3.0L;
	im = (2.0L*b/3.0L) - ((2.0L*a*b)/(3.0L*(a*a+b*b)*(a*a+b*b)));
	
}


inline void formula( double &re, double old_re, double &im, double old_im, int expo )
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

//optimized step 1
   switch( expo )
   {
		case 0: { newton_formula( re, old_re, im, old_im ); break; }
      case 2:{ im = 2*re*im + old_im; re = re*re - temp*temp + old_re; break; }
      case 3:{ im = im*( 3*re*re - im*im) + old_im; re = re*( re*re - 3*temp*temp ) + old_re; break; }
      case 4:{ im = 4*re*im*(re*re - im*im) + old_im; re = re*re*re*re - 6*re*re*temp*temp + temp*temp*temp*temp + old_re; break; }
      case 5:{ im = im*( 5*re*re*( re*re - 2*im*im ) + im*im*im*im ) + old_im; re = re*( re*re*re*re - ( 5*temp*temp*( 2*re*re - temp*temp ) ) ) + old_re; break; }
      case 6:{ im = re*im*( re*re*( 6*re*re - 20*im*im ) + 6*im*im*im*im ) + old_im; re = re*re*re*re*( re*re - 15*temp*temp ) + temp*temp*temp*temp*( 15*re*re - temp*temp ) + old_re; break; }
      case 7:{ im = im*( 7*re*re*( re*re*( re*re - 5*im*im ) + 3*im*im*im*im ) - im*im*im*im*im*im ) + old_im; re = re*( re*re*re*re*re*re - ( 7*temp*temp*( re*re*( 3*re*re - 5*temp*temp ) + temp*temp*temp*temp ) ) ) + old_re; break; }
   }

}


int newton( double _re, double _im, int cur_formula, int max_itter, double newton_abstand, int color1, int color2, int color3 )
{
    double re, old_re, im, old_im;
	re = old_re = _re;
    im = old_im = _im;
    int it=0;

	while( it < max_itter )
    {
    	formula( re, old_re, im, old_im, cur_formula + 1 );

		if( ( fabs( re-1.0f ) < newton_abstand ) && ( fabs( im ) < newton_abstand ) )
			return( 16 + ( (color1+it)%240) );

   		if( ( fabs( re+0.5f ) < newton_abstand ) && ( fabs( im - 0.866025403784f ) < newton_abstand ) )
			return( 16 + ( (color2+it)%240) );

   		if( ( fabs( re+0.5f ) < newton_abstand ) && ( fabs( im + 0.866025403784f ) < newton_abstand ) )
			return( 16 + ( (color3+it)%240) );

        clear_keybuf();
        if( key[KEY_ESC] )
        {
           while( key[KEY_ESC] );
           return -1;
        }
            
        it++;
	}
	return 0;
}    

 
int mandel( double _re, double _im, int cur_formula, int max_itter )
{
	double re, im, old_re, old_im, temp;
	int it = 0;

    re = old_re = _re;
    im = old_im = _im;
    while( ( re*re + im*im < 4.0 ) && ( it < max_itter ) )
    {
		formula( re, old_re, im, old_im, cur_formula + 1 );
//		temp = im;
//        im = 2*re*im + old_im; re = re*re - temp*temp + old_re; 
        clear_keybuf();
        if( key[KEY_ESC] )
        {
            while( key[KEY_ESC] );
            return -1;
        }
            
        it++;
    }
    if( it == max_itter )
        it = -16;

    return( 16 + (it%240) );
}    

 
int mandel_guessing( BITMAP* bmp, g_system* system)
{
   clear_bitmap( bmp );
   double re, old_re, im, old_im, temp, scale_x, scale_y;
   double y_val[SCREEN_H];
   int it;
   scale_x = double(system->right - system->left)/double(SCREEN_W);
   scale_y = double(system->upper - system->lower)/double(SCREEN_H);

   for( int i=0; i<SCREEN_H; i++ )
   {
      y_val[i] = system->upper - i*scale_y;
   }
   for( int i=0; i<SCREEN_W; i+=2 )
   {
      old_re = system->left + i*scale_x;
      textprintf_ex( screen, font, 10, SCREEN_H-17, 15, 0, "%3.3f%%", 25.0*i/SCREEN_W );
      for( int j=0; j<SCREEN_H; j+=2)
      {
		int color;
		if( system->formula == NEWTON )
			color = newton( old_re, y_val[j], system->formula, system->max_itter, system->newton_abstand, system->newton_color1, system->newton_color2, system->newton_color3 );
   		else
     		color = mandel( old_re, y_val[j], system->formula, system->max_itter );
		if( color == -1 )
			return 1;

         acquire_bitmap( bmp );     		
         _putpixel( bmp, i, j, color);
         release_bitmap( bmp );
      }
   }

   blit( bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
   for( int i=0; i<SCREEN_W; i+=2 )
   {
      old_re = system->left + i*scale_x;
      textprintf_ex( screen, font, 10, SCREEN_H-17, 15, 0, "%3.3f%%", 25.0+25.0*i/SCREEN_W );
      for( int j=1; j<SCREEN_H-1; j+=2)
      {
         if( bmp->line[j-1][i]==bmp->line[j+1][i] )
         {
            acquire_bitmap( bmp );
            bmp->line[j][i] = bmp->line[j-1][i];
            release_bitmap( bmp );
         }else
         {
			int color;
			if( system->formula == NEWTON )
				color = newton( old_re, y_val[j], system->formula, system->max_itter, system->newton_abstand, system->newton_color1, system->newton_color2, system->newton_color3 );
	   		else
    	 		color = mandel( old_re, y_val[j], system->formula, system->max_itter );
			if( color == -1 )
				return 1;

        
          	 acquire_bitmap( bmp );
    	     _putpixel( bmp, i, j, color );
	         release_bitmap( bmp );
         }
      }
   }

   blit( bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
   for( int i=1; i<SCREEN_W-1; i+=2 )
   {
      old_re = system->left + i*scale_x;
      textprintf_ex( screen, font, 10, SCREEN_H-17, 15, 0, "%3.3f%%", 50.0+50.0*i/SCREEN_W );
      for( int j=1; j<SCREEN_H; j++)
      {
         if( bmp->line[j][i-1] == bmp->line[j][i+1] )
         {
            acquire_bitmap( bmp );
            bmp->line[j-1][i] = bmp->line[j-1][i-1];
            release_bitmap( bmp );
         }else
         {
			int color;
			if( system->formula == NEWTON )
				color = newton( old_re, y_val[j], system->formula, system->max_itter, system->newton_abstand, system->newton_color1, system->newton_color2, system->newton_color3 );
	   		else
    	 		color = mandel( old_re, y_val[j], system->formula, system->max_itter );
			if( color == -1 )
				return 1;

	         acquire_bitmap( bmp );
    	     _putpixel( bmp, i, j-1, color );
        	 release_bitmap( bmp );
         }
      }
   }
   return 0;
}


int mandel_no_guessing( BITMAP* bmp, g_system* system)
{   
   clear_bitmap( bmp );
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
      old_re = system->left + i*scale_x;
//      textprintf_ex( screen, font, 10, SCREEN_H-17, 15, 0, "%3.3f%%", 100.0f*i/SCREEN_W );
      for( int j=0; j<SCREEN_H; j++)
      {
		int color;
  		if( system->formula == NEWTON )
			color = newton( old_re, y_val[j], system->formula, system->max_itter, system->newton_abstand, system->newton_color1, system->newton_color2, system->newton_color3 );
   		else
     		color = mandel( old_re, y_val[j], system->formula, system->max_itter );
  		if( color == -1 )
			return 1;
		

         acquire_bitmap( bmp );
		 _putpixel( bmp, i, j, color );
         release_bitmap( bmp );

      }
   }

   blit( bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

   return 0;
}

int julia_no_guessing( BITMAP* bmp, BITMAP* background, g_system* system )
{
    clear_bitmap( bmp );
   double re, old_re, im, old_im, temp, scale_x, scale_y;
   double y_val[bmp->h];
   int it;
   scale_x = double(4.0f)/double(bmp->w);
   scale_y = double(3.0f)/double(bmp->h);
	double x, y;
	x = system->left + (double)(mouse_x)*double(system->right - system->left)/double(SCREEN_W);
	y = system->upper  - (double)(mouse_y)*double(system->upper - system->lower)/double(SCREEN_H);
   for( int i=0; i<bmp->h; i++ )
   {
      y_val[i] = 1.5f - i*scale_y;
   }
   for( int i=0; i<bmp->w; i++ )
   {
      old_re = i*scale_x - 2.0f;
      for( int j=0; j<bmp->h; j++)
      {
		int color;
		//julia:
		color = julia( old_re, x, y_val[j], y, 256, system->formula+1 );
		if( key[KEY_ESC] )
        	return -1;		
  		if( color == -1 )
			return 1;
         acquire_bitmap( bmp );
		 _putpixel( bmp, i, j, color );
         release_bitmap( bmp );

      }
   }
   blit( bmp, background, 0, 0, 0, 0, bmp->w, bmp->h );

   return 0;
}
   
