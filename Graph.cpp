#include "Common.h"
#include "Graph.h"
#include "Net.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bc_clustering.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/range/algorithm.hpp>
#include <vector>


void edgeCentrality( Graph & ref )
{
    typedef adjacency_list< 
        boost::setS, 
	boost::vecS,
	boost::undirectedS,
	property< edge_index_t, int >
	> GraphBC;
    typedef GraphBC::vertex_descriptor VertexBC;
    typedef GraphBC::edge_descriptor EdgeBC;
    GraphBC gBC;

    VertexIndexMap	vertexIndex	= get( vertex_index, ref );
    EdgeWeightMap       edgeWeight	= get( edge_weight, ref );
    //update 11/24 
    VertexCentMap vertexCent = get(vertex_mycent, ref);
    VertexFlagMap vertexFlag = get(vertex_myflag, ref);
    EdgeCentMap   edgeCent   = get(edge_mycent, ref);

    vector< VertexDescriptor >		BCtoGVertexVector;
    vector< EdgeDescriptor >		BCtoGEdgeVector;
    map< unsigned int, unsigned int >	GVtoBCVertexMap;

    unsigned int	numV = 0;
    unsigned int	numE = 0;
    //update
    double cent;


    // copy the graph
    numV = 0;
    BGL_FORALL_VERTICES( vd, ref, Graph )
    {
	//#ifdef DEBUG
	//cerr << vertexIndex[ vd ] << " vs. " << numV << endl;
	//#endif	// DEBUG
	add_vertex( gBC );
	BCtoGVertexVector.push_back( vd );
	GVtoBCVertexMap.insert( pair< unsigned int, unsigned int >( vertexIndex[ vd ], numV ) );
	numV++;
    }

    numE = 0;
    BGL_FORALL_EDGES( ed, ref, Graph )
    {
	VertexDescriptor vS = source( ed, ref );
	VertexDescriptor vT = target( ed, ref );
	unsigned int idSBC = GVtoBCVertexMap[ vertexIndex[ vS ] ];
	unsigned int idTBC = GVtoBCVertexMap[ vertexIndex[ vT ] ];
	pair< EdgeBC, bool > edpair = add_edge( vertex( idSBC, gBC ), vertex( idTBC, gBC ), gBC );
	EdgeBC edBC = edpair.first;
	// my_e_index.insert( pair< EdgeDescriptorBC, unsigned int >( edBC, connectedENo ) );
	BCtoGEdgeVector.push_back( ed );
	numE++;
    }

    // std::map used for convenient initialization
    typedef std::map<EdgeBC, int> StdEdgeIndexMap;
    StdEdgeIndexMap my_e_index;
    // associative property map needed for iterator property map-wrapper
    typedef boost::associative_property_map< StdEdgeIndexMap > EdgeIndexMap;
    EdgeIndexMap e_index(my_e_index);
 
    // We use setS as edge-container -> no automatic indices
    // -> Create and set it explicitly
    int i = 0;
    BGL_FORALL_EDGES( edgeBC, gBC, GraphBC ) {
    	my_e_index.insert( std::pair< EdgeBC, int >( edgeBC, i ) );
    	++i;
    }
 
    // Define EdgeCentralityMap
    std::vector< double > e_centrality_vec( boost::num_edges( gBC ), 0.0 );
    // Create the external property map
    boost::iterator_property_map< std::vector< double >::iterator, EdgeIndexMap >
	e_centrality_map( e_centrality_vec.begin(), e_index );
 
    // Define CentralityMap
    typedef boost::property_map< GraphBC, boost::vertex_index_t >::type VertexIndexMapBC;
    VertexIndexMapBC v_index = get( boost::vertex_index, gBC );
    std::vector< double > v_centrality_vec( boost::num_vertices(gBC), 0.0 );
     // Create the external property map
    boost::iterator_property_map< std::vector< double >::iterator, VertexIndexMapBC >
	v_centrality_map( v_centrality_vec.begin(), v_index );
 
    std::cout << "Before" << std::endl;
 
    BGL_FORALL_EDGES(edgeBC, gBC, GraphBC) {
	   //std::cout << edgeBC << ": " << e_centrality_map[edgeBC] << std::endl;
    }

    // Calculate the vertex and edge centralites
    // Can be used to get an initial impression about the edge centrality values for the graph
    brandes_betweenness_centrality( gBC, v_centrality_map, e_centrality_map );
    
    // Print the results
    std::cout << "\nAfter" << std::endl;
 
    BGL_FORALL_EDGES(edgeBC, gBC, GraphBC ) {
	   //std::cout << edgeBC << ": " <<e_centrality_map[edgeBC] << std::endl;
    }

    //update 11/21
    double maxVertexCentrality = 0.0;
    typedef property_map< GraphBC, vertex_index_t >::type     VertexIndexMapBC;
    VertexIndexMapBC  vertexIndexBC = get( vertex_index, gBC );
    BGL_FORALL_VERTICES( vdBC, gBC, GraphBC ){
        if( v_centrality_map[vdBC] > maxVertexCentrality ){
            maxVertexCentrality = v_centrality_map[vdBC];
        }
    }

    BGL_FORALL_VERTICES( vdBC, gBC, GraphBC )
    {
        //int g = boost::num_vertices(gBC);
        unsigned int idBC = vertexIndexBC[vdBC];
        VertexDescriptor vd = BCtoGVertexVector[idBC];
        vertexCent[vd] = v_centrality_map[vdBC] / maxVertexCentrality;
        vertexFlag[vd] = true;
        //vertexCent[vd] = (2 * v_centrality_map[vdBC]) / ( (g - 1)*(g - 2) );
        //setBetCentrality(BetCentrality);
        //cerr << vdBC << ": " << setw(9) << v_centrality_map[vdBC] << "  BetCen :" << vertexCent[vd] <<  endl;
     }
    
    double maxEdgeCentrality = 0.0;
    BGL_FORALL_EDGES( edBC, gBC, GraphBC ) {
	if ( e_centrality_map[ edBC ] > maxEdgeCentrality ) {
	    maxEdgeCentrality = e_centrality_map[ edBC ];
	}
    }
    cerr << " maxEdgeCentrality = " << maxEdgeCentrality << endl;

    numE = 0;
    BGL_FORALL_EDGES( edBC, gBC, GraphBC )
    {
	// #ifdef DEBUG
    //    cerr << edBC << ": " << e_centrality_map[ edBC ] << 
	// "=> " << e_centrality_map[ edBC ] / maxEdgeCentrality << endl;
	// #endif	// DEBUG
	EdgeDescriptor ed = BCtoGEdgeVector[ numE ];
    	edgeWeight[ ed ] = e_centrality_map[ edBC ] / maxEdgeCentrality;
        edgeCent[ ed ] = e_centrality_map[ edBC ] / maxEdgeCentrality;

        //cout << " edgeCent " << edgeCent[ ed ] << endl; 
	// eCentralityMap.insert( pair< EdgeDescriptor, double >( ed, 
	// e_centrality_map[ edge ]/maxEdgeCentrality ) );
    	numE++;
    }
    //setCommunityLabel(ref);

    return;
}


