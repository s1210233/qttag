//==============================================================================
// Net.cc
//	: program file for networks
//
//------------------------------------------------------------------------------
//
//				Date: Thu Nov  3 01:03:37 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <ctime>

using namespace std;

#include <boost/format.hpp>
#include "Common.h"
#include "Net.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//#define RESPONSIVE_CONTROL

#define CHEBYSHEV_DISTANCE

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------

void Net::_changeHSVToColor( double hue, double saturation, double value, unsigned char rgb[ 3 ] )
{
    int r,g,b;
    int region;
    float fraction;
    int min, max, up, down;
    
    // check and revise the input data
    while ( ( hue > 360.0f ) || ( hue < 0.0f ) ) {
        if ( hue >= 360.0f ) hue -= 360.0f;
        else if ( hue < 0.0f ) hue += 360.0f;
    }
    if ( saturation>1.0f ) saturation = 1.0f;
    else if ( saturation<0.0f ) saturation=0.0f;
    if ( value>1.0f ) value = 1.0f;
    else if ( value < 0.0f ) value = 0.0f;
    
    max = (int)( value*255 );
    
    if ( saturation==0.0f ) {
        r = max;
        g = max;
        b = max;
    }
    else{
        region		= (int)(hue/60.0f);
        fraction	= hue/60.0f-region;
        min		= (int)( max * (1.0f-saturation) );
        up		= min + (int)( fraction*max*saturation );
        down		= max - (int)( fraction*max*saturation );
        
        switch (region) {
            case 0:
                r= max; g= up; b= min; break; // red -> yellow
            case 1:
                r=down; g= max; b= min; break; // yellow -> green
            case 2:
                r= min; g= max; b= up; break; // green -> cyan
            case 3:
                r= min; g=down; b= max; break; // cyan -> blue
            case 4:
                r= up; g= min; b= max; break; // blue -> magenta
            case 5:
                r= max; g= min; b=down; break; // magenta -> red
        }   
    }
    
    rgb[ 0 ] = ( unsigned int )MAX2( 0, MIN2( 255, r ) );
    rgb[ 1 ] = ( unsigned int )MAX2( 0, MIN2( 255, g ) );
    rgb[ 2 ] = ( unsigned int )MAX2( 0, MIN2( 255, b ) );
    
    return;
}




//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

//
//  Net::_init --	initialize the network (called once when necessary)
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Net::_init( void )
{
    // const char theName[] = "void Net::_init : ";

    // srand48( time( NULL ) );
#if defined( B143_NETWORK )
    srand48( 7 ); // for b143 
#else	// defined( B143_NETWORK )
    srand48( 3 );
#endif	// defined( B143_NETWORK )
    _diagram = NULL;
    _metric	= 2;		// Chebyshev distance by default
    _mode	= 2;		// Hybrid layout by default
    
    _evaluation = 0;    //----------------------add20151015

    _center_x = 0.0;
    _center_y = 0.0;
    _window_side = 1.0;     //initial window coordinates -> (-2,-2),(2,-2),(2,2),(-2,2)
    _labelFlag = true;
    _clutterBit = false;
    _conflictArea = 0.0;
    _clut = false;
    _ft = false;
    _gp = false;
    _smoothingset = false;
    _spaceBit	= false;
    
}


//
//  Net::_reset --	reset the network (called when being reused)
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Net::_reset( void )
{
    // const char theName[] = "Net::_reset : ";

    VertexCoordMap	vertexCoord     = get( vertex_mycoord, *this );
    VertexOverlapMap    vertexOverlap   = get( vertex_myoverlap, *this );
    VertexRatioMap	vertexRatio     = get( vertex_myratio, *this );

    _step	= 100;
    _ratio	= ( double )_step/100.0;
    if ( _diagram != NULL ) cvReleaseImage( &_diagram );
    BGL_FORALL_VERTICES( vd, *this, Graph ) {
	vertexOverlap[ vd ] = 0.0;
        vertexRatio[ vd ] = 1.0; // 0.88;
    }
    _spaceBit	= false;
}


//
//  Net::_random --	randomize the node coordinates
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Net::_random( void )
{
    // const char theName[] = "Net::_random : ";

    VertexCoordMap      vertexCoord     = get( vertex_mycoord, *this );

    BGL_FORALL_VERTICES( vd, *this, Graph ) {
	vertexCoord[ vd ] = Coord2( CONTENT_RATIO*SCREEN_SIDE*(2.0*drand48()-1.0), 
				    CONTENT_RATIO*SCREEN_SIDE*(2.0*drand48()-1.0) );
    }
}


