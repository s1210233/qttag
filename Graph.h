#ifndef _Graph_H
#define _Graph_H

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>

using namespace boost;

#include "Coord2.h"

//------------------------------------------------------------------------------
//  Boost customization
//------------------------------------------------------------------------------
// Vertex attributes
// Community Flag
enum vertex_mycomflag_t     { vertex_comflag };
// Finish Flag
enum vertex_myflag_t        { vertex_myflag };
// Betweeness Centrality
enum vertex_mycent_t        { vertex_mycent };        
// Label
enum vertex_mylabel_t		{ vertex_mylabel };
// Switch
enum vertex_myswitch_t		{ vertex_myswitch };
// Positions
enum vertex_mycoord_t		{ vertex_mycoord };
// Vertex name
enum vertex_myname_t		{ vertex_myname };
// Weight values
enum vertex_myweight_t		{ vertex_myweight };
// Cell width
enum vertex_mywidth_t		{ vertex_mywidth };
// Cell height
enum vertex_myheight_t		{ vertex_myheight };
// Cell aspect
enum vertex_myaspect_t		{ vertex_myaspect };
// Total displacements
enum vertex_myshift_t		{ vertex_myshift };
// Displacements by the force-directed model
enum vertex_myforce_t		{ vertex_myforce };
// Displacements by the centroidal Voronoi tessellation
enum vertex_myplace_t		{ vertex_myplace };
// Num of pixels
enum vertex_mynum_t		{ vertex_mynum };
// Hybrid force ratio
enum vertex_myratio_t		{ vertex_myratio }; //add20151201
// Overlap ratio
enum vertex_myoverlap_t		{ vertex_myoverlap };
// ID
enum vertex_myid_t		{ vertex_myid };

// Edge Centrality
enum edge_mycent_t      { edge_mycent };
// Finish Flag
enum edge_myfflag_t     { edge_myfflag };
// Label
enum edge_mylabel_t		{ edge_mylabel };
// Flag
enum edge_myflag_t		{ edge_myflag };
// ID
enum edge_myid_t		{ edge_myid };

namespace boost {
    // vertex properties
    BOOST_INSTALL_PROPERTY( vertex, mycomflag );
    BOOST_INSTALL_PROPERTY( vertex, myflag );
    BOOST_INSTALL_PROPERTY( vertex, mycent );
    BOOST_INSTALL_PROPERTY( vertex, mylabel );
    BOOST_INSTALL_PROPERTY( vertex, myswitch );
    BOOST_INSTALL_PROPERTY( vertex, mycoord );
    BOOST_INSTALL_PROPERTY( vertex, myname );
    BOOST_INSTALL_PROPERTY( vertex, myweight );
    BOOST_INSTALL_PROPERTY( vertex, mywidth );
    BOOST_INSTALL_PROPERTY( vertex, myheight );
    BOOST_INSTALL_PROPERTY( vertex, myaspect );
    BOOST_INSTALL_PROPERTY( vertex, myshift );
    BOOST_INSTALL_PROPERTY( vertex, myforce );
    BOOST_INSTALL_PROPERTY( vertex, myplace );
    BOOST_INSTALL_PROPERTY( vertex, mynum );
    BOOST_INSTALL_PROPERTY( vertex, myratio );
    BOOST_INSTALL_PROPERTY( vertex, myoverlap );
    BOOST_INSTALL_PROPERTY( vertex, myid );
    // edge properties
    BOOST_INSTALL_PROPERTY( edge, mycent );
    BOOST_INSTALL_PROPERTY( edge, myfflag );
    BOOST_INSTALL_PROPERTY( edge, mylabel );
    BOOST_INSTALL_PROPERTY( edge, myflag );
    BOOST_INSTALL_PROPERTY( edge, myid );
}


//------------------------------------------------------------------------------
//	Customizing vertex properties
//------------------------------------------------------------------------------
// Vertex Community Flag
typedef property< vertex_mycomflag_t, vector<int> >         MyVComFlag;
// Vertex Finish Flag
typedef property< vertex_myflag_t, bool, MyVComFlag >       MyVFlag;
// Vertex Betweeness Centrality
typedef property< vertex_mycent_t, double, MyVFlag >        MyVCent;  
// Vertex named ID 
typedef property< vertex_mylabel_t, unsigned int, MyVCent >	MyVLabel;
// Vertex annotation switch
typedef property< vertex_myswitch_t, bool, MyVLabel >		MyVSwitch;
// Vertex position
typedef property< vertex_mycoord_t, Coord2, MyVSwitch >		MyVCoord;
// Vertex name
typedef property< vertex_myname_t, string, MyVCoord >		MyVName;
// Vertex weight
typedef property< vertex_myweight_t, double, MyVName >		MyVWeight;
// Vertex width
typedef property< vertex_mywidth_t, int, MyVWeight >		MyVWidth;
// Vertex height
typedef property< vertex_myheight_t, int, MyVWidth >		MyVHeight;
// Vertex aspect
typedef property< vertex_myaspect_t, double, MyVHeight >	MyVAspect;
// Total vertex displacement
typedef property< vertex_myshift_t, Coord2, MyVAspect >		MyVShift;
// Vertex force ratio
typedef property< vertex_myratio_t, double, MyVShift >		MyVRatio;
// Vertex displacement by the force-directed model
typedef property< vertex_myforce_t, Coord2, MyVRatio >		MyVForce;
// Vertex displacement the centroidal voronoi tessellation
typedef property< vertex_myplace_t, Coord2, MyVForce >		MyVPlace;
// Number of pixels associated with the vertex
typedef property< vertex_mynum_t, unsigned int, MyVPlace >	MyVNum;
// Label overlap ratio
typedef property< vertex_myoverlap_t, double, MyVNum >		MyVOverlap;
// Vertex ID
typedef property< vertex_myid_t, unsigned int, MyVOverlap >	MyVID;
// Vertex index
typedef property< vertex_index_t, unsigned int, MyVID >		MyVertexProperty;

