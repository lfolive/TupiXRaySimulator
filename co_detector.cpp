#include "tupi.decl.h" // gerado pela compilação de tupi.ci

//=========================================================
// detector.cpp
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
extern CProxy_Node      Proxy_Node;
extern CProxy_GDetector Proxy_GDetector;

//=========================================================
//=========================================================

// apelido da função de redução que será usado pelo contribute
CkReduction::reducerType DataOp;

// protótipo da função de redução
CkReductionMsg *DataReducer_Fn(int nMsg, CkReductionMsg **msgs);

// faz o link entre o "apelido" e a função de redução
// o protótipo precisa estar no arquivo .ci
/*initnode*/
void registerDataOp(void) {
   DataOp = CkReduction::addReducer(DataReducer_Fn, false, "DataReducer_Fn");
}

// função de redução
CkReductionMsg *DataReducer_Fn(int nMsg, CkReductionMsg **msgs) {
   // Sum starts off at zero
   int      i;
   long int j, ne;

   long int n = msgs[0]->getSize();
   DATATYPE *mdata = (DATATYPE*)msgs[0]->getData();
   int type = mdata[0].Type();

   ne = n/sizeof(DATATYPE);
   DATATYPE *data = (DATATYPE*) new DATATYPE [ne];
   for ( j=0; j<ne; j++ ) data[j].Data(0,type);

//   CkPrintf("==============> nMsg: %d\n",nMsg);
//   CkPrintf("==============> n: %ld\n",n);

   for ( i=0; i<nMsg; i++ ) {
      // Sanity check:
      CkAssert(msgs[i]->getSize()==n);
      // Extract this message's data
      DATATYPE *mdata = (DATATYPE*)msgs[i]->getData();
      CkAssert(mdata!=NULL);
//      CkPrintf("==============> (%d) mdata: %p\n", i, mdata);
      CkPrintf("Reducing... Wait...\n");
      for( j=0; j<ne; j++ ) {
         data[j] += mdata[j];
      }
   }
   return CkReductionMsg::buildNew(n, data);
}

//=========================================================
//=========================================================

// apelido da função de redução que será usado pelo contribute
CkReduction::reducerType DataOp2;

// protótipo da função de redução
CkReductionMsg *DataReducer2_Fn(int nMsg, CkReductionMsg **msgs);

// faz o link entre o "apelido" e a função de redução
// o protótipo precisa estar no arquivo .ci
/*initnode*/
void registerDataOp2(void) {
   DataOp2 = CkReduction::addReducer(DataReducer2_Fn, true, "DataReducer2_Fn");
}

// função de redução
CkReductionMsg *DataReducer2_Fn(int nMsg, CkReductionMsg **msgs) {
   // Sum starts off at zero
   int      i;
   long int j, ne;
   long int n = msgs[0]->getSize();

   DATATYPE *dat = (DATATYPE*)msgs[0]->getData();
   ne = n/sizeof(DATATYPE);

//   CkPrintf("==============> nMsg: %d\n",nMsg);
//   CkPrintf("==============> n   : %ld\tne: %ld\n",n,ne);

   for ( i=1; i<nMsg; i++ ) {
      // Sanity check:
      CkAssert(msgs[i]->getSize()==n);
      // Extract this message's data
      DATATYPE *mdat = (DATATYPE*)msgs[i]->getData();
      CkAssert(mdat!=NULL);
//      CkPrintf("==============> (%d) mdat: %p\n",i,mdat);
      CkPrintf("Reducing... Wait...\n");
      for( j=0; j<ne; j++ ) {
         dat[j] += mdat[j];
      }
   }
   return CkReductionMsg::buildNew(n, dat, DataOp2, msgs[0]);
}

//=========================================================

