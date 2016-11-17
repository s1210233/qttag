#include "window.h"

//------------------------------------------------------------------------------
//  Variables for OpenGL functions
//------------------------------------------------------------------------------
//int     window_width   = 256; // DEFAULT_HEIGHT;
//int     window_height  = 256; // DEFAULT_WIDTH;
//int     window_width  = 512;
//int     window_height = 512;
int     window_width   = 768; // DEFAULT_HEIGHT;
int     window_height  = 768; // DEFAULT_WIDTH;
//int     window_width   = 805; // DEFAULT_HEIGHT;
//int     window_height  = 805; // DEFAULT_WIDTH;
//int     window_width   = 1024; // DEFAULT_HEIGHT;
//int     window_height  = 1024; // DEFAULT_WIDTH;

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

//------------------------------------------------------------------------------
//	Intialize the network
//------------------------------------------------------------------------------
    Net * net		=	new Net();
    net->width()	=	window_width;
    net->height()	=	window_height;

    // net->load( "data/test.dat" );
    // net->load( "data/HYWu.dat" );
    // net->load( "data/dolphin.dat" );  // V = 62, E = 159
    // net->load( "data/b102.gv.dat" );  // V = 302 
    // net->load( "data/b143.gv.dat" );  // V = 135
    // net->load( "data/b106.gv.dat" );  // V = 104
    net->load( "data/b124.gv.dat" );  // V = 79
    // net->load( "data/abstract.dat" );  //
    // net->load( "data/syouhin.dat" );  //Graph.cpp
    // net->load( "data/syouhin2.dat" );  //Graph.cpp
    // net->calcEdgeCentrality();

//----------------------------------------------------------
//	Main window
//----------------------------------------------------------
    //QSurfaceFormat format;
    //format.setSamples( 16 );

    Window window;

    //window.setFormat( format );
    
    window.setNet( net );
    //window.setGeometry( 50, 50, window_width, window_height );
    window.setGeometry( 50, 200, window_width, window_height );
    window.setWindowTitle( "Main" );
    window.show();

    return app.exec();
}


