#include "sys.hpp"
#include <stdlib.h>
#include "formulas.hpp"

#ifdef WIN32
#  include <dirent.h>
#else
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <dirent.h>
#endif

using namespace std;

//const int WIDTH  = 640;
//const int HEIGHT = 480;

static unsigned int createDirectory(const std::string &dir, int mode);

// timing
volatile long int g_timer;
volatile long int old_timer;
volatile long int shift_timer;

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
        shift( UP );
        set_color( 16, &default_palette[i] );
    }

    current_pal = 0;
    // palette is ready

    // init variables
	createDirectory( "temp", 0700 );
    quit = false;
    show_palette = false;
    history_pos = -1;
    shifting = false;
    shift_dir = DOWN;
    shift_delay = 10;
    info = false;
    help = false;
    guessing = true;
    julia = true;
    formula = 1;
    max_itter = 256;
    left  = -2.0;
    right =  2.0;
    lower = -1.5;
    upper =  1.5;
    update = true;
    buffer = create_bitmap( 800, 600 );
    dirt = create_bitmap( 11, 11 );
    info_map = create_bitmap( 800, 600 );
    help_map = create_bitmap( 390, 210 );
    julia_map = create_bitmap( SCREEN_W/3, SCREEN_H/3 );
    startx = starty = destx = desty = -1;
    current_res = 0;
    newton_abstand = 0.0001L;
	newton_color1 = 240;
	newton_color2 = 70;
	newton_color3 = 155;

    // all variables are set
    change_pal();
    add_history();
}

g_system::~g_system()
{
    clear_history( 0 );
	rmdir( "temp" );

    destroy_bitmap( dirt );
    destroy_bitmap( help_map );
    destroy_bitmap( buffer );
}
void g_system::add_history( void )
{
	clear_history( history_pos+1 );

	history_pos++;
	int settingnumber = 0;
	string path;
	char buf[33];
	do
	{
        path = "temp/history";
		if( settingnumber < 100 )
		{
		    path += "0";
		    if( settingnumber < 10 )
		    {
		        path += "0";
		    }
		}
        sprintf( buf, "%d", settingnumber );
        path += buf;
      	//path += itoa( settingnumber, buf, 10 );
		path += ".txt";

		settingnumber++;
	}while( exists( path.c_str() ) );


    std::ofstream OUT ( path.c_str() );
	OUT << "DO NOT MESS WITH THIS FILE!" << endl;
	OUT << "===========================" << endl;
    OUT << "formula: " << formula << endl;
    OUT << "itterations: " << max_itter << endl;
    OUT << "current resolution: " << current_res << endl;
    OUT << "left: " << left << endl;
    OUT << "right: " << right << endl;
    OUT << "bottom: " << lower << endl;
    OUT << "top: " << upper << endl;
    OUT << "shifting: " << shifting << endl;
    OUT << "shifting direction: " << shift_dir << endl;
	OUT << "shifting delay: " << shift_delay << endl;
    OUT << "guessing: " << guessing << endl;
	OUT << "current palette: " << current_pal << endl;
}

void g_system::clear_history( int first )
{
    int settingnumber = first;
    string path = "";
    char buf[33];

    path = "temp/history";
	if( settingnumber < 100 )
	{
	    path += "0";
	    if( settingnumber < 10 )
	    {
	        path += "0";
	    }
	}
    sprintf( buf, "%d", settingnumber );
    path += buf;
//  	path += itoa( settingnumber, buf, 10 );
	path += ".txt";

    while( exists( path.c_str() ) )
    {
		remove( path.c_str() );
		settingnumber++;

        path = "temp/history";
		if( settingnumber < 100 )
		{
		    path += "0";
		    if( settingnumber < 10 )
		    {
		        path += "0";
		    }
		}
        sprintf( buf, "%d", settingnumber );
        path += buf;
//  	path += itoa( settingnumber, buf, 10 );
		path += ".txt";
	}
}