//
//  Net::_load --	load the data from the input file
//
//  Inputs
//	_filename	: file name
//
//  Outputs
//	none
//
void Net::_load( const char * filename )
{
    Graph &		graph = ( Graph & )*this;
    VertexIDMap		vertexID	= get( vertex_myid, graph );
    VertexIndexMap	vertexIndex	= get( vertex_index, graph );
    VertexSwitchMap	vertexSwitch    = get( vertex_myswitch, graph );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, graph );
    VertexNameMap       vertexName      = get( vertex_myname, graph );
    VertexWeightMap     vertexWeight    = get( vertex_myweight, graph );
    VertexAspectMap     vertexAspect    = get( vertex_myaspect, graph );
    EdgeFlagMap		edgeFlag	= get( edge_myflag, graph );	
    EdgeIDMap		edgeID		= get( edge_myid, graph );	
    EdgeIndexMap	edgeIndex	= get( edge_index, graph );	
    EdgeWeightMap	edgeWeight	= get( edge_weight, graph );	
    ifstream		ifs( filename );
    char		buf[ BUFFER_SIZE ];

    graph.clear();

    if ( !ifs ) {
    	cerr << "Cannot open the target file : " << filename << endl;
        return;
    }

    //------------------------------------------------------------------------------
    //  Read vertices
    //------------------------------------------------------------------------------
    istringstream istr;

    // number of vertices
    unsigned int numV;
    ifs.getline( buf, sizeof( buf ) );
    istr.clear();
    istr.str( buf );
    istr >> numV;
    cerr << HERE << "Number of vertices = " << numV << endl;

    // information about each vertex
    for ( unsigned int i = 0; i < numV; ++i ) {
	string name;
	double weight;
	double x, y;
	ifs.getline( buf, sizeof( buf ) );
	istr.clear();
	istr.str( buf );
	istr >> name >> weight >> x >> y;
	//cerr << HERE << "Vertex name = " << name.c_str() << " weight = " << weight << endl;
	VertexDescriptor vd = add_vertex( graph );
	vertexID[ vd ] = vertexIndex[ vd ] = i;
	vertexSwitch[ vd ] = false;
	for ( unsigned int k = 0; k < name.length(); ++k ) 
	    if ( name[ k ] == '_' ) name[ k ] = ' ';
	vertexName[ vd ] = name;
	vertexWeight[ vd ] = weight;
	vertexAspect[ vd ] = 1.0; // tentatively set default value
	double const small = 0.05;
	vertexCoord[ vd ] = Coord2( x+small*(2.0*drand48()-1.0),
				    y+small*(2.0*drand48()-1.0) );
#ifdef SKIP
	vertexCoord[ vd ] = Coord2( CONTENT_RATIO*SCREEN_SIDE*(2.0*drand48()-1.0), 
				    CONTENT_RATIO*SCREEN_SIDE*(2.0*drand48()-1.0) );
#endif	// SKIP
    }

    // number of edges
    unsigned int numE;
    ifs.getline( buf, sizeof( buf ) );
    istr.clear();
    istr.str( buf );
    istr >> numE;
    cerr << HERE << "Number of edges = " << numE << endl;

    // link the pair of endpoints with each edge
    for ( unsigned int i = 0; i < numE; ++i ) {
	unsigned int idO, idD;
	ifs.getline( buf, sizeof( buf ) );
	istr.clear();
	istr.str( buf );
	istr >> idO >> idD;
	// cerr << HERE << "Edge = " << idO << " -- " << idD << endl;
	VertexDescriptor vdO = vertex( idO, graph ); 
	VertexDescriptor vdD = vertex( idD, graph ); 
	bool is;
	EdgeDescriptor ed;
	tie( ed, is ) = add_edge( vdO, vdD, graph );
	edgeWeight[ ed ] = 1.0;
	edgeID[ ed ] = edgeIndex[ ed ] = i;
	edgeFlag[ ed ] = false;
    }

    ifs.close();

    _reset();
    _random();

    // cerr << " No of vertices = " << num_vertices( graph ) << endl;
    // cerr << " No of edges = " << num_edges( graph ) << endl;

    return;
}


//
//  Net::_save --	save the data from the input file
//
//  Inputs
//	_filename	: file name
//
//  Outputs
//	none
//
void Net::_save( const char * filename )
{
    Graph &		graph = ( Graph & )*this;
    VertexIDMap		vertexID	= get( vertex_myid, graph );
    VertexSwitchMap	vertexSwitch    = get( vertex_myswitch, graph );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, graph );
    VertexNameMap       vertexName      = get( vertex_myname, graph );
    VertexWeightMap     vertexWeight    = get( vertex_myweight, graph );
    VertexAspectMap     vertexAspect    = get( vertex_myaspect, graph );
    const double	size = 512;
    ofstream		ofs( filename );

    if ( !ofs ) {
    	cerr << "Cannot open the target file : " << filename << endl;
        return;
    }

    //------------------------------------------------------------------------------
    //  Read vertices
    //------------------------------------------------------------------------------
    // number of vertices
    unsigned int numV = num_vertices( graph );
    ofs << numV << endl;

    // information about each vertex
    BGL_FORALL_VERTICES( vd, graph, Graph ) {
	ofs << vertexName[ vd ].c_str() << "\t";
	// double wx = size * 0.25 * ( vertexCoord[ vd ].x() + 2.0 );
	// double wy = size * 0.25 * ( vertexCoord[ vd ].y() + 2.0 );
	double wx = vertexCoord[ vd ].x() / 1.6;
	double wy = vertexCoord[ vd ].y() / 1.6;
	ofs << boost::format( "%6.3f\t%6.3f\t%6.3f" ) % vertexWeight[ vd ] % wx % wy;
	ofs << endl;
    }

    // number of edges
    unsigned int numE = num_edges( graph );
    
    ofs << numE << endl;

    // link the pair of endpoints with each edge
    BGL_FORALL_EDGES( ed, graph, Graph ) {
        VertexDescriptor vdS = source( ed, graph );
        VertexDescriptor vdT = target( ed, graph );
	unsigned int idS = vertexID[ vdS ];
	unsigned int idT = vertexID[ vdT ];
	ofs << idS << "\t" << idT << endl;
    }

    ofs.close();

    return;
}