void minimumSpanningTree( Graph & g )
{
    // VertexActiveMap     vertexActive =  get( vertex_myactive, g );
    EdgeFlagMap     flagMap =   get( edge_myflag, g );
    EdgeFflagMap    fflagMap = get( edge_myfflag, g);
    VertexForceMap  vertexForce = get( vertex_myforce, g );
    
    // EdgeTopologyMap topologyMap = get( edge_mytopology, g );
    // pair< EdgeIterator, EdgeIterator > ep;

    BGL_FORALL_EDGES( ed, g, Graph ) {
	   flagMap[ ed ] = false;
       if(fflagMap[ ed ] == false) fflagMap[ ed ] = true;
       else fflagMap[ ed ] = false;
    }
    

    vector< EdgeDescriptor >    span;

    kruskal_minimum_spanning_tree( g, back_inserter( span ) );

    for ( unsigned int k = 0; k < span.size(); ++k ) {
        VertexDescriptor vdS = source( span[ k ], g );
        VertexDescriptor vdT = target( span[ k ], g );
	    flagMap[ span[ k ] ] = true;
        //if(flagMap[span[k]] == false){
        //    BGL_FORALL_VERTICES( span, g, Graph ) {
        //    Coord2 shift(0.0,0.0);
        //    vertexForce[ span ] = shift;
        //    }
        //}
    }

}

