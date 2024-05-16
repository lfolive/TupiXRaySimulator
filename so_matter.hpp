#ifndef MATTER_HPP
#define MATTER_HPP

#include "cMT19937ar.hpp"

//---------------------------------------------------
// definicao da classe Foton
//---------------------------------------------------
class Foton {
private:
   int    id;
   double e;
   XYZ    p, d;

public:
   Foton(void) { e = 0; }

   void E(double val) { e = val;  }
   void P(XYZ val)    { p = val;  }
   void D(XYZ val)    { d = val;  }
   void ID(int val)   { id = val; }

   double E(void) { return e;  }
   XYZ    P(void) { return p;  }
   XYZ    D(void) { return d;  }
   int   ID(void) { return id; }

   void Translate(double dist) {
      p += (dist*d);
   }

   void Translate(XYZ step) {
      p += step;
   }

   void Translate(double dist, XYZ dir) {
      p += (dist*dir);
   }

   void Print(void) {
      printf("E : %lf\n", e);
      printf("P : "); p.Print();
      printf("D : "); d.Print();
      printf("ID: %d\n", id);
   }

   void Print(const char *str) {
      printf("%sE : %lf\n", str, e);
      printf("%sP : ", str); p.Print();
      printf("%sD : ", str); d.Print();
      printf("%sID: %d\n", str, id);
   }
};
//---------------------------------------------------

//---------------------------------------------------
//
//---------------------------------------------------
class  RadRate {
public:
   char   target[10];   //  10 B
   char   source[10];   //  10 B
//   int    z;            //   4 B
   double rate, acumm;  // +16 B
   double xray;         //   8 B
   //                    --------
   //                       44 B
   RadRate(void) {
//      z = 0;
      rate = 0; acumm = 0; xray = 0;
      strcpy(target,""); strcpy(source,"");
   }

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &p) {
//      p|z; 
      p|rate; p|acumm; p|xray;
      PUParray(p,target,10);
      PUParray(p,source,10);
   }
};

//---------------------------------------------------
//
//---------------------------------------------------
class Sublevel {
public:
   char    simb[3];   //       3 B; Nome do subnível
   double  Eb, rs, w, //      24 B; energia de ligacao e razao de salto
           coef[6];   //      48 B; coeficientes de absorcao fotoeletrica
   short   nr;
   RadRate rr[20];    // + 20*44 B
   //                   --------
   //                            B

   Sublevel(void) {
      nr = 0; Eb = 0; w = 0; rs = 0;
      strcpy(simb,"");
   }

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &p) {
      p|Eb; p|rs; p|w; p|nr;
      PUParray(p,simb,3);
      PUParray(p,coef,6);
      PUParray(p,rr,20);
   }
};

//---------------------------------------------------
//
//---------------------------------------------------
class Atom {
private:
   char      id;                   //        1 B indice do meio
   char      simb[3];              //        3 B simbolo quimico
   int       z;                    //        4 B numero atomico
   double    massa;                //        8 B massa atomica e densidade atomica
   double    coer[6], incoer[6];   //     12*8 B coefs esp elastico e inelastico

   double    den, fracao;          //       16 B densidade relativa e fracao de massa
   char      multi;                //        1 B multiplicidade elementar

