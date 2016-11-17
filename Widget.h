#ifndef WIDGET_H
#define WIDGET_H

#define WAITING_MODE		( 0)
#define SELECTING_MODE		( 1)
#define DRAGGING_MODE		( 2)

#include <QtOpenGL/QGLWidget>
#include <QtCore/QBasicTimer>

#include "Net.h"
#include "Timer.h"

extern int timecount;    //add 20151231
extern int fixratio;     //add 20160108

class Widget : public QGLWidget
{
    Q_OBJECT
    
private:
    QBasicTimer	*	_timer;

protected:
    // mouse pointer coordinates
    int			_last_pointer_x, _last_pointer_y;

    // corner coordinates
    double		_corner_x, _corner_y;
    
    // mouse mode
    int			_mouse_mode = 0;
			// WAITING_MODE -> SELECTING_MODE -> DRAGGING_MODE -> SELECTING_MODE
			//				  -> CONFIRMING_MODE -> WAITING_MODE
			// WAITING_MODE -> ROTATING_MODE -> WAITING_MODE
			// 
			// WAITING_MODE -> SWITCHING_MODE -> WAITING_MODE

    QFont		_font;

    bool		_flagCell;
    bool		_flagLabel;
    bool		_flagAspect;
    bool		_flagConflict;
    bool		_flagAdaptive;
    bool        _flagsmoothing;
    bool        _flag2stepsmoothing;

    Net			* _net;

    void		_cone		( Coord2 & coord, double weight, double aspect, double up );
    void		_pyramid	( Coord2 & coord, double weight, double aspect, double up );
    void		_prism		( Coord2 & coord, double weight, double aspect, double up );
    void		_draw		( GLenum mode );
    void		_cell		( GLenum mode );
    void		_annotate	( void );
    void		_rectangle	( void );

    void		initializeGL	( void );
    void		paintGL		( void );
    
    void        label_overlap     ( void );
    void        label_evaluation  ( void );       //------------------------add20151006

    void		timerEvent	( QTimerEvent * e );

    bool		_handleVertex	( int nHits, unsigned int * buf, VertexDescriptor & vd );
    void		_retrieveCluster( int nHits, unsigned int * buf, vector< unsigned int > & ids );

    bool		_pickVertex	( int x, int y, VertexDescriptor & curVD );
    void		_selectVertices	( int x, int y, bool sw );



public:
    explicit Widget( QWidget *parent = 0 );
    ~Widget();
    
    const int & pointer_x( void ) const		{ return _last_pointer_x; }
    int & pointer_x( void )			{ return _last_pointer_x; }

    const int & pointer_y( void ) const		{ return _last_pointer_y; }
    int & pointer_y( void )			{ return _last_pointer_y; }

    const double & center_x( void ) const	{ return _net->center_x(); }
    double & center_x( void )			{ return _net->center_x(); }

    const double & center_y( void ) const	{ return _net->center_y(); }
    double & center_y( void )			{ return _net->center_y(); }

    const double & window_side( void ) const	{ return _net->window_side(); }
    double & window_side( void )		{ return _net->window_side(); }

    const int & mouse_mode( void ) const	{ return _mouse_mode; }
    int & mouse_mode( void )			{ return _mouse_mode; }

    const double & corner_x( void ) const	{ return _corner_x; }
    double & corner_x( void )			{ return _corner_x; }
    const double & corner_y( void ) const	{ return _corner_y; }
    double & corner_y( void )			{ return _corner_y; }

    void setNet( Net * __net ) {
	_net = __net;
	_annotate();
    }

    VertexDescriptor & curVD( void )		{ return _net->curVD(); }
    void setVDCoord( int x, int y )		{ _net->setVDCoord( x, y ); }

    const bool & flagCell( void )	const	{ return _flagCell; }
    bool & flagCell( void )			{ return _flagCell; }

    const bool & flagLabel( void )	const	{ return _flagLabel; }
    bool & flagLabel( void )			{ return _flagLabel; }

    const bool & flagAspect( void )	const	{ return _flagAspect; }
    bool & flagAspect( void )			{ return _flagAspect; }

    const bool & flagConflict( void )	const	{ return _flagConflict; }
    bool & flagConflict( void )			{ return _flagConflict; }

    //const bool & flagAdaptive( void )	const	{ return _flagAdaptive; }
    //bool & flagAdaptive( void )			{ return _flagAdaptive; }
    
    const bool & flagsmoothing( void )	const	{ return _flagsmoothing; }
    bool & flagsmoothing( void )			{ return _flagsmoothing; }
    
    const bool & flag2stepsmoothing( void )	const	{ return _flag2stepsmoothing; }
    bool & flag2stepsmoothing( void )			{ return _flag2stepsmoothing; }
    
    void annotate( void )			{ _annotate(); }

    bool pickVertex	( int x, int y, VertexDescriptor & vd ) {
	return _pickVertex( x, y, vd );
    }
    void selectVertices	( int x, int y, bool sw ) {
	_selectVertices( x, y, sw );
    }

    void start( void )				{
	_flagLabel = false;
	checkInETime();
	cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
	checkInCPUTime();
	cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
	_timer->start( 30, this );
    }
    void stop( void )				{
	_timer->stop();
	_flagLabel = true;
    label_evaluation();
    }

    Net *		net( void )		{ return _net; }

    void		flushGL( void )		{ paintGL(); }

signals:

public slots:

};

#endif // WIDGET_H