void divideComunity( Graph & g){
    Graph gN = g;
    vector<VertexDescriptor> v;
    setCommunityLabel( v, g);
    v = searchConnect(g);
    removeEdge(gN);
    setCommunityLabel( v, gN);
}   

void removeEdge( Graph & g )
{
    VertexCentMap   vertexCent  = get( vertex_mycent, g);
    EdgeCentMap     edgeCent    = get( edge_mycent, g); 
    VertexIndexMap  vertexIn    = get( vertex_index, g);
    double maxVertexCentrality = 0.0;
    double maxEdgeCentrality = 0.0;
    bool tole = false;
    EdgeDescriptor   edMax;
    VertexDescriptor vdMax, vT, vS, vN;
    unsigned int id;
    BGL_FORALL_VERTICES( vd, g, Graph ){
      if( maxVertexCentrality < vertexCent[vd]){
            maxVertexCentrality = vertexCent[vd];  
            vdMax = vd;
        }
    }
    //searchConnect(vdMax,g);
    std::cout << " maxVertexCentrality :" << maxVertexCentrality << std::endl;
    BGL_FORALL_EDGES( ed, g, Graph ){
        if( maxEdgeCentrality < edgeCent[ed]){
            maxEdgeCentrality = edgeCent[ed];
        }
    }
    BGL_FORALL_EDGES( ed, g, Graph ){
        if(maxEdgeCentrality == edgeCent[ed]){
            edMax = ed;
            vT = target( edMax, g);
            vS = source( edMax, g);
            //std::cout << " maxEdgeCentrality :" << maxEdgeCentrality << std::endl;
            //std::cout << "vdMax : " << vertexIn[vdMax] << endl << "vTID : "<< vertexIn[vT] <<" vTCent : " << vertexCent[vT] << endl << " vSID : "<< vertexIn[vS] << " vSCent : " << vertexCent[vS] << endl;
            tole = checkTolerance(vS,vT);
            if(tole) boost::remove_edge(edMax, g);
            else if(vdMax == vT || vdMax == vS) boost::remove_edge(edMax, g); 
        }
    }
    // searchConnect(vdMax,g);
    //std::cout << " maxEdgeCentrality :" << maxEdgeCentrality << std::endl;
    //std::cout << "vdMax : " << vdMax << endl << "vTID : "<< vertexIn[vT] <<" vTCent : " << vertexCent[vT] << endl << " vSID : "<< vertexIn[vS] << " vSCent : " << vertexCent[vS] << endl;
   
    //divide max edge centrality
    //countCommunityLabel(g);
}

vector<VertexDescriptor>  searchConnect ( Graph & g )
{
    VertexDescriptor vS, vT,vdMax;
    VertexComFlagMap vc = get( vertex_comflag, g);
    VertexIndexMap vI = get(vertex_index, g);
    double maxv;
    VertexCentMap vce = get( vertex_mycent, g);
    vector<VertexDescriptor> v;
    BGL_FORALL_VERTICES( vd, g, Graph ){
      if( maxv < vce[vd]){
            maxv = vce[vd];  
            vdMax = vd;
        }
    }
    BGL_FORALL_EDGES( ed, g, Graph ){
        vT = target( ed, g);
        vS = source( ed, g);
        if( vdMax == vT ) v.push_back( vS );
        else if(vdMax == vS) v.push_back( vT );
    }
    vector<VertexDescriptor>::iterator it = v.begin();
    while(it != v.end()){
        std::cout << " " << "[" << vI[*it] << "]";
        printCommunityLabel(*it); 
        ++it;
    }
    std::cout<<endl;
    return v;

} 

