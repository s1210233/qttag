//==============================================================================
// Widget.cc
//	: program file for the class WIDGET
//
//------------------------------------------------------------------------------
//
//				Date: Thu Nov  3 01:26:37 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>
#include <algorithm>
#include <strstream>
#include <GLUT/glut.h>
#include "Common.h"
#include "Widget.h"
#include "Window.h"
#include "Net.h"
//#include "Graph.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//#define COMMON_RADIUS	(2.4)
//#define COMMON_RADIUS	(3.6)
#define COMMON_RADIUS	(10.0)
//#define COMMON_RADIUS	(100.0)
#define COMMON_HEIGHT 	(-1.0)

int timecount = 0;       //add 20151230
int fixratio  = 0;       //add 20160108


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  Widget::_cone --	draw a cone
//
//  Inputs
//	coord:		coordinates of the center
//	weight:		assigned weight
//	aspect :	aspect ratio of the distance metric
//	up:		vertical translation
//
//  Outputs
//	none
//
void Widget::_cone( Coord2 & coord, double weight, double aspect, double up )
{
    glPushMatrix();
    glTranslated( coord.x(), coord.y(), COMMON_HEIGHT+up );
    glScaled( aspect, 1.0, 1.0 );
    glutSolidCone( COMMON_RADIUS*weight, 1.0, 64, 16 );		// radius, height,
								// number of circular ars,
								// number of vertical segments
    // glutSolidCone( COMMON_RADIUS, 1.0, 64, 16 );
    glPopMatrix();
}


//
//  Widget::_pyramid --	draw a pyramid
//
//  Inputs
//	coord :		coordinates of the center
//	weight :	assigned weight
//	aspect :	aspect ratio of the distance metric
//	up :		vertical translation
//
//  Outputs
//	none
//
void Widget::_pyramid( Coord2 & coord, double weight, double aspect, double up )
{
    glPushMatrix();
    glTranslated( coord.x(), coord.y(), COMMON_HEIGHT+up );
    glScaled( aspect, 1.0, 1.0 );
    glRotated( 45.0, 0.0, 0.0, 1.0 );
    glutSolidCone( COMMON_RADIUS*weight, 1.0, 4, 16 );
    //glutSolidCone( COMMON_RADIUS, 1.0, 4, 16 );
    glPopMatrix();
}


//
//  Widget::_prism --	draw a prism
//
//  Inputs
//	coord :		coordinates of the center
//	weight :	assigned weight
//	aspect :	aspect ratio of the distance metric
//	up :		vertical translation
//
//  Outputs
//	none
//
void Widget::_prism( Coord2 & coord, double weight, double aspect, double up )
{
    glPushMatrix();
    glTranslated( coord.x(), coord.y(), COMMON_HEIGHT+up );
    glScaled( aspect, 1.0, 1.0 );
    // No rotation here!!
    // glRotated( 45.0, 0.0, 0.0, 1.0 );
    glutSolidCone( COMMON_RADIUS*weight, 1.0, 4, 16 );    //半径，高さ，円の分割数(4->角錐)，高さの分割数
    //glutSolidCone( COMMON_RADIUS, 1.0, 4, 16 );
    glPopMatrix();
}


//
//  Widget::_draw --	draw a graph
//
//  Inputs
//	mode :	rendering mode
//
//  Outputs
//	none
//

