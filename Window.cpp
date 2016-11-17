//==============================================================================
// Window.cc
//	: program file for WINDOW class
//
//------------------------------------------------------------------------------
//
//				Date: Thu Nov  3 01:07:46 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Window.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	File menu
void Window::selectTest( void )
{
    cerr << "Test menu is selected!!!" << endl;
}

void Window::loadData()
{
    QString fileName = QFileDialog::getOpenFileName( this,
						     tr("Choose a file name"), "./data/",
						     tr("Data files (*.dat)"));
    if ( fileName.isEmpty() )
        return;

    _widget->net()->load( fileName.toStdString().c_str() );
    _widget->annotate();

    cerr << "Load a graph from : " << fileName.toStdString() << endl;
    _widget->repaint();
}


void Window::saveData()
{
    QString fileName = QFileDialog::getSaveFileName( this,
						     tr("Choose a file name"), "./data/",
						     tr("Data files (*.dat)"));
    if ( fileName.isEmpty() )
        return;

    _widget->net()->save( fileName.toStdString().c_str() );

    cerr << "Save a graph to : " << fileName.toStdString() << endl;
    _widget->repaint();
}


void Window::capture()
{
    QString fileName = QFileDialog::getSaveFileName( this,
						     tr("Choose a file name"), ".",
						     tr("Image files (*.png *.jpg)"));
    if ( fileName.isEmpty() )
        return;

    _widget->flushGL();
    _widget->flushGL();
    _widget->flushGL();

    QImage screenshot;

    screenshot = _widget->grabFrameBuffer();
    screenshot.save( fileName );
    cerr << "Saving an image as : " << fileName.toStdString() << endl;
    _widget->repaint();
    
}

//------------------------------------------------------------------------------
//	Optimize menu
void Window::oprReset( void )
{
    _widget->net()->reset();
    _widget->repaint();
}

void Window::oprStart( void )
{
    _widget->start();
}

void Window::oprStop( void )
{
    _widget->stop();
    _widget->repaint();
}

void Window::oprRandom( void )
{
    _widget->net()->random();
    _widget->repaint();
}

//------------------------------------------------------------------------------
//	Analyze menus
void Window::calcEdgeCentrality( void )
{
    _widget->net()->calcEdgeCentrality();
    _widget->repaint();
}

void Window::calcMST( void )
{
    _widget->net()->calcMST();
    _widget->repaint();
}

//------------------------------------------------------------------------------
//	Annotate menus
void Window::allSet( void )
{
    _widget->net()->allset();
    _widget->repaint();
}

void Window::allClear( void )
{
    _widget->net()->allclear();
    _widget->repaint();
}

void Window::onLabel( void )
{
    _widget->net()->onLabel();
    _widget->repaint();
}

void Window::offLabel( void )
{
    _widget->net()->offLabel();
    _widget->repaint();
}

//------------------------------------------------------------------------------
//	Layout menus
void Window::setLayout( int sw )
{
    _widget->net()->reset();

    switch ( sw ) {
      case 0:
	  cerr << "Force-directed layout" << endl;
	  break;
      case 1:
	  cerr << "Centroidal Voronoi layout" << endl;
	  _widget->flagCell() = true;
	  break;
      case 2:
	  cerr << "Hybrid simple layout" << endl;
	  _widget->flagCell() = true;
	  break;
      case 3:
	  cerr << "Hybrid adaptive layout" << endl;
	  _widget->flagCell() = true;
	  break;
      case 4:
	  cerr << "Hybrid onestep smoothing" << endl;
	  _widget->flagCell() = true;
	  break;
      case 5:
	  cerr << "Hybrid twostep smoothing" << endl;
	  _widget->flagCell() = true;
	  break;
    }
    _widget->net()->setMode( sw );
    _widget->repaint();

    checkInCPUTime();
}