void g_system::save( void )
{
	createDirectory( "fractals", 0700 );
	int settingnumber = 0;
	string path;
	char buf[33];
	do
	{
	    path = "fractals/fractal";
		if( settingnumber < 100 )
		{
		    path += "0";
	    	if( settingnumber < 10 )
		    {
		        path += "0";
	    	}
		}
        sprintf( buf, "%d", settingnumber );
        path += buf;
//  	path += itoa( settingnumber, buf, 10 );
		path += ".txt";

		settingnumber++;
	}while( exists( path.c_str() ) );


    std::ofstream OUT ( path.c_str() );
	OUT << "DO NOT MESS WITH THIS FILE!" << endl;
	OUT << "===========================" << endl;
    OUT << "formula: " << formula << endl;
    OUT << "itterations: " << max_itter << endl;
    OUT << "current resolution: " << current_res << endl;
    OUT << "left: " << left << endl;
    OUT << "right: " << right << endl;
    OUT << "bottom: " << lower << endl;
    OUT << "top: " << upper << endl;
    OUT << "shifting: " << shifting << endl;
    OUT << "shifting direction: " << shift_dir << endl;
	OUT << "shifting delay: " << shift_delay << endl;
    OUT << "guessing: " << guessing << endl;
	OUT << "current palette: " << current_pal << endl;

  	textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2, 4, 0, "settings are saved to: %s", path.c_str() );
	while( !keypressed() );
}

int g_system::load_file( string filename )
{
	if( !exists( filename.c_str() ) )
	{
//	    log_error( "ERROR: could not load file ""%s""! File does not exist!!\n", filename.c_str() );
	    return 1;
	}
	std::ifstream IN ( filename.c_str() );

	string temp;
	for( int i=0; i<9; i++)
 		IN >> temp;

	formula = atoi( temp.c_str() );

	IN >> temp;
	IN >> temp;
	max_itter = atoi( temp.c_str() );

	IN >> temp;
	IN >> temp;
	IN >> temp;
	current_res = atoi( temp.c_str() );

	IN >> temp;
	IN >> temp;
	left = atof( temp.c_str() );

	IN >> temp;
	IN >> temp;
	right = atof( temp.c_str() );

	IN >> temp;
	IN >> temp;
	lower = atof( temp.c_str() );

	IN >> temp;
	IN >> temp;
	upper = atof( temp.c_str() );

	IN >> temp;
	IN >> temp;
	shifting = atoi( temp.c_str() );

	IN >> temp;
	IN >> temp;
	IN >> temp;
	shift_dir = atoi( temp.c_str() );

	IN >> temp;
	IN >> temp;
	IN >> temp;
	shift_delay = atoi( temp.c_str() );

	IN >> temp;
	IN >> temp;
	guessing = atoi( temp.c_str() );

	IN >> temp;
	IN >> temp;
	IN >> temp;
	if( atoi( temp.c_str() ) != current_pal )
		change_pal();

	update = true;
	return 0;
}