void Widget::_draw( GLenum mode )           /*********************************************
                                                  memo   glColor3f RGB -> (0~255)/255
                                             *********************************************/
{
    ostringstream ostr;
    Coord2 curcoord;
    Graph &		g		= ( Graph & )(*_net);
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    VertexNameMap	vertexName	= get( vertex_myname, g );
    VertexWidthMap	vertexWidth	= get( vertex_mywidth, g );
    VertexHeightMap	vertexHeight	= get( vertex_myheight, g );
    VertexOverlapMap	vertexOverlap	= get( vertex_myoverlap, g );
    VertexRatioMap	vertexRatio	= get( vertex_myratio, g );
    //update 11/24
    VertexCentMap   vertexCent  = get( vertex_mycent, g );
    VertexFlagMap   vertexFlag  = get( vertex_myflag, g );
    EdgeFflagMap    edgeFflag   = get( edge_myfflag, g);

    EdgeFlagMap		edgeFlag	= get( edge_myflag, g );
    EdgeWeightMap	edgeWeight	= get( edge_weight, g );
    

    // VertexShiftMap	vertexShift	= get( vertex_myshift, g );
    // for Picking
    glInitNames();
    glPushName( NO_INDEX );

    glShadeModel( GL_SMOOTH );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    
    // for enabling antialiasing
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glDisable( GL_LIGHTING );
    
    // glLineWidth( 1.0 );
    glLineWidth( 2.0 );
    //glColor3f( 0.0, 0.0, 0.0 );
    glColor3d( 0.5, 0.5, 0.5 );        //---------------add20151007
    
    //cout << "widget  "<< net.getFinishFlag() << endl;
    //draw graph edges
    BGL_FORALL_EDGES( ed, g, Graph ) {
        VertexDescriptor vdS = source( ed, g );
        VertexDescriptor vdT = target( ed, g );
        //cout << "vertexCoord[ vdS ]=";_Print( vertexCoord[ vdS ]);
        //cout << "vertexCoord[ vdT ]=";_Print( vertexCoord[ vdT ]);
    	if ( edgeFlag[ ed ] == true) {
    	    glLineWidth( 4.0 );
    	    glColor3d( 1.0, 0.0, 0.0 );
    	}
        else{
             if( edgeFflag[ ed ] == true ){
                continue;
    	       //glLineWidth( 2.0 );
                //glColor3d( 1.0, 1.0, 1.0);
            }
            else{    
                glLineWidth( 2.0 );
               glColor3d( 0.5, 0.5, 0.5);
            }
        }   
    // else if( edgeFlag[ ed ] == false && edgeFflag[ ed ] == true ){
    //             glLineWidth( 2.0 );
    //             //glColor3d( 1.0, 1.0, 1.0);
    //    }

	glBegin( GL_LINES );
	glVertex2dv( vertexCoord[ vdS ].element() );
        glVertex2dv( vertexCoord[ vdT ].element() );
	glEnd();
    }
    

    // for disabling antialiasing
    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_BLEND );

    // draw graph nodes
    double radius = 5.0;
    glEnable( GL_POINT_SMOOTH );
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        
        // cerr << HERE << "vertexCoord[" << vd << "]=" << vertexCoord[ vd ];
        glLoadName( vertexID[ vd ] );
        curcoord = vertexCoord[ vd ];
	//cout << "curcoord=" << curcoord << endl;
        if( vertexFlag[ vd ]) radius = 14.0;
        else{
            if ( vertexSwitch[ vd ] ) radius = 8.0;
            else radius = 5.0;
        }   
	if ( vertexSwitch[ vd ] ) glColor3f( 0.2, 0.2, 0.2 );
	else glColor3f( 0.0, 0.0, 0.0 );
	// drawing silhouettes
	//if ( ( _net->mode() == 4 ) || ( _net->mode() == 5 ) )
	    glPointSize( vertexRatio[ vd ]*radius+4.0 );
	    //else
	    //glPointSize( radius );

        glBegin( GL_POINTS );
        glVertex2dv( curcoord.element() );  //node position
        glEnd();

	if ( _flagConflict ) {
	    // drawing interior disks
	    radius -= 1.0;

	    if ( vertexOverlap[ vd ] > 0.0 ) glColor3f( 1.0, 0.0, 0.0 );
	    else glColor3f( 1.0, 1.0, 1.0 );
	    if ( ( _net->mode() == 4 ) || ( _net->mode() == 5 ) )
		glPointSize( vertexRatio[ vd ]*radius+4.0 );
	    else
		glPointSize( radius );
	    
	    glBegin( GL_POINTS );
	    glVertex2dv( curcoord.element() );  //node position
	    glEnd();
	}
    //mark centrality
    double t = 0.0;
    t = vertexCent[vd];
    if(vertexFlag[vd]){
        if(1 >= t && 0.5 <= t) glColor3f(2*t + 1, -2*t +2, 0.0);
        else glColor3f(0.0, 2*t, -2 * t +1);
        glBegin( GL_POINTS );
        glVertex2dv( curcoord.element() );  //node position
        glEnd();
    }

	glLoadName( NO_INDEX );
        
        if ( ( _flagLabel ) && ( vertexSwitch[ vd ] ) ) {             //***************draw Graph label
	    // cerr << " Width = " << vertexWidth[ vd ] << " Height = " << vertexHeight[ vd ] << endl;
            string & name = vertexName[ vd ];
            const double unitL = LABEL_UNIT_RATIO * window_side();
            unsigned int len = name.length();
            double cx = curcoord[ 0 ];
            double cy = curcoord[ 1 ];
            double lw = unitL * vertexWidth[ vd ];
            double lh = unitL * vertexHeight[ vd ];

            glColor3f( 1.0, 1.0, 1.0 );
            glBegin( GL_QUADS );    //square entire color -> white
            glVertex2d( curcoord[0]-lw, curcoord[1]-lh );
            glVertex2d( curcoord[0]+lw, curcoord[1]-lh );
            glVertex2d( curcoord[0]+lw, curcoord[1]+lh );
            glVertex2d( curcoord[0]-lw, curcoord[1]+lh );
            glEnd();
	    
            if ( _flagConflict && ( vertexOverlap[ vd ] > 0.0 ) )
                glColor3f( 1.0, 0.0, 0.0 ); //overlap labels color to red
            else
                glColor3f( 0.0, 0.0, 0.0 );
                    
            glBegin( GL_LINE_LOOP );    //square edge color -> black or red
            glVertex2d( curcoord[0]-lw, curcoord[1]-lh );
            glVertex2d( curcoord[0]+lw, curcoord[1]-lh );
            glVertex2d( curcoord[0]+lw, curcoord[1]+lh );
            glVertex2d( curcoord[0]-lw, curcoord[1]+lh );
            glEnd();
                    
            // glDisable( GL_DEPTH_TEST );
            renderText( curcoord[ 0 ]-0.75*lw, curcoord[ 1 ]-0.65*lh, 0.0,
                       vertexName[ vd ].c_str(),
                       _font );
	    // glEnable( GL_DEPTH_TEST );
        }
    }
    
    
    if ( _mouse_mode == SELECTING_MODE ) _rectangle();

#ifdef DEBUG
    const double central = CONTENT_RATIO;
    double minX = center_x() - central * window_side() * SCREEN_SIDE;
    double minY = center_y() - central * window_side() * SCREEN_SIDE;
    double maxX = center_x() + central * window_side() * SCREEN_SIDE;
    double maxY = center_y() + central * window_side() * SCREEN_SIDE;
    glColor3d( 0.8, 0.9, 1.0 );
    glBegin( GL_LINE_LOOP );
    glVertex2d( minX, minY );
    glVertex2d( minX, maxY );
    glVertex2d( maxX, maxY );
    glVertex2d( maxX, minY );
    glEnd();
#endif	// DEBUG
}