//
//  Net::_allset --	set all the node annotations
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Net::_allset( void )
{
    VertexSwitchMap      vertexSwitch     = get( vertex_myswitch, *this );

    BGL_FORALL_VERTICES( vd, *this, Graph ) {
	vertexSwitch[ vd ] = true;
    }
    _nAnnotated = num_vertices( *this );
    clearclut();
    cleargp();
}


//
//  Net::_allclear --	clear all the node annotations
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Net::_allclear( void )
{
    VertexSwitchMap      vertexSwitch     = get( vertex_myswitch, *this );

    BGL_FORALL_VERTICES( vd, *this, Graph ) {
	vertexSwitch[ vd ] = false;
    }
    _nAnnotated = 0;
}


//
//  Net::force --	compute the displacements exerted by the force-directed model
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Net::_force( void )
{
    // const char theName[] = "Net::force : ";
    // const double ka = 0.001;
    Graph & g	= ( Graph & )*this;
#if defined( B143_NETWORK ) || defined( B106_NETWORK )
    const double ka = 0.5;	// b143
    const double kr = 0.005;	// b143
#else	// defined( B143_NETWORK ) || defined( B106_NETWORK )
    const double ka = 1.0;
    const double kr = 0.01;
#endif	// defined( B143_NETWORK ) || defined( B106_NETWORK )
    //const double ka = 5.0;
    //const double kr = 0.05;
    // const double L = 0.4;
    // const double L = 1.0;
    double side = _window_side * SCREEN_SIDE;
    double L = sqrt( SQUARE( side ) / ( double )max( 1.0, ( double )_nAnnotated ) );
    // double L = 3.20 * sqrt( SQUARE( side ) / ( double )max( 1.0, ( double )_nAnnotated ) );
    // double L = 5.00 * sqrt( SQUARE( side ) / ( double )max( 1.0, ( double )_nAnnotated ) );
    
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexForceMap	vertexForce	= get( vertex_myforce, g );

    EdgeFlagMap     edgeFlag    = get( edge_myflag, g );
    EdgeFflagMap    edgeFflag   = get( edge_myfflag, g );

    BGL_FORALL_VERTICES( vdi, g, Graph ) {
        Coord2 shift( 0.0, 0.0 );
        // put( vertex_myforce, g, vdi, shift );
        vertexForce[ vdi ] = shift;
        BGL_FORALL_VERTICES( vdj, g, Graph ) {
            if ( vdi == vdj ) {
                ; // do nothing
            }
            else {
                Coord2 diff, unit;
                double dist;
                
                diff = vertexCoord[ vdj ] - vertexCoord[ vdi ];
                dist = diff.norm();
                unit = diff.unit();
                
                // pair< EdgeDescriptor, bool > isConnected = edge( vdi, vdj );
                EdgeDescriptor	ed;
                bool		is;   
                tie( ed, is ) = edge( vdi, vdj, g );
                if ( is ) {
                    // Drawing force by the spring
                    //update 11/17
                        if(edgeFlag[ ed ]== false && edgeFflag[ ed ] == false){
                            vertexForce[ vdi ] += ka * ( dist - L ) * unit;
                        }else if(edgeFlag[ ed ]== true && edgeFflag[ ed ] == true){
                            vertexForce[ vdi ] += ka * ( dist - (L*0.3) ) * unit;
                        }else if(edgeFlag[ ed ]== false && edgeFflag[ ed ] == true){
                            vertexForce[ vdi ] += ka * ( dist - (L*3) ) * unit;
                        }
                }
                // Replusive force by Couloum's power
                vertexForce[ vdi ] -= ( kr/SQUARE( dist ) ) * unit;
            }
        }
    }
    //cout << "net  "<< _finishFlag << endl;
}


