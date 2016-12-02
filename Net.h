//==============================================================================
// Net.h
//	: program file for networks
//
//------------------------------------------------------------------------------
//
//				Date: Thu Nov  3 01:02:45 2016
//
//==============================================================================

#ifndef _Net_H		// begining of header file
#define _Net_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <opencv/cv.h>
using namespace cv;

#include "Graph.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
// for b143 network
// #define B143_NETWORK
// for b106 network
// #define B106_NETWORK

#ifndef	DIVISOR
#define DIVISOR		(29)
#endif	// DIVISOR

#ifndef UNIT_STEP
#define UNIT_STEP	(2)
#endif	// UNIT_STEP

#ifndef MAX_LIMIT_STEP
#define MAX_LIMIT_STEP	(90)    //origin->40
#endif	// MAX_LIMIT_STEP

#ifndef STANDARD_LIMIT_STEP
#define STANDARD_LIMIT_STEP	(40)
#endif	// STARNDARD_LIMIT_STEP

#ifndef MIN_LIMIT_STEP
#define MIN_LIMIT_STEP	(10)
#endif	// MIN_LIMIT_STEP

#ifndef K_NEIGHBORS
//#define K_NEIGHBORS	(3)
#define K_NEIGHBORS	(4)	// This is the best?
//#define K_NEIGHBORS	(6)
#endif	// K_NEIGHBORS

#define	LAMBDA		(0.6307)
#define MU		(-0.673155945)
//#define	LAMBDA		(0.330)
//#define MU		(-0.331)

#ifdef B143_NETWORK
#define	MIN_RATIO	(0.01)	// b143
#else	// B143_NETWORK
#define	MIN_RATIO	(0.0)
#endif	// B143_NETWORK
#define MAX_RATIO	(1.0)
#define STEP_RATIO	(0.1)

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------



class Net : public Graph {

  private:

    static void		_changeHSVToColor	( double hue, double saturation, double value, 
						  unsigned char rgb[ 3 ] );
  protected:

    IplImage *		_diagram;
    map< unsigned int, unsigned int >	_colorMap;
    unsigned int	_nAnnotated;

    unsigned int	_step;	
    double		_ratio;

    int			_width;
    int			_height;

    // center coordinates
    double		_center_x, _center_y;
    
    // scale of the window (length of the side)
    double		_window_side;

    // metric -> 0=EUCLIDEAN,1=MANHATTAN,2=CHEBYSHEV
    int			_metric;
    // mode -> 0=force-directed, 1=centroidal Voronoi, 2=hybrid simple
    // 3=hybrid adaptive, 4=hybrid onestep, 5=hybrid twostep
    int			_mode;
    
    
    //evaluation -> 0=not evaluate, 1 = evaluate
    int         _evaluation;        //---------------add20151015*/

    VertexDescriptor	_curVD;

    unsigned int	_prevID;
    unsigned int	_nextID;

    bool		_labelFlag;	

    bool		_clutterBit;
    double		_conflictArea;
    bool        _clut;
    bool        _ft;
    bool        _smoothingset;
    bool        _gp;

    bool		_spaceBit;

    void		_init	( void );
    void		_reset	( void );
    void		_random	( void );

    void		_load	( const char * filename );
    void		_save	( const char * filename );

    void		_allset		( void );
    void		_allclear	( void );

    void		_force		( void );
    void		_centroid	( void );

    double		_gap		( void );

#ifdef SKIP
    void	        _para		( void );
#endif	// SKIP

    void		_proceed	( void ); // proceed with the blending parameter
    void		_onestep	( void ); // one-step smoothing
    void		_twostep	( void ); // two-step smoothing


  public:

//------------------------------------------------------------------------------
//	コンストラクタ
//	Constructors
//------------------------------------------------------------------------------
    Net();			// default constructor
				// コンストラクタ(デフォルト)	
    Net( const Net & obj );
				// copy constructor
				// コピー・コンストラクタ

//------------------------------------------------------------------------------
//	デストラクタ
//	Destructor
//------------------------------------------------------------------------------
    ~Net();			// destructor
				// デストラクタ

//------------------------------------------------------------------------------
//	メンバ参照
//	Reffwering to members
//------------------------------------------------------------------------------
    IplImage *&			diagram( void )		{ return _diagram; }

    const unsigned int &	nAnnotated( void ) const 
							{ return _nAnnotated; }
    unsigned int &		nAnnotated( void )	{ return _nAnnotated; }

    const unsigned int &	step  ( void )	const	{ return _step; }
    unsigned int &		step  ( void )		{ return _step; }
    const double &		ratio ( void )	const	{ return _ratio; }
    double &			ratio ( void )		{ return _ratio; }
    const double &		ratioF( void )	const	{ return _ratio; }
    const double 		ratioV( void )	const	{ return ( 1.0 - _ratio ); }

    const int &			width ( void )	const	{ return _width; }
    int &			width ( void )		{ return _width; }
    const int &			height( void )	const	{ return _height; }
    int &			height( void )		{ return _height; }

    const double &		center_x( void ) const	{ return _center_x; }
    double &			center_x( void ) 	{ return _center_x; }
    const double &		center_y( void ) const	{ return _center_y; }
    double &			center_y( void ) 	{ return _center_y; }