//
//  Widget::_cell --	draw centroidal Voronoi tesselations
//
//  Inputs
//	mode	: rendering mode
//
//  Outputs
//	none
//
void Widget::_cell( GLenum mode )
{
    unsigned char	col[ 3 ];
    Graph &		g		= ( Graph & )(*_net);
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexWeightMap	vertexWeight	= get( vertex_myweight, g );
    VertexAspectMap	vertexAspect	= get( vertex_myaspect, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    VertexRatioMap	vertexRatio	= get( vertex_myratio, g );
    const bool		weight_mode	= false;

    BGL_FORALL_VERTICES( vd, g, Graph ) {

	if ( ! vertexSwitch[ vd ] ) continue;

	unsigned int idV = vertexID[ vd ];
        //cout << "vertexID=" << idV <<endl;
	Net::changeNumberToColor( DIVISOR, idV, col );
	glColor4ub( col[0], col[1], col[2], 255 );
	Coord2 coord = vertexCoord[ vd ];
        vertexWeight[ vd ] = 1.0;
        //cerr << "vertexWeight[" << vertexID[ vd ] << "]=" << vertexWeight[ vd ] << endl;
        
	if ( _flagAspect ) {
	    switch ( _net->metric() ) {
	      case 0:		// euclidean
		  // cerr << "Euclidean" << endl;
                _cone		( coord, vertexWeight[ vd ], vertexAspect[ vd ], 0.0 );
		  break;
	      case 1:		// manhattan
		  // cerr << "Manhattan" << endl;
                _prism		( coord, vertexWeight[ vd ], vertexAspect[ vd ], 0.0 );
		  break;
	      case 2:		// chevyshev
		  // cerr << "Chebyshev" << endl;
                _pyramid		( coord, vertexWeight[ vd ], vertexAspect[ vd ], 0.0 );
          break;
	    }
	}
	else {
	    switch ( _net->metric() ) {
	      case 0:
		  // cerr << "Euclidean" << endl;
                 _cone		( coord, 1.0, 1.0, 0.0 );
		  break;
	      case 1:
		  // cerr << "Manhattan" << endl;
                 _prism		( coord, 1.0, 1.0, 0.0 );
		  break;
	      case 2:
		  // cerr << "Chebyshev" << endl;
                 _pyramid	( coord, 1.0, 1.0, 0.0 );
		  break;
	    }
	}
    }

    
    static unsigned char	*data = NULL;
    if ( data == NULL ) data = new unsigned char [ _net->width() * _net->height() * 3 ];
    glReadPixels( 0, 0, _net->width(), _net->height(), GL_RGB, GL_UNSIGNED_BYTE, data );
    
    if ( _net->diagram() == NULL )
        _net->diagram() = cvCreateImage( cvSize( _net->width(), _net->height() ), IPL_DEPTH_8U, 3 );
    memcpy( _net->diagram()->imageData, data, _net->diagram()->imageSize );
    
    cvFlip( _net->diagram(), NULL, 0 );
    
    glEnable( GL_POINT_SMOOTH );
    //glPointSize( 7.0 );
    glPointSize( 3.0 );       //add 20151106
    
    //cout << point.size() << endl;
    BGL_FORALL_VERTICES( vd, g, Graph ) {
	Coord2 coord = vertexCoord[ vd ];
        glColor4d( 1.0, 1.0, 1.0, 1.0 );
        glBegin( GL_POINTS );
        glVertex3d( coord.x(), coord.y(), 0.0 );
        glEnd();
        // drawCube( point[ k ] );
    }
}


//
//  Widget::_annotate --	setup aspects
//
//  Inputs
//	none
//
//  Outputs
//	onoe
//
void Widget::_annotate( void )
{
    Graph &		g		= ( Graph & )(*_net);
    VertexNameMap	vertexName	= get( vertex_myname, g );
    VertexWidthMap	vertexWidth	= get( vertex_mywidth, g );
    VertexHeightMap	vertexHeight	= get( vertex_myheight, g );
    VertexAspectMap	vertexAspect	= get( vertex_myaspect, g );

    QFontMetrics qfm( _font );
    BGL_FORALL_VERTICES( vd, g, Graph ) {
	QString qstr( vertexName[ vd ].c_str() );
	int width  = qfm.width( qstr );
	int height = qfm.height();
	vertexWidth [ vd ]	= width;
	vertexHeight[ vd ]	= height;
	vertexAspect[ vd ]	= ( double )width/( double )height;
    }
}


void Widget::_rectangle( void )
{
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();		// <====
    glLoadIdentity();
    glOrtho( 0, width(), height(), 0, -1.0, 1.0 );
    
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();		// <====
    glLoadIdentity();

    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    glLineWidth( 7.0 );
    // glColor3d( 1.00, 0.36, 0.36 );
    glColor3d( 0.50, 0.50, 0.50 );
    glBegin( GL_LINE_LOOP );
    glVertex2i(	_corner_x,  _corner_y  );
    glVertex2i( _last_pointer_x, _corner_y  );
    glVertex2i( _last_pointer_x, _last_pointer_y );
    glVertex2i( _corner_x,  _last_pointer_y );
    glEnd();
    glLineWidth( 3.0 );
    //glColor3d( 1.00, 0.647, 0.00 );
    glColor3d( 1.00, 1.00, 1.00 );
    glBegin( GL_LINE_LOOP );
    glVertex2i(	_corner_x,  _corner_y  );
    glVertex2i( _last_pointer_x, _corner_y  );
    glVertex2i( _last_pointer_x, _last_pointer_y );
    glVertex2i( _corner_x,  _last_pointer_y );
    glEnd();

    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_BLEND );

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}



void Widget::initializeGL( void )
{
    // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );

    _mouse_mode = WAITING_MODE;
    
#ifdef SKIP
    _timer->start( 30, this );
    // _timer->start( 20, this );
    // _timer->start( 16, this );
#endif	// SKIP
}

void Widget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    //glOrtho( -SCREEN_SIDE, SCREEN_SIDE, -SCREEN_SIDE, SCREEN_SIDE, -2.0, 2.0 ); //common.h->SCREEN_SIDE=2.0
    glOrtho( 
	    center_x() - window_side()*SCREEN_SIDE,
	    center_x() + window_side()*SCREEN_SIDE,
	    center_y() - window_side()*SCREEN_SIDE,
	    center_y() + window_side()*SCREEN_SIDE,
	    -2.0, 2.0 );
    // glOrtho( -SCREEN_SIDE, SCREEN_SIDE, -SCREEN_SIDE, SCREEN_SIDE, -2.0, 2.0 ); //common.h->SCREEN_SIDE=2.0
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    _net->initColorMap();
    
    glEnable( GL_DEPTH_TEST );
     // Depth buffering enabled
    glDepthFunc( GL_LESS );
    if ( _flagCell ) _cell ( GL_RENDER );
    glDisable( GL_DEPTH_TEST );
    
    _draw( GL_RENDER );
    
    // Return to normal rendering
    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_BLEND );
    
    // _paintName();

    // cerr << "*" << ends;
    glFlush();
    // glutSwapBuffers();

#ifdef ACTIVATE_SNAPS
    QImage screenshot;
    static unsigned int counter = 0;
    screenshot = grabFrameBuffer();
    ostringstream ostr;
    ostr << "snap/snap" << setw( 4 ) << setfill('0') << counter++ << ".png" << ends;
    screenshot.save( ostr.str().c_str() );
    cerr << "Saving an image as : " << ostr.str().c_str() << endl;