   int       ns;                   //        4 B total de subniveis de energia
   Sublevel  sn[30];               // + 30*815 B informacoes dos subniveis de energia
                                   // ---------
                                   //    24583 B / 1024 B ~ 24 kB

public:
   Atom(void) {
      strcpy(simb,"");
      z = ns = multi = 0;
      massa = den = fracao = 0;
      id = 0;
   };

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &p) {
      p|id; p|z; p|massa;
      p|ns; p|den; p|fracao;
      p|multi;
      PUParray(p,simb,3);
      PUParray(p,coer,6);
      PUParray(p,incoer,6);
      PUParray(p,sn,30);
   }

   // atribui valores
   void D(double val)  { den = val;         } // densidade relativa atribuída ao elemento
   void FM(double val) { fracao = val;      } // fração de massa
   void M(int val)     { multi = (char)val; } // multiplicidade atômica

   void W(int i, double val) { sn[i].w = val; } // fator de prod. fluorescência

   void SOURCE(int i, int j, char *str) { strcpy(sn[i].rr[j].source,str); }
   void TARGET(int i, int j, char *str) { strcpy(sn[i].rr[j].target,str); }
   void RATE(int i, int j, double val)  { sn[i].rr[j].rate = val;         }
   void ACUMM(int i, int j, double val) { sn[i].rr[j].acumm = val;        }

   // retorna valores
   char   ID(void) { return id;    } // identificador do elemento
   char*  X(void)  { return simb;  } // símbolo químico
   int    Z(void)  { return z;     } // número atômico
   double A(void)  { return massa; } // massa atômica
   int    NS(void) { return ns;    } // total de subníveis de energia

   double D(void)  { return den;    } // densidade relativa atribuída ao elemento
   double FM(void) { return fracao; } // fração de massa
   int    M(void)  { return multi;  } // multiplicidade atômica

   char*  Simb(int i) { return sn[i].simb; } // símbolo do subnível de energia
   double Eb(int i)   { return sn[i].Eb;   } // energia de ligação associado ao subnível
   double W(int i)    { return sn[i].w;    } // fator de prod. fluorescência

   int    NR(int i)            { return sn[i].nr;           }
   char*  SOURCE(int i, int j) { return sn[i].rr[j].source; }
   char*  TARGET(int i, int j) { return sn[i].rr[j].target; }
   double RATE(int i, int j)   { return sn[i].rr[j].rate;   }
   double ACUMM(int i, int j)  { return sn[i].rr[j].acumm;  }
   double XRAY(int i, int j)   { return sn[i].rr[j].xray;   }

   void ReadRadiativeRate(const char *dir) {
      FILE    *fp;
      RadRate  tmp;
      char     arquivo[128],str[10];
      int      i, j, k, l;
      int      idx;
      double   sum;

      int      ln;
      RadRate *rr = NULL;

      for ( i=0; i<ns; i++ ) sn[i].nr = 0;

      // read the correspondent fluorescence yield 
      sprintf(arquivo,"%s/radrate.dat", dir);
      if (!(fp = fopen(arquivo,"rt"))) {
          CkPrintf("[Atom.ReadRadiativeRate] Error opening the file %s!!!\n", arquivo);
          CkExit();
      }

      ln = 0;
      while (!feof(fp)) {
         fscanf(fp,"%d %s %lf\n", &idx, str, &sum);
         if (z==idx) ln++;
      }

      printf("\t\t\t[Atom.ReadRadiativeRate] Lines: %d\n",ln);

      rr = new RadRate [ln];

      fseek(fp,0,SEEK_SET);
      i = 0;
      while (!feof(fp)) {
         fscanf(fp,"%d %s %lf\n", &idx, str, &sum);
         if (z==idx) {
//            rr[i].z = (char)z;
            strcpy(rr[i].target,str);
            strcpy(rr[i].source,str);
            rr[i++].rate = sum;
         }
      }
      fclose(fp);

      // parsing the target and source
      for ( i=0; i<ln; i++ ) {
         j = isalpha(rr[i].target[1]) ? 1 : 2;
         strcpy(rr[i].source,rr[i].target+j);
         rr[i].target[j] = '\0';
      }

//      for ( i=0; i<ln; i++ ) {
//         CkPrintf("%s %s %lf\n", rr[i].target, rr[i].source, rr[i].rate);
//      }
//      CkPrintf("^^^^\n");

      // making the acummulated probabilities
      for ( i=j=k=0; i<ln; i=j ) {
         for ( j=i+1; (j<ln)?!strcmp(rr[i].target,rr[j].target):0; j++ );
         sn[k++].nr = j-i;
         for ( sum=0, l=i; l<j; l++ ) rr[l].acumm = (sum += rr[l].rate);
         for ( l=i; l<j; l++ ) rr[l].acumm /= sum;

//         for ( l=i; l<j; l++ ) {
//            CkPrintf("%d %s %s %lf %lf\n",
//               sn[k-1].nr, rr[l].target, rr[l].source, rr[l].rate, rr[l].acumm);
//         }
//         CkPrintf("\n");
      }
//      CkPrintf("====\n");

      // for each energy level
      for ( i=j=0; i<ns && k<ln; i++ ) {
         // for each sublevel rate rated
         for ( k=0; k<sn[i].nr; k++ ) {
            // copy the rate data
            sn[i].rr[k] = rr[j+k];
            // search the energy of the source sublevel
            for ( l=i+1; l<ns; l++ ) {
               // if source name is 1-letter length
               if (strlen(rr[j+k].source)==1) strcat(rr[j+k].source,"1");
               // if source name are 3-letter length
               rr[j+k].source[2] = '\0';

//               CkPrintf("\t%s\t%s\n",rr[j+k].source,sn[l].simb);

               if (!strcmp(rr[j+k].source,sn[l].simb)) break;
            }
            if (l==ns) {
               CkPrintf("\t\t\t[Atom.ReadRadiativeRate] "
                        "Without correspondent sublevel: [%d:%s] %s %s.\n",
                        z, simb, sn[i].rr[k].target, rr[j+k].source);
               --l;
            }
//            if (sn[i].Eb < sn[l].Eb) {
//               CkPrintf("\t\t\t[Atom.ReadRadiativeRate] "
//                        "Problem with binding energy: [%s] %s: %lf -- %s:%lf.\n",
//                        simb, sn[i].rr[k].target, sn[i].Eb, rr[j+k].source, sn[l].Eb);
//               --l;
//            }
            sn[i].rr[k].xray = sn[i].Eb - sn[l].Eb;

//            CkPrintf("\t%lf\n",sn[l].Eb);

         }

         // update the rate index list
         j += sn[i].nr;
      }

      if (rr) delete[] rr;

   }

   // le arquivo de database
   // dir: diretorio de database
   // idx: indice da substancia
   // str: arquivo .MU
   void Read(const char *dir, const char *str) {
      FILE  *fp;
      char   arquivo[128], nome[10];
      char   lixo[128];
      int    i, j, k;
      double val;

      // converte símbolo químico para letras maiúsculas
      strcpy(nome,str);
      for ( i=0; i<strlen(nome); i++ )
         nome[i] = toupper(nome[i]);
      sprintf(arquivo,"%s/%s.MU", dir, nome);

      // abre arquivo do elemento químico
      fp = fopen(arquivo,"rt");
      if (!fp) {
         CkPrintf("[Atom.Read] Error opening the file %s!!!\n", arquivo);
         CkExit();
      }

      // lê símbolo químico e descarta o resto
      fscanf(fp,"%s %s %s %s\n", simb, lixo, lixo, lixo);
      // lê número atômico, massa atômica e densidade do elemento químico
      fscanf(fp,"%d %lf %lf\n", &z, &massa, &den);
      // descarta linha do arquivo
      fgets(lixo,127,fp);
      // lê coeficientes de espalhamento elástico e inelástico
      for ( i=0; i<6; i++ ) fscanf(fp,"%lf ", &coer[i]);
      for ( i=0; i<6; i++ ) fscanf(fp,"%lf ", &incoer[i]);
      // descarta linha
      fgets(lixo,127,fp);
      //CkPrintf("%s\n",lixo);

      j = ftell(fp);  // memoriza posiçao no arquivo
      ns = 0;         // inicializa contador de subníveis
      while (1) {     // repetir
         fscanf(fp,"%s ",lixo);    // lê linha
         if (lixo[0]=='g') break;  // se primeira letra for 'g' (global), interromper
         /*else*/
         if (lixo[0]>='K' && lixo[0]<='Q') ns++; // se primeira letra está entre 'K' e 'Q', contar
         fgets(lixo,127,fp);  // descartar linha
         fgets(lixo,127,fp);  // descartar linha
      }

      // volta para a posição memorizada
      // já se sabe o total de subníveis...
      fseek(fp,j,SEEK_SET);
      for ( i=0; i<ns; i++ ) {          // para cada subnível
         fscanf(fp,"%s ", sn[i].simb); // ler símbolo do subnível
         fscanf(fp,"%lf %lf\n", &(sn[i].Eb), &(sn[i].rs)); // ler energia de ligação e razão de salto
         for ( j=0; j<6; j++ )         // ler coeficientes de absorção fotoelétrica
            fscanf(fp,"%lf ", &(sn[i].coef[j]));
         sn[i].w = 0;
      }
      fclose(fp);

      // read the correspondent fluorescence yield 
      sprintf(arquivo,"%s/fluor_yield.dat", dir);
      fp = fopen(arquivo,"rt");
      if (!fp) {
         CkPrintf("[Atom.Read] Error opening the file %s!!!\n", arquivo);
         CkExit();
      }

      for ( i=j=k=0;;) {
         if (i<z) j=ftell(fp);
         if (fscanf(fp,"%d %s %lf",&i,nome,&val)!=3) break;
         if (i==z) k++;

//         printf(">>> %d %d %d %s %lf\n", j, k, i, nome, val);

         if (i>z) break;
      }
      fseek(fp,j,SEEK_SET);
      for ( i=0; i<k; i++ ) {
         fscanf(fp,"%d %s %lf",&j,nome,&val);
         for ( j=0; j<ns; j++ ) {
            if (!strcmp(sn[j].simb,nome)) {
               sn[j].w = val;
               break;
            }
         }
      }
      fclose(fp);

      ReadRadiativeRate(dir);
   }

   // calcula a secao de choque de absorcao fotoeletrica
   // para o subnivel quantico snivel[idx].simb em cm2/g
   // idx: indice do subnivel de energia
   // hv: energia do foton
   double SC_Abs(int idx, double hv) {
      int j;
      double mt, lnhv, tau;

      tau = 0;
      if ( hv >= sn[idx].Eb ) {
         mt = 0;
         lnhv = 1;
         for ( j=0; j<6; j++ ) {
            mt += sn[idx].coef[j]*lnhv;
            lnhv *= log(hv);
         }
         tau = exp(mt);
      }

      return tau;
   }

   // calcula a secao de choque total para
   // absorcao fotoeletrica atomica em cm2/g
   // hv: energia do foton
   double SC_Abs(double hv) {
      int k;
      double tau;

      tau = 0;
      for ( k=0; k<ns; k++ )
         tau += SC_Abs(k,hv);

      return tau;
   }

   // calcula a secao de choque para
   // espalhamento elastico em cm2/g
   // hv: energia do foton
   double SC_Elast(double hv) {
      int j;
      double sigma, mt, lnhv;

      mt = 0;
      lnhv = 1;
      for ( j=0; j<6; j++ ) {
         mt += coer[j]*lnhv;
         lnhv *= log(hv);
      }
      sigma = exp(mt);

      return sigma;
   }

   // calcula a secao de choque para
   // espalhamento inelastico em cm2/g
   // hv: energia do foton
   double SC_Inelast(double hv) {
      int j;
      double sigma, mt, lnhv;

      mt = 0;
      lnhv = 1;
      for ( j=0; j<6; j++ ) {
         mt += incoer[j]*lnhv;
         lnhv *= log(hv);
      }
      sigma = exp(mt);

      return sigma;
   }

   void Print(void) {
      int i, j;

      CkPrintf("+---------+----------+\n");
      CkPrintf("| ID: %3d | Simb: %2s |\n", id, simb);
      CkPrintf("+---------+----------+\n");
      CkPrintf("+--------+----------------------------------------------------+\n");
      CkPrintf("| Z: %3d | A: %6.2lf g/mol | Den rel: %5.2lf g/cm3 | FM: %5.3lf |\n",
                  z, massa, den, fracao);
      CkPrintf("+--------+----------------------------------------------------+\n");
      CkPrintf("| coer   |\n");
      CkPrintf("+--------+----------------------------------------------------+\n");
      CkPrintf("| %9.2e %9.2e %9.2e %9.2e %9.2e %9.2e |\n",
                  coer[0],coer[1],coer[2],coer[3],coer[4],coer[5]);
      CkPrintf("+--------+----------------------------------------------------+\n");
      CkPrintf("| incoer |\n");
      CkPrintf("+--------+----------------------------------------------------+\n");
      CkPrintf("| %9.2e %9.2e %9.2e %9.2e %9.2e %9.2e |\n",
                  incoer[0],incoer[1],incoer[2],incoer[3],incoer[4],incoer[5]);
      CkPrintf("+-------------------------------------------------------------+\n");
      for ( i=0; i<ns; i++ ) {
         CkPrintf("+-------------+-----------------------------------------------+\n");
         CkPrintf("| SN[%02d]: %2s  ", i, sn[i].simb);
         CkPrintf("| Eb: %8.5lf keV | rs: %5.2lf  | w: %9.3e  |\n", 
                     sn[i].Eb, sn[i].rs, sn[i].w);
         CkPrintf("+-------------+-----------------------------------------------+\n");
         CkPrintf("| %9.2e %9.2e %9.2e %9.2e %9.2e %9.2e |\n",
                     sn[i].coef[0],sn[i].coef[1],sn[i].coef[2],
                     sn[i].coef[3],sn[i].coef[4],sn[i].coef[5]);
         CkPrintf("+-------------+-----------------------------------------------+\n");
         for ( j=0; j<sn[i].nr; j++ ) {
            CkPrintf("| %3s%-3s | %9.2e %9.2e | %8.3lf |\n",
                     sn[i].rr[j].target, sn[i].rr[j].source,
                     sn[i].rr[j].rate, sn[i].rr[j].acumm, sn[i].rr[j].xray);
         }
      }
      CkPrintf("+-------------------------------------------------------------+\n");
      CkPrintf("\n\n");
   }

   void Print(const char *filename) {
      FILE *fp;
      int i, j;

      fp = fopen(filename,"wt");
         fprintf(fp,"+---------+----------+\n");
         fprintf(fp,"| ID: %3d | Simb: %2s |\n", id, simb);
         fprintf(fp,"+---------+----------+\n");
         fprintf(fp,"+--------+----------------------------------------------------+\n");
         fprintf(fp,"| Z: %3d | A: %6.2lf g/mol | Den rel: %5.2lf g/cm3 | FM: %5.3lf |\n",
                     z, massa, den, fracao);
         fprintf(fp,"+--------+----------------------------------------------------+\n");
         fprintf(fp,"| coer   |\n");
         fprintf(fp,"+--------+----------------------------------------------------+\n");
         fprintf(fp,"| %9.2e %9.2e %9.2e %9.2e %9.2e %9.2e |\n",
                     coer[0],coer[1],coer[2],coer[3],coer[4],coer[5]);
         fprintf(fp,"+--------+----------------------------------------------------+\n");
         fprintf(fp,"| incoer |\n");
         fprintf(fp,"+--------+----------------------------------------------------+\n");
         fprintf(fp,"| %9.2e %9.2e %9.2e %9.2e %9.2e %9.2e |\n",
                     incoer[0],incoer[1],incoer[2],incoer[3],incoer[4],incoer[5]);
         fprintf(fp,"+-------------------------------------------------------------+\n");
         for ( i=0; i<ns; i++ ) {
            fprintf(fp,"+-------------+-----------------------------------------------+\n");
            fprintf(fp,"| SN[%02d]: %2s  ", i, sn[i].simb);
            fprintf(fp,"| Eb: %8.5lf keV | rs: %5.2lf  | w: %9.3e  |\n", 
                        sn[i].Eb, sn[i].rs, sn[i].w);
            fprintf(fp,"+-------------+-----------------------------------------------+\n");
            fprintf(fp,"| %9.2e %9.2e %9.2e %9.2e %9.2e %9.2e |\n",
                        sn[i].coef[0],sn[i].coef[1],sn[i].coef[2],
                        sn[i].coef[3],sn[i].coef[4],sn[i].coef[5]);
            fprintf(fp,"+-------------+-----------------------------------------------+\n");
            for ( j=0; j<sn[i].nr; j++ ) {
               fprintf(fp,"| %3s%-3s | %9.2e %9.2e | %8.3lf |\n",
                        sn[i].rr[j].target, sn[i].rr[j].source,
                        sn[i].rr[j].rate, sn[i].rr[j].acumm, sn[i].rr[j].xray);
            }
         }
         fprintf(fp,"+-------------------------------------------------------------+\n");
      fclose(fp);
   }

   void PrintDB(const char *filename) {
      FILE *fp;
      char tmp[16];
      int i, j;

      fp = fopen(filename,"wt");
         fprintf(fp,"%2s\n", simb);
         fprintf(fp,"# Z   A      rho\n");
         fprintf(fp,"  %-3d %-6.2lf %-.3e\n", z, massa, den);
         fprintf(fp,"# coefficients for coherent scattering cross-section\n");
         fprintf(fp,"  %-+9.4e %-+9.4e %-+9.4e %-+9.4e %-+9.4e %-+9.4e\n",
                     coer[0],coer[1],coer[2],coer[3],coer[4],coer[5]);
         fprintf(fp,"# coefficients for incoherent scattering cross-section\n");
         fprintf(fp,"  %-+9.4e %-+9.4e %-+9.4e %-+9.4e %-+9.4e %-+9.4e\n",
                     incoer[0],incoer[1],incoer[2],incoer[3],incoer[4],incoer[5]);
         fprintf(fp,"# \n");
         for ( i=0; i<ns; i++ ) {
            fprintf(fp,"%-2s\n", sn[i].simb);
            fprintf(fp,"# Eb         rs    w\n");
            fprintf(fp,"  %-+10.5lf %-+5.2lf %-+9.4e\n", 
                        sn[i].Eb, sn[i].rs, sn[i].w);
            fprintf(fp,"# coefficients for absorption cross-section\n");
            fprintf(fp,"  %-+9.4e %-+9.4e %-+9.4e %-+9.4e %-+9.4e %-+9.4e\n",
                        sn[i].coef[0],sn[i].coef[1],sn[i].coef[2],
                        sn[i].coef[3],sn[i].coef[4],sn[i].coef[5]);
            fprintf(fp,"# \n");
            fprintf(fp,"# xray   rate        irate       hv\n");
            for ( j=0; j<sn[i].nr; j++ ) {
               sprintf(tmp,"%s%s",sn[i].rr[j].target, sn[i].rr[j].source);
               fprintf(fp,"  %-6s %-+9.4e %-+9.4e %-+8.3lf\n",
                        tmp, sn[i].rr[j].rate, sn[i].rr[j].acumm, sn[i].rr[j].xray);
            }
            fprintf(fp,"*\n");
         }
      fclose(fp);
   }

};

