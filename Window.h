#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;

#include <QtGui/QMainWindow>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>

#include "Widget.h"

class Window : public QMainWindow
{
    Q_OBJECT

private slots:
    void selectTest( void );

    // file
    void loadData( void );
    void saveData( void );
    void capture( void );

    // optimize
    void oprReset( void );
    void oprStart( void );
    void oprStop( void );
    void oprRandom( void );

    // analyze
    void calcEdgeCentrality( void );
    void calcMST( void );
    void calcDivideComunity( void );

    // annotate
    void allSet( void );
    void allClear( void );
    void onLabel( void );
    void offLabel( void );

    // layout
    void setLayout( int sw );
    void setForce( void )		{ setLayout( 0 ); } // force-directed
    void setCentroid( void )		{ setLayout( 1 ); } // centroidal
    void setSimple( void )		{ setLayout( 2 ); } // hybrid simple
    void setAdaptive( void )		{ setLayout( 3 ); } // hybrid adaptive
    void setOnestep( void )		{ setLayout( 4 ); } // hybrid onestep
    void setTwostep( void )		{ setLayout( 5 ); } // hybrid twostep

    // metric
    void setMetric( int sw );
    void setEuclidean( void )		{ setMetric( 0 ); }
    void setManhattan( void )		{ setMetric( 1 ); }
    void setChebyshev( void )		{ setMetric( 2 ); }

    // flag
    void switchFlag( bool & flag );
    // void switchAdaptive( bool & flag );
    void switchsmoothing( bool & flag );
    void switch2stepsmoothing( bool & flag );
    void switchCell( void )		{ switchFlag( _widget->flagCell() ); }
    void switchLabel( void )		{ switchFlag( _widget->flagLabel() ); }
    void switchAspect( void )		{ switchFlag( _widget->flagAspect() ); }
    void switchConflict( void )		{ switchFlag( _widget->flagConflict() ); }
    // void switchAdaptive( void )		{ switchAdaptive( _widget->flagAdaptive() ); }
    void switchsmoothing( void )    { switchsmoothing( _widget->flagsmoothing() ); }
    void switch2stepsmoothing( void )    { switch2stepsmoothing( _widget->flag2stepsmoothing() ); }

private:

    // static QGLContext * openglContext( void );
    Widget *_widget;


protected slots:

    void keyPressEvent		( QKeyEvent *event );
    void mouseMoveEvent		( QMouseEvent *event );
    void mousePressEvent	( QMouseEvent *event );
    void mouseReleaseEvent	( QMouseEvent *event );
    void wheelEvent		( QWheelEvent *event );
    // void contentMenuEvent	( QContentMenuEvent *event );

protected:

    // mouse button statuses
    bool	_left_button, _middle_button, _right_button;

    // file
    QMenu	*fileMenu;
    QAction	*selTestAct;
    QAction	*actLoad;
    QAction	*actSave;
    QAction	*actCapture;

    // optimize
    QMenu	*optimizeMenu;
    QAction	*actReset;
    QAction	*actStart;
    QAction	*actStop;
    QAction	*actRandom;

    // analyze
    QMenu	*analyzeMenu;
    QAction	*actEdgeCentrality;
    QAction	*actMST;
    QAction *actDivideComunity;

    // annotate
    QMenu	*annotateMenu;
    QAction	*actSet;
    QAction	*actClear;
    QAction	*actOn;
    QAction	*actOff;

    // layout
    QMenu	*layoutMenu;
    QAction	*actForce;
    QAction	*actCentroid;
    QAction	*actSimple;
    QAction	*actAdaptive;
    QAction	*actOnestep;
    QAction	*actTwostep;

    // metric
    QMenu	*metricMenu;
    QAction	*actEuclidean;
    QAction	*actManhattan;
    QAction	*actChebyshev;

    // flag
    QMenu	*flagMenu;
    QAction	*actCell;
    QAction	*actLabel;
    QAction	*actAspect;
    QAction	*actConflict;
    
    //Evaluate
    QMenu   *evaluationMenu;
    QAction *actOffevaluation;
    QAction *actOnevaluation;

    void initParams( void );

    void createActions( void );
    void createMenus( void );

public:
    explicit Window( QGLWidget *parent = 0 );
    ~Window();

    void setNet( Net * __net ) {
	_widget->setNet( __net );
    }

    // void initializeGL( void );
    // void paintGL( void );
};


#endif // WINDOW_H