#endif	// ACTIVATE_SNAPS
    
}

/********************************************************
 measure hidden label area by overlapping   add20151112
 *******************************************************/
void Widget::label_evaluation( void )
{
    Graph &		g		= ( Graph & )(*_net);
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexWidthMap	vertexWidth	= get( vertex_mywidth, g );
    VertexHeightMap	vertexHeight	= get( vertex_myheight, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    //VertexOverlapMap	vertexOverlap	= get( vertex_myoverlap, g );
    VertexWeightMap     vertexWeight    = get( vertex_myweight, g );
    Coord2		coord_i, coord_j, diff;
    double		lw_i, lw_j, lh_i, lh_j, lap_w, lap_h;
    double      lwi_1, lwi_2, lwj_1, lwj_2, lhi_1, lhi_2, lhj_1, lhj_2;
    double		T_lap = 0;
    Coord2      vcoord, lap_rect, lap_cg_s1, lap_cg_s2;
    vector < Coord2 > lap_q1, lap_q2, lap_q3, lap_q4;
    vector < double > lap_init;
    double      all_label_area = 0;
    
    // initialize the overlap ratios
    _net->clearClutterBit();
    const double unitL = LABEL_UNIT_RATIO * window_side();
    double lap_final = 0.0;
    
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        
        lw_i = unitL * vertexWidth[ vd ];
        lh_i = unitL * vertexHeight[ vd ];
        
        all_label_area += 2*lw_i*2*lh_i;
    }
    
    // Compute overlap area of node with their sum
    BGL_FORALL_VERTICES( vdi, g, Graph ) {
        
        lap_q1.clear();
        lap_q2.clear();
        lap_q3.clear();
        lap_q4.clear();
        lap_init.clear();
        
        coord_i = vertexCoord[ vdi ];
        vcoord = vertexCoord[ vdi ];
        lw_i = unitL * vertexWidth[ vdi ];  //horizontal half length of label-i
        lh_i = unitL * vertexHeight[ vdi ]; //vertical half length of label-i
        
        lwi_1 = vcoord[0]-lw_i;
        lwi_2 = vcoord[0]+lw_i;
        lhi_1 = vcoord[1]-lh_i;
        lhi_2 = vcoord[1]+lh_i;
        
        BGL_FORALL_VERTICES( vdj, g, Graph ) {
            
            if( vertexID[ vdi ] >= vertexID[ vdj ] ) continue;
            //i.e. vertexID[ vdi ] >= vetexIS[ vdj ] => no calculate ( skip )
            
            //read vertexcoord[ vdi ] and [ vdj ]
            coord_j = vertexCoord[ vdj ];
            vcoord = vertexCoord[ vdj ];
            lw_j = unitL * vertexWidth[ vdj ];
            lh_j = unitL * vertexHeight[ vdj ];
            
            lwj_1 = vcoord[0]-lw_j;
            lwj_2 = vcoord[0]+lw_j;
            lhj_1 = vcoord[1]-lh_j;
            lhj_2 = vcoord[1]+lh_j;
            
            double lwij_1 = 0, lwij_2 = 0, lhij_1 = 0, lhij_2 = 0, lap_ij = 0;
            
            //CHECK horizontal overlap between two labels
            if( lwi_2 > lwj_1 ){
                if( lwi_1 < lwj_2 ) {
                    if( lwi_1 >= lwj_1 ) {
                        lwij_1 = lwi_1;
                        if( lwi_2 >= lwj_2 )    lwij_2 = lwj_2;
                        else    lwij_2 = lwi_2;
                    }
                    else {
                        lwij_1 = lwj_1;
                        if( lwi_2 <= lwj_2 )    lwij_2 = lwi_2;
                        else    lwij_2 = lwj_2;
                    }
                }
            }
            
            //CHECK vertical overlap between two labels
            if( lhi_2 > lhj_1 ) {
                if( lhi_1 < lhj_2 ) {
                    if( lhi_1 >= lhj_1 ) {
                        lhij_1 = lhi_1;
                        if( lhi_2 >= lhj_2 )    lhij_2 = lhj_2;
                        else    lhij_2 = lhi_2;
                    }
                    else {
                        lhij_1 = lhj_1;
                        if( lhi_2 <= lhj_2 )    lhij_2 = lhi_2;
                        else    lhij_2 = lhj_2;
                    }
                }
            }
            
            double S_overlap = 0;
            
            lap_rect.x() = lwij_2;
            lap_rect.y() = lhij_2;
            lap_q1.push_back( lap_rect );
            
            lap_rect.x() = lwij_1;
            lap_rect.y() = lhij_2;
            lap_q2.push_back( lap_rect );
            
            lap_rect.x() = lwij_1;
            lap_rect.y() = lhij_1;
            lap_q3.push_back( lap_rect );
            
            lap_rect.x() = lwij_2;
            lap_rect.y() = lhij_1;
            lap_q4.push_back( lap_rect );
            
            S_overlap = ( lwij_2 - lwij_1 )*( lhij_2 - lhij_1 );
            lap_init.push_back( S_overlap );  //1-1 label overlap area
        }
        
        double lap_part = 0.0;
        
        //caluculate center of g, width and height
        for( unsigned int s1 = 0; s1 < lap_q1.size(); s1++ ) {
            lap_cg_s1.x() = ( lap_q1[ s1 ].x() - lap_q3[ s1 ].x() )/2;
            lap_cg_s1.y() = ( lap_q1[ s1 ].y() - lap_q3[ s1 ].y() )/2;
            
            double lap_max = 0.0;
            
            for( unsigned int s2 = 0; s2 < lap_q1.size(); s2++ ) {
                if( s1 >= s2 ) continue;
                lap_cg_s2.x() = ( lap_q1[ s2 ].x() - lap_q3[ s2 ].x() )/2;
                lap_cg_s2.y() = ( lap_q1[ s2 ].y() - lap_q3[ s2 ].y() )/2;
                
                diff = lap_cg_s1 - lap_cg_s2;
                
                lw_i = ( lap_q1[ s1 ].x()- lap_q3[ s1 ].x() )/2;
                lw_j = ( lap_q1[ s2 ].x()- lap_q3[ s2 ].x() )/2;
                lh_i = ( lap_q1[ s1 ].y()- lap_q3[ s1 ].y() )/2;
                lh_j = ( lap_q1[ s2 ].y()- lap_q3[ s2 ].y() )/2;
                
                lap_w = lw_i + lw_j - abs( diff[ 0 ] );
                lap_w = min( lap_w, 2.0*lw_i );
                lap_w = min( lap_w, 2.0*lw_j );
                lap_w = max( lap_w, 0.0 );
                
                lap_h = lh_i + lh_j - abs( diff[ 1 ] );
                lap_h = min( lap_h, 2.0*lh_i );
                lap_h = min( lap_h, 2.0*lh_j );
                lap_h = max( lap_h, 0.0 );
                
                lap_max = max( lap_w*lap_h, lap_max );
            }
            
            //Finally seek hidden area by overlapping for vdi label
            lap_init[ s1 ] = lap_init[ s1 ] - lap_max;
            lap_part += lap_init[ s1 ];
        }
        
        lap_final += lap_part;
        T_lap = lap_final;
    }

    cerr << "*****************evaluation********************" << endl;
    cerr << " side = " << window_side() << endl;
    cerr << " * all_label_area = " << all_label_area << endl;
    cerr << " * hidden label area = " << T_lap << endl;
    cerr << " * hidden area by overlapping is " <<
        T_lap/all_label_area*100 << " % of label area" <<endl;
    cerr << "***********************************************" << endl;
    
}