//
//  Net::_centroid --	place every node at the center of the corresponding site
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Net::_centroid( void )
{
    // const char theName[] = "Net::centroid : ";
    Graph  &			g = ( Graph & )*this;
    int				h = ( int )_height;
    int				w = ( int )_width;
    VertexCoordMap		vertexCoord	= get( vertex_mycoord, g );
    //VertexIDMap			vertexID	= get( vertex_myid, g );
    VertexPlaceMap		vertexPlace	= get( vertex_myplace, g );
    VertexNumMap		vertexNum	= get( vertex_mynum, g );
    VertexSwitchMap		vertexSwitch	= get( vertex_myswitch, g );
    
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        vertexPlace[ vd ].zero();
        vertexNum[ vd ] = 0;
    }

    int ws = _diagram->widthStep;
    int nc = _diagram->nChannels;
    Coord2 cur;
    for ( int i = 0; i < h; ++i ) {
        cur.y() = 1.0 - ( ( double )i+0.5 )/( double )h;
        for ( int j = 0; j < w; ++j ) {
            cur.x() = ( ( double )j+0.5 )/( double )w;
            
            unsigned char R = ( unsigned char )( _diagram->imageData[ i*ws + j*nc + 0 ] );
            unsigned char G = ( unsigned char )( _diagram->imageData[ i*ws + j*nc + 1 ] );
            unsigned char B = ( unsigned char )( _diagram->imageData[ i*ws + j*nc + 2 ] );
            unsigned int colorID = (unsigned int)R*65536 + (unsigned int)G*256 + (unsigned int)B;
            // if the pixel retains the background color
            map< unsigned int, unsigned int >::iterator	iter = _colorMap.find( colorID );
            
            if ( iter == _colorMap.end() ) { //colorMap -> ( colorID, vertexID[vd] )
                // cerr << " colorID = " << colorID << endl;
                // cerr << " R = " << (int)R << " G = " << (int)G << " B = " << (int)B << endl;
                // assert( iter != colorMap.end() );
            }
            else {
                unsigned int idV = iter->second;    //find vertexID[vd] from colorID
                VertexDescriptor vd = vertex( idV, g );
                vertexPlace[ vd ] += cur; //point of each pixels
                vertexNum[ vd ]++;  // number of pixels belonging to node territory
            }
        }
    }
    
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        // Find the average pixel coordinates of each vertex
	if ( ! vertexSwitch[ vd ] ) {
	    vertexPlace[ vd ].zero();
	}
	else if ( vertexNum[ vd ] != 0 ) {
            Coord2 dest = vertexPlace[ vd ] /= ( double )vertexNum[ vd ];
	    // Screen coordinates have been already translated
	    // double nx =  SCREEN_SIDE * ( 2.0*dest.x() - 1.0 ); //SCREEN_SIDE == 2
	    // double ny =  SCREEN_SIDE * ( 2.0*dest.y() - 1.0 );
	    double nx = _window_side * SCREEN_SIDE * ( 2.0*dest.x() - 1.0 ) + _center_x;
	    double ny = _window_side * SCREEN_SIDE * ( 2.0*dest.y() - 1.0 ) + _center_y;
	    vertexPlace[ vd ] = Coord2( nx, ny ) - vertexCoord[ vd ];
        }
        else {
            cerr << "%%%%%%%%%% Number of pixels vanishes!!!" << endl;
            vertexPlace[ vd ].zero();
        }
    }
}


