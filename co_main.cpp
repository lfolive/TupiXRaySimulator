#include "tupi.decl.h" // gerado pela compilação de tupi.ci

//===================================================
// main.cpp
//===================================================

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "so_geometry.hpp"
#include "so_matter.hpp"
#include "so_xrtube.hpp"

#include "co_main.hpp"
#include "co_node.hpp"
#include "co_detector.hpp"
#include "co_simulation.hpp"

#include <queue>

//---------------------------------------------------------
//---------------------------------------------------------

CProxy_Main       Proxy_Main;       // Mestre
CProxy_Node       Proxy_Node;       // Nós
CProxy_Simulation Proxy_Simulation; // Simulador
CProxy_GDetector  Proxy_GDetector;  // Detector

//===================================================
// construtor
//===================================================
Main::Main(CkArgMsg *m) {
   time_t rawtime;

   CkEntryOptions opts;
   unsigned long i, j;
   char   code[16], param[256], tmp[128];
   char   perform[64], setupsim[64];
   char   mediafile[64], spectrum[64];

   char   solidname[64], solidfile[64];
   int    solidmat, solidface;

   int    type_detector, type_spot;

   int    bg, an, wn, ln, co, cs, nch;
   double ang, dx, dy, dz;
   double th, en, rd, slx, sly, dv, efs;
   double lx, ly, ds, ga, gt;
   double tpx, tpy, tpz, tdx, tdy, tdz;
   double mtdx, mtdy, mtdz;
   double mrang, mrix, mriy, mriz;
   FILE *fp;

   std::string             info = "";
   std::queue<std::string> body;
   std::queue<std::string> move;
   std::queue<std::string> config;

   unsigned long nkeys, *init_key;

   //---------------------------------------------------
   // defaults
   //---------------------------------------------------

   outfile[0] = outdir[0] = perform[0] = '\0';
   setupsim[0] = spectrum[0] = '\0';

   strcpy(mediafile,"meio.cfg");
   nev = 1;
   bg = 0;
   an = 22;
   wn = 2;
   th = 0;
   en = 1; // keV
   rd = slx = sly = 0;
   dv = 0;
   nch = 1;
   efs = 10.0; // keV
   lx = ly = 1.0;
   ln = co = 1;
   ds = ga = gt = 1.0;
   tpx = tpy = tpz = -1.0;
   tdx = tdy = tdz = 1.0;
   type_spot = 0;
   type_detector = 2;

   cs = 0; // solid counter

   //---------------------------------------------------
   // Proxy_Main
   //---------------------------------------------------
   Proxy_Main = thisProxy;
   count = CkNumPes();
   iter = lap = rec = 0;

   //---------------------------------------------------
   // parâmetros de entrada (linha de comando)
   //---------------------------------------------------
   for ( int i=1; i<m->argc; i+=2 ) {
      if (!strcmp(m->argv[i],"-c")) {       // setup file
         strcpy(setupsim,m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];

      // config parameters: -n -o -s -f -it -r
      } else if (!strcmp(m->argv[i],"-n")) { // number of events
         nev = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-o")) { // output directory
         strcpy(outdir,m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-s")) { // output filename (base)
         strcpy(outfile,m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-f")) { // performance file
         strcpy(perform,m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-it")) { // number of laps
         iter = atoi(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-r")) { // recover the break run
         rec = 1; i--;
         info = m->argv[i];

      // media parameters: -md -bg
      } else if (!strcmp(m->argv[i],"-md")) { // media file
         strcpy(mediafile,m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-bg")) { // background index
         bg = atoi(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];

      // source parameters if no tube: -sp
      } else if (!strcmp(m->argv[i],"-sp")) { // spectrum file
         strcpy(spectrum,m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];

      // source parameters if xray tube: -an -wn -th -en
      } else if (!strcmp(m->argv[i],"-an")) { // anode index
         an = atoi(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-wn")) { // window index
         wn = atoi(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-th")) { // window thickness
         th = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-en")) { // highest energy
         en = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];

      // spot geometric parameters: -rd -slx -sly -dv -dg
      } else if (!strcmp(m->argv[i],"-rd")) { // spot radius
         rd = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-slx")) { // spot width
         slx = atof(m->argv[i+1]); type_spot=1;
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-sly")) { // spot height
         sly = atof(m->argv[i+1]); type_spot=1;
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-dv")) { // aperture (radians)
         dv = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-dg")) { // aperture (degrees)
         dv = atof(m->argv[i+1])*deg;
         info = m->argv[i]; info += " "; info += m->argv[i+1];

      // spatial parameters: -tp -tpx -tpy -tpz -td -tdx -tdy -tdz
      } else if (!strcmp(m->argv[i],"-tp")) { // position: x y z
         tpx = atof(m->argv[i+1]);
         tpy = atof(m->argv[i+2]);
         tpz = atof(m->argv[i+3]); i+=2;
         info = m->argv[i];    info += " "; info += m->argv[i+1]; info += " "; 
         info += m->argv[i+2]; info += " "; info += m->argv[i+3];
      } else if (!strcmp(m->argv[i],"-tpx")) { // position: x
         tpx = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-tpy")) { // position: y
         tpy = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-tpz")) { // position: z
         tpz = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-td")) { // direction: x y z
         tdx = atof(m->argv[i+1]);
         tdy = atof(m->argv[i+2]);
         tdz = atof(m->argv[i+3]); i+=2; 
         info = m->argv[i];    info += " "; info += m->argv[i+1]; info += " "; 
         info += m->argv[i+2]; info += " "; info += m->argv[i+3];
      } else if (!strcmp(m->argv[i],"-tdx")) { // direction: x
         tdx = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-tdy")) { // direction: y
         tdy = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-tdz")) { // direction: z
         tdz = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];

      // detector parameters: -type
      } else if (!strcmp(m->argv[i],"-type")) { // detector type
         type_detector = atoi(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];

      // ideal or xray film: -ln -co -lx -ly
      } else if (!strcmp(m->argv[i],"-ln")) { // lines
         ln = atoi(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-co")) { // columns
         co = atoi(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-lx")) { // width
         lx = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-ly")) { // hight
         ly = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];

      // xray film: -ds -ga -gt
      } else if (!strcmp(m->argv[i],"-ds")) { // superficial density of grains
         ds = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-ga")) { // grain area
         ga = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      } else if (!strcmp(m->argv[i],"-gt")) { // grain thickness
         gt = atof(m->argv[i+1]);
         info = m->argv[i]; info += " "; info += m->argv[i+1];
      }
      if (m->argv[i][0]=='-') {
         config.push(info);
         CkPrintf("%s\n", info.c_str());
      }
   }

   //---------------------------------------------------

   if (strlen(setupsim)) {
      fp = fopen(setupsim,"rt");
         if (!fp) {
            CkPrintf("Error opening file %s. Aborting!!!\n", setupsim);
            CkExit();
         }
      CkPrintf("Reading setup file: %s\n", setupsim);
      while (fscanf(fp,"%s",code)>0) {
         //fscanf(fp,"%s",code);
         if (code[0]=='#') {              // comment
            fgets(param,63,fp);

      // config parameters: -n -o -s -f -it -r
         } else if (!strcmp(code,"-n")) { // number of events
            fscanf(fp,"%s",param);
            nev = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-o")) { // output directory
            fscanf(fp,"%s",param);
            strcpy(outdir,param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-s")) { // output filename (base)
            fscanf(fp,"%s",param);
            strcpy(outfile,param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-f")) { // performance file
            fscanf(fp,"%s",param);
            strcpy(perform,param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-it")) { // background index
            fscanf(fp,"%s",param);
            iter = atoi(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-r")) {  // recover the break run
            rec = 1;
            info = code;

      // media parameters: -md -bg
         } else if (!strcmp(code,"-md")) { // media filename
            fscanf(fp,"%s",param);
            strcpy(mediafile,param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-bg")) { // background index
            fscanf(fp,"%s",param);
            bg = atoi(param);
            info = code; info += " "; info += param;

      // source parameters if no tube: -sp
         } else if (!strcmp(code,"-sp")) { // spectrum filename
            fscanf(fp,"%s",param);
            strcpy(spectrum,param);
            info = code; info += " "; info += param;

      // source parameters if xray tube: -an -wn -th -en
         } else if (!strcmp(code,"-an")) { // anode index
            fscanf(fp,"%s",param);
            an = atoi(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-wn")) { // window index
            fscanf(fp,"%s",param);
            wn = atoi(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-th")) { // number of events
            fscanf(fp,"%s",param);
            th = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-en")) { // highest energy
            fscanf(fp,"%s",param);
            en = atof(param);
            info = code; info += " "; info += param;

      // spot geometric parameters: -rd -slx -sly -dv -dg
         } else if (!strcmp(code,"-rd")) { // spot radius
            fscanf(fp,"%s",param);
            rd = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-slx")) { // spot width
            fscanf(fp,"%s",param);
            slx = atof(param); type_spot = 1;
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-sly")) { // spot height
            fscanf(fp,"%s",param);
            sly = atof(param); type_spot = 1;
            info = code; info += " "; info += param;

         } else if (!strcmp(code,"-dv")) { // aperture (radians)
            fscanf(fp,"%s",param);
            dv = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-dg")) { // aperture (degrees)
            fscanf(fp,"%s",param);
            dv = atof(param)*deg;
            info = code; info += " "; info += param;

      // spatial parameters: -tp -tpx -tpy -tpz -td -tdx -tdy -tdz
         } else if (!strcmp(code,"-tp")) { // position: x y z
            fscanf(fp,"%s",tmp); strcpy(param,tmp);
            tpx = atof(tmp);     strcat(param," ");
            fscanf(fp,"%s",tmp); strcat(param,tmp);
            tpy = atof(tmp);     strcat(param," ");
            fscanf(fp,"%s",tmp); strcat(param,tmp);
            tpz = atof(tmp);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-tpx")) { // position: x
            fscanf(fp,"%s",param);
            tpx = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-tpy")) { // position: y
            fscanf(fp,"%s",param);
            tpy = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-tpz")) { // position: z
            fscanf(fp,"%s",param);
            tpz = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-td")) { // direction: x y z
            fscanf(fp,"%s",tmp); strcpy(param,tmp);
            tdx = atof(tmp);     strcat(param," ");
            fscanf(fp,"%s",tmp); strcat(param,tmp);
            tdy = atof(tmp);     strcat(param," ");
            fscanf(fp,"%s",tmp); strcat(param,tmp);
            tdz = atof(tmp);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-tdx")) { // direction: x
            fscanf(fp,"%s",param);
            tdx = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-tdy")) { // direction: y
            fscanf(fp,"%s",param);
            tdy = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-tdz")) { // direction: z
            fscanf(fp,"%s",param);
            tdz = atof(param);
            info = code; info += " "; info += param;

      // detector parameters: -type
         } else if (!strcmp(code,"-type")) { // detector type
            fscanf(fp,"%s",param);
            type_detector = atoi(param);
            info = code; info += " "; info += param;

      // spectrometer: -nc -fs
         } else if (!strcmp(code,"-nc")) { // number of channels
            fscanf(fp,"%s",param);
            nch = atoi(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-fs")) { // full scale (keV)
            fscanf(fp,"%s",param);
            efs = atoi(param);
            info = code; info += " "; info += param;

         } else if (!strcmp(code,"-lx")) { // width
            fscanf(fp,"%s",param);
            lx = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-ly")) { // hight
            fscanf(fp,"%s",param);
            ly = atof(param);
            info = code; info += " "; info += param;

      // ideal or xray film: -ln -co -lx -ly
         } else if (!strcmp(code,"-ln")) { // lines
            fscanf(fp,"%s",param);
            ln = atoi(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-co")) { // columns
            fscanf(fp,"%s",param);
            co = atoi(param);
            info = code; info += " "; info += param;

         } else if (!strcmp(code,"-lx")) { // width
            fscanf(fp,"%s",param);
            lx = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-ly")) { // hight
            fscanf(fp,"%s",param);
            ly = atof(param);
            info = code; info += " "; info += param;

      // xray film: -ds -ga -gt
         } else if (!strcmp(code,"-ds")) { // superficial density of grains
            fscanf(fp,"%s",param);
            ds = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-ga")) { // grain area
            fscanf(fp,"%s",param);
            ga = atof(param);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-gt")) { // grain thickness
            fscanf(fp,"%s",param);
            gt = atof(param);
            info = code; info += " "; info += param;

         } else if (!strcmp(code,"-mc")) { // solid: name mat file face
//                                                      info = code;
//                                                      move.push(info);
            fscanf(fp,"%s",tmp);   strcpy(param,tmp); info = tmp;
                                   strcat(param," "); body.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); body.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); body.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                                      body.push(info);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-mt")) { // translate: name dx dy dz
                                                      info = code;
                                                      move.push(info);
            fscanf(fp,"%s",tmp);   strcpy(param,tmp); info = tmp;
                                   strcat(param," "); move.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); move.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); move.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                                      move.push(info);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-mr") || !strcmp(code,"-mrd")) { // rotate: name ang(rad) ix iy iz
                                                      info = code;
                                                      move.push(info);
            fscanf(fp,"%s",tmp);   strcpy(param,tmp); info = tmp;
                                   strcat(param," "); move.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); move.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); move.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); move.push(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                                      move.push(info);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-mti")) { // translate: name ix iy iz
                                                      info = code;
                                                      itmove.push_back(info);
            fscanf(fp,"%s",tmp);   strcpy(param,tmp); info = tmp;
                                   strcat(param," "); itmove.push_back(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); itmove.push_back(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); itmove.push_back(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); itmove.push_back(info);
            info = code; info += " "; info += param;
         } else if (!strcmp(code,"-mri") || !strcmp(code,"-mrdi")) { // rotate: name ang(rad) ix iy iz
                                                      info = code;
                                                      itmove.push_back(info);
            fscanf(fp,"%s",tmp);   strcpy(param,tmp); info = tmp;
                                   strcat(param," "); itmove.push_back(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); itmove.push_back(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); itmove.push_back(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                   strcat(param," "); itmove.push_back(info);
            fscanf(fp,"%s",tmp);   strcat(param,tmp); info = tmp;
                                                      itmove.push_back(info);
            info = code; info += " "; info += param;
         }
         //if (param[strlen(param)-1]=='\n') param[strlen(param)-1] = '\0';
         if (code[0]=='-') {
            config.push(info);
            CkPrintf("%s\n", info.c_str());
         }
      }
      fclose(fp);
      CkPrintf("\n");
   }

   //---------------------------------------------------

   CkPrintf("[Main].Construtor\n");
   CkPrintf("\tnprocs: %d\n", CkNumPes());
   CkPrintf("\tnnds: %d\n", CkNumNodes());
   CkPrintf("\tnpp: %d\n", CkNumPes()/CkNumNodes());
   CkPrintf("\tnev: %lf\n", nev);

   srand(time(NULL));

   //---------------------------------------------------
   // setting the output directory
   //---------------------------------------------------

   time (&rawtime);
   CkPrintf("[Main].Construtor %s", ctime (&rawtime));

   if (!strlen(outfile)) {
      strcpy(outfile,"saida");
   }

   CkPrintf("\tOutput file basename: %s\n",outfile);

   if (!strlen(outdir)) {
      sprintf(outdir,"%s",ctime(&rawtime));
      outdir[strlen(outdir)-1] = '\0';
      struct stat st = {0};
      if (stat(outdir, &st) == -1) mkdir(outdir, 0755);
   } else {
      char *sptr, str1[64], str2[64];
      strcpy(str1,outdir);

      sptr = strtok(str1,"/\\");
      while (sptr != NULL) {
         sprintf(str2,"%s%s",str2,sptr); strcat(str2,"/");
         CkPrintf("\tCreating directory %s\n",str2);

         struct stat st = {0};
         if (stat(str2, &st) == -1) mkdir(str2, 0755);

         sptr = strtok (NULL, "/\\");
      }
      strcpy(outdir,str2);
   }

   CkPrintf("\tOutput directory: %s\n",outdir);
   sprintf(param,"%s/%s.config.txt",outdir,outfile);
   fp = fopen(param,"wt");
      for ( ; !config.empty(); config.pop() ) {
        fprintf(fp,"%s\n",config.front().c_str());
      }
   fclose(fp);

   //---------------------------------------------------
   // Loading performance file
   //---------------------------------------------------

   factor = new float[CkNumPes()];
   if (strlen(perform)) {
      fp = fopen(perform,"rt");
         if (!fp) {
            CkPrintf("Error opening file %s. Aborting!!!\n", perform);
            CkExit();
         }
         for (i=0;i<CkNumPes();i++)
            fscanf(fp,"%ld %f\n",&j,factor+i);
      fclose(fp);

      for (i=0;i<CkNumPes();i++)
         printf("%ld\t%f\n",i,factor[i]);
   } else {
      for (i=0;i<CkNumPes();i++) factor[i] = 1;
   }

   rate  = new float[CkNumPes()];
   irate = new float[CkNumPes()];
//   memset(irate,0,CkNumPes()*sizeof(float));
   for ( i=0; i<CkNumPes(); irate[i++]=1 );

   //---------------------------------------------------
   // Loading database
   //---------------------------------------------------

//   Atom atm;
//   fp = fopen("./database/dbase.dat","rt");
//   for (i=0; i<94; i++) {
//      fscanf(fp,"%s ",tmp);
//      atm.Read("./database",tmp);
//      sprintf(tmp,"./dbase/%s.txt",atm.X());
//      atm.PrintDB(tmp);
//   }
//   fclose(fp);
//   CkExit();

   Media media("./database",mediafile);

//   media.Print();

   //---------------------------------------------------
   // Setting source
   //---------------------------------------------------

   XRTube xrtb;

   if (!strlen(spectrum)) {
      // polychromatic source
      Atom anode  = media.Search(an)->DB(0); // W
      Atom window = media.Search(wn)->DB(0); // Be

      sprintf(param,"%s/db_anode.txt",outdir);
      anode.Print(param);
      sprintf(param,"%s/db_window.txt",outdir);
      window.Print(param);

      xrtb.Setup(anode, window, th/*cm*/, en/*keV*/, 0.01/*keV*/);
   } else {
      // monochromatic source
      xrtb.Setup(spectrum);
   }

   if (!type_spot) // circular
      xrtb.Spot(bg,rd,dv);
   else            // rectangular
      xrtb.Spot(bg,slx,sly,rd,dv);

   xrtb.P(XYZ(tpx,tpy,tpz));
   xrtb.D(XYZ(tdx,tdy,tdz));
//   xrtb.Rotate(15*deg,XYZ(0,1,0));

   if (!nch)   nch = xrtb.N();
   if (efs==0) efs = xrtb.HE();

   sprintf(param,"%s/xr_spectrum.txt",outdir);
   xrtb.Print(param);

   {
      FILE *fp;
      CrossSections cs;

//      //---------------------------------------------------
//      // Saving the silver halide cross-sections
//      //---------------------------------------------------

//      CkPrintf("Saving cross-sections file for %s\n",media.Name(21));
//      sprintf(param,"%s/cs-%s.txt",outdir,media.Name(21));
//      fp = fopen(param,"wt");
//      for ( i=0; i<xrtb.N(); i++ ) {
//         cs = media.ComputeCS(21,xrtb.E(i)); // 21 -> silver halide
//         fprintf(fp,"%04ld\t%7.3lf\t%e\n",i,xrtb.E(i),cs.mu);
//         //CkPrintf("%04ld\t%7.3lf\t%e\n",i,xrtb.E(i),cs.mu);
//      }
//      fclose(fp);
//      //CkPrintf("\n");

      //---------------------------------------------------
      // Saving the background cross-sections
      //---------------------------------------------------

      CkPrintf("Cross-sections for %s\n",media.Name(bg));
      sprintf(param,"%s/cs-%s.txt",outdir,media.Name(bg));
      fp = fopen(param,"wt");
      fprintf(fp,"   En photo elas inel mu\n");
      for ( i=0; i<xrtb.N(); i++ ) {
         cs = media.ComputeCS(bg,xrtb.E(i)); 
         fprintf(fp,"%04ld\t%7.3lf\t%e\t%e\t%e\t%e\n",i,xrtb.E(i),
                                          cs.photo, cs.elast, cs.inelast, cs.mu);
         //CkPrintf("%04ld\t%7.3lf\t%e\n",i,xrtb.E(i),cs.mu);
      }
      fclose(fp);
      //CkPrintf("\n");
   }

   //---------------------------------------------------
   // Loading models
   //---------------------------------------------------
   Models model;

   while (!body.empty()) {
      strcpy(solidname,body.front().c_str());
      body.pop();
      solidmat = atoi(body.front().c_str());
      body.pop();
      strcpy(solidfile,body.front().c_str());
      body.pop();
      solidface = atoi(body.front().c_str());
      body.pop();
      model.Add(solidname, bg, solidmat, solidfile, solidface);
      CkPrintf("Model %s added.\n",solidname);
   }

   CkPrintf("Moving models.\n");
   while (!move.empty()) {
      strcpy(code,move.front().c_str()); move.pop();
      if (!strcmp(code,"-mt")) {
         strcpy(solidname,move.front().c_str()); move.pop();
         mtdx = atof(move.front().c_str());      move.pop();
         mtdy = atof(move.front().c_str());      move.pop();
         mtdz = atof(move.front().c_str());      move.pop();
         CkPrintf("\tMoving model %s.\n",solidname);
         model[solidname].Translate(XYZ(mtdx,mtdy,mtdz));
         CkPrintf("\tModel %s moved.\n",solidname);
      } else if (!strcmp(code,"-mr") || !strcmp(code,"-mrd")) {
         strcpy(solidname,move.front().c_str());  move.pop();
         mrang = atof(move.front().c_str());      move.pop();
         mrang *= !strcmp(code,"-mrd")?deg:1.0;
         mrix  = atof(move.front().c_str());      move.pop();
         mriy  = atof(move.front().c_str());      move.pop();
         mriz  = atof(move.front().c_str());      move.pop();
         CkPrintf("\tTurning model %s.\n",solidname);
         model[solidname].Rotate(mrang, XYZ(mrix,mriy,mriz));
         CkPrintf("\tModel %s turned.\n",solidname);
      }
      CkPrintf("\n");
   }

   if (rec) {
      sprintf(param,"%s/lap",outdir);
      fp = fopen(param,"rt");
      if (!fp) {
         CkPrintf("File %s does not exit!\n",param);
         CkPrintf("Do not use '-r' option.\n");
         CkExit();
      }
      fscanf(fp,"%d",&lap);
      for (j=0; j<lap; j++) {
         i = 0;
      //   CkPrintf("itmove.size() %ld\n",itmove.size());
         while (i<itmove.size()) {
            strcpy(code,itmove[i++].c_str());
      //      CkPrintf("%d %s \n",i, code);
            if (!strcmp(code,"-mti")) {
               strcpy(solidname,itmove[i++].c_str());
      //            CkPrintf("%d %s \n", i, solidname);
               dx = atof(itmove[i++].c_str());
      //            CkPrintf("%d %lf \n", i, dx);
               dy = atof(itmove[i++].c_str());
      //            CkPrintf("%d %lf \n", i, dy);
               dz = atof(itmove[i++].c_str());
      //            CkPrintf("%d %lf \n", i, dz);
               model[solidname].Translate(XYZ(dx,dy,dz));
            } else if (!strcmp(code,"-mri") || !strcmp(code,"-mrdi")) {
               strcpy(solidname,itmove[i++].c_str());
      //            CkPrintf("%d %s \n", i, solidname);
               ang = atof(itmove[i++].c_str())*(strcmp(code,"-mrdi")?1.0:deg);
      //            CkPrintf("%lf \n",ang);
               dx = atof(itmove[i++].c_str());
      //            CkPrintf("%d %lf \n", i, dx);
               dy = atof(itmove[i++].c_str());
      //            CkPrintf("%d %lf \n", i, dy);
               dz = atof(itmove[i++].c_str());
      //            CkPrintf("%d %lf \n", i, dz);
               model[solidname].Rotate(ang, XYZ(dx,dy,dz));
            }
         }
      }
      fclose(fp);
   }

   CkPrintf("Saving models.\n");
   sprintf(param,"%s/md_solid.txt",outdir);
   model.Print(param);

   modelo = model;

   //---------------------------------------------------
   // node allocation
   //---------------------------------------------------
//   nkeys = 16*CkNumPes();
//   init_key = new unsigned long [nkeys];
//   for( i=0; i<nkeys; i++ ) init_key[i] = rand();

   // instancia os nós
//   Proxy_Node = CProxy_Node::ckNew(model);
//   Proxy_Node = CProxy_Node::ckNew(nkeys, init_key, media, model);
//   Proxy_Node = CProxy_Node::ckNew(nkeys, init_key, xrtb, model);

   // constructor sending geometric modeling
   Proxy_Node = CProxy_Node::ckNew(modelo);
      opts.setGroupDepID(Proxy_Node);

//   delete [] init_key;

   //---------------------------------------------------
   // detector allocation
   //---------------------------------------------------
   nkeys = 16*CkNumPes();
   init_key = new unsigned long [nkeys];
   for( i=0; i<nkeys; i++ ) init_key[i] = rand();

   switch (type_detector) {
      case 0:
         Proxy_GDetector = CProxy_GDetector::ckNew(nkeys, init_key, 
                           co, ln, ds, ga/*cm2*/, gt/*cm*/, lx, ly, //cm,cm
                           strlen(outdir)+1, outdir, strlen(outfile)+1, outfile, media, model, &opts);
         break;
      case 1:
         Proxy_GDetector = CProxy_GDetector::ckNew(nch,efs,strlen(outdir)+1,outdir,
                           strlen(outfile)+1, outfile, model, &opts);
         break;
      case 2:
         Proxy_GDetector = CProxy_GDetector::ckNew( co, ln, strlen(outdir)+1, outdir,
                           strlen(outfile)+1, outfile, &opts);
         break;
      default:
         CkPrintf("Type of detector (%d) not recognized. Aborting!!!\n",type_detector);
         CkExit();
   }
   // constructor sending detector parameters
   opts.setGroupDepID(Proxy_GDetector);

   delete [] init_key;

   //---------------------------------------------------
   // core allocation
   //---------------------------------------------------
   // instancia os simuladores
   nkeys = 16*CkNumPes();
   init_key = new unsigned long [nkeys];
   for( i=0; i<nkeys; i++ ) init_key[i] = rand();

//   Proxy_Simulation = CProxy_Simulation::ckNew(nev, &opts);
//   Proxy_Simulation = CProxy_Simulation::ckNew(nkeys, init_key, nev, xrtb, media, &opts);
//   Proxy_Simulation = CProxy_Simulation::ckNew(nkeys, init_key, nev, media, &opts);

   t1 = CkWallTimer();

   // constructor sending geometric modeling
   Proxy_Simulation = CProxy_Simulation::ckNew(nkeys, init_key, nev, CkNumPes(), factor, xrtb, media, &opts);

   delete [] init_key;

}
//---------------------------------------------------

void Main::EnlapsedTime(int pe, float tm) {
   FILE  *fp;
   char   str[256];
   int    i;
   double sum = 0, cte;

   if (pe<CkNumPes()) {
      rate[pe] = factor[pe]/tm;
      --count;
      if (!count) {
         for ( i=0; i<CkNumPes(); sum+=rate[i], i++);
         cte = CkNumPes()/sum;
         for ( i=0; i<CkNumPes(); rate[i]*=cte, i++ );

//         for ( i=0; i<CkNumPes(); irate[i]=(irate[i]*(lap+1)+rate[i])/(lap+2),i++ );
//         for ( sum=i=0; i<CkNumPes(); sum+=irate[i],i++);
//         cte = CkNumPes()/sum;
//         for ( i=0; i<CkNumPes(); irate[i]*=cte,i++ );

         for ( i=0; i<CkNumPes(); factor[i]=rate[i],i++ );

         sprintf(str,"./%s/%s.perform-%03d.%03d.txt",outdir,outfile,CkNumPes(),lap+1);
         CkPrintf("Saving %s\n",str);
         fp = fopen(str,"wt");
            for ( i=0; i<CkNumPes(); i++ ) {
               fprintf(fp,"%03d\t%f\n",i,rate[i]);
//               fprintf(fp,"%03d\t%f\n",i,factor[i]);
            }
         fclose(fp);

         count = CkNumPes();
      }
   } else {
      CkAbort("pe>=CkNumPes()!!\n");
   }
//   delete rate;
}

//===================================================
// implementação do método de redução
//===================================================
void Main::Reduction2(void) {
   FILE *fp;
   char param[256];

   sprintf(param,"%s/lap",outdir);
   fp = fopen(param,"wt");
      fprintf(fp,"%d",lap);
   fclose(fp);

   CkPrintf("=============================\n"
            " Starting a new lap (%d)\n"
            "=============================\n\n",lap+1);
   t2 = CkWallTimer();
   Proxy_Simulation.Run(lap+1);
}
//===================================================
// implementação do método de redução
//===================================================
void Main::Reduction(void) {
   FILE  *fp;

   int    horas, minutos, segundos;
   double tempo;
   time_t rawtime;

   char   code[16], solidname[64], str[256];
   int    i;
   double ang, dx, dy, dz;

   sprintf(str,"%s/%s.config.txt",outdir,outfile);
   fp = fopen(str,"at");

      time (&rawtime);
      CkPrintf("[Main].Reduction %s", ctime (&rawtime));
      fprintf(fp,"[Main].Reduction %s", ctime (&rawtime));

      t2 = CkWallTimer() - t2;
      tempo = t2;          minutos = (int)(tempo/60);
      tempo -= 60*minutos; segundos = (int)(tempo);
      CkPrintf("[Main].Reduction    %02d:%02d (%.2lf s)\n", minutos, segundos, t2);
      fprintf(fp,"[Main].Reduction    %02d:%02d (%.2lf s)\n", minutos, segundos, t2);

      t2 = CkWallTimer() - t1;
      tempo = t2;          horas = (int)(t2/3600);
      tempo -= 3600*horas; minutos = (int)(tempo/60);
      tempo -= 60*minutos; segundos = (int)(tempo);
      CkPrintf("[Main].Reduction %02d:%02d:%02d (%.2lf s)\n", horas, minutos, segundos, t2);
      fprintf(fp,"[Main].Reduction %02d:%02d:%02d (%.2lf s)\n", horas, minutos, segundos, t2);

      CkPrintf("\n");
      CkPrintf("===============================\n");
      CkPrintf("End of lap %d from %d\n",lap+1,iter);
      CkPrintf("===============================\n\n");

      fprintf(fp,"\n");
      fprintf(fp,"===============================\n");
      fprintf(fp,"End of lap %d from %d\n",lap+1,iter);
      fprintf(fp,"===============================\n\n");

   fclose(fp);

   if (lap+1<iter) {
      i = 0;
   //   CkPrintf("itmove.size() %ld\n",itmove.size());
      while (i<itmove.size()) {
         strcpy(code,itmove[i++].c_str());
   //      CkPrintf("%d %s \n",i, code);
         if (!strcmp(code,"-mti")) {
            strcpy(solidname,itmove[i++].c_str());
   //            CkPrintf("%d %s \n", i, solidname);
            dx = atof(itmove[i++].c_str());
   //            CkPrintf("%d %lf \n", i, dx);
            dy = atof(itmove[i++].c_str());
   //            CkPrintf("%d %lf \n", i, dy);
            dz = atof(itmove[i++].c_str());
   //            CkPrintf("%d %lf \n", i, dz);
            modelo[solidname].Translate(XYZ(dx,dy,dz));
         } else if (!strcmp(code,"-mri") || !strcmp(code,"-mrdi")) {
            strcpy(solidname,itmove[i++].c_str());
   //            CkPrintf("%d %s \n", i, solidname);
            ang = atof(itmove[i++].c_str())*(strcmp(code,"-mrdi")?1.0:deg);
   //            CkPrintf("%lf \n",ang);
            dx = atof(itmove[i++].c_str());
   //            CkPrintf("%d %lf \n", i, dx);
            dy = atof(itmove[i++].c_str());
   //            CkPrintf("%d %lf \n", i, dy);
            dz = atof(itmove[i++].c_str());
   //            CkPrintf("%d %lf \n", i, dz);
            modelo[solidname].Rotate(ang, XYZ(dx,dy,dz));
         }
      }
      Proxy_Node.Model(modelo);
      Proxy_GDetector.Model(modelo);
      Proxy_Simulation.Factor(nev,CkNumPes(),factor);
      CkPrintf("\n");
      
      ++lap;
      Reduction2();
      
   } else {
//      sprintf(str,"./%s/%s.perform-%03d.mean.txt",outdir,outfile,CkNumPes());
//      fp = fopen(str,"wt");
//         for ( i=0; i<CkNumPes(); i++ ) {
//            fprintf(fp,"%03d\t%f\n",i,irate[i]);
////            CkPrintf("%03d\t%f\n",i,irate[i]);
//         }
//      fclose(fp);

      sprintf(str,"%s/lap",outdir);
      fp = fopen(str,"wt");
         fprintf(fp,"%d",lap);
      fclose(fp);

      CkExit();
   }
}

//===================================================

#include "tupi.def.h" // gerado pela compilação de tupi.ci