void Widget::label_overlap( void )
{
    Graph &		g		= ( Graph & )(*_net);
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexWidthMap	vertexWidth	= get( vertex_mywidth, g );
    VertexHeightMap	vertexHeight	= get( vertex_myheight, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    VertexOverlapMap	vertexOverlap	= get( vertex_myoverlap, g );
    VertexWeightMap     vertexWeight    = get( vertex_myweight, g );
    Coord2		coord_i, coord_j, diff;
    double		lw_i, lw_j, lh_i, lh_j, lap_w, lap_h;
    double		T_overlap = 0;
    
    // initialize the overlap ratios
    _net->clearClutterBit();
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        vertexOverlap[ vd ] = 0.0;
    }
    
    const double unitL = ( LABEL_UNIT_RATIO + LABEL_MARGIN ) * window_side();
    
    // Compute the overlap ratios of nodes together with their sum
    BGL_FORALL_VERTICES( vdi, g, Graph ) {

	if ( ! vertexSwitch[ vdi ] ) continue;

        coord_i = vertexCoord[ vdi ];
	lw_i = unitL * vertexWidth[ vdi ];
	lh_i = unitL * vertexHeight[ vdi ];
        
        BGL_FORALL_VERTICES( vdj, g, Graph ) {

	    if ( ! vertexSwitch[ vdj ] ) continue;

            coord_j = vertexCoord[ vdj ];
	    lw_j = unitL * vertexWidth[ vdj ];
	    lh_j = unitL * vertexHeight[ vdj ];
            
            double S_overlap = 0;   //add 20150512
            
            if ( vdi == vdj ) {
                ; // do nothing
            }
            else {
                //min1 = min{ lw_i + lw_j - |curcoord1[0] - curcoord2[0]|, 2*lw_i, 2*lw_j }
		diff = coord_j - coord_i;

		lap_w = lw_i + lw_j - abs( diff[ 0 ] );
		lap_w = min( lap_w, 2.0*lw_i );
		lap_w = min( lap_w, 2.0*lw_j );
		lap_w = max( lap_w, 0.0 );

		lap_h = lh_i + lh_j - abs( diff[ 1 ] );
		lap_h = min( lap_h, 2.0*lh_i );
		lap_h = min( lap_h, 2.0*lh_j );
		lap_h = max( lap_h, 0.0 );
                
                double S_overlap = lap_w*lap_h;
                T_overlap += S_overlap;
        
		if ( vertexOverlap[ vdi ] < S_overlap ) {
		    vertexOverlap[ vdi ] = S_overlap;
		}
	    }
        }
    }
    _net->conflictArea() = T_overlap;
    if ( T_overlap > 1.0e-8 ) _net->setClutterBit();
    //cerr << "Total relative area of label overlaps = " << T_overlap << endl; //------commentout20151006
    
}


