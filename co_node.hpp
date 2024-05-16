//---------------------------------------------------
// node.hpp
//---------------------------------------------------

#ifndef NODE_HPP
#define NODE_HPP

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cMT19937ar.hpp"

#include "so_geometry.hpp"
#include "so_matter.hpp"
#include "so_xrtube.hpp"

//---------------------------------------------------
// definicao da classe do nodechare Node
//---------------------------------------------------
class Node : public CBase_Node {
private:
   MT19937ar dice;

public:
   Models model;
   Media  media;
   XRTube tube;

   //-----------------------------------------------
   // construtor
   //-----------------------------------------------
   Node(int n, unsigned long *init_key, Media mt, Models md); // implementado em node.cpp
   Node(int n, unsigned long *init_key, XRTube tb, Models md); // implementado em node.cpp
   Node(Models md);                                           // implementado em node.cpp

   //-----------------------------------------------
   // protótipos dos métodos do nodechare Node
   //-----------------------------------------------
//   unsigned long Draw(void);
   void Reduction(CkReductionMsg* m);
   void Model(Models md) { model = md; }
};
//---------------------------------------------------

#endif