void g_system::load( void )
{
	clear_bitmap( screen );

	if( file_exists( "fractals", FA_DIREC, NULL ) == 0 )
	{
 		textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2, 4, 15, "the directory \"fractals\" does not exist! setting files need to be stored there!" );
		while( !keypressed() );
		return;
	}

	char filename[512];
	int a;
	//get the full path and cut of the executable
	get_executable_name( filename, 510 );
	a = strlen( filename );
	while( filename[a] != '\\' )
		a--;
	filename[a+1] = '\0';
	string path = filename;
 	path += "fractals\\fractal*.txt";

	al_ffblk info;
	BITMAP* sel_screen = create_bitmap( SCREEN_W, SCREEN_H );
	textprintf_ex( sel_screen, font, 30, 10, 15,  0, "select a file:" );
	textprintf_ex( sel_screen, font, 30, 18, 15, -1, "==============" );

	vector<string> files;
	files.clear();
	if( !al_findfirst( path.c_str(), &info, -1 ) )
		files.push_back( info.name );
	else
	{
 		textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2, 15, 0, "ERROR: did not find valid files in directory %s!", path.c_str() );
		destroy_bitmap( sel_screen );
		while( !keypressed() );
        return;
	}

	while( !al_findnext( &info ) )
		files.push_back( info.name );
	al_findclose( &info );

	for( int i=0; i<files.size(); i++ )
		textprintf_ex( sel_screen, font, 30, 30+i*10, 15, 0, files[i].c_str() );

	int pos = 0;
	while( !key[KEY_ENTER] )
	{
	    if( key[KEY_ESC] )
	    {
	        destroy_bitmap( sel_screen );
			while( key[KEY_ESC] );
	        return;
	    }
		textprintf_ex( sel_screen, font, 10, 30+pos*10, 5, 0, "  " );
		if( ( key[KEY_UP] ) && ( pos > 0 ) )
		{
  			pos--;
  			while( key[KEY_UP] );
		}
		if( ( key[KEY_DOWN] ) && ( pos < files.size()-1 ) )
		{
			pos++;
			while( key[KEY_DOWN] );
		}
		textprintf_ex( sel_screen, font, 10, 30+pos*10, 5, 0, "->" );
		blit( sel_screen, screen, 0,0,0,0,SCREEN_W, SCREEN_H );
	}
	destroy_bitmap( sel_screen );

	if( !load_file( "fractals/" + files[pos] ) )
		add_history();
}

void g_system::screenshot( void )
{
//	if( file_exists( "screenshots", FA_DIREC, NULL ) == 0 )
	createDirectory( "screenshots", 0700 );
	string path;
	int shotnumber = 0;
	do
	{
		char buf[33];
		if( shotnumber < 100 )
		{
  			path = "screenshots/shot0";
			if( shotnumber < 10 )
  			{
     			path += "0";
                sprintf( buf, "%d", shotnumber );
                path += buf;
        		//path += itoa( shotnumber, buf, 10 );
                path += ".bmp";
  			}else
  			{
                sprintf( buf, "%d", shotnumber );
                path += buf;
        		//path += itoa( shotnumber, buf, 10 );
     			path += ".bmp";
  			}
		}else
		{
			path = "screenshots/shot";
            sprintf( buf, "%d", shotnumber );
            path += buf;
            //path += itoa( shotnumber, buf, 10 );
     		path += ".bmp";
  		}

		shotnumber++;
	}while( exists( path.c_str() ) );
	PALETTE pal;
	get_palette( pal );
	BITMAP* temp = create_sub_bitmap( screen, 0, 0, SCREEN_W, SCREEN_H);
	if( save_bitmap( path.c_str(), temp, pal ) )
	{
		textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2, 3, 0, "ERROR: could not take screenshot!!!" );
		textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2+10, 3, 0, ": %s", path.c_str() );
  		while( !keypressed() );
	}
	destroy_bitmap( temp );
}

void g_system::reset_pal( void )
{
    set_palette( default_palette );
    for( int i=16; i>=0; i-- )
    {
        shift( UP );
        set_color( 16, &default_palette[i] );
    }

    current_pal = 0;
}

