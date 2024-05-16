#include "tupi.decl.h" // gerado pela compilação de tupi.ci

//=========================================================
// main.cpp
//=========================================================

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "so_geometry.hpp"
#include "so_matter.hpp"

#include "co_main.hpp"
#include "co_node.hpp"
#include "co_detector.hpp"
#include "co_simulation.hpp"

extern CProxy_Main      Proxy_Main; // Servidor
extern CProxy_GDetector Proxy_GDetector;

//===================================================
// construtor
//===================================================
Node::Node(Models md) {

   CkPrintf("[Node.%d].Construtor\n", CkMyNode());

   model = md;
}

Node::Node(int n, unsigned long *init_key, Media mt, Models md) {

   CkPrintf("[Node.%d].Construtor\n", CkMyNode());

   model = md;
   media = mt;

   media.Dice(8,init_key+8*CkMyNode());
}

Node::Node(int n, unsigned long *init_key, XRTube tb, Models md) {

   CkPrintf("[Node.%d].Construtor\n", CkMyNode());

   model = md;
   tube = tb;

   tube.Dice(8,init_key+8*CkMyNode());

   if (!CkMyNode()) tube.Print();
}

//===================================================
// implementação do método de redução
//===================================================
void Node::Reduction(CkReductionMsg* m) {

   DATATYPE *data = (DATATYPE*)m->getData();
   int  size = m->getSize()/sizeof(DATATYPE);

   CkPrintf("\t[Node.%d].Reduction\n",CkMyNode());

   // retorna os dados finais
//   CkCallback cb(CkIndex_Main::Reduction(NULL), Proxy_Main);
   CkCallback cb(CkIndex_GDetector::Reduction(NULL), Proxy_GDetector[0]);
   contribute(size*sizeof(DATATYPE), data, CkReduction::nop, cb);
//   contribute(cb);
}
//===================================================