//=========================================================
// radiographic film construtor
//=========================================================
// grains: superficial density (grains/cm2)
// area  : grain area (cm2)
// thick : grain thick (cm)
// lx, ly: film dimensions (cm)
//=========================================================
GDetector::GDetector(int n, unsigned long *init_key, int col, int lin,
          double grains, double area, double thick, double lx, double ly,
          int nd, char *dir, int nf, char *file, Media mt, Models md) {

   // radiographic film modelling
   CkPrintf("[Detector.%d.%d].Construtor(nx,ny,grains,area,thick,lx,ly)\n", CkMyNode(), CkMyPe());

   nprocs = CkNumPes();
   nnds = CkNumNodes();
   npp = nprocs/nnds;

   emax = 1;

   type = 0;

   nx = col; ny = lin;
   ag = area;                      // grain area
   thk = thick;                    // grain thickness
   dx = lx/nx;
   dy = ly/ny;
   ap = lx*ly/(nx*ny);                 // pixel area
   n0p = (long int)ceil(grains*ap);    // superficial density per pixel (grains/pixel)
   nBp = (long int)ceil(grains*ap/32); // total of DATATYPE per pixel (ints)

   ndata = nx*ny*nBp; // total of DATATYPE: total of pixels x total of DATATYPE per pixel (ints)
   strcpy(outfile,file);
   strcpy(outdir,dir);

   if (!CkMyPe()) {
      CkPrintf("\t==================================================\n");
      CkPrintf("\t\tdx (cm)  : %lf\tdy (cm)  : %lf\n",dx,dy);
      CkPrintf("\t\tap (cm^2): %e\tag (cm^2): %e\n",ap,ag);
      CkPrintf("\t\tn0p (grais/px): %ld\tn0p*ag/ap (%%): %.3lf\n",n0p,100*n0p*ag/ap);
      CkPrintf("\t\tnBp (DATATYPE): %ld\n",nBp);
      CkPrintf("\t\tndata (DATATYPE): %ld\n",ndata);
//      CkPrintf("\t\tsize(DATAINFO) (B): %ld\n",sizeof(DATAINFO));
      CkPrintf("\t\tsize(DATATYPE) (B): %ld\n",sizeof(DATATYPE));
      CkPrintf("\t\tndata*size(DATATYPE) (MB): %.1f\n",ndata*sizeof(DATATYPE)*1.0/(1<<20));
      CkPrintf("\t==================================================\n");
   }

   model = md;

   media = mt;
   media.Dice(8,init_key+8*CkMyPe());

   dice.setup(init_key+8*(CkMyPe()+CkNumPes()),8);

   data = (DATATYPE*) new DATATYPE [ndata];
//   memset(data, 0, ndata*sizeof(DATATYPE));
   for ( int i=0; i<ndata; i++ ) {
      data[i].Data(0,type);
   }
}

//=========================================================
// XRF detector construtor
//=========================================================
GDetector::GDetector(int val1, double val2, int nd, char *dir, int nf, char *file, Models md) {

   // Detector for x-ray spectrometry
   CkPrintf("[Detector.%d.%d].Construtor(ndata,emax)\n", CkMyNode(), CkMyPe());

   nprocs = CkNumPes();
   nnds = CkNumNodes();
   npp = nprocs/nnds;
//   strcpy(outdir,dir);

   nx = ny = 1;

   type = 1;

   ndata = val1;        // number of channels
   emax = val2;         // highest energy (full scale)
   strcpy(outfile,file);// output file
   strcpy(outdir,dir);  // path to save spectra

   model = md;

   data = (DATATYPE*) new DATATYPE [ndata];
//   memset(data, 0, ndata*sizeof(DATATYPE));
   for ( int i=0; i<ndata; i++ ) {
      data[i].Data(0,type);
   }

   XYZ p = model["detector"].P();
   CkPrintf("\tPosition: %lf, %lf, %lf\n",p.X(),p.Y(),p.Z());
   CkPrintf("\t\tndata = %ld channels\temax = %.3lf keV\tsize = %ld\n", ndata, emax, sizeof(data));
   CkPrintf("\t\tOutput dir: %s\tOutput file: %s\n", outdir, outfile);

}

//=========================================================
// image detector construtor
//=========================================================
GDetector::GDetector(int col, int lin, int nd, char *dir, int nf, char *file) {

   // detector for imaging
   CkPrintf("[Detector.%d.%d].Construtor(nx,ny)\n", CkMyNode(), CkMyPe());

   nprocs = CkNumPes();
   nnds = CkNumNodes();
   npp = nprocs/nnds;
//   strcpy(outdir,dir);

   emax = 1;

   type = 2;

   nx = col; ny = lin;  // image dimensions
   ndata = nx*ny;       // total of pixels
   strcpy(outfile,file);// output file
   strcpy(outdir,dir);  // path to save images

   data = (DATATYPE*) new DATATYPE [ndata];
//   memset(data, 0, ndata*sizeof(DATATYPE));
   for ( int i=0; i<ndata; i++ ) {
      data[i].Data(0,type);
   }
}