void setCommunityLabel( vector<VertexDescriptor> v, Graph & g)
{
    boost::graph_traits<Graph>::in_edge_iterator ei, edge_end;
    VertexIndexMap  vertexIn    = get( vertex_index, g );
    EdgeIndexMap    edgeIn      = get( edge_index, g );
    VertexComFlagMap vertexCom  = get( vertex_comflag, g );
    int find;
    VertexDescriptor vF;

    std::vector< int > component( num_vertices( g ));
    connected_components( g, 
        make_iterator_property_map( component.begin(), get(vertex_index, g ), component[0] ) );
    BGL_FORALL_VERTICES(vd, g, Graph){
        int index = vertexIn[vd];
        vertexCom[ vd ].push_back(component[ index ]);
        std::cerr << " => " << std::setw( 3 ) << index;

        if(boost::count(component, component[ index ]) == 1) {
            int isolate = component[ index ];
            // std::cout << " isolate " << isolate;
            vF = vd;
            vertexCom[vd].pop_back();
            vector<VertexDescriptor>::iterator it = v.begin();
            while(it != v.end()){
                int i = vertexIn[ *it ];
                // if( component[i] > isolate ) {
                //    // std::cout << " com[i] " << component[i];
                //     vertexCom[vd].push_back(component[i] - 1);
                // }
                vertexCom[vd].push_back(component[i]);
                ++it;
            }
        }   
        std::sort(vertexCom[vd].begin(), vertexCom[vd].end());
        vertexCom[vd].erase(std::unique(vertexCom[vd].begin(), vertexCom[vd].end()), vertexCom[vd].end() );

        printCommunityLabel(vd);
    }       
    //std::cout << " found " << find << " id " << vertexIn[ vF ] << endl;
}

// void countCommunityLabel( Graph & g)
// {
//     VertexIndexMap  vertexIn    = get( vertex_index, g );
//     std::vector< int > component( num_vertices( g ));
//     connected_components( g, 
//         make_iterator_property_map( component.begin(), get(vertex_index, g ), component[0] ) );
//     BGL_FORALL_VERTICES(vd, g, Graph){
//         int index = vertexIn[vd];
//         if(nindex < component[ index ]){
//             nindex = component[ index ];
//         }   
//     }
// }

void printCommunityLabel( VertexDescriptor vd)
{
    VertexComFlagMap vertexCom;
    vector<int>::iterator it = vertexCom[ vd ].begin();
    while(it != vertexCom[ vd ].end()){
        std::cout << " " << "[" << *it << "]";
        ++it;
    }
    std::cout<<endl;
}

bool checkTolerance( VertexDescriptor vs, VertexDescriptor vt)
{
    VertexCentMap vertexCent;
    double tole1 = vertexCent[vs]*0.9;
    double tole2 = vertexCent[vs]*1.1;
    if(tole1 <= vertexCent[vt] && tole2 >= vertexCent[vs])  return true;
    else return false;
}

// void printCommunityLabel2( Graph & g )
// {
//     VertexComFlagMap vertexCom = get( vertex_comflag, g );
//     BGL_FORALL_VERTICES(vd, g, Graph){
//         std::vector<int>::iterator it = vertexCom[ vd ].begin();
//         while(it != vertexCom[ vd ].end()){
//         std::cout << " : " << "[" << *it << "]";
//         ++it;
//         }
//     std::cout<<endl;
//     }
// }


