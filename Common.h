//------------------------------------------------------------------------------
//	Switches
//------------------------------------------------------------------------------
// #define VALIDATE_WEIGHT

/***************************************************
 実は，Widget.cpp に描画するごとにウィンドウの画像を保存でき
 るように，ACTIVATE_SNAPS という ifdef スイッチが入っています．
 Common.h の中の
 #define ACTIVATE_SNAPS
 を有効にしてもらうと，snap ディレクトリの下に，カウントを増
 やしながら画像を保存していくようになっています．投稿時の画像
 で，対話処理の途中の画像は実はこうやって作成していました．
 ***************************************************/

// #define ACTIVATE_SNAPS

#define LABELS_WHEN_COMPLETE


//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------
#ifndef TRUE
#define TRUE	(1)
#endif	// TRUE

#ifndef FALSE
#define FALSE	(0)
#endif	// FALSE

#ifndef MAX2
#define MAX2( x, y )		(( (x)<(y) )? (y) : (x) )
#endif	// MAX2

#ifndef MIN2
#define MIN2( x, y )		(( (x)>(y) )? (y) : (x) )
#endif	// MIN2

#ifndef SQUARE
#define SQUARE( x )		((x)*(x))
#endif	// SQUARE

#ifndef NO_INDEX
#define NO_INDEX        (90001)
#endif	// NO_INDEX

// EPS should be avoided due to conflicts with CGAL
#ifndef EPSILON
#define EPSILON		(1.0e-8)
#endif	// EPSILON

#ifndef INTERMEDIATE_EPSILON
//#define INTERMEDIATE_EPSILON	(5.0e-6)
//#define INTERMEDIATE_EPSILON	(2.0e-6)
#define INTERMEDIATE_EPSILON	(TRANSFORMATION_STEP*(1.0e-4))
#endif	// INTERMEDIATE_EPSILON

#ifndef FINAL_EPSILON
//#define FINAL_EPSILON		(5.0e-7)
//#define FINAL_EPSILON		(2.0e-7)
#define FINAL_EPSILON		(TRANSFORMATION_STEP*(1.0e-5))
#endif	// FINAL_EPSILON

#ifndef TRANSFORMATION_STEP
//#define TRANSFORMATION_STEP		(0.25) // too strong
//#define TRANSFORMATION_STEP		(0.20)
//#define TRANSFORMATION_STEP		(0.15)
//#define TRANSFORMATION_STEP		(0.10) // <==
//#define TRANSFORMATION_STEP		(0.05) // large graphs
#define TRANSFORMATION_STEP		(0.04)
// #define TRANSFORMATION_STEP		(0.02) // originally employed
// #define TRANSFORMATION_STEP		(0.01)
#endif	// TRANSFORMATION_STEP

#ifndef DISPLACEMENT_LIMIT
#define DISPLACEMENT_LIMIT		(1.00)
#endif // DISPLACEMENT_LIMIT

#ifndef CELL_RATIO
//#define CELL_RATIO			(4.00)
#define CELL_RATIO			(10.00)
#endif	// CELL_RATIO


#ifndef BUFFER_SIZE
#define	BUFFER_SIZE		(256)
#endif	// BUFFER_SIZE

#ifndef SELECTION_SIZE
#define	SELECTION_SIZE		(4096)
#endif	// SELECTION_SIZE

#ifndef MAX_INT
#define MAX_INT		(1.0e+8)
#endif	// MAX_INT

#ifndef NO_DIST
#define NO_DIST		(-1)
#endif	// NO_DIST

#ifndef INFINITY
#define INFINITY	(1.0e+8)
#endif	// INFINITY

#ifndef HERE
#define HERE		" " << __FILE__ << ":" << __LINE__ << " "
#endif	// HERE

#ifndef INFINITY
#define	INFINITY		(1.0e+8)
#endif	// INFINITY
#ifndef MAX_VALUE
#define MAX_VALUE		(1.0e+8)
#endif	// MAX_VALUE


#define SCREEN_SIDE		(2.0)

#define LABEL_UNIT_RATIO	(0.0036)
// #define LABEL_MARGIN		(0.0002)	// for abstract data
#define LABEL_MARGIN		(0.0001)


#ifndef CONTENT_RATIO
//#define CONTENT_RATIO		(0.85)
#define CONTENT_RATIO		(0.90)
#endif	// CONTENT_RATIO

#ifndef FONT_SIZE
#define FONT_SIZE		(12)
//#define FONT_SIZE		(16)
//#define FONT_SIZE		(24)
#endif