//=========================================================
void GDetector::Zeroing(void) {
   // zera dados
//    memset(data, 0, ndata*sizeof(DATATYPE));
   for ( int i=0; i<ndata; i++ ) data[i].Data(0);
}
//=========================================================
void GDetector::Model(Models md) {
//   CkPrintf("\t[GDetector.%d].Model: Updating model.\n", CkMyNode());
   model = md;
}
//=========================================================


//=========================================================
// implementação dos métodos de detecção
//=========================================================
void GDetector::Detection(int pe, double e, XYZ p) {
   int px, py;

   if (type==0) {
      GrainDiscriminator(e,p);
   } else if (type==1) {
      EnergyDiscriminator(e);
   } else if (type==2) {
      PositionDiscriminator(p);
   } else {
      CkAbort("[GDetector.Detection] Error at Detection method!!!\n");
      CkExit();
   }
}
//=========================================================
void GDetector::GrainDiscriminator(double e, XYZ p) {
   unsigned long b, b32, r32;
   unsigned long n;
   double        eps;
   CrossSections cs;

   // test if photon hit any grain
   eps = dice.drand();
   if (eps<=n0p*(ag/ap)) {
      // if so, draft a pixel grain 
      b = (long int)(n0p*dice.drand());
      // which DATATYPE belonging to the pixel
      b32 = b/32;
      // which bit belonging to the DATATYPE
      r32 = b%32;
      // compute the DATATYPE index
      n = (unsigned long)(p.Y()*ny)*(nx*nBp) + (unsigned long)(p.X()*nx)*nBp + b32;

      //----------------------------------------
      // eval the r32 bit from that DATATYPE
      //----------------------------------------

      // if the grain is virgin
      // proceed with the interaction
      if (!data[n].Bit(r32)) {
         // eval cross-sections
         cs = media.ComputeCS(21,e); // <====== 21
         // if photon hit the grain
         //    and is absorved or scattered inelasticly
         if (cs.step<thk && cs.mec!=1) {
            data[n].Set(r32);
//            if (!CkMyPe()) { CkPrintf("====> %ld\t%ld\t%ld\t%ld\n",l ong(p.Y()*ny),l ong(p.X()*nx), b32, r32); }
         }
      }
   }
}
//=========================================================
void GDetector::EnergyDiscriminator(double e) {

   // faz a detecção
   int n = (int)((ndata-1)*e/emax+0.5);

   data[n<ndata?n:ndata-1]++;
}
//=========================================================
void GDetector::PositionDiscriminator(XYZ p) {

   // faz a detecção
   unsigned long n = (unsigned long)(p.Y()*ny)*nx + (unsigned long)(p.X()*nx);

//   CkPrintf("\t\t[Detector.%d.%d].PositionDiscriminator: lin=%d\tcol=%d\n",
//               CkMyNode(), CkMyPe(), int(p.Y()*ny),int(p.X()*nx));

   ++data[n];
}
//=========================================================


//=========================================================
// implementação do método de contribuição
//=========================================================
void GDetector::Contribution(void) {
   int i, j;

   CkPrintf("\t\t[Detector.%d.%d].Contribution\n", CkMyNode(), CkMyPe());

//   for ( int i=0; i<ndata; i++ ) data[i].Data(1,type,emax);

   // retorna os dados finais
//   CkCallback cb(CkIndex_Main::Reduction(NULL), Proxy_Main);
//   CkCallback cb(CkIndex_Node::Reduction(NULL), Proxy_Node);
   CkCallback cb(CkIndex_GDetector::Reduction(NULL), Proxy_GDetector[0]);
   contribute(ndata*sizeof(DATATYPE), data, DataOp2, cb);
}

//=========================================================
typedef union {
   unsigned short us;
   unsigned char  uc[2];
} endian;