void Widget::timerEvent( QTimerEvent *e )
{
    Graph & g = ( Graph & )(*_net);
    VertexIDMap         vertexID      = get( vertex_myid, g );
    VertexOverlapMap    vertexOverlap = get( vertex_myoverlap, g );
    VertexRatioMap      vertexRatio   = get( vertex_myratio, g );
    
    Q_UNUSED( e );

    double err = 0.0;
    //cerr << "_net->gp() = " << _net->gp() << endl;
    //cerr << "_net->clut() = " << _net->clut() << endl;
    //cerr << " timecount = " << timecount << endl;
    //cerr << " smoothingset =  " << _net->smoothingset() << endl;
    //cerr << "_net->ft() = " << _net->ft() << endl;
    
    switch ( _net->mode() ) {
      case 0:			// force-directed
	  _net->force();
	  err = _net->gap();
	  if ( _flagConflict ) label_overlap(); //overlap measure
	  if ( err < FINAL_EPSILON ) {
#ifdef LABELS_WHEN_COMPLETE
	      _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
	      if ( _flagConflict ) {
		  label_overlap(); //overlap measure
	      }
	      _timer->stop();
	      cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
	      cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
	      //cerr << "FINAL widget_count" << widget_count << endl;
	  }
	  break;
      case 1:			// centroidal Voronoi
	  _net->centroid();
	  err = _net->gap();
	  if ( _flagConflict ) label_overlap(); //overlap measure
	  if ( err < FINAL_EPSILON ) {
#ifdef LABELS_WHEN_COMPLETE
	      _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
	      if ( _flagConflict ) label_overlap(); //overlap measure
	      _timer->stop();
	      cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
	      cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
	  }
	  break;
      case 2:			// simple hybrid
	  _net->force();
	  if ( _net->spaceBit() ) _net->centroid();
	  err = _net->gap();

	  if ( _flagConflict ) label_overlap(); //overlap measure
	  else {
	      _net->conflictArea() = 0.0;
	      _net->clearClutterBit();
	  }

	  if ( ( err < INTERMEDIATE_EPSILON ) &&
	       ( _net->step() >= STANDARD_LIMIT_STEP + UNIT_STEP ) ) {
	      _net->setSpaceBit();
	      _net->step() -= UNIT_STEP;
	      _net->ratio() = ( double )_net->step()/100.0;
	      // cerr << HERE;
	      cerr << "[Hybrid simple]" << ends;
	      if ( _flagConflict ) {
		  if ( _net->clutterBit() ) cerr << " XXX ";
		  else cerr << " OOO ";
	      }
	      cerr << " ratioF = " << _net->ratioF() << " ratioV = " << _net->ratioV() 
		   << " area = " << _net->conflictArea() << " error = " << err << endl;
	  }
#ifdef SKIP
	  else if ( ( err < FINAL_EPSILON ) &&
		    ( _net->step() >= STANDARD_LIMIT_STEP+UNIT_STEP ) ) {
	      _net->step() -= UNIT_STEP;
	      _net->ratio() = ( double )_net->step()/100.0;
	      cerr << "[Hybrid simple]" << ends;
	      if ( _flagConflict ) {
		  if ( _net->clutterBit() ) cerr << " XXX ";
		  else cerr << " OOO ";
	      }
	      cerr << " ratioF = " << _net->ratioF() << " ratioV = " << _net->ratioV() 
		   << " area = " << _net->conflictArea() << " error = " << err << endl;
	  }
#endif	// SKIP
	  else if ( ( err < FINAL_EPSILON ) &&
		    ( _net->step() <= STANDARD_LIMIT_STEP ) ) {
	      cerr << " ==> Finalized" << endl;
#ifdef LABELS_WHEN_COMPLETE
	      _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
	      _timer->stop();
	      cerr << "[Simple] Finished Execution Time = " << checkOutETime() << endl;
	      cerr << "[Simple] Finished CPU Time = " << checkOutCPUTime() << endl;
	  }
	  break;
      case 3:			// hybrid adaptive
	  _net->force();
	  if ( _net->spaceBit() ) _net->centroid();
	  err = _net->gap();

	  if ( _flagConflict ) label_overlap(); //overlap measure
	  else {
	      _net->conflictArea() = 0.0;
	      _net->clearClutterBit();
	  }

	  if ( _net->clutterBit() &&
	       ( err < INTERMEDIATE_EPSILON ) &&
	       ( _net->step() >= MIN_LIMIT_STEP + UNIT_STEP ) ) {
	      _net->setSpaceBit();
	      _net->step() -= UNIT_STEP;
	      _net->ratio() = ( double )_net->step()/100.0;
	      cerr << "[Hybrid Adaptive]" << ends;
	      if ( _flagConflict ) {
		  if ( _net->clutterBit() ) cerr << " XXX ";
		  else cerr << " OOO ";
	      }
	      cerr << " ratioF = " << _net->ratioF() << " ratioV = " << _net->ratioV() 
		   << " area = " << _net->conflictArea() << endl;
	  }
	  else if ( ( err < FINAL_EPSILON ) &&
		    ( ( ! _net->clutterBit() ) || ( _net->step() <= MIN_LIMIT_STEP ) ) ) {
	      cerr << " ==> Finalized" << endl;
#ifdef LABELS_WHEN_COMPLETE
	      _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
	      _timer->stop();
	      cerr << "[Adaptive] Finished Execution Time = " << checkOutETime() << endl;
	      cerr << "[Adaptive] Finished CPU Time = " << checkOutCPUTime() << endl;
	  }
	  break;
      case 4:			// Laplacian smoothing
	  _net->force();
	  if ( _net->spaceBit() ) _net->centroid();
	  // if ( _net->step() < 100 ) _net->centroid();
	  err = _net->gap();
	  // print vertex ratios
#ifdef DEBUG_SMOOTH
	  BGL_FORALL_VERTICES( vd, g, Graph ) {
	      cerr << vertexRatio[ vd ] << " ";
	  }
	  cerr << " error = " << err << endl;
	  getchar();
#endif	// DEBUG_SMOOTH
	  if ( _flagConflict ) label_overlap(); //overlap measure
	  else {
	      _net->conflictArea() = 0.0;
	      _net->clearClutterBit();
	  }

	  if ( _net->clutterBit() &&
	       ( err < INTERMEDIATE_EPSILON ) &&
	       ( _net->step() >= MIN_LIMIT_STEP + UNIT_STEP ) ) {
#ifdef DEBUG_SMOOTH
	      cerr << "##############################" << endl;
	      BGL_FORALL_VERTICES( vds, g, Graph ) {
		  cerr << vertexRatio[ vds ] << " ";
	      }
#endif	// DEBUG_SMOOTH
	      _net->setSpaceBit();
	      _net->proceed();
#ifdef DEBUG_SMOOTH
	      cerr << "##############################" << endl;
	      BGL_FORALL_VERTICES( vds, g, Graph ) {
		  cerr << vertexRatio[ vds ] << " ";
	      }
#endif	// DEBUG_SMOOTH
	      _net->onestep();
#ifdef DEBUG_SMOOTH
	      cerr << "##############################" << endl;
	      BGL_FORALL_VERTICES( vds, g, Graph ) {
		  cerr << vertexRatio[ vds ] << " ";
	      }
	      cerr << "[Onestep Smoothing]" << ends;
#endif	// DEBUG_SMOOTH
	      if ( _flagConflict ) {
		  if ( _net->clutterBit() ) cerr << " XXX ";
		  else cerr << " OOO ";
	      }
	  }
	  // else if ( ( err < FINAL_EPSILON ) && ( ! _net->clutterBit() ) )
	  else if ( ( ! _net->clutterBit() ) ) {
	      cerr << " ==> Finalized" << endl;
#ifdef LABELS_WHEN_COMPLETE
	      _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
	      _timer->stop();
	      cerr << "[OneStep] Finished Execution Time = " << checkOutETime() << endl;
	      cerr << "[OneStep] Finished CPU Time = " << checkOutCPUTime() << endl;
	  }
	  else if ( err >= INTERMEDIATE_EPSILON ) {
	      _net->onestep();
	  }
	  break;
      case 5:			// Two-step smoothing
	  _net->force();
	  if ( _net->spaceBit() ) _net->centroid();
	  // if ( _net->step() < 100 ) _net->centroid();
	  err = _net->gap();
	  // print vertex ratios
#ifdef DEBUG_SMOOTH
	  BGL_FORALL_VERTICES( vd, g, Graph ) {
	      cerr << vertexRatio[ vd ] << " ";
	  }
	  cerr << endl;
#endif	// DEBUG_SMOOTH
	  if ( _flagConflict ) label_overlap(); //overlap measure
	  else {
	      _net->conflictArea() = 0.0;
	      _net->clearClutterBit();
	  }

	  if ( _net->clutterBit() &&
	       ( err < INTERMEDIATE_EPSILON ) &&
	       ( _net->step() >= MIN_LIMIT_STEP + UNIT_STEP ) ) {
	      _net->setSpaceBit();
	      _net->proceed();
	      // cerr << "[Twostep Smoothing]" << ends;
	      if ( _flagConflict ) {
		  if ( _net->clutterBit() ) cerr << " XXX ";
		  else cerr << " OOO ";
	      }
	  }
	  // else if ( ( err < FINAL_EPSILON ) && ( ! _net->clutterBit() ) )
	  else if ( ( ! _net->clutterBit() ) ) {
	      cerr << " ==> Finalized" << endl;
#ifdef LABELS_WHEN_COMPLETE
	      _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
	      _timer->stop();
	      cerr << "[TwoStep] Finished Execution Time = " << checkOutETime() << endl;
	      cerr << "[TwoStep] Finished CPU Time = " << checkOutCPUTime() << endl;
	  }
	  else if ( err >= INTERMEDIATE_EPSILON ) {
	      _net->twostep();
	  }
	  break;
    }
    
#ifdef SKIP
    if( _flag2stepsmoothing && _net->smoothingset() ) timecount++;
    //cerr << " fixratio = " << fixratio << endl;
    
    if( timecount > 1 ) {
        fixratio++;
        if( fixratio < 100 ) {
            cerr << "************************step 2-B[test up to 100]************************" << endl;
            cerr << "smoothing of parameter" << " " << fixratio <<  endl;
            _net->para();
            cerr << "///////////after smoothing 2-B[test_100step] start///////////" << endl;
            BGL_FORALL_VERTICES( vd, g, Graph ) {
                //cerr << "vertexRatio[ " << vertexID[ vd ] << " ] = "
                //<< vertexRatio[ vd ] << endl;
                cerr << vertexRatio[ vd ] << endl;
            }
            cerr << "///////////after smoothing 2-B[test_100step] end///////////" << endl;
            cerr << HERE;
            if ( _flagConflict ) {
                if ( _net->clutterBit() ) cerr << " XXX ";
                else cerr << " OOO ";
            }
            timecount = 0;
        }
        else ;
    }
#endif	// SKIP
    
    if ( ( err < FINAL_EPSILON ) && ( _flagLabel ) && ( _flagConflict ) ) {
        //_net->evaluation() = 1;
        //updateGL();
        label_evaluation();
        // _net->cleargp();
        // _net->clearclut();
    }
    updateGL();
}


