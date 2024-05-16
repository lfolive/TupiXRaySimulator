//---------------------------------------------------
// main.hpp
//---------------------------------------------------

#ifndef MAIN_HPP
#define MAIN_HPP

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <deque>

//---------------------------------------------------
// definicao da classe do mainchare Main
//---------------------------------------------------
class Main : public CBase_Main {
private:
   char outfile[64], outdir[64];
   int    count, iter, lap, rec;
   float *rate, *irate;
   float *factor;
   double nev;
   double t1, t2;

   std::deque<std::string> itmove;

public:
   Models modelo;

   //-----------------------------------------------
   // construtor
   //-----------------------------------------------
   Main(CkArgMsg *m); // implementado em main.cpp

   //-----------------------------------------------
   // protótipos dos métodos do mainchare Main
   //-----------------------------------------------
   void EnlapsedTime(int pe, float tm);
   void Reduction(void);
   void Reduction2(void);
};
//---------------------------------------------------

#endif