void GDetector::Reduction(CkReductionMsg* m) {
   FILE *fp, *fp2;
   char fname[256], fname2[256];
   unsigned char uc;
   int  i, j, k, l;
   unsigned utmp;
   long int sum, val;
   unsigned short px;
   XYZ p;

   endian tmp;

   DATATYPE *dta = (DATATYPE*)m->getData();
   long int  size = m->getSize()/sizeof(DATATYPE);


   CkPrintf("[Detector.%d.%d].Reduction\n", CkMyNode(), CkMyPe());

   CkPrintf("\tdata size = %.2lf MB (%d)\n", m->getSize()*1.0/sqr(1024), m->getSize());

   if (type==1) {
      CkPrintf("Output dir: %s\toutfile: %s\trun: %03d\n",outdir,outfile,lap);
//      sprintf(fname,"%s/saida.spe",outdir);
      sprintf(fname,"./%s/%s.%03d.spe",outdir,outfile,lap);
      CkPrintf("Saving the file %s\n",fname);
      fp = fopen(fname,"wt");
//         CkPrintf("\t\tdata[%2d] = %d\ttype = %d\n", i, dta[i].Data(), dta[0].Type());

         p = model["detector"].P();
         fprintf(fp,"Position: %lf %lf %lf\n",p.X(),p.Y(),p.Z());
         sum = 0;
         for ( i=0; i<size; i++ ) {
            sum += dta[i].Data();
            fprintf(fp,"%lf\t%d\n", i*emax/(size-1), dta[i].Data());

//            CkPrintf("\t\tdata[%2d]: %lf\t%d\n", i, i*emax/(size-1), dta[i].Data());

         }
      fclose(fp);

   } else if (type==2) {
      val = 256;
      sum = 0;
      for ( i=0; i<ny; i++ ) {
         for ( j=0; j<nx; j++ ) {
            sum += dta[i*nx+j].Data();      //
            if ( val < dta[i*nx+j].Data() )
               val = dta[i*nx+j].Data();
         }
      }

      CkPrintf("\tmax val = %ld\n", val);
      CkPrintf("\tsum = %ld\n", sum);

      sprintf(fname,"%s%s.%03d.pgm",outdir,outfile,lap);
      CkPrintf("Saving the file %s\n",fname);

#if 1
      // saving image on binary format
      fp = fopen(fname,"wt");
         fprintf(fp,"P5 %d %d %ld\n", nx, ny, val);
      fclose(fp);
      fp = fopen(fname,"ab");
         for ( i=0; i<ny*nx; i++ ) {
            tmp.us = (unsigned short)(dta[i].Data()&0xffff);
            fwrite(&tmp.uc[1],sizeof(unsigned char),1,fp);
            fwrite(&tmp.uc[0],sizeof(unsigned char),1,fp);
         }
      fclose(fp);
#else
      // saving on text format
      fp = fopen(fname,"wt");
         fprintf(fp,"P2 %d %d %ld\n", nx, ny, val);
         for ( i=0; i<ny; i++ ) {
            for ( j=0; j<nx; j++ ) {
               fprintf(fp,"%8d", dta[i*nx+j].Data());
            }
            fprintf(fp,"\n");
         }
      fclose(fp);
#endif
   } else if (type==0) {

      val = 256;
      // salvar imagem no formato pgm modo binário
      sprintf(fname,"%s%s.%03d.pgm",outdir,outfile,lap);
//      sprintf(fname,"%s%s.bin.pgm",outdir,outfile);
      CkPrintf("Saving the file %s\n",fname);

      sprintf(fname2,"%s/%s.%dx%dx%ld.%03d.raw",outdir,outfile,nx,ny,nBp,lap);
      CkPrintf("Saving the file %s\n",fname2);

      fp = fopen(fname,"wt");
         fprintf(fp,"P5 %9d %9d %9ld\n", nx, ny, val);
      fclose(fp);

      fp2 = fopen(fname2,"wb");
      fp = fopen(fname,"ab");
         for ( i=0; i<ny; i++ ) {
            for ( j=0; j<nx; j++ ) {
               sum = 0;
               for ( k=0; k<nBp; k++ ) {
                  utmp = dta[(i*nx+j)*nBp+k].Data();
                  fwrite(&utmp,sizeof(unsigned),1,fp2);
                  for ( l=0; l<32; l++ ) {
                     sum += dta[(i*nx+j)*nBp+k].Bit(l);
                  }
               }
               if ( val < sum ) val = sum;
               tmp.us = (unsigned short)sum;
//               uc = tmp.uc[0]; tmp.uc[0] = tmp.uc[1]; tmp.uc[1] = uc;
               fwrite(&tmp.uc[1],sizeof(unsigned char),1,fp);
               fwrite(&tmp.uc[0],sizeof(unsigned char),1,fp);
            }
         }
      CkPrintf("\tmax val = %ld\n", val);

      fclose(fp);
      fclose(fp2);

      fp = fopen(fname,"r+t");
         fprintf(fp,"P5 %9d %9d %9ld", nx, ny, val);
      fclose(fp);

   }

   Proxy_Main.Reduction();
}

//=========================================================