//
//  Widget::_handleVertex --	
//
//  Inputs
//	variable : explanation of a variable
//
//  Outputs
//
//
bool Widget::_handleVertex( int nHits, unsigned int * buf, VertexDescriptor & vd )
{
    unsigned int *	ptr		= NULL; //, names;
    float		minDepth	= 1000.0;
    int			hitID		= NO_INDEX;
    Graph &		g		= *_net;
    
    ptr = buf;
    
    for ( int i = 0; i < nHits; ++i ) { // for each bit
	if ( ptr[ 0 ] != 1 ) {
	    cerr << " Number of names for hit = " << ( int )ptr[ 0 ] << endl;
	    assert( ptr[ 0 ] == 1 );
	}
	float curDepth = (float)ptr[ 1 ]/0xffffffff;
	int curID = ( int )ptr[ 3 ];
	//#ifdef DEBUG
	cerr << " i = " << i 
	     << " [0]: " << ptr[ 0 ]
	     << " [1]: " << ptr[ 1 ]
	     << " [2]: " << ptr[ 2 ]
	     << " [3]: " << ptr[ 3 ] << endl;
	// #endif	// DEBUG
	if ( ( curDepth < minDepth ) && ( curID != NO_INDEX ) ) {
	    minDepth = curDepth;
	    hitID = ptr[ 3 ];
	}
	ptr += 4;
    }
    
    cerr << " hitID = " << hitID << " depth = " << minDepth << endl;
    
    if ( hitID != NO_INDEX ) {
	vd = vertex( ( unsigned int )hitID, g );
	// change_flag = true;
	return true;
    }

    return false;
}


//
//  Widget::_retrieveCluster --	
//
//  Inputs
//	variable : explanation of a variable
//	variable : explanation of a variable
//
//  Outputs
//
//
void Widget::_retrieveCluster( int nHits, unsigned int * buffer, vector< unsigned int > & ids )
{
    unsigned int * ptr = NULL; //, names;
    //float minDepth = 1000.0;
    //int hitID = NO_INDEX;

    // cerr << "**** retrieveCluster ****" << endl;
    ids.clear();

    ptr = buffer;

    for ( int i = 0; i < nHits; ++i ) { // for each bit
	if ( ptr[ 0 ] != 1 ) {
	    cerr << " Number of names for hit = " << ( int )ptr[ 0 ] << endl;
	    assert( ptr[ 0 ] == 1 );
	}
	//float curDepth = (float)ptr[ 1 ]/0xffffffff;
	int curID = ( int )ptr[ 3 ];
#ifdef DEBUG
	cerr << " i = " << i 
	     << " [0]: " << ptr[ 0 ]
	     << " [1]: " << ptr[ 1 ]
	     << " [2]: " << ptr[ 2 ]
	     << " [3]: " << ptr[ 3 ] << endl;
#endif	// DEBUG
	if ( curID != NO_INDEX ) {
	    vector< unsigned int >::iterator it = std::find( ids.begin(), ids.end(), curID );
	    if ( it == ids.end() ) ids.push_back( curID );
	}
	ptr += 4;
    }

    for ( unsigned int k = 0; k < ids.size(); ++k ) {
	cerr << " set[ " << setw( 2 ) << k << " ] = " << setw( 3 ) << ids[ k ];
	if ( k % 2 == 1 ) cerr << endl;
    }
    if ( ids.size() % 2 == 1 ) cerr << endl;

    update();
}