//---------------------------------------------------
//
//---------------------------------------------------
class Substance {
private:
   Atom  *db;
   int    na;
   double density;
   double weight;

public:
   Substance(void) { db = NULL; na = 0; density = weight = 0; }
  ~Substance(void) { delete [] db; db=NULL; na = 0; }

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &p) {
      p|na; 
      p|density; 
      p|weight;
      if (p.isUnpacking()) {
         db = new Atom [na];
      }
      PUParray(p,db,na);
   }

   int     NA(void)    { return na;      }
   Atom   &DB(int idx) { return db[idx]; }

   double  D(void)  { return density; }
   double  WT(void) { return weight;  }

   void D(double val)  { density = val; }
   void WT(double val) { weight = val;  }

   void Parse(const char *dir, double pp, const char *info, double dd) {
      char   subs[64], buff[5];
      int    i, j, n;
      double mt, fm;

      density = dd;
      weight = pp;

      strcpy(subs,info);  na = 0;
      for ( i=0; i<strlen(subs); i++ ) {
         if (isupper(subs[i])) na++; // if the character is upper
      }
      db = (Atom*) new Atom[na]; // allocate memory to atoms

      n = i = 0; /* inicio da formula */
      while (1) {
         /* se no inicio de um simbolo quimico */
         if (isupper(subs[i])) {
            buff[0] = subs[i];
            i++;
            /* e o proximo eh outro simbolo quimico */
            if (isupper(subs[i])) {
               buff[1] = '\0';              /* encerra captura */
               db[n].M(1);                 /* multiplicidade unitaria */
               db[n].Read(dir,buff); /* carrega simbolo quimico */
               n++;
               /* e o proximo eh o final da formula */
            } else if (subs[i] == '\0') {
               buff[1] = '\0';              /* encerra captura */
               db[n].M(1);                 /* multiplicidade unitaria */
               db[n].Read(dir,buff); /* carrega simbolo quimico */
               n++;
               break;
               /* e o proximo eh numero */
            } else if (isdigit(subs[i])) {
               buff[1] = '\0';              /* encerra captura */
               db[n].Read(dir,buff); /* carrega simbolo quimico */
               j = 0;
            }
         /* se na segunda letra do simbolo quimico */
         } else if (islower(subs[i])) {
            buff[1] = subs[i];
            i++;
            /* e o proximo eh outro simbolo quimico */
            if (isupper(subs[i])) {
               buff[2] = '\0';             /* encerra captura */
               db[n].M(1);                /* multiplicidade unitaria */
               db[n].Read(dir,buff); /* carrega simbolo quimico */
               n++;
               /* e o proximo eh o final da formula */
            } else if (subs[i] == '\0') {
               buff[2] = '\0';             /* encerra captura */
               db[n].M(1);                /* multiplicidade unitaria */
               db[n].Read(dir,buff); /* carrega simbolo quimico */
               n++;
               break;
               /* e o proximo eh numero */
            } else if (isdigit(subs[i])) {
               buff[2] = '\0';             /* encerra captura */
               db[n].Read(dir,buff); /* carrega simbolo quimico */
               j = 0;
            }
            /* se no inicio da multiplicidade */
         } else if (isdigit(subs[i])) {
            buff[j] = subs[i];
            i++; j++;
            /* e o proximo eh o inicio de outro simbolo quimico */
            if (isupper(subs[i])) {
               buff[j] = '\0';      /* encerra captura */
               db[n].M(atoi(buff)); /* multiplicidade unitaria */
               n++;
               /* e o proximo eh o final da formula */
            } else if (subs[i] == '\0') {
               buff[j] = '\0';      /* encerra captura */
               db[n].M(atoi(buff)); /* multiplicidade unitaria */
               n++;
               break;
            }
         }
      }

      // compute mass fraction
      mt = 0;
      for ( i=0; i<na; i++ )          // para cada elemento i
         mt += db[i].M()*db[i].A();   //    multiplicidade vezes massa atômica
      for ( i=0; i<na; i++ ) {        // para cada elemento i
         fm = db[i].M()*db[i].A()/mt; //    fracao de massa calculada    FM_calc
                                      //        levando em consideração a multiplicidade
         db[i].FM(fm);                //    atualiza fração de massa
         db[i].D(fm*density);         //    densidade atomica relativa calculada  D_calc
      }

   }

   void Print(void) {
      int i;
      CkPrintf("weight: %lf\tdensity: %lf\n",weight,density);
      for ( i=0; i<na; i++ ) 
         db[i].Print();
      CkPrintf("\n");
   }
};