//------------------------------------------------------------------------------
//	Customizing edge properties
//------------------------------------------------------------------------------
// Edge Centrality
typedef property< edge_mycent_t, double >                   MyECent;
// Edge Finish Flag
typedef property< edge_myfflag_t, bool, MyECent >           MyEFflag;
// Edge Label
typedef property< edge_mylabel_t, unsigned int, MyEFflag>	MyELabel;
// Edge Flag
typedef property< edge_myflag_t, bool, MyELabel >		    MyEFlag;
// Edge ID
typedef property< edge_myid_t, unsigned int, MyEFlag >		MyEID;
// Edge weight
typedef property< edge_weight_t, double, MyEID >		    MyEWeight;
// Edge index
typedef property< edge_index_t, unsigned int, MyEWeight >	MyEdgeProperty;

//------------------------------------------------------------------------------
//	Composing graph structure
//------------------------------------------------------------------------------
typedef adjacency_list< vecS, listS, undirectedS,
			MyVertexProperty, MyEdgeProperty >	Graph;

typedef graph_traits< Graph >					GraphTraits;
typedef graph_traits< Graph >::vertex_descriptor		VertexDescriptor;
typedef graph_traits< Graph >::edge_descriptor			EdgeDescriptor;

typedef graph_traits< Graph >::vertex_iterator			VertexIterator;
typedef graph_traits< Graph >::edge_iterator			EdgeIterator;

// Vertex property map
typedef property_map< Graph, vertex_mycomflag_t >::type     VertexComFlagMap;
typedef property_map< Graph, vertex_myflag_t >::type        VertexFlagMap;
typedef property_map< Graph, vertex_mycent_t >::type        VertexCentMap;
typedef property_map< Graph, vertex_mylabel_t >::type		VertexLabelMap;
typedef property_map< Graph, vertex_myswitch_t >::type		VertexSwitchMap;
typedef property_map< Graph, vertex_mycoord_t >::type		VertexCoordMap;
typedef property_map< Graph, vertex_myname_t >::type		VertexNameMap;
typedef property_map< Graph, vertex_myweight_t >::type		VertexWeightMap;
typedef property_map< Graph, vertex_mywidth_t >::type		VertexWidthMap;
typedef property_map< Graph, vertex_myheight_t >::type		VertexHeightMap;
typedef property_map< Graph, vertex_myaspect_t >::type		VertexAspectMap;
typedef property_map< Graph, vertex_myshift_t >::type		VertexShiftMap;
typedef property_map< Graph, vertex_myforce_t >::type		VertexForceMap;
typedef property_map< Graph, vertex_myplace_t >::type		VertexPlaceMap;
typedef property_map< Graph, vertex_mynum_t >::type		       VertexNumMap;
typedef property_map< Graph, vertex_myratio_t >::type		VertexRatioMap;
typedef property_map< Graph, vertex_myoverlap_t >::type		VertexOverlapMap;
typedef property_map< Graph, vertex_myid_t >::type		VertexIDMap;
typedef property_map< Graph, vertex_index_t >::type		VertexIndexMap;

// Edge property map
typedef property_map< Graph, edge_mycent_t >::type      EdgeCentMap;
typedef property_map< Graph, edge_myfflag_t >::type     EdgeFflagMap;
typedef property_map< Graph, edge_mylabel_t >::type		EdgeLabelMap;
typedef property_map< Graph, edge_myflag_t >::type		EdgeFlagMap;
typedef property_map< Graph, edge_myid_t >::type		EdgeIDMap;
typedef property_map< Graph, edge_weight_t >::type		EdgeWeightMap;
typedef property_map< Graph, edge_index_t >::type		EdgeIndexMap;


//------------------------------------------------------------------------------
//	Functions
//------------------------------------------------------------------------------
// void edgeCentrality	( Graph & g, map< EdgeDescriptor, double > & eCentralityMap );
void edgeCentrality	( Graph & g );
void minimumSpanningTree( Graph & g );
void divideComunity ( Graph & g );
void setCommunityLabel ( Graph & g );
void countCommunityLabel ( Graph & g );
void printCommunityLabel ( VertexDescriptor );
// double betw;
// void setBetCentrality( double b ){double betw = b; };
    // //update 11/21
    // double getBetCentrality(void){ return bet;};


#endif  // _Graph_H