    const double &		window_side( void ) const
							{ return _window_side; }
    double &			window_side( void )	{ return _window_side; }

    const int &			metric( void )	const	{ return _metric; }
    int &			metric( void )		{ return _metric; }
    
    const int &			evaluation( void )	const	{ return _evaluation; }
    int &			evaluation( void )		{ return _evaluation; }     //------------add20151015

    const int &			mode( void )	const	{ return _mode; }
    void			setMode( int __mode ) {
	_mode = __mode;
	if ( ( _mode == 0 ) || ( _mode == 2 ) ) {	// force-directed or hybrid
	    _ratio = 1.0;
	}
	else if ( _mode == 1 ) {	// centroidal Voronoi
	    _ratio = 0.0;
	}
    }

    VertexDescriptor &		curVD( void )		{ return _curVD; }
    void setVDCoord( int x, int y );

    const unsigned int &	prevID( void )	const	{ return _prevID; }
    unsigned int &		prevID( void )		{ return _prevID; }
    const unsigned int &	nextID( void )	const	{ return _nextID; }
    unsigned int &		nextID( void )		{ return _nextID; }

    void onLabel( void )			{ _labelFlag = true; }
    void offLabel( void )			{ _labelFlag = false; }
    const bool & labelFlag( void ) const	{ return _labelFlag; }

    void setClutterBit( void )			{ _clutterBit = true; }
    void clearClutterBit( void )		{ _clutterBit = false; }
    const bool & clutterBit( void ) const	{ return _clutterBit; }

    double & conflictArea( void )		{ return _conflictArea; }
    const double & conflictArea( void ) const	{ return _conflictArea; }
    
    void setclut( void )			{ _clut = true; }
    void clearclut( void )		{ _clut = false; }
    const bool & clut( void ) const	{ return _clut; }
    
    void setft( void )   { _ft = true; }
    void clearft( void )  { _ft = false; }
    const bool & ft( void ) const { return _ft; }
    
    void setsmoothingset( void )   { _smoothingset = true; }
    void clearsmoothingset( void )  { _smoothingset = false; }
    const bool & smoothingset( void ) const { return _smoothingset; }
    
    void setgp( void )   { _gp = true; }
    void cleargp( void )  { _gp = false; }
    const bool & gp( void ) const { return _gp; }
    
    void setSpaceBit( void )			{ _spaceBit = true; }
    void clearSpaceBit( void )			{ _spaceBit = false; }
    const bool & spaceBit( void )	const	{ return _spaceBit; }

//------------------------------------------------------------------------------
//	Annotateion labels
//------------------------------------------------------------------------------
    void allset( void )				{ _allset(); }
    void allclear( void )			{ _allclear(); }

//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void	reset( void )			{ _reset(); }
    void	random( void )			{ _random(); }

//------------------------------------------------------------------------------
//	Analysis functions
//------------------------------------------------------------------------------
    void	calcEdgeCentrality( void );
    void	calcMST( void );
    void    calcDivideComunity( void );
//------------------------------------------------------------------------------
//	Misc. functions
//------------------------------------------------------------------------------
    void	force	( void )		{ _force(); }
    void	centroid( void )		{ _centroid(); }

    double	gap	( void )		{ return _gap(); }
#ifdef SKIP
    void	para	( void )		{ _para(); }
#endif	// SKIP

    void	proceed	( void )		{ _proceed(); }
    void	onestep	( void )		{ _onestep(); }
    void	twostep	( void )		{ _twostep(); }



    void	initColorMap	( void );

//------------------------------------------------------------------------------
//	file I/O
//------------------------------------------------------------------------------
    void load	( const char * filename )	{ _load( filename ); }
    void save	( const char * filename )	{ _save( filename ); }


//------------------------------------------------------------------------------
//	static functions
//------------------------------------------------------------------------------
    static void		changeNumberToColor	( unsigned int step, unsigned int number,
						  unsigned char rgb[ 3 ] );

//------------------------------------------------------------------------------
//	ソートのための関数
//	Functions for sorting
//------------------------------------------------------------------------------
#ifdef SKIP
    friend int compare ( const Net * a, const Net * b );
    friend int compare ( const Net ** a, const Net ** b );
				// comparison
				//  比較関数
#endif	// SKIP

//------------------------------------------------------------------------------
//	等号
//	equality
//------------------------------------------------------------------------------
#ifdef SKIP
    friend int operator == ( const Net & a, const Net & b );
    friend int operator != ( const Net & a, const Net & b ) {
	return ( ! ( a == b ) );
    }				// equality
				// 等号
#endif	// SKIP

//------------------------------------------------------------------------------
//	代入演算子
//	Assignment opereators
//------------------------------------------------------------------------------
    Net & operator = ( const Net & obj );
				// assignment
				// 代入

//------------------------------------------------------------------------------
//	入出力
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Net & obj );
				// output
				// 出力
    friend istream & operator >> ( istream & stream, Net & obj );
				// input
				// 入力

//------------------------------------------------------------------------------
//	クラス名
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Net"; }
				// class name
				// クラス名
};

#ifdef SKIP
inline int arrange( const Net * a, const Net * b ) {
    return compare( a, b );
}				// ソートのためのふたつのベクトルの比較
#endif	// SKIP

#endif // _Net_H

// end of header file
// Do not add any stuff under this line.