//---------------------------------------------------
//
//---------------------------------------------------
typedef struct {
   double photo, elast, inelast;
   int    mec;
   double mu;
   double step;
} CrossSections;

//---------------------------------------------------
//
//---------------------------------------------------
class Compound {
private:
   Substance *substance;
   Atom      *db;
   double     density;
   int        id;
   int        nm, na;
   char       name[64];

public:
   Compound(void) { substance = NULL; db = NULL; nm = na = id = 0; density = 0; strcpy(name,"---"); }

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &p) {
      p|na; p|id;
      p|density;
//      p|nm;
      if (p.isUnpacking()) {
//         substance = new Substance [nm];
         db = new Atom [na];
      }
//      PUParray(p,substance,nm);
      PUParray(p,db,na);
      PUParray(p,name,64);
   }

   int     ID(void)    { return id;      }
   int     NA(void)    { return na;      }
   Atom   &DB(int idx) { return db[idx]; }
   double   D(void)    { return density; }
   char* NAME(void)    { return name;    }

   void Parse(const char *dir_dbase, const char *cname, const char *info) {
      double prop, den, val;
      char subs[64], comp[128], *str;
      int i, j;

      strcpy(name,cname);
      strcpy(comp,info);

      str = strtok(comp," ");
      sscanf(str,"%d",&id);

      for (nm=0;;nm++) {
         str = strtok(NULL," ");  if (!str) break;
         str = strtok(NULL," ");
         str = strtok(NULL," ");
      }

      substance = new Substance [nm];

      strcpy(comp,info);
      str = strtok(comp," ");

      CkPrintf("\t[Compound.Parse] id = %d\n", id);

      for ( i=0; i<nm; i++) {
         str = strtok(NULL," ");
         sscanf(str,"%lf",&prop);

         str = strtok(NULL," ");
         sscanf(str,"%s",subs);

         str = strtok(NULL," ");
         sscanf(str,"%lf",&den);

         CkPrintf("\t\t[Compound.Parse] prop = %lf\n", prop);
         CkPrintf("\t\t[Compound.Parse] subs = %s\n", subs);
         CkPrintf("\t\t[Compound.Parse] den = %lf\n", den);

         // decodificar a fórmula química em subs
         substance[i].Parse(dir_dbase, prop, subs, den);
      }
      CkPrintf("\n");
      prop = 0;
      for ( i=0; i<nm; i++ ) {
         prop += substance[i].WT();
      }
      for ( i=0; i<nm; i++ ) {
         val = substance[i].WT()/prop;
         substance[i].WT(val);
      }
   }

   void Join(void) {
      int   i, j, k;
      Atom *tmp;
      double sum, val;

      na = substance[0].NA();
      db = new Atom [na];

      for ( i=0; i<na; i++ ) {
         db[i] = substance[0].DB(i);
         // update the weight fraction
         val = substance[0].WT()*db[i].FM(); db[i].FM(val);
         // update the relative atomic density
         val = substance[0].WT()*db[i].D();  db[i].D(val);
      }

//      for ( i=0; i<na; i++ )
//         CkPrintf("%3s", db[i].X());
//      CkPrintf("\n");

      for ( j=1; j<nm; j++ ) {
         for ( k=0; k<substance[j].NA(); k++ ) {
            for ( i=0; i<na?strcmp(substance[j].DB(k).X(),db[i].X()):0; i++ );
            if (!(na-i)) { // you didn't find the element (k) in db array
               // increase the db array length
               tmp = new Atom [na+1];

               // copy elements between arrays
               for ( i=0; i<na; i++ ) tmp[i] = db[i];

               // add the element (k) to db array
               tmp[na] = substance[j].DB(k);
               // update the weight fraction
               val = substance[j].WT()*tmp[na].FM(); tmp[na].FM(val);
               // update the relative atomic density
               val = substance[j].WT()*tmp[na].D();  tmp[na].D(val);

               // release memory
               delete [] db;
               // update the pointer db and the db length
               db = tmp;  na++;

//               for ( i=0; i<na; i++ )
//                  CkPrintf("%3s", db[i].X());
//               CkPrintf("\n");

            } else {
               // update the weight fraction
               val = db[i].FM() + substance[j].WT()*substance[j].DB(k).FM(); db[i].FM(val);
               // update the relative atomic density
               val = db[i].D() + substance[j].WT()*substance[j].DB(k).D(); db[i].D(val);
            }
         }
      }

      // correct the weight fractions of compound
      sum = db[0].FM();
      for ( i=1; i<na; i++ )
         sum += db[i].FM();
      for ( i=0; i<na; i++ ) {
         val = db[i].FM()/sum;
         db[i].FM(val);
      }

      density = db[0].D();
      for ( i=1; i<na; i++ )
         density += db[i].D();
         
      delete [] substance;
   }

   double CS_Abs(double hv) {
      int    i;
      double tau;

      for ( tau=i=0; i<na; i++ ) {
         tau += db[i].FM()*db[i].SC_Abs(hv);
      }
      return tau;
   }

   CrossSections ComputeCS(double hv) {
      int    i;
      double val;
      //double photo, elast, inelast;
      CrossSections cs;

      cs.mu = cs.photo = cs.elast = cs.inelast = cs.mec = 0;
      for ( i=0; i<na; i++ ) {
         val = db[i].FM();
         cs.photo   += val*db[i].SC_Abs(hv);
         cs.elast   += val*db[i].SC_Elast(hv);
         cs.inelast += val*db[i].SC_Inelast(hv);
      }
      cs.mu = cs.photo + cs.elast + cs.inelast;
      cs.photo /= cs.mu;
      cs.elast /= cs.mu;
      cs.inelast /= cs.mu;

      return cs;
   }