void g_system::shift( bool dir )
{
    PALETTE temp_pal;
    RGB temp_index;
    get_palette( temp_pal );
    if( dir == UP )
    {
        temp_index = temp_pal[255];
        for( int i=255; i > 16; i-- )
            temp_pal[i] = temp_pal[i-1];
        temp_pal[16] = temp_index;
    }else
    {
        temp_index = temp_pal[16];
        for( int i=16; i < 255; i++ )
            temp_pal[i] = temp_pal[i+1];
        temp_pal[255] = temp_index;
    }
    set_palette( temp_pal );
    shift_timer = g_timer + shift_delay;
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
 * - ESC            quit / abort calculation
 * - 1-6            change formula
 * - r              reset everything to default
 * - SPACE          redraw
 * - c              show palette
 * - p              switch palette
 * - s              palette shifting
 * - <              switch shifting direction
 * - ,/.            de-/increase shifting delay
 * - +/-            increase/decrease Itterations
 * - h              toggle help
 * - g              switch guessing mode
 * - ALT+ENTER      toggle Fullscreen mode
 * - F5			    save settings
 * - F6			    load settings
 * - F12            take a screenshot
 * - i              show information
 * - ALT+RIGHT/LEFT move forth and back in history
 */

int g_system::update_logic( void )
{
    clear_keybuf();
    if( key[KEY_ESC] )
    {
        quit = true;
        return 0;
    }
    if( key[KEY_ALT] && key[KEY_ENTER])
    {
        PALETTE temp;
        get_palette( temp );
        if( current_res == 0 )
        {
            if( set_gfx_mode( GFX_AUTODETECT_FULLSCREEN, 800, 600, 0, 0 ) )
            {
                if( set_gfx_mode( GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0 ) )
                    set_gfx_mode( GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0 );
            }else
                current_res = 1;
        }else
        {
            set_gfx_mode( GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0 );
            current_res = 0;
        }
        set_palette( temp );
        update = true;
    }
    if( key[KEY_F5] )
    {
        save();
        while( key[KEY_F5] );
    }
    if( key[KEY_F6] )
    {
    	load();
	   	while( key[KEY_F6] );
    }
    if( key[KEY_F12] )
    {
   		screenshot();
   		while( key[KEY_F11] );
    }
    if( key[KEY_0] && ( formula != NEWTON ) )
    {
        formula = NEWTON;
		add_history();
        update = true;
    }
    if( key[KEY_1] && ( formula != MANDEL2 ) )
    {
        formula = MANDEL2;
		add_history();
        update = true;
    }
    if( key[KEY_2] && ( formula != MANDEL3 ) )
    {
        formula = MANDEL3;
		add_history();
        update = true;
    }
    if( key[KEY_3] && ( formula != MANDEL4 ) )
    {
        formula = MANDEL4;
		add_history();
        update = true;
    }
    if( key[KEY_4] && ( formula != MANDEL5 ) )
    {
        formula = MANDEL5;
		add_history();
        update = true;
    }
    if( key[KEY_5] && ( formula != MANDEL6 ) )
    {
        formula = MANDEL6;
		add_history();
        update = true;
    }
    if( key[KEY_6] && ( formula != MANDEL7 ) )
    {
        formula = MANDEL7;
		add_history();
        update = true;
    }
    if( key[KEY_J] )
    {
        julia = !julia;
        while( key[KEY_J] )
            clear_keybuf();
    }

    if( key[KEY_LCONTROL] )
    {
        //set current mouse coords respecting the aspect ratio 3:4
        int mousex;
        int mousey;
        if( startx > mouse_x )
            mousex = ( mouse_x % 4 == 0 ? mouse_x : mouse_x - ( 4 - mouse_x % 4 ) );
        else
            mousex = ( mouse_x % 4 == 0 ? mouse_x : mouse_x + ( 4 - mouse_x % 4 ) );
        if( ( ( starty > mouse_y ) && ( startx < mousex ) ) || ( ( starty < mouse_y ) && ( startx > mousex ) ) )
            mousey = starty - (mousex-startx)*SCREEN_H/SCREEN_W;
        else
            mousey = starty + (mousex-startx)*SCREEN_H/SCREEN_W;
        //done setting mouse coords

        if( mouse_b & 1 )
        {
            if( startx == -1 )
            {
                startx = mouse_x;
                starty = mouse_y;
            }else
            {
                if( ( starty != mouse_y ) && ( startx != mouse_x ) )
                    rect( screen, startx, starty, mousex, mousey, 4 );
            }
        }else if( startx != -1 )
        {
            int x1, y1;
            int x2 = mousex;
            int y2 = mousey;
            x1 = (startx + x2 - abs( startx-x2 ))>>1;
            y1 = (starty + y2 - abs( starty-y2 ))>>1;
            x2 = (startx + x2 + abs( startx-x2 ))>>1;
            y2 = (starty + y2 + abs( starty-y2 ))>>1;
            double temp;
            temp = left;
            left  = left + x1 * double(right - left)/double(SCREEN_W);
            right = temp + x2 * double(right - temp)/double(SCREEN_W);

            temp = upper;
            upper = upper - y1 * double(upper - lower)/double(SCREEN_H);
            lower = temp  - y2 * double(temp  - lower)/double(SCREEN_H);
            startx = -1;
            update = true;
            add_history();
        }

    }else if( mouse_b & 1 )
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
        update = true;
        add_history();
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
        update = true;
        add_history();
    }
    if( key[KEY_PLUS_PAD] )
    {
        while( key[KEY_PLUS_PAD] )
        {
            max_itter++;
            textprintf_ex( screen, font, 10, 10, 15, 0, "iterations   : %6d", max_itter );
            rest(10);
        }
        update = true;
    }
    if( key[KEY_MINUS_PAD] )
    {
        while( key[KEY_MINUS_PAD] )
        {
            max_itter--;
            textprintf_ex( screen, font, 10, 10, 15, 0, "iterations   : %6d", max_itter );
            rest(10);
        }
//      update = true;
    }
    if( key[KEY_R] )
    {
        reset_pal();
        left  = -2.0;
        right =  2.0;
        lower = -1.5;
        upper =  1.5;
        max_itter = 256;
        shifting = false;
        shift_delay = 10;
        shift_dir = DOWN;
		change_pal();
        update = true;
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
    if( key[KEY_COMMA] )
    {
        if( ( shift_delay > 0 ) && ( g_timer - 30 > old_timer ) )
        {
            shift_delay--;
            textprintf_ex( screen, font, 10, 20, 15, 0, "shift delay  : %6d", shift_delay );
            old_timer = g_timer;
        }
    }
    if( key[KEY_STOP] )
    {
        if( g_timer - 30 > old_timer )
        {
            shift_delay++;
            textprintf_ex( screen, font, 10, 20, 15, 0, "shift delay  : %6d", shift_delay );
            old_timer = g_timer;
        }
    }
    if( key[KEY_BACKSLASH2] )
    {
        shift_dir = !shift_dir;
        while( key[KEY_BACKSLASH2] );
    }
    if( key[KEY_C] )
    {
        show_palette = !show_palette;
        while( key[KEY_C] );
    }
    if( key[KEY_G] )
    {
        guessing = !guessing;
//      update = true;
        while( key[KEY_G] );
    }
    if( key[KEY_I] )
    {
        info = !info;
        while( key[KEY_I] );
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
    if( key[KEY_ALT] )
		if( key[KEY_LEFT] && ( history_pos > 0 ) )
		{
			history_pos--;
			char buf[33];
	        string path = "temp/history";
			if( history_pos < 100 )
			{
		    	path += "0";
			    if( history_pos < 10 )
			    {
			        path += "0";
		    	}
			}
            sprintf( buf, "%d", history_pos );
            path += buf;
    	  	//path += itoa( history_pos, buf, 10 );
			path += ".txt";
			load_file( path );
		}
    if( key[KEY_RIGHT] )
    {
        char buf[33];
        string path = "temp/history";
        if( history_pos+1 < 100 )
        {
            path += "0";
            if( history_pos+1 < 10 )
            {
                path += "0";
            }
        }
        sprintf( buf, "%d", history_pos + 1 );
        path += buf;
        //path += itoa( history_pos+1, buf, 10 );
        path += ".txt";
        if( !load_file( path ) )
            history_pos++;
    }

    return 0;
}


int g_system::update_gfx( BITMAP* bmp )
{
//   clear( bmp );
//   textprintf_ex(screen, font, 10,100,3,-1,"servus");
    int old_x, old_y;

    if( update )
    {
        // main drawing routine
        if( guessing )
        {
            old_timer = g_timer;
            mandel_guessing( bmp, self);
            old_timer = g_timer - old_timer;
        }else
        {
            old_timer = g_timer;
            mandel_no_guessing( bmp, self );
            old_timer = g_timer - old_timer;
        }
        //DONE main drawing routine

        // preparing info_map & help_map message
        blit( bmp, info_map, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
        blit( bmp, help_map, 20, 50, 0, 0, help_map->w, help_map->h );
        //DONE preparing info_map & help_map message

        update = false;
    }
    // palette shifting
    if( shifting )
    {
        if( shift_timer < g_timer )
            shift( shift_dir );
        textprintf_ex( bmp, font, 10, 20, 15, 0, "shift delay  : %6d", shift_delay );
    }
    else
        textprintf_ex( bmp, font, 10, 20, 15, 0, "shift delay  :    OFF" );
    //DONE palette shifting

    //output info
    textprintf_ex( bmp, font, 10, 10, 15, 0, "iterations   : %6d", max_itter );
    textprintf_ex( bmp, font, 10, 30, 15, 0, "guessing mode: %s", guessing?"    ON":"   OFF" );

    textprintf_centre_ex( bmp, font, SCREEN_W/2, 10, 15, 0, "%ld ms", old_timer );

    textprintf_ex( bmp, font, SCREEN_W - 160,  5, 15, 0, "mouse x : %1.8f", left  + (double)mouse_x*(right-left) /double(SCREEN_W) );
    textprintf_ex( bmp, font, SCREEN_W - 160, 14, 15, 0, "mouse y : %1.8f", upper - (double)mouse_y*(upper-lower)/double(SCREEN_H) );
	textprintf_ex( bmp, font, SCREEN_W - 160, 23, 15, 0, "pixel x : %3d", mouse_x );
	textprintf_ex( bmp, font, SCREEN_W - 160, 32, 15, 0, "pixel y : %3d", mouse_y );

    switch( formula )
    {
    case 1:{ textprintf_ex( bmp, font, 10, SCREEN_H - 8, 15, 0, "Formula: z^2+c"); break; }
    case 2:{ textprintf_ex( bmp, font, 10, SCREEN_H - 8, 15, 0, "Formula: z^3+c"); break; }
    case 3:{ textprintf_ex( bmp, font, 10, SCREEN_H - 8, 15, 0, "Formula: z^4+c"); break; }
    case 4:{ textprintf_ex( bmp, font, 10, SCREEN_H - 8, 15, 0, "Formula: z^5+c"); break; }
    case 5:{ textprintf_ex( bmp, font, 10, SCREEN_H - 8, 15, 0, "Formula: z^6+c"); break; }
    case 6:{ textprintf_ex( bmp, font, 10, SCREEN_H - 8, 15, 0, "Formula: z^7+c"); break; }
    case -1:{ textprintf_ex( bmp, font, 10, SCREEN_H - 8, 15, 0, "Formula: NEWTON"); break; }
    }

    textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H - 45, 15, 0, "zoom : %15.0f", double(4.0/fabs( right - left ) ) );
    textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H - 35, 15, 0, "left : %1.12f", left );
    textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H - 26, 15, 0, "right: %1.12f", right );
    textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H - 17, 15, 0, "upper: %1.12f", upper );
    textprintf_ex( bmp, font, SCREEN_W - 180, SCREEN_H -  8, 15, 0, "lower: %1.12f", lower );
    //DONE output info

    if( !info )
        blit( info_map, bmp, 0, 0, 0, 0, 800, 600 );

    if( !help )
        blit( help_map, bmp, 0, 0, 20, 50, help_map->w, help_map->h );

    // display help
    if( help )
    {
        textprintf_ex( bmp, font, 20,  50, 15,  0, "HELP"                                             );
        textprintf_ex( bmp, font, 20,  56, 15, -1, "===="                                             );
        textprintf_ex( bmp, font, 20,  66, 15,  0, "keys:"                                            );
        textprintf_ex( bmp, font, 20,  72, 15, -1, "-----"                                            );
        textprintf_ex( bmp, font, 20,  82, 15,  0, " * ESC            quit / abort calculation"       );
        textprintf_ex( bmp, font, 20,  92, 15,  0, " * 1-6 + 0        change formula"                 );
        textprintf_ex( bmp, font, 20, 102, 15,  0, " * r              reset everything to default"    );
        textprintf_ex( bmp, font, 20, 112, 15,  0, " * SPACE          redraw"                         );
        textprintf_ex( bmp, font, 20, 122, 15,  0, " * +/-            increase/decrease Itterations"  );
        textprintf_ex( bmp, font, 20, 132, 15,  0, " * c              show palette"                   );
        textprintf_ex( bmp, font, 20, 142, 15,  0, " * p              switch palette"                 );
        textprintf_ex( bmp, font, 20, 152, 15,  0, " * s              palette shifting"               );
        textprintf_ex( bmp, font, 20, 162, 15,  0, " * ,/.            de-/increase shifting delay"    );
        textprintf_ex( bmp, font, 20, 172, 15,  0, " * h              toggle help"                    );
        textprintf_ex( bmp, font, 20, 182, 15,  0, " * g              switch guessing mode"           );
        textprintf_ex( bmp, font, 20, 192, 15,  0, " * ALT+ENTER      toggle Fullscreen mode"         );
        textprintf_ex( bmp, font, 20, 202, 15,  0, " * F5             save settings"                  );
        textprintf_ex( bmp, font, 20, 212, 15,  0, " * F6             load settings"                  );
        textprintf_ex( bmp, font, 20, 222, 15,  0, " * F12            take a screenshot"              );
        textprintf_ex( bmp, font, 20, 232, 15,  0, " * i              show information"               );
        textprintf_ex( bmp, font, 20, 232, 15,  0, " * j              show julia"                     );

		textprintf_ex( bmp, font, 20, 242, 15, 0, " * ALT+RIGHT/LEFT move forth and back in history" );
    }
    //DONE display help

    // backup dirt & draw mouse
    blit( bmp, dirt, mouse_x-5, mouse_y-5, 0, 0, 11, 11 );
    hline( bmp, mouse_x-5, mouse_y, mouse_x+5, g_timer%16 );
    vline( bmp, mouse_x, mouse_y-5, mouse_y+5, g_timer%16 ); //8*(g_timer%3)-1*(g_timer%2) );
    old_x = mouse_x-5;
    old_y = mouse_y-5;
    //DONE backup dirt & draw mouse

	// show julia
	if( julia && formula != NEWTON )
	{
	    if( julia_no_guessing( julia_map, bmp, self ) == -1 )
	    	quit = true;
	}

    // show palette or blit buffer
    if( show_palette )
    {
        for( int i=0 ; i< 16; i++ )
            for( int j=0; j< 16; j++ )
                rectfill( screen, j*SCREEN_W/16, i*SCREEN_H/16, (j+1)*SCREEN_W/16 -1, (i+1)*SCREEN_H/16 - 1, j+(i*16) );
    }else
    {

        blit( bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
        blit( dirt, bmp, 0, 0, old_x, old_y, 11, 11 );
    }
    //DONE show palette or blit buffer

    return 0;
}

static unsigned int createDirectory(const std::string &dir, int mode) {
    std::string s_tmpString = dir;
    DIR *p_Directory;
    p_Directory = opendir(dir.c_str());
    if(p_Directory != NULL) {
        closedir(p_Directory);
        return 2;
    }
#ifdef WIN32
    int result = mkdir(dir.c_str());
#else
    int result = mkdir(dir.c_str(), mode);
#endif
    if(result == -1) {
        fprintf(stderr, "misc:: could not create dir: %s\n", dir.c_str());
        return 0;
    }
    return 1;
}
