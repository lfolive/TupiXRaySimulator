//---------------------------------------------------
// so_xrtube.hpp
//---------------------------------------------------

#ifndef SO_XRTUBE_HPP
#define SO_XRTUBE_HPP

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cMT19937ar.hpp"

#include "so_geometry.hpp"
#include "so_matter.hpp"

//---------------------------------------------------
// definicao da classe do grupo Source
//---------------------------------------------------
class XRTube {
private:
   MT19937ar dice;

   XYZ       p, d;
   XYZ       u, v;
   int       bg;
   int       type;
   double    lx, ly;   // retangular spot
   double    radius;   // focus (cm)
   double    aperture; // cone aperture (rad)
   double    emax;     // highest energy (keV)
   double    delta;    // spectrum energy step
   int       nch;      // bins;
   double   *ene;      // energy
   double   *dNdE;     // probability

public:
   //-----------------------------------------------
   // construtors
   //-----------------------------------------------
   XRTube(void) { 
      type = 0;
      aperture = radius = 0;
      emax = delta = 0;
      lx = ly = 0;
      bg = nch = 0;
      dNdE = NULL; ene = NULL;
   }

   XRTube(const char *spe)                                            { Setup(spe); }
   XRTube(Atom &anode, Atom &window, double dx, double he, double de) { Setup(anode,window,dx,he,de); }

   void Setup(const char *spe) {
      FILE  *fp;
      int    i;
      double prob;

      fp = fopen(spe,"rt");
         if (!fp) {
            printf("Error opening spectrum file %s! Aborting!!!\n",spe);
            CkExit();
         }
         for ( nch=0; fscanf(fp,"%lf %lf\n",&emax,&prob)==2; nch++ );

         ene = new double [nch];
         dNdE = new double [nch];

         fseek(fp,0,SEEK_SET);
         for ( i=0; i<nch; i++ ) {
            fscanf(fp,"%lf %lf\n",ene+i,dNdE+i);
         }
      fclose(fp);

      // calcular probabilidade acumulada do espectro de emissão
      for ( i=1; i<nch; i++ ) dNdE[i] += dNdE[i-1];
      if (dNdE[nch-1])
         for ( i=0; i<nch; i++ ) dNdE[i] /= dNdE[nch-1];
   }

