//---------------------------------------------------
// detector.hpp
//---------------------------------------------------

#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cMT19937ar.hpp"

#include "so_matter.hpp"

// apelido da função de redução que será usado pelo contribute
extern CkReduction::reducerType DataOp;
void   registerDataOp(void);

// apelido da função de redução que será usado pelo contribute
extern CkReduction::reducerType DataOp2;
void   registerDataOp2(void);

//---------------------------------------------------
//typedef union { unsigned word, bit:32; /* set of 32 bits*/ } DATAINFO;
//---------------------------------------------------

//---------------------------------------------------
class DATATYPE {
private:
   char         type;
   unsigned int word;

public:
   DATATYPE(void) { word = 0; type = 1; }
   DATATYPE(char t) {
      type = t;
      word = 0;
   }

   void     Data(unsigned  val, char t) { word = val; type = t; }
   void     Data(unsigned  val)         { word = val; }
   void     Type(int t)                 { type = t; }

   unsigned Data(void)           { return word; }
   char     Type(void)           { return type; }

   void Set(int val)   { word |= (1<<val); }
   void Reset(int val) { word &= ~(1<<val); }
   int  Bit(int val)   { return (int)((word>>val)&(unsigned)0x1); }

   // binary and
   DATATYPE operator & (DATATYPE vdata) {
      DATATYPE temp;
      temp.word = word & vdata.word;

      temp.type = vdata.type;

      return temp;
   }

   // binary and
   DATATYPE operator &= (DATATYPE vdata) {
      word &= vdata.word;

      type = vdata.type;

      return *this;
   }

   // binary or
   DATATYPE operator | (DATATYPE vdata) {
      DATATYPE temp;
      temp.word = word | vdata.word;

      temp.type = vdata.type;

      return temp;
   }

   // binary or
   DATATYPE operator |= (DATATYPE vdata) {
      word |= vdata.word;

      type = vdata.type;

      return *this;
   }

   // same as |
   DATATYPE operator + (DATATYPE vdata) {
      DATATYPE temp;
      if (!type)
         temp.word = word | vdata.word;
      else
         temp.word = word + vdata.word;

      temp.type = vdata.type;

      return temp;
   }

   // same as |=
   DATATYPE operator += (DATATYPE vdata) {
      if (!type)
         word |= vdata.word;
      else
         word += vdata.word;

      type = vdata.type;

      return *this;
   }

   // prefixado
   DATATYPE operator ++(void) {
      word++;
      return *this;
   }

   // posfixado
   DATATYPE operator ++(int) {
      DATATYPE temp = *this;
      ++word;
      return temp;
   }

};
//---------------------------------------------------

//---------------------------------------------------
// definicao da classe do grupo Detector
//---------------------------------------------------
class GDetector : public CBase_GDetector {
private:

   MT19937ar dice;

   DATATYPE *data;
   int       type;
   int       nprocs, nnds, npp;
   int       nx, ny;
   long int  ndata, n0p, nBp;
   double    emax;
   double    dx, dy, ag, ap, thk;
   char      outdir[127];
   char      outfile[64];
   int       lap;

   Media  media;
   Models model;

public:
   //-----------------------------------------------
   // construtor
   //-----------------------------------------------
   GDetector(int val1, double val2, int nd, char *dir, int nf, char *file, Models md);
   GDetector(int val1, int val2, int nd, char *dir, int nf, char *file);
   GDetector(int n, unsigned long *init_key, int val1, int val2, 
             double grains, double area, double thick, double lx, double ly, 
             int nd, char *dir, int nf, char *file, Media mt, Models md);

   //-----------------------------------------------
   // protótipos dos métodos do grupo Detector
   //-----------------------------------------------
   void Lap(int val)   { lap = val; }

   void Em(double val) { emax = val; }
   void Nx(int val)    { nx = val;   }
   void Ny(int val)    { ny = val;   }

   int Type(void) { return type; }

   double Em(void) { return emax; }
   int    Nx(void) { return nx;   }
   int    Ny(void) { return ny;   }

   void Model(Models md);

   void Zeroing(void);
   void Detection(int pe, double e, XYZ p);

   void GrainDiscriminator(double e, XYZ p);
   void EnergyDiscriminator(double e);
   void PositionDiscriminator(XYZ p);

   void Contribution(void);
   void Reduction(CkReductionMsg* m);
};
//---------------------------------------------------

#endif