//------------------------------------------------------------------------------
//	Metric menus
void Window::setMetric( int sw )
{
    _widget->net()->metric() = sw;
    switch ( sw ) {
      case 0:
	  cerr << "Euclidean distance" << endl;
	  break;
      case 1:
	  cerr << "Manhattan distance" << endl;
	  break;
      case 2:
	  cerr << "Chebyshev distance" << endl;
	  break;
    }
    _widget->repaint();
}

//------------------------------------------------------------------------------
//	Flag menu
void Window::switchFlag( bool & flag )
{
    flag = !flag;
    if ( flag ) cerr << "Flag enabled" << endl;
    else cerr << "Flag disabled" << endl;
    _widget->repaint();
}
//------------------------------------------------------------------------------
// Adaptive menu
//void Window::switchAdaptive( bool & flag )
//{
//    flag = !flag;
//    if ( flag ) cerr << "Adaptive layout" << endl;
//    else cerr << "NOT Adaptive layout" << endl;
//    _widget->repaint();
//}
//------------------------------------------------------------------------------
// smoothing menu
void Window::switchsmoothing( bool & flag )
{
    flag = !flag;
    if ( flag ) cerr << "smoothing layout" << endl;
    else cerr << "NOT smoothing layout" << endl;
    _widget->repaint();
}
//------------------------------------------------------------------------------
// 2stepsmoothing menu
void Window::switch2stepsmoothing( bool & flag )
{
    flag = !flag;
    if ( flag ) cerr << "2step smoothing layout" << endl;
    else cerr << "NOT 2 step smoothing layout" << endl;
    _widget->repaint();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Protected Slots
//------------------------------------------------------------------------------

void Window::keyPressEvent( QKeyEvent *e )
{
    cerr << " Modifiers = " << e->modifiers() << endl;
    if ( e->key() == Qt::Key_Alt ) cerr << "Alt key" << endl;

    switch( e->key() ){
    case Qt::Key_Escape:
        close();
        break;
    default:
        QWidget::keyPressEvent( e );
    }
    update();
}


void Window::mouseMoveEvent( QMouseEvent *e )
{
    const double ratioV = 2.0;
    const double ratioS = 4.0;
    const double ratioT = 2.0;

    switch ( e->buttons() ){
    case Qt::LeftButton:
	if ( e->type() == QEvent::MouseMove ) {
	    if ( _widget->mouse_mode() == DRAGGING_MODE ) {
		_widget->setVDCoord( e->x(), e->y() );
	    }
	    _widget->pointer_x() = e->x();
	    _widget->pointer_y() = e->y();
	}
        break;
    case Qt::RightButton:
	if ( e->type() == QEvent::MouseMove ) {
	    // cerr << "right button x = " << e->x() << " y = " << e->y() << endl;
	    _widget->pointer_x() = e->x();
	    _widget->pointer_y() = e->y();
	}
        break;
      case Qt::MiddleButton:
	if ( e->type() == QEvent::MouseMove ) {
#ifdef SKIP
	    _widget->maxSide() -= 
		ratioS * ( ( double ) e->y() - _last_pointer_y ) / ( double )( height() );
#endif	// SKIP
	    _widget->center_x() += 
		-ratioT * ( double )( e->x() -  _widget->pointer_x() ) / ( double )width();
	    _widget->center_y() += 
		ratioT * ( double )( e->y() -  _widget->pointer_y() ) / ( double )height();
#ifdef DEBUG
	    cerr << " centerx = " << _widget->center_x()
		 << " centery = " << _widget->center_y() << endl;
#endif	// DEBUG
	    _widget->pointer_x() = e->x();
	    _widget->pointer_y() = e->y();
	}
        break;
      default:
	  QWidget::mouseMoveEvent( e );
    }
    update();
}


void Window::mousePressEvent( QMouseEvent *e )
{
    // cerr << "**** Press" << endl;
#ifdef DEBUG
    cerr << " ebuttons = " << e->buttons() << endl;
    cerr << " left = " << Qt::LeftButton << endl;
    cerr << " right = " << Qt::RightButton << endl;
#endif	// DEBUG

    if ( e->buttons() & Qt::LeftButton ) {
	// cerr << "**** left Press" << endl;
	if ( e->type() == QEvent::MouseButtonPress ) {
	    _left_button = true;
	    if ( _widget->pickVertex( e->x(), e->y(), _widget->curVD() ) )
		_widget->mouse_mode() = DRAGGING_MODE;
	    else
		_widget->mouse_mode() = WAITING_MODE;
	    _widget->pointer_x() = e->x();
	    _widget->pointer_y() = e->y();
	}
    }
    else if ( e->buttons() & Qt::RightButton ) {
	// cerr << "**** right Press: type = " << e->type() << endl;
	if ( e->type() == QEvent::MouseButtonPress ) {
	    cerr << "Right mouse button pressed" << endl;
	    _right_button = true;
	    _widget->mouse_mode() = SELECTING_MODE;
	    _widget->corner_x() = e->x();
	    _widget->corner_y() = e->y();
	    _widget->pointer_x() = e->x();
	    _widget->pointer_y() = e->y();
	}
    }
    else if ( e->buttons() & Qt::MiddleButton ) {
	// cerr << "**** middle Press" << endl;
	if ( e->type() == QEvent::MouseButtonPress ) {
	    _widget->pointer_x() = e->x();
	    _widget->pointer_y() = e->y();
	}
    }
    else {
	  QWidget::mousePressEvent( e );
    }

    update();
}


void Window::mouseReleaseEvent( QMouseEvent *e )
{
    const int limit = 4;
    // cerr << "**** Release" << endl;

#ifdef DEBUG
    cerr << " ebuttons = " << e->buttons() << endl;
    cerr << " left = " << Qt::LeftButton << endl;
    cerr << " right = " << Qt::RightButton << endl;
#endif	// DEBUG

    if ( e->type() == QEvent::MouseButtonRelease ) {
	if ( _left_button ) {
	    // cerr << "**** left Release" << endl;
	    _left_button = false;
	    // _last_pointer_x = e->x();
	    // _last_pointer_y = e->y();
	}
	else if ( _right_button ) {
	    _widget->selectVertices( e->x(), e->y(), _widget->net()->labelFlag() );
	    _widget->mouse_mode() = WAITING_MODE;
	    _right_button = false;
	}
	else if ( _middle_button ) {
	    // _last_pointer_x = e->x();
	    // _last_pointer_y = e->y();
	    _middle_button = false;
	}
    }
    else {
	QWidget::mouseReleaseEvent( e );
    }

    _widget->repaint();
    update();
}


void Window::wheelEvent( QWheelEvent *e )
{
#ifdef DEBUG
    cerr << " delta = " << e->delta() << ends;
#endif	// DEBUG
    // _widget->window_side() += e->delta()/12000.0;
    _widget->window_side() += e->delta()/1200.0;
    _widget->window_side() = max( 0.1, min( 10.0, _widget->window_side() ) );
    cerr << " side = " << _widget->window_side() << endl;
    _widget->repaint();
    update();
}


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
void Window::initParams( void )
{
    _left_button	= false;
    _middle_button	= false;
    _right_button	= false;
}

void Window::createActions( void )
{
    // File:	test
    selTestAct = new QAction( tr("T&est"), this );
    connect( selTestAct, SIGNAL( triggered() ), this, SLOT( selectTest() ) );
    // File:	load
    actLoad		= new QAction( tr("L&oad"), this );
    actLoad->setShortcut( tr("Ctrl+L") );
    actLoad->setStatusTip( tr("Load a graph file") );
    connect( actLoad, SIGNAL( triggered() ), this, SLOT( loadData() ) );
    // File:	save
    actSave		= new QAction( tr("S&ave"), this );
    actSave->setShortcut( tr("Ctrl+S") );
    actSave->setStatusTip( tr("Save a graph file") );
    connect( actSave, SIGNAL( triggered() ), this, SLOT( saveData() ) );
    // File:	capture
    actCapture		= new QAction( tr("Cap&ture"), this );
    actCapture->setShortcut( tr("Ctrl+P") );
    actCapture->setStatusTip( tr("Capture the main window as an image") );
    connect( actCapture, SIGNAL( triggered() ), this, SLOT( capture() ) );

    // Optimize:reset
    actReset		= new QAction( tr("R&eset"), this );
    actReset->setShortcut( tr("Ctrl+R") );
    actReset->setStatusTip( tr("Reset the graph layout") );
    connect( actReset, SIGNAL( triggered() ), this, SLOT( oprReset() ) );
    // Optimize:start
    actStart		= new QAction( tr("S&tart"), this );
    actStart->setShortcut( tr("Ctrl+S") );
    actStart->setStatusTip( tr("Start the graph layout") );
    connect( actStart, SIGNAL( triggered() ), this, SLOT( oprStart() ) );
    // Optimize:start
    actStop		= new QAction( tr("St&op"), this );
    actStop->setShortcut( tr("Ctrl+T") );
    actStop->setStatusTip( tr("Stop the graph layout") );
    connect( actStop, SIGNAL( triggered() ), this, SLOT( oprStop() ) );
    // Optimize:reset
    actRandom		= new QAction( tr("R&andom"), this );
    actRandom->setShortcut( tr("Ctrl+D") );
    actRandom->setStatusTip( tr("Randominze the graph layout") );
    connect( actRandom, SIGNAL( triggered() ), this, SLOT( oprRandom() ) );

    // Analyze: edge centrality
    actEdgeCentrality	= new QAction( tr("Edge Centrality"), this );
    actEdgeCentrality->setShortcut( tr("Alt+E") );
    actEdgeCentrality->setStatusTip( tr("Compute edge centrality") );
    connect( actEdgeCentrality, SIGNAL( triggered() ), this, SLOT( calcEdgeCentrality() ) );
    // Analyze: MST
    actMST	= new QAction( tr("MST"), this );
    actMST->setShortcut( tr("Alt+M") );
    actMST->setStatusTip( tr("Compute minimum spanning tree") );
    connect( actMST, SIGNAL( triggered() ), this, SLOT( calcMST() ) );

    // Annotate: allSet
    actSet		= new QAction( tr("All Set"), this );
    actSet->setShortcut( tr("Alt+S") );
    actSet->setStatusTip( tr("Set all the node annotations") );
    connect( actSet, SIGNAL( triggered() ), this, SLOT( allSet() ) );
    // Annotate: allClear
    actClear		= new QAction( tr("All Clear"), this );
    actClear->setShortcut( tr("Alt+C") );
    actClear->setStatusTip( tr("Clear all the node annotations") );
    connect( actClear, SIGNAL( triggered() ), this, SLOT( allClear() ) );
    // Annotate: onLabel
    actOn		= new QAction( tr("Label ON"), this );
    actOn->setShortcut( tr("ALT+N") );
    actOn->setStatusTip( tr("Label ON") );
    connect( actOn, SIGNAL( triggered() ), this, SLOT( onLabel() ) );
    // Annotate: offLabel
    actOff		= new QAction( tr("Label OFF"), this );
    actOff->setShortcut( tr("ALT+F") );
    actOff->setStatusTip( tr("Label OFF") );
    connect( actOff, SIGNAL( triggered() ), this, SLOT( offLabel() ) );

    // Layout:	force
    actForce		= new QAction( tr("F&orce"), this );
    actForce->setShortcut( tr("Ctrl+F") );
    actForce->setStatusTip( tr("Select force-directed layout") );
    connect( actForce, SIGNAL( triggered() ), this, SLOT( setForce() ) );
    // Layout:	centroid
    actCentroid		= new QAction( tr("Centroidal"), this );
    actCentroid->setShortcut( tr("Ctrl+V") );
    actCentroid->setStatusTip( tr("Select centroidal Voronoi layout") );
    connect( actCentroid, SIGNAL( triggered() ), this, SLOT( setCentroid() ) );
    // Layout:	hybrid simple
    actSimple		= new QAction( tr("S&imple"), this );
    actSimple->setShortcut( tr("Ctrl+i") );
    actSimple->setStatusTip( tr("Select hybrid simple") );
    connect( actSimple, SIGNAL( triggered() ), this, SLOT( setSimple() ) );
    // Layout:	hybrid simple
    actAdaptive		= new QAction( tr("A&daptive"), this );
    actAdaptive->setShortcut( tr("Ctrl+d") );
    actAdaptive->setStatusTip( tr("Select hybrid adaptive") );
    connect( actAdaptive, SIGNAL( triggered() ), this, SLOT( setAdaptive() ) );
    // Layout:	hybrid onestep
    actOnestep		= new QAction( tr("Onestep"), this );
    actOnestep->setShortcut( tr("Ctrl+1") );
    actOnestep->setStatusTip( tr("Select hybrid onestep") );
    connect( actOnestep, SIGNAL( triggered() ), this, SLOT( setOnestep() ) );
    // Layout:	hybrid twostep
    actTwostep		= new QAction( tr("Twostep"), this );
    actTwostep->setShortcut( tr("Ctrl+2") );
    actTwostep->setStatusTip( tr("Select hybrid twostep") );
    connect( actTwostep, SIGNAL( triggered() ), this, SLOT( setTwostep() ) );

    // Metric:	Euclidean
    actEuclidean	= new QAction( tr("E&ulidean"), this );
    actEuclidean->setShortcut( tr("Ctrl+E") );
    actEuclidean->setStatusTip( tr("set Euclidean distance metric") );
    connect( actEuclidean, SIGNAL( triggered() ), this, SLOT( setEuclidean() ) );
    // Metric:	Manhattan
    actManhattan	= new QAction( tr("M&anhattan"), this );
    actManhattan->setShortcut( tr("Ctrl+M") );
    actManhattan->setStatusTip( tr("set Manhattan distance metric") );
    connect( actManhattan, SIGNAL( triggered() ), this, SLOT( setManhattan() ) );
    // Metric:	Chebyshev
    actChebyshev	= new QAction( tr("C&hebyshev"), this );
    actChebyshev->setShortcut( tr("Ctrl+C") );
    actChebyshev->setStatusTip( tr("set Chebyshev distance metric") );
    connect( actChebyshev, SIGNAL( triggered() ), this, SLOT( setChebyshev() ) );

    // Flag:	Cell
    actCell		= new QAction( tr("Ce&ll"), this );
    actCell->setShortcut( tr("Ctrl+B") );
    actCell->setStatusTip( tr("active/deactivate Voronoi cells") );
    connect( actCell, SIGNAL( triggered() ), this, SLOT( switchCell() ) );
    // Flag:	Label
    actLabel		= new QAction( tr("La&bel"), this );
    actLabel->setShortcut( tr("Ctrl+A") );
    actLabel->setStatusTip( tr("active/deactivate label cells") );
    connect( actLabel, SIGNAL( triggered() ), this, SLOT( switchLabel() ) );
    // Flag:	Aspect
    actAspect		= new QAction( tr("Aspect"), this );
    actAspect->setShortcut( tr("Ctrl+Z") );
    actAspect->setStatusTip( tr("active/deactivate aspect ratios in annotation") );
    connect( actAspect, SIGNAL( triggered() ), this, SLOT( switchAspect() ) );
    // Flag:	Conflict
    actConflict		= new QAction( tr("Conflict"), this );
    actConflict->setShortcut( tr("Ctrl+O") );
    actConflict->setStatusTip( tr("active/deactivate conflict check in annotation") );
    connect( actConflict, SIGNAL( triggered() ), this, SLOT( switchConflict() ) );
    // Flag:	Adaptive
    // actAdaptive		= new QAction( tr("Adaptive"), this );
    // actAdaptive->setShortcut( tr("Ctrl+I") );
    // actAdaptive->setStatusTip( tr("active/deactivate adaptive adjustment of blending parameter") );
    // connect( actAdaptive, SIGNAL( triggered() ), this, SLOT( switchAdaptive() ) );
    // Flag:	smoothing
    // actsmoothing	= new QAction( tr("smoothing"), this );
    // actsmoothing->setShortcut( tr("Ctrl+Shift+I") );
    // actsmoothing->setStatusTip( tr("active/deactivate adaptive adjustment of blending parameter") );
    // connect( actsmoothing, SIGNAL( triggered() ), this, SLOT( switchsmoothing() ) );
    // Flag:	2stepsmoothing
    // act2stepsmoothing	= new QAction( tr("2stepsmoothing"), this );
    // act2stepsmoothing->setShortcut( tr("Ctrl+Shift+I") );
    // act2stepsmoothing->setStatusTip( tr("active/deactivate adaptive adjustment of blending parameter") );
    // connect( act2stepsmoothing, SIGNAL( triggered() ), this, SLOT( switch2stepsmoothing() ) );
}

void Window::createMenus( void )
{
    // File
    fileMenu = menuBar()->addMenu( tr("&File") );
    fileMenu->addAction( selTestAct );
	    // Separator
    fileMenu->addSeparator();
    fileMenu->addAction( actLoad );
    fileMenu->addAction( actSave );
    fileMenu->addAction( actCapture );
    // Optimize
    optimizeMenu = menuBar()->addMenu( tr("Optim&ize") );
    optimizeMenu->addAction( actReset );
    optimizeMenu->addAction( actStart );
    optimizeMenu->addAction( actStop );
    optimizeMenu->addAction( actRandom );
    // Analyze
    analyzeMenu = menuBar()->addMenu( tr("Analyze") );
    analyzeMenu->addAction( actEdgeCentrality );
    analyzeMenu->addAction( actMST );
    // Annotate
    annotateMenu = menuBar()->addMenu( tr("Annotate") );
    annotateMenu->addAction( actSet );
    annotateMenu->addAction( actClear );
    annotateMenu->addAction( actOn );
    annotateMenu->addAction( actOff );
    // Layout
    layoutMenu = menuBar()->addMenu( tr("Lay&out") );
    layoutMenu->addAction( actForce );
    layoutMenu->addAction( actCentroid );
    layoutMenu->addAction( actSimple );
    layoutMenu->addAction( actAdaptive );
    layoutMenu->addAction( actOnestep );
    layoutMenu->addAction( actTwostep );
    // Metric
    metricMenu = menuBar()->addMenu( tr("&Metric") );
    metricMenu->addAction( actEuclidean );
    metricMenu->addAction( actManhattan );
    metricMenu->addAction( actChebyshev );
    // Flag
    flagMenu = menuBar()->addMenu( tr("&Flag") );
    flagMenu->addAction( actCell );
    flagMenu->addAction( actLabel );
    flagMenu->addAction( actAspect );
    flagMenu->addAction( actConflict );
    // flagMenu->addAction( actAdaptive );
    // flagMenu->addAction( actsmoothing );
    // flagMenu->addAction( act2stepsmoothing );
}

//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	コンストラクタ
//	Constructors
//------------------------------------------------------------------------------

//
//  Window::Window --	default constructor
//			コンストラクタ(デフォルト)
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Window::Window( QGLWidget *parent )
    : QMainWindow( parent )
{
    _widget = new Widget( this );
    setCentralWidget( _widget );

    initParams();
    createActions();
    createMenus();
}


//
//  Window::Window --	copy constructor
//			コピー・コンストラクタ
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
#ifdef SKIP
Window::Window( const Window & obj )
      : %%%( ( const %%% & ) obj )
{

}
#endif	// SKIP


//------------------------------------------------------------------------------
//	デストラクタ
//	Destructor
//------------------------------------------------------------------------------

//
//  Window::~Window --	destructor
//			デストラクタ
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Window::~Window()
{

}


//------------------------------------------------------------------------------
//	メンバ参照
//	Referrring to members
//------------------------------------------------------------------------------




// end of program file
// Do not add any stuff under this line.



