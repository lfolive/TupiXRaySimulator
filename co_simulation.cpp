#include "tupi.decl.h" // gerado pela compilação de tupi.ci

//-----------------------------------------------
// simulation.cpp
//-----------------------------------------------

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "so_matter.hpp"
#include "so_xrtube.hpp"

#include "co_main.hpp"
#include "co_node.hpp"
#include "co_detector.hpp"
#include "co_simulation.hpp"

extern CProxy_Main      Proxy_Main; // Servidor
extern CProxy_Node      Proxy_Node;
extern CProxy_GDetector Proxy_GDetector;

#define TOL 0.1

//-----------------------------------------------
// construtors
//-----------------------------------------------
Simulation::Simulation(double val) {

   CkPrintf("[Simulation.%d.%d(%d)].Construtor\n", CkMyNode(), CkMyPe(), CkMyRank());

   node     = Proxy_Node.ckLocalBranch();
//   source   = Proxy_Source.ckLocalBranch();
   detector = Proxy_GDetector.ckLocalBranch();

   nev = ceil(val/CkNumPes());
   CkPrintf("\tnev/proc: %.0lf\n", nev);
}

//-----------------------------------------------
Simulation::Simulation(int n, unsigned long *init_key, double val, Media mt) {

   CkPrintf("[Simulation.%d.%d(%d)].Construtor\n", CkMyNode(), CkMyPe(), CkMyRank());

   node     = Proxy_Node.ckLocalBranch();
   detector = Proxy_GDetector.ckLocalBranch();

   media = mt;
   media.Dice(8,init_key+8*CkMyPe());

   nev = ceil(val/CkNumPes());
   CkPrintf("\tnev/proc: %.0lf\n", nev);
}

//-----------------------------------------------
Simulation::Simulation(int n, unsigned long *init_key, double val, int nd, float *fct, XRTube xrt, Media mt) {

   CkPrintf("[Simulation.%d.%d(%d)].Construtor\n", CkMyNode(), CkMyPe(), CkMyRank());

   node     = Proxy_Node.ckLocalBranch();
   detector = Proxy_GDetector.ckLocalBranch();

   media = mt;
   media.Dice(8,init_key+8*CkMyPe());

   xrtube = xrt;
   xrtube.Dice(8,init_key+8*(CkMyPe()+CkNumPes()));

   factor = fct[CkMyPe()];

   nev = ceil(factor*val/CkNumPes());
   CkPrintf("\tnev/proc: %.0lf\n", nev);

   CkCallback cb(CkIndex_Main::Reduction2(), Proxy_Main);
   contribute(cb);
}

//-----------------------------------------------
//
//-----------------------------------------------
void Simulation::Factor(double val, int nd, float *fct) {
   factor = fct[CkMyPe()];

   nev = ceil(factor*val/CkNumPes());
//   CkPrintf("[Simulation.Factor(%d)]\tfactor: %lf\tnev/proc: %.0lf\n", CkMyPe(), factor, nev);
}

//-----------------------------------------------
// implementação do método de simulação
//-----------------------------------------------
void Simulation::Run(int lap) {
   long int i;
   unsigned long int n;
   Point2FaceInfo    pf;
   CrossSections     cs;
   Foton             hv;
   XYZ               p;

   double tf, ti = CkWallTimer();

   CkPrintf("\t[Simulation.%d.%d(%d)].Run\n", CkMyNode(), CkMyPe(), CkMyRank());

   detector->Lap(lap);
   detector->Zeroing();

   for ( i=0; i<nev; i++ ) {
//      if (!CkMyPe() && i==1e5*int(i*1e-5)) CkPrintf(".");
//      if (!CkMyPe() && i==1e6*int(i*1e-6)) CkPrintf("~%.2e",(CkNumPes()*int(i*1e-6)*1e6));
      if (i && i==1.0e6*int(i*1.0e-6)) {
         CkPrintf("[%03d]",CkMyPe());
         if (!CkMyPe()) CkPrintf("~%.2e\n",(CkNumPes()*int(i*1e-6)*1e6));
      }

      hv = xrtube.FotoEmission();
//      hv = node->tube.FotoEmission();

      while (hv.E()>TOL) {
         pf = node->model.Impact_Point(hv.P(),hv.D());

         if (!pf.flag) {
            break; // if there isn't face in front, break
         }
         cs = media.ComputeCS(hv.ID(),hv.E());

         // check if the photon traverses the body
         if (cs.step>pf.dvp) { // if yes...

                             // if it traverses...
            hv.P(pf.pt);     //     move the photon to the impact point
            hv.ID(pf.ids);   //     update the matter id

            if (!strcmp(pf.sname,"detector")) {

               if (detector->Type()!=1) {
                  p = node->model["detector"].Global2Local(pf);
//                  if (!CkMyPe()) {CkPrintf("=> "); hv.P().Print(); p.Print();}
               }
               detector->Detection(CkMyPe(),hv.E(),p);
               break;
            }
         } else {
                                   // else...
            hv.Translate(cs.step); //     just move to interaction point
                                   //     and process the interaction mechanism
            // mechanismis....
            switch(cs.mec) {
               case 0:
                  media.PhotoEletricAbsortion(hv);
                  break;
               case 1:
                  media.ElasticScattering(hv);
                  break;
               case 2:
                  media.InelasticScattering(hv);
                  break;
            }
         }
      }
   }

   tf = CkWallTimer()-ti;
   Proxy_Main.EnlapsedTime(CkMyPe(),tf);

   if (!CkMyPe()) CkPrintf("\n");

   detector->Contribution();

}

//-----------------------------------------------
//-----------------------------------------------

