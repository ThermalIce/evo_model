#ifndef RPI_EDGE
#define RPI_EDGE

#include "Group.h"
#include "../../Libraries/Params/Parameters.h"
#include "../../Libraries/Random/PowerLaw.h"

using namespace std;

class Edge:public Group {
 public:  
 Edge():Group(), wait_time_(0), edge_weight_(0){}
  ~Edge(){}
  
  bool generateWeight(unique_ptr < Parameters >& P ) {
    PowerLaw pl ( -1.75, getTotalEnergy(P->get < double > ( "grav",0.2 ) ), 3.0 );
    edge_weight_ = 0;
    
    while ( wait_time_ < 1.0 ){
      wait_time_ += pl.Sample();
      ++edge_weight_;
    }

    wait_time_ -= 1.0;

    if ( edge_weight_ > 0 ){
      //Increase edge count for all vertices in this edge
      vset::iterator it_v;
      for ( it_v = members_.begin(); it_v != members_.end(); it_v++ ){
	(*it_v)->incrementEdgeCount();
      }
    }

    return edge_weight_ > 0;
  }

  double getTotalEnergy( const double& gravity ){
    double res = 1000;
    double min_res = 0;
    bool considered = false;
    
    vset::iterator it_v;
    for ( it_v = members_.begin(); it_v != members_.end(); it_v++ ){
      if ( (*it_v)->getEnergy() < res ) {
	res = (*it_v)->getEnergy();
      }
    }
    
    for ( it_v = members_.begin(); it_v != members_.end(); it_v++ ){
      if ( ( (*it_v)->getEnergy() == min_res ) && (!considered) ){
	considered = true;
	continue;
      } else {
	res += ( gravity * ( ((*it_v)->getEnergy()) - (res) ) );
      }
    }
    
    return res;
  }

  double getWeight ( ) { return edge_weight_; }

 private:
  double wait_time_;
  double edge_weight_;
};

struct cmp_pedge{
  bool operator() ( const shared_ptr < Edge >& A, const shared_ptr < Edge >& B ){
    return ( *A < *B );
  }
};

typedef set < shared_ptr < Edge >, cmp_pedge > eset;

#endif
