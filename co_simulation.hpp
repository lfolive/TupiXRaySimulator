//---------------------------------------------------
// simulation.hpp
//---------------------------------------------------

#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "so_matter.hpp"
#include "so_xrtube.hpp"

//---------------------------------------------------
// definicao da classe do grupo Simulation
//---------------------------------------------------
class Simulation : public CBase_Simulation {
private:
//   MT19937ar dice;

   Node      *node;
//   Source    *source;
   GDetector *detector;

   Media      media;
   XRTube     xrtube;

   char   dir[127];
   double nev;
   float  factor;

public:
   //-----------------------------------------------
   // construtor
   //-----------------------------------------------
   Simulation(double val);
   Simulation(int n, unsigned long *init_key, double val, Media mt);
   Simulation(int n, unsigned long *init_key, double val, int nd, float *fct, XRTube xrt, Media mt);

   //-----------------------------------------------
   // protótipos dos métodos do grupo Simulation
   //-----------------------------------------------
   void Factor(double val, int nd, float *fct);
   void Run(int lap);
};
//---------------------------------------------------

#endif