//
//  Net::_gap --	return the convergence error in laying out the graph
//
//  Inputs
//	none
//
//  Outputs
//	error value
//
double Net::_gap( void )
{
    const char theName[] = "Net::_gap : ";
    	
    Graph &		g	= ( Graph & )*this;
    vector< Coord2 >	displace;
    double		err		= 0.0;
    // const double	scale		= TRANSFORMATION_STEP;
    const double	scale		= TRANSFORMATION_STEP*min(1.0,100.0/( double )num_vertices( *this ));
    const double	force		= CELL_RATIO;
    
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexShiftMap	vertexShift	= get( vertex_myshift, g );
    VertexForceMap	vertexForce	= get( vertex_myforce, g );
    VertexPlaceMap	vertexPlace	= get( vertex_myplace, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    VertexOverlapMap	vertexOverlap	= get( vertex_myoverlap, g );
    VertexRatioMap	vertexRatio	= get( vertex_myratio, g );
    
    //cerr << "Net::_net->gp() = " << gp() << endl;
    //cerr << "Net::_net->clut() = " << clut() << endl;
    
    displace.clear();
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        Coord2 val;
	switch ( _mode ) {
	  case 0:		// force-directed
	      val = vertexForce[ vd ];
	      break;
	  case 1:		// centroidal
	      val = force*vertexPlace[ vd ];
	      break;
	  case 2:		// hybrid simple & adaptive
	  case 3:
	      val = ratioF()*vertexForce[ vd ] + force*ratioV()*vertexPlace[ vd ];
	      break;
	  case 4:		// smoothing
	  case 5:
	      val = vertexRatio[ vd ]*vertexForce[ vd ] +
		  force*( 1.0 - vertexRatio[ vd ] )*vertexPlace[ vd ];
#ifdef DEBUG_SMOOTH
	      cerr << " force = " << vertexForce[ vd ];
	      cerr << " place = " << vertexPlace[ vd ];
	      cerr << " ratio = " << vertexRatio[ vd ] << " : " << 1.0 - vertexRatio[ vd ] << endl;
#endif	// DEBUG_SMOOTH
	      break;
	  default:
	      cerr << "Unexpected error mode" << endl;
	      break;
        }
	assert( vertexID[ vd ] == displace.size() );
        displace.push_back( val );
    }

    // Scale the displacement of a node at each scale
    const double limit = DISPLACEMENT_LIMIT;
    BGL_FORALL_VERTICES( vd, g, Graph ) {
	unsigned int idV = vertexID[ vd ];
	vertexShift[ vd ] = scale * displace[ idV ];
	double norm = vertexShift[ vd ].norm();
	if ( norm > limit ) {
	    vertexShift[ vd ] *= ( limit/norm );
	}
    }
    
    // Move the entire layout while keeping its barycenter at the origin of the
    // screen coordinates
    Coord2 average;
    average.zero();
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        Coord2 newcoord = vertexCoord[ vd ] + vertexShift[ vd ];
        average += newcoord;
    }
    average /= ( double )num_vertices( g );
    
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        vertexShift[ vd ] -= average;
    }
    
    // Force the vertices stay within the screen
    const double central = CONTENT_RATIO;
    // double central = 0.90;
    // double minX = -central * SCREEN_SIDE;
    // double minY = -central * SCREEN_SIDE;
    // double maxX =  central * SCREEN_SIDE;
    // double maxY =  central * SCREEN_SIDE;
    double minX = _center_x - central * _window_side * SCREEN_SIDE;
    double minY = _center_y - central * _window_side * SCREEN_SIDE;
    double maxX = _center_x + central * _window_side * SCREEN_SIDE;
    double maxY = _center_y + central * _window_side * SCREEN_SIDE;
    // cerr << "[ " << minX << " , " << maxX << " ]  x [ " << minY << " , " << maxY << " ] " << endl;
    BGL_FORALL_VERTICES( vd, g, Graph ) {
	if ( vertexSwitch[ vd ] ) {
	    Coord2 newcoord = vertexCoord[ vd ] + vertexShift[ vd ];
	    if ( newcoord.x() < minX )
		vertexShift[ vd ].x() += minX - newcoord.x();
	    if ( newcoord.x() > maxX )
		vertexShift[ vd ].x() += maxX - newcoord.x();
	    if ( newcoord.y() < minY )
		vertexShift[ vd ].y() += minY - newcoord.y();
	    if ( newcoord.y() > maxX )
		vertexShift[ vd ].y() += maxY - newcoord.y();
	}
    }

    BGL_FORALL_VERTICES( vd, g, Graph ) {
        vertexCoord[ vd ] += vertexShift[ vd ]; //vertexCoord[ vd ] renew;
        err += vertexShift[ vd ].squaredNorm();
    }

    return err/( double )num_vertices( g );
}