//
//  Widget::_pickVertex --	
//
//  Inputs
//	x, y : mouse coordinates
//
//  Outputs
//	none
//
bool Widget::_pickVertex( int x, int y, VertexDescriptor & curVD )
{
    Graph &		g		= *_net;
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    unsigned int	selectBuf[ SELECTION_SIZE ];
    int			nHits;
    int			viewport[ 4 ];

    cerr << " pickVertex " << endl;

    glGetIntegerv	( GL_VIEWPORT, viewport );
    
    // Picking begins here
    glSelectBuffer	( SELECTION_SIZE, selectBuf );
    glRenderMode	( GL_SELECT );
    
    glInitNames();
    
    glMatrixMode	( GL_PROJECTION );
    glPushMatrix(); // <====
    glLoadIdentity();
    // create small picking region near cursor location
    //double const size tolerance = 1.0;
    //double const size tolerance = 5.0;
    //const double tolerance = 10.0;
    const double tolerance = 20.0;
#ifdef DEBUG
    cerr << "viewport = " << viewport[0] << "," << viewport[1] << ","
         << viewport[2] << "," << viewport[3] << endl;
    cerr << " x = " << x << " y = " << y << endl;
#endif	// DEBUG
    gluPickMatrix( (double)x, (double)(viewport[3]-y), tolerance, tolerance, viewport );
    
    glOrtho( 
	    center_x() - window_side()*SCREEN_SIDE,
	    center_x() + window_side()*SCREEN_SIDE,
	    center_y() - window_side()*SCREEN_SIDE,
	    center_y() + window_side()*SCREEN_SIDE,
	    -2.0, 2.0 );
    
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();	// <====
    glLoadIdentity();
    
    _draw( GL_SELECT );
    
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();	// <====
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();	// <====
    
    glFlush();

    nHits = glRenderMode( GL_RENDER );
    VertexDescriptor vd = NULL;

    update();

    if ( _handleVertex( nHits, selectBuf, vd ) ) {
	curVD = vd;
	return true;
    }

    return false;
}


//
//  Widget::_selectVertices --	
//
//  Inputs
//	x, y : screen coordinates
//
//  Outputs
//	none
//
void Widget::_selectVertices( int x, int y, bool sw )
{
    Graph &		g		= *_net;
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    unsigned int	selectBuf[ SELECTION_SIZE ];
    int			nHits;
    int			viewport[ 4 ];

    cerr << " selectVertices " << endl;

    double origin_x =	( ( double )_corner_x + x ) / 2.0;
    double origin_y =	( ( double )_corner_y + y ) / 2.0;
    double full_x =	MAX2( 1.0, fabs( _corner_x - x ) );
    double full_y =	MAX2( 1.0, fabs( _corner_y - y ) );

    glGetIntegerv( GL_VIEWPORT, viewport );

    // Selection begins here
    glSelectBuffer( SELECTION_SIZE, selectBuf );
    glRenderMode( GL_SELECT );

    glInitNames();
    
    glMatrixMode( GL_PROJECTION );
    glPushMatrix(); // <====
    glLoadIdentity();

    gluPickMatrix( origin_x, ( double )viewport[ 3 ] - origin_y,
		   full_x, full_y,
		   viewport );

    glOrtho( 
	    center_x() - window_side()*SCREEN_SIDE,
	    center_x() + window_side()*SCREEN_SIDE,
	    center_y() - window_side()*SCREEN_SIDE,
	    center_y() + window_side()*SCREEN_SIDE,
	    -2.0, 2.0 );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix(); // <====
    glLoadIdentity();

    _draw( GL_SELECT );

    glMatrixMode( GL_PROJECTION );
    glPopMatrix(); // <====
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix(); // <====

    glFlush();

    vector< unsigned int > ids;
    nHits = glRenderMode( GL_RENDER );
    _retrieveCluster( nHits, selectBuf, ids );

    for ( unsigned int k = 0; k < ids.size(); ++k ) {
	VertexDescriptor vd = vertex( ids[ k ], g );
	put( vertex_myswitch, g, vd, sw );
    }

    unsigned int nAnnotated = 0;
    BGL_FORALL_VERTICES( vd, g, Graph ) {
	if ( vertexSwitch[ vd ] ) nAnnotated++;
    }
    _net->nAnnotated() = nAnnotated;
    cerr << " No of annotated vertices = " << _net->nAnnotated() << endl;

    update();
}


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	コンストラクタ
//	Constructors
//------------------------------------------------------------------------------
//
//  Widget::~Widget --	destructor
//			デストラクタ
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Widget::Widget( QWidget *parent )
    : QGLWidget( parent )
{
    _timer = new QBasicTimer();
    // _font = QFont( "Arial", FONT_SIZE, QFont::Bold, false );
    _font = QFont( "Tahoma", FONT_SIZE, QFont::Normal, false );
    _flagCell = true;
    _flagLabel = true;
    // _flagLabel = false;
    _flagAspect = true;
    _flagConflict = true;
    _flagAdaptive = false;
    _flagsmoothing = false;
    _flag2stepsmoothing = false;
}




//------------------------------------------------------------------------------
//	デストラクタ
//	Destructor
//------------------------------------------------------------------------------
Widget::~Widget()
{
    //#ifdef ENABLE_TIMER
    delete _timer;
    _timer = NULL;
    //#endif	// ENABLE_TIMER
}


//------------------------------------------------------------------------------
//	メンバ参照
//	Referrring to members
//------------------------------------------------------------------------------




// end of header file
// Do not add any stuff under this line.