   void Setup(Atom &anode, Atom &window, double dx, double he, double de) {
      int i;
      double prob;

      emax = he;
      delta = de;
      nch = 1+int(he/de);

      ene = new double [nch];
      dNdE = new double [nch];

      for ( i=0; i<nch; i++ ) ene[i] = i*delta;

      Continuous(anode);
      Discrete(anode);

      // filtragem pela janela de berílio de dx cm de espessura
      for ( i=1; i<nch; i++ ) {
         prob = exp(-window.SC_Abs(i*delta)*window.D()*dx);
         dNdE[i] *= prob;
      }

      // calcular probabilidade acumulada do espectro de emissão
      for ( i=1; i<nch; i++ ) dNdE[i] += dNdE[i-1];
      if (dNdE[nch-1])
         for ( i=0; i<nch; i++ ) dNdE[i] /= dNdE[nch-1];
   }

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &q) {
      q|p; q|d; q|u; q|v;
      q|bg; q|radius; q|aperture;
      q|emax; q|delta; q|nch;
      q|lx; q|ly; q|type;

      if (q.isUnpacking()) {
         ene = new double [nch];
         dNdE = new double [nch];
      }
      PUParray(q,ene,nch);
      PUParray(q,dNdE,nch);
   }

   //-----------------------------------------------
   // protótipos dos métodos do grupo Source
   //-----------------------------------------------
   void Dice(unsigned long n, unsigned long *init_key) {
      dice.setup(init_key,n); //<========
      CkPrintf("\t[Source.Dice] sample: %lf\n",dice.drand());
   }

   void P(XYZ val) { p = val; }
   void D(XYZ val) {
      d = val.Unit();
      u = (XYZ(0,0,1)^d).Unit();
      v = (d^u).Unit();
   }

   void Translate(XYZ vec) {
      p += vec;
   }

   void Rotate(double ang, XYZ vec) {
      p = p.Rotate(ang,vec);
      d = d.Rotate(ang,vec);
      u = u.Rotate(ang,vec);
      v = v.Rotate(ang,vec);
   }

   void BG(int val)    { bg = val;       }
   void R(double val)  { radius = val;   }
   void A(double val)  { aperture = val; }
   void DE(double val) { delta = val;    }
   void HE(double val) { emax = val;     } // highest energy
   void N(int val)     { nch = val;      }

   void Spot(int ibg, double rd, double theta)
   {
      type = 0; bg = ibg;
      radius = rd; aperture = theta;
   }
   void Spot(int ibg, double ilx, double ily, double rd, double theta) {
      type = 1; bg = ibg;
      radius = rd; aperture = theta;
      lx = ilx; ly = ily;
   }

   XYZ    P(void) { return p; }
   XYZ    D(void) { return d; }

   int    BG(void)    { return bg;        }
   double R(void)     { return radius;    }
   double A(void)     { return aperture;  }
   double DE(void)    { return delta;     }
   double HE(void)    { return emax;      }
   int    N(void)     { return nch;       }
   double E(int val)  { return ene[val];  }
   double PE(int val) { return dNdE[val]; }

   void   Discrete(Atom &db) {
      int    i, j, n, pos;
      double Z, A, Ek, E0, U0, rU0, R;
      double lnZ, lnU0, U0lnU0;
      double zkbk, rz, iS, cte, tau, f, ft;

      double omega = 0.00367;      // srad
      double phi = 0.5*M_PI;       // rad
      double vareps = 35*M_PI/180; // rad
      double spsv = sin(phi)/sin(vareps);

      Z = db.Z(); //<<====================
      A = db.A(); //<<====================

      E0 = emax;

      double m = 0.1382 - 0.9211/sqrt(Z);
      double J = 0.0135*Z;
      double eta = pow(E0,m)*(0.1904 - 0.2236*lnZ + 0.1292*sqr(lnZ) - 0.0149*lnZ*sqr(lnZ));
      double rzm = (A/Z)*(0.787e-5*sqrt(J)*pow(E0,1.5) + 0.735e-6*sqr(E0));

      int    ns, k, l;
      double Nkl, Ekl, mtp=1e13;

      ns = db.NS();    //<<====================

      for ( j=0; j<ns; j++ ) {
         Ek = db.Eb(j); // binding energy of j-level  //<<====================
         if (0<Ek && Ek<E0) {
            U0 = E0/Ek;

            switch (db.Simb(j)[0]) { //<<====================
               case 'K': 
                  zkbk = 0.7;
                  cte = mtp*(4.697+0.134*U0-0.00268*sqr(U0))/(1-exp(-7*(U0-1)));
                  break;
               case 'L':
                  zkbk = 2;
                  switch (db.Simb(j)[1]) { //<<====================
                     case '1': cte = 0.71*mtp*(-0.4814 + 0.03781*Z - 2.413e-4*sqr(Z)); break;
                     case '2': cte = 2.7*mtp*(-0.4814 + 0.03781*Z - 2.413e-4*sqr(Z)); break;
                     case '3': cte = 4.94*mtp; break;
                     default : cte = 0;
                  }
                  break;
               case 'M':
                  zkbk = 0.84;
                  switch (db.Simb(j)[1]) { //<<====================
                     case '3': cte = 2.32*mtp; break;
                     case '4': cte = 15.8*mtp; break;
                     case '5': cte = 20.5*mtp; break;
                     default : cte = 0;
                  }
                  break;
               default : zkbk = 0;
            }

            lnU0 = log(U0);
            rz = rzm*lnU0*   (0.49269 - 1.0987*eta + 0.78557*sqr(eta))/
                          (0.70256 - 1.09865*eta + 1.0046*sqr(eta) + lnU0);

            U0lnU0 = U0*(lnU0-1)+1;
            rU0 = sqrt(U0);

            //iS = (1+16.05*sqrt(J/Ek)*(rU0*lnU0+2*(1-rU0))/U0lnU0)*U0lnU0*zkbk/Z;
            iS = (zkbk/Z)*(U0lnU0 + 16.05*sqrt(J/Ek)*(rU0*lnU0+2*(1-rU0)));
            R = 1 - 0.0081517*Z + 3.613e-5*sqr(Z) + 0.009583*Z*exp(-U0) + 0.001141*E0;

            for ( i=0; i<db.NR(j); i++ ) {
               if (Ekl = db.XRAY(j,i)) {
                  tau = db.SC_Abs(Ekl);                  //<<====================

                  ft = 2*tau*rz*spsv;
                  f = (1-exp(-ft))/ft;

                  Nkl = cte*R*iS*db.W(j)* db.RATE(j,i) *f;  //<<====================
                  dNdE[(int)(Ekl/delta)] += Nkl;    //<<====================
               }
            }
         }
      }
   }

   void   Continuous(Atom &db) {
      double omega = 0.00367;      // srad
      double phi = 0.5*M_PI;       // rad
      double vareps = 35*M_PI/180; // rad
      double spsv = sin(phi)/sin(vareps);

      int    i, n, pos;
      int    Z;            // número atômico do anodo
      double E, E0;        // Energia e energia máxima
      double A;            // massa atômica

      double U0, lnU0, rz, ft, f, tau;

      // recupera número atômico
      Z = db.Z();             //<<====================
      // recupera massa atômica
      A = db.A();             //<<====================
      E0 = emax;

      double cte = 1.35e9;                       // fator de correção Ebel(2006)
      double x = 1.109 - 0.00435*Z + 0.00175*E0; // fator de correção Ebel(2006)

      double m = 0.1382 - 0.9211/sqrt(Z);
      double J = 0.0135*Z;
      double lnZ = log(Z);
      double eta = pow(E0,m)*(0.1904 - 0.2236*lnZ + 0.1292*sqr(lnZ) - 0.0149*lnZ*sqr(lnZ));
      double rzm = (A/Z)*(0.787e-5*sqrt(J)*pow(E0,1.5) + 0.735e-6*sqr(E0));

      dNdE[0] = 0;
      for ( i=1; i<nch-1; i++ ) {
         E = i*delta;
         U0 = E0/E;
         lnU0 = log(U0);
         rz = rzm*lnU0*   (0.49269 - 1.0987*eta + 0.78557*sqr(eta))/
                       (0.70256 - 1.09865*eta + 1.0046*sqr(eta) + lnU0);

         tau = db.SC_Abs(E);            //<<====================

         ft = 2*tau*rz*spsv;
         f = (1-exp(-ft))/ft;

         dNdE[i] = cte*Z*pow(U0-1,x)*f; //<<====================
      }
   }

   Foton  FotoEmission(void) {
      Foton  hv;
      XYZ    r, s;
      int    i;
      double e, eps;

   //   CkPrintf("\t\t[Source.%d.%d].FotoEmission\n", CkMyNode(), CkMyRank());

      eps = dice.drand();
      for ( i=0; i<nch? dNdE[i]<eps: 0; i++ );

      hv.E(ene[i]);
      hv.ID(bg);

      if (!type) { // circular spot
         r = radius*sqrt(dice.drand())*u;
         hv.P(p+r.Rotate(2*M_PI*dice.drand(),d));

         r = d.Rotate(atan(tan(aperture)*sqrt(dice.drand())),u);
         hv.D(r.Rotate(2*M_PI*dice.drand(),d).Unit());
      } else {
         r = radius*sqrt(dice.drand())*u;
         hv.P(p+r.Rotate(2*M_PI*dice.drand(),d));

         // |(s*u)|/|(s*d)| = tan(theta)
         // |(s*v)|/|(s*d)| = tan(phi)
         // lx is the absolute limit to tan(theta)
         // ly is the absolute limit to tan(phi)
         do {
            r = d.Rotate(atan(tan(aperture)*sqrt(dice.drand())),u);
            s = r.Rotate(2*M_PI*dice.drand(),d).Unit();
         } while (fabs(s*u)>lx*fabs(s*d) || fabs(s*v)>ly*fabs(s*d));
         hv.D(s);
      }

      return hv;
   }

   void Print(void) {
      CkPrintf("[Source.%d.%d].Print\n", CkMyNode(), CkMyRank());
      CkPrintf("\tP             : "); p.Print();
      CkPrintf("\tD             : "); d.Print();
      CkPrintf("\tu             : "); u.Print();
      CkPrintf("\tv             : "); v.Print();
      CkPrintf("\tbackgroud id  : %d\n", bg);
      CkPrintf("\tradius (cm)   : %lf\n", radius);
      CkPrintf("\taperture (rad): %lf\n", aperture);
      CkPrintf("\temax (keV)    : %lf\n", emax);
      CkPrintf("\tchannels      : %d\n", nch);
      for ( int i=0; i<nch; i++ ) CkPrintf("\t\t%04d\t%lf\t%lf\n", i, ene[i], dNdE[i]);
   }

   void Print(const char* filename) {
      FILE *fp;
      fp = fopen(filename,"wt");
         fprintf(fp,"[Source.%d.%d].Print\n", CkMyNode(), CkMyRank());
         fprintf(fp,"\tP             : %lf\t%lf\t%lf\n", p.X(), p.Y(), p.Z());
         fprintf(fp,"\tD             : %lf\t%lf\t%lf\n", d.X(), d.Y(), d.Z());
         fprintf(fp,"\tu             : %lf\t%lf\t%lf\n", u.X(), u.Y(), u.Z());
         fprintf(fp,"\tv             : %lf\t%lf\t%lf\n", v.X(), v.Y(), v.Z());
         fprintf(fp,"\tbackgroud id  : %d\n", bg);
         fprintf(fp,"\tradius (cm)   : %lf\n", radius);
         fprintf(fp,"\taperture (rad): %lf\n", aperture);
         fprintf(fp,"\temax (keV)    : %lf\n", emax);
         fprintf(fp,"\tchannels      : %d\n", nch);
         for ( int i=0; i<nch; i++ )
            fprintf(fp,"\t\t%04d\t%lf\t%lf\t%lf\n", i, ene[i], dNdE[i]-dNdE[i?i-1:0], dNdE[i]);
      fclose(fp);
   }
};
//---------------------------------------------------

#endif