void Net::_proceed( void )
{
    Graph &		g	= ( Graph & )*this;
    VertexOverlapMap	vertexOverlap	= get( vertex_myoverlap, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    VertexRatioMap	vertexRatio	= get( vertex_myratio, g );
    
    // cerr << "##############################" << "Net::_proceed" << endl;

    BGL_FORALL_VERTICES( vd, g, Graph ) {
#ifdef DEBUG_SMOOTH
	cerr << " Overlap = " << vertexOverlap[ vd ];
#endif	// DEBUG_SMOOTH
	if ( ! vertexSwitch[ vd ] ) {
	    vertexRatio[ vd ] = 1.0;
	}
	else if ( vertexOverlap[ vd ] > EPSILON ) {
	    // double amount = min( 0.1, vertexOverlap[ vd ]*(1.0e+4) );
	    double amount = min( STEP_RATIO, vertexOverlap[ vd ]*(1.0e+3) );
	    // cerr << amount << " " << ends;
	    double newRatio = vertexRatio[ vd ] - amount;
	    vertexRatio[ vd ] = max( MIN_RATIO, newRatio );
	}
    }
}


void Net::_onestep( void )
{
    Graph &		g	= ( Graph & )*this;
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexShiftMap	vertexShift	= get( vertex_myshift, g );
    // VertexForceMap	vertexForce	= get( vertex_myforce, g );
    // VertexPlaceMap	vertexPlace	= get( vertex_myplace, g );
    VertexAspectMap     vertexAspect    = get( vertex_myaspect, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    VertexOverlapMap	vertexOverlap	= get( vertex_myoverlap, g );
    VertexRatioMap	vertexRatio	= get( vertex_myratio, g );
    vector< double >	postRatio;
    // vector< Coord2 >	displace;
    
    // const double	scale		= TRANSFORMATION_STEP;
    const double	scale		= TRANSFORMATION_STEP*min(1.0,100.0/( double )num_vertices( *this ));
    const double	force		= CELL_RATIO;
    unsigned int	nNeighbors = 0;
    multimap< double, VertexDescriptor > neighbor;

    // double Vnum =	num_vertices(*this);
    //double rad = 2.0*sqrt(2.0/Vnum);
    //double rad = 2.0*sqrt(2.0/Vnum)*window_side();
    // double rad = 2.0*sqrt(4.0/Vnum)*window_side();
    // cerr << "##############################" << "Net::_onestep" << endl;

    postRatio.clear();
    BGL_FORALL_VERTICES( vdi, g, Graph ) {
	double curRatio = 1.0;
	if ( vertexSwitch[ vdi ] ) {
	    nNeighbors = 0;
	    neighbor.clear();
	    BGL_FORALL_VERTICES( vdj, g, Graph ) {
		if ( vertexSwitch[ vdj ] && ( vdi != vdj ) ) {
#ifdef CHEBYSHEV_DISTANCE
		    // *** Chebychev distance
		    double aspect = vertexAspect[ vdi ];
		    Coord2 diff = vertexCoord[ vdj ] - vertexCoord[ vdi ];
		    double d = max( abs( diff.x() ), aspect * abs( diff.y() ) );
#else	// CHEBYSHEV_DISTANCE
		    // *** Euclidean distance
		    double d = ( vertexCoord[ vdi ] - vertexCoord[ vdj ] ).norm();
#endif	// CHEBYSHEV_DISTANCE
		    neighbor.insert( pair< double, VertexDescriptor >( d, vdj ) );
		    if ( nNeighbors < K_NEIGHBORS ) nNeighbors++;
		    else {
			multimap< double, VertexDescriptor >::iterator last = neighbor.end();
			last--;
			neighbor.erase( last );
		    }
		}
	    }
	    curRatio = 0.0;
	    // unsigned int counter = 0;
	    for ( multimap< double, VertexDescriptor >::iterator iter = neighbor.begin();
		  iter != neighbor.end(); ++iter ) {
		curRatio += vertexRatio[ iter->second ];
		// counter++;
	    }
	    // cerr << " nNeighbors = " << nNeighbors << " counter = " << counter << endl;
	    // assert( nNeighbors == counter );
#ifdef DEBUG_SMOOTH
	    cerr << " nAdj = " << nAdj;
#endif	// DEBUG_SMOOTH
	    if ( nNeighbors > 0 ) curRatio /= ( double )nNeighbors;
	    else {
#ifdef RESPONSIVE_CONTROL
		curRatio = 1.0; 
#else	// RESPONSIVE_CONTROL
		curRatio = vertexRatio[ vdi ];
#endif	// RESPONSIVE_CONTROL
	    }
	}
	postRatio.push_back( curRatio );
	assert( vertexID[ vdi ] == postRatio.size() - 1 );
    }

    BGL_FORALL_VERTICES( vd, g, Graph ) {
	if ( vertexSwitch[ vd ] ) {
	    vertexRatio[ vd ] = min( MAX_RATIO, max( MIN_RATIO, postRatio[ vertexID[ vd ] ] ) );
	}
    }
}

void Net::_twostep( void )
{
    Graph &		g	= ( Graph & )*this;
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexShiftMap	vertexShift	= get( vertex_myshift, g );
    // VertexForceMap	vertexForce	= get( vertex_myforce, g );
    // VertexPlaceMap	vertexPlace	= get( vertex_myplace, g );
    VertexAspectMap	vertexAspect	= get( vertex_myaspect, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    VertexOverlapMap	vertexOverlap	= get( vertex_myoverlap, g );
    VertexRatioMap	vertexRatio	= get( vertex_myratio, g );
    vector< double >	postRatio;
    // vector< Coord2 >	displace;
    // const double	scale		= TRANSFORMATION_STEP;
    const double	scale		= TRANSFORMATION_STEP*min(1.0,100.0/( double )num_vertices( *this ));
    const double	force		= CELL_RATIO;

    unsigned int	nNeighbors = 0;
    multimap< double, VertexDescriptor > neighbor;

    // double Vnum =	num_vertices( *this );
    //double rad = 2.0*sqrt(2.0/Vnum);
    // double rad = 2.0*sqrt(2.0/Vnum)*window_side();
    // double rad = 2.0*sqrt(4.0/Vnum)*window_side();
    // double d;
    // double pre_vr, d_vr;    // double para;
    
    //cerr << "Vnum = " << Vnum << endl;

//------------------------------------------------------------------------------
//	1st shrinking step
//------------------------------------------------------------------------------
    postRatio.clear();
    BGL_FORALL_VERTICES( vdi, g, Graph ) {
	double curRatio = 1.0;
	if ( vertexSwitch[ vdi ] ) {
	    nNeighbors = 0;
	    neighbor.clear();
	    BGL_FORALL_VERTICES( vdj, g, Graph ) {
		if ( vertexSwitch[ vdj ] && ( vdi != vdj ) ) {
#ifdef CHEBYSHEV_DISTANCE
		    // *** Chebychev distance
		    double aspect = vertexAspect[ vdi ];
		    Coord2 diff = vertexCoord[ vdj ] - vertexCoord[ vdi ];
		    double d = max( abs( diff.x() ), aspect * abs( diff.y() ) );
#else	// CHEBYSHEV_DISTANCE
		    // *** Euclidean distance
		    double d = ( vertexCoord[ vdi ] - vertexCoord[ vdj ] ).norm();
#endif	// CHEBYSHEV_DISTANCE
		    neighbor.insert( pair< double, VertexDescriptor >( d, vdj ) );
		    if ( nNeighbors < K_NEIGHBORS ) nNeighbors++;
		    else {
			multimap< double, VertexDescriptor >::iterator last = neighbor.end();
			last--;
			neighbor.erase( last );
		    }
		}
	    }
	    curRatio = 0.0;
	    // unsigned int counter = 0;
	    for ( multimap< double, VertexDescriptor >::iterator iter = neighbor.begin();
		  iter != neighbor.end(); ++iter ) {
		curRatio += vertexRatio[ iter->second ];
		// counter++;
	    }
	    // cerr << " nNeighbors = " << nNeighbors << " counter = " << counter << endl;
	    // assert( nNeighbors == counter );
	    if ( nNeighbors > 0 ) curRatio /= ( double )nNeighbors;
	    else {
#ifdef RESPONSIVE_CONTROL
		curRatio = 1.0;
#else	// RESPONSIVE_CONTROL
		curRatio = vertexRatio[ vdi ];
#endif	// RESPONSIVE_CONTROL
	    }
	}
	postRatio.push_back( curRatio );
	assert( vertexID[ vdi ] == postRatio.size() - 1 );
    }

    BGL_FORALL_VERTICES( vd, g, Graph ) {
	if ( vertexSwitch[ vd ] ) {
	    double newRatio =
		LAMBDA * ( postRatio[ vertexID[ vd ] ] - vertexRatio[ vd ] ) + vertexRatio[ vd ];
	    vertexRatio[ vd ] = min( MAX_RATIO, max( MIN_RATIO, newRatio ) );
	}
    }
	
//------------------------------------------------------------------------------
//	2nd expanding step
//------------------------------------------------------------------------------
    postRatio.clear();
    BGL_FORALL_VERTICES( vdi, g, Graph ) {
	double curRatio = 1.0;
	if ( vertexSwitch[ vdi ] ) {
	    nNeighbors = 0;
	    neighbor.clear();
	    BGL_FORALL_VERTICES( vdj, g, Graph ) {
		if ( vertexSwitch[ vdj ] && ( vdi != vdj ) ) {
#ifdef CHEBYSHEV_DISTANCE
		    // *** Chebychev distance
		    double aspect = vertexAspect[ vdi ];
		    Coord2 diff = vertexCoord[ vdj ] - vertexCoord[ vdi ];
		    double d = max( abs( diff.x() ), aspect * abs( diff.y() ) );
#else	// CHEBYSHEV_DISTANCE
		    // *** Euclidean distance
		    double d = ( vertexCoord[ vdi ] - vertexCoord[ vdj ] ).norm();
#endif	// CHEBYSHEV_DISTANCE
		    neighbor.insert( pair< double, VertexDescriptor >( d, vdj ) );
		    if ( nNeighbors < K_NEIGHBORS ) nNeighbors++;
		    else {
			multimap< double, VertexDescriptor >::iterator last = neighbor.end();
			last--;
			neighbor.erase( last );
		    }
		}
	    }
	    curRatio = 0.0;
	    // unsigned int counter = 0;
	    for ( multimap< double, VertexDescriptor >::iterator iter = neighbor.begin();
		  iter != neighbor.end(); ++iter ) {
		curRatio += vertexRatio[ iter->second ];
		// counter++;
	    }
	    // cerr << " nNeighbors = " << nNeighbors << " counter = " << counter << endl;
	    // assert( nNeighbors == counter );
	    if ( nNeighbors > 0 ) curRatio /= ( double )nNeighbors;
	    else {
#ifdef RESPONSIVE_CONTROL
		curRatio = 1.0;
#else	// RESPONSIVE_CONTROL
		curRatio = vertexRatio[ vdi ];
#endif	// RESPONSIVE_CONTROL
	    }
	}
	postRatio.push_back( curRatio );
	assert( vertexID[ vdi ] == postRatio.size() - 1 );
    }

    BGL_FORALL_VERTICES( vd, g, Graph ) {
	if ( vertexSwitch[ vd ] ) {
	    double newRatio =
		MU * ( postRatio[ vertexID[ vd ] ] - vertexRatio[ vd ] ) + vertexRatio[ vd ];
	    vertexRatio[ vd ] = min( MAX_RATIO, max( MIN_RATIO, newRatio ) );
	}
    }
}

//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	コンストラクタ
//	Constructors
//------------------------------------------------------------------------------

//
//  Net::Net --	default constructor
//			コンストラクタ(デフォルト)
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Net::Net() 
      : Graph()  	
{
    _init();
    _reset();
    _random();
}


//
//  Net::Net --	copy constructor
//			コピー・コンストラクタ
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Net::Net( const Net & obj )
      : Graph( ( const Graph & ) obj )
{
    _init();
    _reset();
    _random();
    if ( this != &obj ) {
	( Graph & )*this = obj;
    }
}


//------------------------------------------------------------------------------
//	デストラクタ
//	Destructor
//------------------------------------------------------------------------------

//
//  Net::~Net --	destructor
//			デストラクタ
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Net::~Net()
{

}


//------------------------------------------------------------------------------
//	メンバ参照
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Analysis functions
//------------------------------------------------------------------------------

//
//  Net::calcEdgeCentrality --	calculate the edge centrality
//
//  Inputs
//	none
//
//  Outputs
//
//
void Net::calcEdgeCentrality( void )
{
    edgeCentrality( *this );
}


//
//  Net::calcMST --	calculate the minimum spanning tree
//
//  Inputs
//	none
//
//  Outputs
//
//
void Net::calcMST( void )
{
    minimumSpanningTree( *this );

   // cout << "test " << _finishFlag << endl;    
}



//------------------------------------------------------------------------------
//	Misc. functions
//------------------------------------------------------------------------------
//
//  Net::initColorMap --	initialize the color map
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Net::initColorMap( void )
{
    const char theName[] = "Net::initColorMap : ";
    Graph &		g = ( Graph & )*this;
    unsigned char	col[ 3 ];
    VertexIDMap		vertexID =	get( vertex_myid, g );
    VertexSwitchMap	vertexSwitch =	get( vertex_myswitch, g );
    // cerr << HERE << " Number of vertices = " << num_vertices( g ) << endl;

    _colorMap.clear();
    BGL_FORALL_VERTICES( vd, g, Graph ) {
	if ( vertexSwitch[ vd ] ) {
	    unsigned int idV = vertexID[ vd ];
	    changeNumberToColor( DIVISOR, idV, col );
	    unsigned int colorID = ( (unsigned int)col[0]*65536 + 
				     (unsigned int)col[1]*256 + (unsigned int)col[2] );
	    // cerr << HERE << " ID = " << idV << " color = " << colorID << endl;
	    _colorMap.insert( pair< unsigned int, unsigned int >( colorID, idV ) );
	}
    }
}



//------------------------------------------------------------------------------
//	Static functions
//------------------------------------------------------------------------------
void Net::changeNumberToColor( unsigned int step, unsigned int number, unsigned char rgb[ 3 ] )
{
    const int nOffsets = 7;
    double offset[ nOffsets ] = {
        0.50, 0.25, 0.75, 0.125, 0.375, 0.625, 0.875
        // 0.50, 0.75, 0.375, 0.125, 0.625, 0.250, 0.875
    };
    int quotient = number / step;
    int residue = number % step;
    int index = quotient % nOffsets;
    double hue = ( 360.0f / (double)step ) * ( ( double )residue + offset[ index ] );
#ifdef DEBUG
    cerr << " quotient = " << quotient;
    cerr << " residue = " << residue;
    cerr << " offset = " << offset;
    cerr << " hue = " << hue;
    cerr << endl;
#endif	// DEBUG
    _changeHSVToColor ( hue, 1.0f, 1.0f, rgb );
}



void Net::setVDCoord( int x, int y )
{
    double rx = 2.0 * ( double )x /( double )_width - 1.0;
    double ry = -2.0 * ( double )y /( double )_height + 1.0;
    double sx = rx * _window_side * SCREEN_SIDE + _center_x;
    double sy = ry * _window_side * SCREEN_SIDE + _center_y;
    Coord2 coord = Coord2( sx, sy );
    put( vertex_mycoord, *this, _curVD, coord );
}


//------------------------------------------------------------------------------
//	ソートのための関数
//	Functions for sorting
//------------------------------------------------------------------------------

#ifdef SKIP
//
//  compare --	comparison function for sorting
//		ソートのための比較関数
//
//  Inputs
//	a, b	: pointers to objects of this class
//		  このクラスへのオブジェクトへのポインタ
//
//  Outputs
//	a <  b	-1
//	a == b	 0
//	a >  b	 1
//
int compare ( const Net * a, const Net * b )
{
    int (*cf)( const Graph *, const Graph * );
    cf = compare;
    return ( (*cf)( &(a->Graph), &(b->Graph) ) );

    return compare( ( const Graph * )a, ( const Graph * )b );
}
#endif	// SKIP


//------------------------------------------------------------------------------
//	等号
//	equality
//------------------------------------------------------------------------------

#ifdef SKIP
//
//  operator == --	equality
//			等号
//
//  Inputs
//	a, b	: objects of this class
//
//  Outputs
//	boolean value
//
int operator == ( const Net & a, const Net & b )
{

    return ( ( const Graph & )a == ( const Graph & )b );
}
#endif	// SKIP


//------------------------------------------------------------------------------
//	代入演算子
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Net::operator = --	assignement
//				代入
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Net & Net::operator = ( const Net & obj )
{
    if ( this != &obj ) {
	( Graph & )*this = obj;
    }
    return *this;
}


//------------------------------------------------------------------------------
//	入出力
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Argument
//	stream	: output stream
//	obj	: object of this class
//
//  Outputs
//	output stream
//
ostream & operator << ( ostream & stream, const Net & obj )
{
    // stream << ( const Graph & )obj;	
    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: input stream
//	obj	: object of this class
//
//  Outputs
//	input stream
//
istream & operator >> ( istream & stream, Net & obj )
{
    // stream >> ( Graph & )obj;
    return stream;
}


// end of header file
// Do not add any stuff under this line.