//   void Print(void) {
//      CkPrintf("\t[Compound.Print] name: %s\tden: %lf\n\n", name, density);
//      for ( int i=0; i<nm; i++ )
//         substance[i].Print();
//   }

   void PrintDB(void) {
      CkPrintf("\t[Compound.PrintDB] name: %s\tden: %lf\n\n", name, density);
      for ( int i=0; i<na; i++ )
         db[i].Print();
      CkPrintf("\n");
   }
};

//---------------------------------------------------
//
//---------------------------------------------------
class Media {
private:
   int       nm;
   Compound *compound;
   double    i511;

   MT19937ar dice;

public:
   Media(void) { compound = NULL; nm = 0; i511 = 1.0/(double)511.0; }
   Media(const char *dir, const char *fname) {
      i511 = 1.0/(double)511.0; // +- 3eV
      Read(dir,fname);
   }

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &p) {
      p|nm; p|i511;
      if (p.isUnpacking()) {
         compound = new Compound [nm];
      }
      PUParray(p,compound,nm);
   }

   void Dice(unsigned long n, unsigned long *init_key) {
      dice.setup(init_key,n); //<========
      CkPrintf("\t[Media.Dice] sample: %lf\n",dice.drand());
   }

   void Read(const char *dir_dbase, const char *fname) {
      FILE *fp;
      char txt[64], info[256], name[64];
      int  pos, i;

      fp = fopen(fname,"rt");
         if (!fp) {
             CkPrintf("[Media.Read] Error opening the file %s!!!\n", fname);
             CkExit();
         }

         nm = 0;
         while (1) {
             fgets(txt,63,fp);
             if (txt[0]=='I') nm++;
             else if (txt[0]=='E') break;
         }
         // allocate space to matter list
         compound = new Compound [nm];

         CkPrintf("[Media.Read] Reading the file %s.\n", fname);

         // return to beggining of the file
         fseek(fp,0,SEEK_SET);

         // for each matter, do
         for ( i=0; i<nm; i++ ) {
            fscanf(fp,"%s %s\n",txt,txt); // read "ID 'id'"
            strcpy(info,txt);             // copy id to info

            fgets(name,63,fp);             // read the name compound
            name[strlen(name)-1] = '\0';

            while (1) {
               pos = ftell(fp);
               fgets(txt,63,fp);          // read the next line
               txt[strlen(txt)-1] = '\0';
               if (txt[0]=='I' || txt[0]=='E') break;
               strcat(info," ");          // insert a space
               strcat(info,txt);          // copy the line to info
            }
            fseek(fp,pos,SEEK_SET);

            compound[i].Parse(dir_dbase,name,info);
            compound[i].Join();
         }

      fclose(fp);

   }

   Compound *Search(int idx) {
      int i;
      for ( i=0; i<nm?compound[i].ID()-idx:0; i++ );
      return (nm-i)? compound+i : NULL;
   }

   char* Name(int idx) {
      Compound *comp = Search(idx);
      if (!comp) CkAbort("Compound (id = %d) didn't find\n",idx);
      return comp->NAME();
   }

   double CS_Abs(int idx, double hv) {
      Compound *comp = Search(idx);
      if (!comp) CkAbort("Compound (id = %d) didn't find\n",idx);
      return comp->CS_Abs(hv);
   }

   CrossSections ComputeCS(int idx, double hv) {
      CrossSections cs;
      double draw;

      Compound *comp = Search(idx);
      if (!comp) CkAbort("Compound (id = %d) didn't find\n",idx);
      cs = comp->ComputeCS(hv);

      cs.step = -log(dice.drand())/(cs.mu*comp->D());

      draw = dice.drand();
      if (draw<cs.photo)               cs.mec = 0;
      else if (draw<cs.photo+cs.elast) cs.mec = 1;
      else                             cs.mec = 2;

      return cs;
   }

   //==================================================
   //
   //==================================================

   void TotalReflexion(Foton &hv) {
   }

   void PhotoEletricAbsortion(Foton &hv) {
      int     i, j, k, l, idx;
      double  draw;
      double *photo;
      Atom    db;
      XYZ     d, r;

      Compound *comp = Search(hv.ID());
      if (!comp) CkAbort("Compound (id = %d) didn't find\n",hv.ID());

      //---------------------------------------------
      // select the target element
      //---------------------------------------------

      photo = new double [comp->NA()];

      photo[0] = comp->DB(0).FM()*comp->DB(0).SC_Abs(hv.E());
      for ( i=1; i<comp->NA(); i++ )
         photo[i] = photo[i-1] + comp->DB(i).FM()*comp->DB(i).SC_Abs(hv.E());

      for ( i=0; i<comp->NA()-1; i++ )
         photo[i] /= photo[comp->NA()-1];
      photo[comp->NA()-1] = 1;

//      CkPrintf("\t*** ");
//      for ( i=0; i<comp->NA(); i++ ) CkPrintf("%lf ",photo[i]);
//      CkPrintf("\n");

      draw = dice.drand();
      for ( idx=0; idx<comp->NA()?photo[idx]<draw:0; idx++ );

      //---------------------------------------------
      // set the target element
      //---------------------------------------------

      db = comp->DB(idx);

//      CkPrintf("\t--- Absorption by %s\n",db.X());

      delete [] photo;

      //---------------------------------------------
      // draft the energy sublevel to ionization
      //---------------------------------------------

      photo = new double [db.NS()];

      photo[0] = db.SC_Abs(0,hv.E());
      for ( i=1; i<db.NS(); i++ )
         photo[i] = photo[i-1] + db.SC_Abs(i,hv.E());

      for ( i=0; i<db.NS()-1; i++ )
         photo[i] /= photo[db.NS()-1];
      photo[db.NS()-1] = 1;

//      CkPrintf("\t\t*** ");
//      for ( i=0; i<db.NS(); i++ ) CkPrintf("%lf ",photo[i]);
//      CkPrintf("\n");

      draw = dice.drand();
      for ( idx=0; idx<db.NS()?photo[idx]<draw:0; idx++ );

//      CkPrintf("\t\t--- Absorption by energy sublevel %s\n",db.Simb(idx));

      delete[] photo;

      //---------------------------------------------
      // draft whether fluorescence or Auger emission
      //---------------------------------------------

      draw = dice.drand();
      if (draw>db.W(idx)) { // if Auger emission then break.

//         CkPrintf("\t\t\t\t=== %lf %lf\n",draw,db.W(idx));
//         CkPrintf("\t\t\t\t--- Auger emission!\n");

         hv.E(0);
         return;
      }

      //---------------------------------------------
      // draft the energy sublevel as transition source
      //---------------------------------------------

      draw = dice.drand();
      for ( k=0; k<db.NR(idx) ? draw>db.ACUMM(idx,k) : 0 ; k++ );
      hv.E(db.XRAY(idx,k));

      //---------------------------------------------
      // draft the new photon direction
      //---------------------------------------------

      r = hv.D()^XYZ(0,0,1);
      if (r.Length()<0.1) r = hv.D()^XYZ(0,1,0);
      d = hv.D().Rotate(2*M_PI*dice.drand(),r.Unit());
      r = d.Rotate(2*M_PI*dice.drand(),hv.D());
      hv.D(r);

   }

   void ElasticScattering(Foton &hv) {
      double teta, eps;
      XYZ d, r;

     //===========================================
     // compute the scattering angle
     //===========================================
      do {
         eps = dice.drand();
         teta = M_PI*dice.drand();
      } while (eps<0.5*(1+sqr( cos(teta) ) ) );

      r = hv.D()^XYZ(0,0,1);
      if (r.Length()<0.1) r = hv.D()^XYZ(0,1,0);
      // turn d=hv.D() theta radians around r
      d = hv.D().Rotate(teta/**dice.drand()*/,r.Unit());
      // turn r=d eps radians around hv.D()
      r = d.Rotate(2*M_PI*dice.drand(),hv.D());
      // update direction
      hv.D(r);
   }

   void InelasticScattering(Foton &hv) {
      double EcE, teta, eps;
      XYZ d, r;

      //===========================================
      // compute the energy e the angle of scattering
      //===========================================
      do {
         eps = dice.drand();
         teta = M_PI*dice.drand();
         // klein-nishina...
         EcE = 1/(1+(hv.E()*i511)*(1-cos(teta)));
      } while (eps<0.5*sqr(EcE)*(EcE+1/EcE-sqr(sin(teta))));

      // compute the energy of the scatted photon
      eps = EcE*hv.E();
      // update energy
      hv.E(eps);

      r = hv.D()^XYZ(0,0,1);
      if (r.Length()<0.1) r = hv.D()^XYZ(0,1,0);
      // turn d=hv.D() theta radians around r
      d = hv.D().Rotate(teta/**dice.drand()*/,r.Unit());
      // turn r=d eps radians around hv.D()
      r = d.Rotate(2*M_PI*dice.drand(),hv.D());
      // update direction
      hv.D(r);
   }

   void Print(void) {
      for ( int i=0; i<nm; i++ ) {
         CkPrintf("[Media.Print] Compound[%d].ID(): %d\n",i,compound[i].ID());
         compound[i].PrintDB();
      }
   }
};

#endif