#ifdef SKIP
void edgeCentrality( Graph & g )
{
    typedef adjacency_list< 
        boost::setS, 
	boost::vecS,
	boost::undirectedS,
	property< edge_index_t, int >
	> GraphBC;
    typedef GraphBC::vertex_descriptor VertexDescriptorBC;
    typedef GraphBC::edge_descriptor EdgeDescriptorBC;
    typedef property_map< GraphBC, vertex_index_t >::type VertexIndexMapBC;
    typedef map< unsigned int, unsigned int > IndexToLabel;
    // eCentralityMap.clear();

    // double max_centrality = 4;
    GraphBC gBC;

    VertexIndexMap	vertexIndex	= get( vertex_index, g );
    EdgeWeightMap       edgeWeight	= get( edge_weight, g );
    unsigned int	connectedVNo = 0;
    unsigned int	connectedENo = 0;

    vector< VertexDescriptor >	BCtoGVertexVector;
    vector< EdgeDescriptor >	BCtoGEdgeVector;
    map< unsigned int, unsigned int > GVtoBCVertexMap;

    // std::map used for convenient initialization
    typedef map< EdgeDescriptorBC, unsigned int > StdEdgeIndexMap;
    StdEdgeIndexMap my_e_index;
    // associative property map needed for iterator property map-wrapper
    typedef associative_property_map< StdEdgeIndexMap > EdgeIndexMapBC;
    EdgeIndexMapBC e_index( my_e_index );

    // copy the graph
    BGL_FORALL_VERTICES( vertex, g, Graph )
    {
	//#ifdef DEBUG
	cerr << vertexIndex[ vertex ] << endl;
	//#endif	// DEBUG
	add_vertex( gBC );
	BCtoGVertexVector.push_back( vertex );
	GVtoBCVertexMap.insert( pair< unsigned int, unsigned int >( vertexIndex[ vertex ], connectedVNo ) );
	connectedVNo++;
    }

    cerr << HERE << "[O] num of edges = " << boost::num_edges( g ) << endl;

    BGL_FORALL_EDGES( edge, g, Graph )
    {
	VertexDescriptor vS = source( edge, g );
	VertexDescriptor vT = target( edge, g );
	unsigned int idS = GVtoBCVertexMap[ vertexIndex[ vS ] ];
	unsigned int idT = GVtoBCVertexMap[ vertexIndex[ vT ] ];

	pair< EdgeDescriptorBC, unsigned int > edpair = add_edge( vertex( idS, gBC ), 
								  vertex( idT, gBC ), gBC );
	EdgeDescriptorBC edBC = edpair.first;
	my_e_index.insert( pair< EdgeDescriptorBC, unsigned int >( edBC, connectedENo ) );
	BCtoGEdgeVector.push_back( edge );
	connectedENo++;
    }
    
    cerr << HERE << " connectedENo = " << connectedENo << endl;
    cerr << HERE << "[N] num of edges = " << boost::num_edges( gBC ) << endl;
    
    // Define EdgeCentralityMap
    vector< double > e_centrality_vec( boost::num_edges( gBC ), 0.0 );
    // Create the external property map
    iterator_property_map< vector< double >::iterator, EdgeIndexMapBC > e_centrality_map( e_centrality_vec.begin(), e_index );
    
    // Define VertexCentralityMap
    typedef boost::property_map< GraphBC, vertex_index_t >::type VertexIndexMapBC;
    VertexIndexMapBC v_index = get( vertex_index, gBC );
    vector< double > v_centrality_vec( boost::num_vertices( gBC ), 0.0 );
    // Create the external property map
    iterator_property_map< vector< double >::iterator, VertexIndexMapBC > v_centrality_map( v_centrality_vec.begin(), v_index );

    // Calculate the vertex and edge centralites
    // Can be used to get an initial impression about the edge centrality values for the graph

    // This causes segmentaion fault !!
    brandes_betweenness_centrality( gBC, v_centrality_map, e_centrality_map );


    // Define the done-object:
    // 'false' means here that no normalization is performed, so edge centrality-values can become big
    // If set to 'true', values will range between 0 and 1 but will be more difficult to use for this
    // illustrative example.
    //boost::bc_clustering_threshold< double > terminate( max_centrality, gBC, false );

    //
    // Do the clustering
    // Does also calculate the brandes_betweenness_centrality and stores it in e_centrality_map
    //
    //betweenness_centrality_clustering( gBC, terminate, e_centrality_map );


    // Update the weight
    //#ifdef DEBUG
    cout << "\nAfter" << endl;
    
    print_graph( gBC );
    BGL_FORALL_VERTICES( vertex, gBC, GraphBC )
    {
        cerr << vertex << ": " << v_centrality_map[vertex] << endl;
    }
    //#endif	// DEBUG

    double maxEdgeCentrality = 0.0;
    BGL_FORALL_EDGES( edge, gBC, GraphBC ) {
	if ( e_centrality_map[ edge ] > maxEdgeCentrality ) {
	    maxEdgeCentrality = e_centrality_map[ edge ];
	}
    }

    connectedENo = 0;
    BGL_FORALL_EDGES( edge, gBC, GraphBC )
    {
	// #ifdef DEBUG
        cerr << edge << ": " << e_centrality_map[ edge ] << 
	    "=> " << e_centrality_map[ edge ] / maxEdgeCentrality << endl;
	// #endif	// DEBUG
	EdgeDescriptor ed = BCtoGEdgeVector[ connectedENo ];
    	edgeWeight[ ed ] = e_centrality_map[ edge ] / maxEdgeCentrality;
	// eCentralityMap.insert( pair< EdgeDescriptor, double >( ed, 
	// e_centrality_map[ edge ]/maxEdgeCentrality ) );
    	connectedENo++;
    }

#ifdef SKIP
#endif	// SKIP
    v_centrality_vec.clear();
    e_centrality_vec.clear();

    cerr << HERE << " Finished edge centrality computation" << endl;
}
#endif	// SKIP
