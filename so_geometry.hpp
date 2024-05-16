#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define sqr(val) ((val)*(val))
#define deg (M_PI/180.0)

//===================================================
//
//===================================================
class XYZ {
private:
    double x, y, z;

public:
   // construtor
   XYZ(void) { x = y = z = 0.0; }
   XYZ(double ix, double iy, double iz) { x = ix; y = iy; z = iz; }

   // atribuir valores
   void X(double ix) { x = ix; }
   void Y(double iy) { y = iy; }
   void Z(double iz) { z = iz; }

   // retornar valores
   double X(void) { return x; }
   double Y(void) { return y; }
   double Z(void) { return z; }

   XYZ operator +(XYZ v) {
      XYZ r;
      r.x = x + v.x;
      r.y = y + v.y;
      r.z = z + v.z;
      return r;
   }
   XYZ operator +=(XYZ v) {
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
   }
   XYZ operator -(XYZ v) {
      XYZ r;
      r.x = x - v.x;
      r.y = y - v.y;
      r.z = z - v.z;
      return r;
   }
   XYZ operator -=(XYZ v) {
      x -= v.x;
      y -= v.y;
      z -= v.z;
      return *this;
   }
   // produto escalar
   double operator *(XYZ v) {
      double d;
      d = x*v.x + y*v.y + z*v.z;
      return d;
   }
   XYZ operator *(double f) {
      XYZ r;
      r.x = f*x;
      r.y = f*y;
      r.z = f*z;
      return r;
   }
   XYZ operator *=(double f) {
      x *= f;
      y *= f;
      z *= f;
      return *this;
   }
   friend XYZ operator *(double f, XYZ v) {
      XYZ r;
      r.x = f*v.x;
      r.y = f*v.y;
      r.z = f*v.z;
      return r;
   }
   XYZ operator /(double f) {
      XYZ r;
      r.x = x/f;
      r.y = y/f;
      r.z = z/f;
      return r;
   }
   XYZ operator /=(double f) {
      x /= f;
      y /= f;
      z /= f;
      return *this;
   }
   // escala coordenadas independentes
   XYZ operator %(XYZ v) {
      XYZ r;
      r.x = x*v.x;
      r.y = y*v.y;
      r.z = z*v.z;
      return r;
   }
   XYZ operator %=(XYZ v) {
      x *= v.x;
      y *= v.y;
      z *= v.z;
      return *this;
   }
   // produto vetorial
   XYZ operator ^(XYZ v) {
      XYZ r;
      r.x = y*v.z - z*v.y;
      r.y = z*v.x - x*v.z;
      r.z = x*v.y - y*v.x;
      return r;
   }
   XYZ Unit(void) {
      XYZ r;
      double l = 1/sqrt(x*x + y*y + z*z);
      return r = XYZ(x*l,y*l,z*l);
   }
   double Length(void) {
      return sqrt(x*x + y*y + z*z);
   }
   XYZ Rotate(double ang, XYZ n) {
      XYZ r(x,y,z), v, res;
      double s;
      s = cos(0.5*ang);
      v = sin(0.5*ang)*(n.Unit());
      res = (((s*s)*r) - ((v*v)*r)) + (((2*(v*r))*v) + (((2*s)*v)^r));
      return res; 
   }

   void Print(void) {
      CkPrintf("%+9.5lf %+9.5lf %+9.5lf\n", x, y, z);
   }

   void Print(FILE *fp) {
      fprintf(fp,"%+9.5lf %+9.5lf %+9.5lf\n", x, y, z);
   }

   void Print(const char *s) {
      CkPrintf("%+9.5lf %+9.5lf %+9.5lf%s", x, y, z, s);
   }

   void Print(FILE *fp, const char *s) {
      fprintf(fp,"%+9.5lf %+9.5lf %+9.5lf%s", x, y, z, s);
   }

   void Print(const char *s1, const char *s2) {
      CkPrintf("%s%+9.5lf %+9.5lf %+9.5lf%s", s1, x, y, z, s2);
   }

   void Print(FILE *fp, const char *s1, const char *s2) {
      fprintf(fp,"%s%+9.5lf %+9.5lf %+9.5lf%s", s1, x, y, z, s2);
   }

   void pup(PUP::er &p) {
      p|x; p|y; p|z;
   }
};

//===================================================
//
//===================================================
typedef struct {
   char   sname[64];
   XYZ    pt;     // point over plan
   char   flag;   // 1: valid
   char   side;   // 1: outside; 0: over; -1: inside
   int    idx;    // face index
   int    ids;    // media identifier
   double dvp;    // distance to plan
}  Point2FaceInfo;
//---------------------------------------------------

//===================================================
//
//===================================================
class Face {
protected:
   int     idx;    // face index
   int     nvf;    // number of face vertices
   XYZ    *vf;     // face vertices
   double  d;      // distance to origin
   XYZ     n;      // plan normal

public:
   Face(void) {
      idx = 0; d = 0;
      nvf = 0; vf = NULL;
   }
  ~Face(void) { delete [] vf; vf = NULL; nvf = 0; }

   // set attributes
   void NVF(int i, int val)  { idx = i; nvf = val; vf  = (XYZ*) new XYZ [nvf]; }
   void VF(int val, XYZ pt)  { vf[val] = pt; }
   void N(XYZ val)           { n = val;      } // normal vector
   void D(double val)        { d =  val;     } // distance to origin

   // return attributes
   int    NVF(void)   { return nvf;   }
   XYZ    VF(int i)   { return vf[i]; }
   XYZ    N(void)     { return n;     }
   double D(void)     { return d;     }

   double Distance(XYZ val) { return (n*val)-d; }

   Face &operator = (Face &t) {
      int i;
      
      idx = t.idx;
      nvf = t.nvf;
      if (!vf) { vf = (XYZ*) new XYZ [nvf]; }
      for ( i=0; i<nvf; i++ ) { vf[i] = t.vf[i]; }
      d = t.d;
      n = t.n;
      return *this;
   }

   Point2FaceInfo Impact_Point(XYZ vtx, XYZ vec) {
      Point2FaceInfo pfi;
      char   i, ok;

      double l = (vtx*n)-d; // l<0, if vtx under the plan
      double s = vec*n;     // s<0, if vec opposed to n
      double t = -l*s;

//      printf("===> l = %lf\n", l);
//      printf("===> s = %lf\n", s);
//      printf("===> t = %lf\n", t);

      pfi.flag = 0;
      // analize the case of total refletion!!!!
      if (t>1e-6) { // (l>0 && s<0) || (l<0 && s>0), both are indication of impact
         pfi.dvp = (-l/s);
         pfi.pt  = vtx+(pfi.dvp*vec);

//         printf("===> vtx      = "); vtx.Print();
//         printf("===> vec      = "); vec.Print();
//         printf("===> pfi.dvp  = %lf\n",pfi.dvp);
//         printf("===> dvp*vec  = "); (pfi.dvp*vec).Print();
//         printf("===> pfi.pt   = "); pfi.pt.Print();

         // check sanity
         CkAssert((pfi.pt*n)-d>0);

         pfi.side = 2*(l>1e-6)-1; // 1: above plan; -1: below plan
         pfi.idx  = idx;

//         printf("===> pfi.idx  = %d\n",  pfi.idx);

         // test if pt is inside the faces
         for ( ok=1, i=0; i<nvf; i++ ) {
            // check if pt is at left
            ok = ok && ((vf[(i+1+nvf)%nvf] - vf[(i+nvf)%nvf])^(pfi.pt - vf[(i+nvf)%nvf]))*n>0;
         }
         pfi.flag = ok;
      }

//      printf("===> pfi.flag = %d\n", pfi.flag);
//      printf("\n");

      return pfi;
   }

   Face &Translate(XYZ pt) {
      for ( int i=0; i<nvf; i++ ) vf[i] += pt;
      d = vf[0]*n;
      return *this;
   }

   Face &Rotate(double ang, XYZ dir) {
      for ( int i=0; i<nvf; i++ ) vf[i] = vf[i].Rotate(ang,dir);
      n = n.Rotate(ang,dir);
      return *this;
   }

   void Print(void) {
      int i;

      printf("\tnvf: %d\n", nvf);
      for ( i=0; i<nvf; i++ ) {
         printf("\t\tvf[%d]: ", i); vf[i].Print("\n");
      }
      printf("\t\t    n: "); n.Print("\n");
      printf("\t\t    d:  %+lf\n", d);
   }

   void Print(FILE *fp) {
      int i;

      fprintf(fp,"\tnvf: %d\n", nvf);
      for ( i=0; i<nvf; i++ ) {
         fprintf(fp,"\t\tvf[%d]: ", i); vf[i].Print(fp,"\n");
      }
      fprintf(fp,"\t\t    n: "); n.Print(fp,"\n");
      fprintf(fp,"\t\t    d:  %+lf\n", d);
   }

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &p) {
      p|d; p|n; p|idx;
      p|nvf;
      if (p.isUnpacking()) {
         vf  = (XYZ*) new XYZ [nvf];
      }
      PUParray(p,vf,nvf);
   }
};
//---------------------------------------------------

//===================================================
//
//===================================================
typedef struct { int i, j; } IJ;

class Solid {
protected:
   char  sname[64]; // solid name
   int   id;        // media identifier
   int   bg;        // background
   int   nf;        // total of faces
   int   isf;       // index of sensitive face
   Face *f;         // list of faces
   Face  bb[6];     // bounding box
   XYZ   pos;       // geometric center

public:
   Solid(void) {
      strcpy(sname,"");
      id = bg = 0; isf = -1;
      nf = 0; f = NULL;
   }
  ~Solid(void) { delete [] f; f = NULL; nf = 0; }

   int  ID(void)    { return id; }
   void ID(int val) { id = val; }

   XYZ  P(void)     { return pos; }

   char* Name(void) { return sname; }
   
   Solid &operator = (Solid &t) {
      int i;

      strcpy(sname,t.sname);
      id = t.id;
      bg = t.bg;
      nf = t.nf;
      isf = t.isf;
      if (!f) { f = (Face*) new Face [nf]; }
      for ( i=0; i<nf; i++ ) { f[i] = t.f[i]; }
      for ( i=0; i<6; i++ ) { bb[i] = t.bb[i]; }
      return *this;
   }

   void Read_Off_File(const char *filename, int val) {
      FILE  *fp;
      char   str[128];
      int    i, j, k, l;
      double x, y, z;

      int  nv;
      XYZ *v, vmax, vmin;

      isf = val; // index of sensitive face if detector

      fp = fopen(filename,"rt");
         if (!fp) {
            CkAbort("Error opening file %s!!!\n",filename);
         }

         fscanf(fp,"%s %d %d %s\n", str, &nv, &nf, str);

         // ler lista de vertices
         v = (XYZ*) new XYZ [nv];
         fscanf(fp,"%lf %lf %lf\n", &x, &y, &z);
         pos = vmax = vmin = v[0] = XYZ(x,y,z);
         for ( i=1; i<nv; i++ ) {
            fscanf(fp,"%lf %lf %lf\n", &x, &y, &z);
            pos += (v[i] = XYZ(x,y,z));
            if (v[i].X()>vmax.X()) vmax.X(v[i].X());
            if (v[i].Y()>vmax.Y()) vmax.Y(v[i].Y());
            if (v[i].Z()>vmax.Z()) vmax.Z(v[i].Z());
            if (v[i].X()<vmin.X()) vmin.X(v[i].X());
            if (v[i].Y()<vmin.Y()) vmin.Y(v[i].Y());
            if (v[i].Z()<vmin.Z()) vmin.Z(v[i].Z());
         }
         pos *= (1.0/nv);

         for ( j=0; j<6; j++ ) bb[j].NVF(j,4);
         // upper face
         bb[0].VF(0,XYZ(vmax.X(),vmax.Y(),vmax.Z()));
         bb[0].VF(1,XYZ(vmin.X(),vmax.Y(),vmax.Z()));
         bb[0].VF(2,XYZ(vmin.X(),vmin.Y(),vmax.Z()));
         bb[0].VF(3,XYZ(vmax.X(),vmin.Y(),vmax.Z()));
         // lower face
         bb[1].VF(0,XYZ(vmin.X(),vmax.Y(),vmin.Z()));
         bb[1].VF(1,XYZ(vmax.X(),vmax.Y(),vmin.Z()));
         bb[1].VF(2,XYZ(vmax.X(),vmin.Y(),vmin.Z()));
         bb[1].VF(3,XYZ(vmin.X(),vmin.Y(),vmin.Z()));
         // left face
         bb[2].VF(0,XYZ(vmax.X(),vmax.Y(),vmax.Z()));
         bb[2].VF(1,XYZ(vmax.X(),vmax.Y(),vmin.Z()));
         bb[2].VF(2,XYZ(vmin.X(),vmax.Y(),vmin.Z()));
         bb[2].VF(3,XYZ(vmin.X(),vmax.Y(),vmax.Z()));
         // right face
         bb[3].VF(0,XYZ(vmin.X(),vmin.Y(),vmin.Z()));
         bb[3].VF(1,XYZ(vmax.X(),vmin.Y(),vmin.Z()));
         bb[3].VF(2,XYZ(vmax.X(),vmin.Y(),vmax.Z()));
         bb[3].VF(3,XYZ(vmin.X(),vmin.Y(),vmax.Z()));
         // front face
         bb[4].VF(0,XYZ(vmax.X(),vmax.Y(),vmax.Z()));
         bb[4].VF(1,XYZ(vmax.X(),vmin.Y(),vmax.Z()));
         bb[4].VF(2,XYZ(vmax.X(),vmin.Y(),vmin.Z()));
         bb[4].VF(3,XYZ(vmax.X(),vmax.Y(),vmin.Z()));
         // back face
         bb[5].VF(0,XYZ(vmin.X(),vmin.Y(),vmin.Z()));
         bb[5].VF(1,XYZ(vmin.X(),vmin.Y(),vmax.Z()));
         bb[5].VF(2,XYZ(vmin.X(),vmax.Y(),vmax.Z()));
         bb[5].VF(3,XYZ(vmin.X(),vmax.Y(),vmin.Z()));

         for ( j=0; j<6; j++ ) {
            bb[j].N( ((bb[j].VF(2)-bb[j].VF(1))^(bb[j].VF(0)-bb[j].VF(1))).Unit() );
            bb[j].D(bb[j].VF(0)*bb[j].N());
         }

         // ler lista de faces
         f = (Face*) new Face [nf];
         for ( j=0; j<nf; j++ ) {
            // ler lista de vertices da face
            fscanf(fp,"%d", &k);
            f[j].NVF(j,k);
            for ( i=0; i<k; i++ ) {
               fscanf(fp,"%d", &l);
               f[j].VF(i,v[l]);
            }

            f[j].N( ((f[j].VF(2)-f[j].VF(1))^(f[j].VF(0)-f[j].VF(1))).Unit() );
            f[j].D(f[j].VF(0)*f[j].N());
         }

         delete [] v;
      fclose(fp);
   }

   void Read_Off_File_Original(const char *filename, int val) {
      FILE  *fp;
      char   str[128];
      int    i, j, k, l;
      double x, y, z;

      int  nv;
      XYZ *v;

      isf = val; // index of sensitive face if detector

      fp = fopen(filename,"rt");
         if (!fp) {
            CkAbort("Error opening file %s!!!\n",filename);
         }

         fscanf(fp,"%s %d %d %s\n", str, &nv, &nf, str);

         // ler lista de vertices
         v = (XYZ*) new XYZ [nv];
         for ( i=0; i<nv; i++ ) {
            fscanf(fp,"%lf %lf %lf\n", &x, &y, &z);
            v[i] = XYZ(x,y,z);
         }

         // ler lista de faces
         f = (Face*) new Face [nf];
         for ( j=0; j<nf; j++ ) {
            // ler lista de vertices da face
            fscanf(fp,"%d", &k);
            f[j].NVF(j,k);
            for ( i=0; i<k; i++ ) {
               fscanf(fp,"%d", &l);
               f[j].VF(i,v[l]);
            }

            f[j].N( ((f[j].VF(2)-f[j].VF(1))^(f[j].VF(0)-f[j].VF(1))).Unit() );
            f[j].D(f[j].VF(0)*f[j].N());
         }

         delete [] v;
      fclose(fp);
   }

   void Read_Off_File(const char *name, int val1, int val2, const char *filename, int val3) {
      bg = val1;
      id = val2;
      strcpy(sname,name);
      Read_Off_File(filename,val3);
   }

   Point2FaceInfo Impact_Point(XYZ vtx, XYZ vec) {
      Point2FaceInfo pfi, p;
      int i;

      for ( i=pfi.flag=0; (i<6)?(!bb[i].Impact_Point(vtx,vec).flag):0; i++ );

      if (i<6) {
         p.flag = 0;
         for ( i=0; (i<nf)?(!p.flag):0; i++ )
            p = f[i].Impact_Point(vtx,vec);
         pfi = p;

         for ( ; i<nf; i++ ) {
            p = f[i].Impact_Point(vtx,vec);
            if (p.flag && pfi.dvp>p.dvp) pfi = p;
         }

         if (pfi.side>0)
            pfi.ids = id; // identificador do meio (não é o índice do sólido)
         else if (pfi.side<0)
            pfi.ids = bg; // identificador do meio (não é o índice do sólido)
         strcpy(pfi.sname,sname);
      }
      return pfi;
   }

   Point2FaceInfo Impact_Point_Original(XYZ vtx, XYZ vec) {
      Point2FaceInfo pfi, p;
      int i;

      p.flag = 0;
      for ( i=0; (i<nf)?(!p.flag):0; i++ )
         p = f[i].Impact_Point(vtx,vec);
      pfi = p;

      for ( ; i<nf; i++ ) {
         p = f[i].Impact_Point(vtx,vec);
         if (p.flag && pfi.dvp>p.dvp) pfi = p;
      }

      if (pfi.side>0)
         pfi.ids = id; // identificador do meio (não é o índice do sólido)
      else if (pfi.side<0)
         pfi.ids = bg; // identificador do meio (não é o índice do sólido)
      strcpy(pfi.sname,sname);

      return pfi;
   }

   Solid &Translate(XYZ pt) {
      for ( int i=0; i<6; i++ ) bb[i].Translate(pt);
      for ( int i=0; i<nf; i++ ) f[i].Translate(pt);
      pos += pt;
      return *this;
   }

   Solid &Rotate(double ang, XYZ dir) {
      for ( int i=0; i<6; i++ ) bb[i].Rotate(ang,dir);
      for ( int i=0; i<nf; i++ ) f[i].Rotate(ang,dir);
      pos = pos.Rotate(ang,dir);
      return *this;
   }

   XYZ Global2Local(Point2FaceInfo pfi) {
      // f[pfi.idx]       : the face of solid
      // f[pfi.idx].N()   : normal of face
      // f[pfi.idx].NVF   : number of vertices
      // f[pfi.idx].VF(i) : i-vertex of face
      // f[pfi.idx].D()   : distance of face to origin

      XYZ p(0,0,-1), u, v;

      if (isf==pfi.idx) {
         u = (f[isf].VF(2)-f[isf].VF(3));
         v = (f[isf].VF(0)-f[isf].VF(3));
         p.X(((pfi.pt-f[pfi.idx].VF(3))*u)/sqr(u.Length()));
         p.Y(((pfi.pt-f[pfi.idx].VF(3))*v)/sqr(v.Length()));
         p.Z(0);

//         printf("\t\t\tVF(2):"); f[isf].VF(2).Print();
//         printf("\t\t\tVF(3):"); f[isf].VF(3).Print();
//         printf("\t\t\tVF(0):"); f[isf].VF(0).Print();
//         printf("\t\t\tu:"); u.Print();
//         printf("\t\t\tv:"); v.Print();
//         printf("\t\t\tp:"); p.Print();
      }

      return p;
   }

   void Print(void) {
      int i, j;

      printf("[Solid] %s\n",sname);
      printf("\tid: %d\n", id);
      printf("\tnf: %d\n", nf);

      for ( j=0; j<nf; j++ ) {
         printf("\tf[%d]", j);
         f[j].Print();
      }
   }

   void Print(FILE *fp) {
      int i, j;

      fprintf(fp,"[Solid] %s\n",sname);
      fprintf(fp,"\tid: %d\n", id);
      fprintf(fp,"\tnf: %d\n", nf);

      for ( j=0; j<nf; j++ ) {
         fprintf(fp,"\tf[%d]", j);
         f[j].Print(fp);
      }
      for ( j=0; j<6; j++ ) {
         fprintf(fp,"\tbb[%d]", j);
         bb[j].Print(fp);
      }
   }

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &p) {
      p(sname,64);
      p|id; p|nf; p|bg; p|isf; p|pos;
      if (p.isUnpacking()) {
         f = (Face*) new Face [nf];
      }
      PUParray(p,f,nf);
      PUParray(p,bb,6);
   }
};
//---------------------------------------------------

//===================================================
//
//===================================================
class Models {
private:
   int    ns;       // total of solids
   Solid *s, *temp; // list of solids

   void IncrementList(void) {
      if (ns) {
         temp = (Solid*) new Solid [ns+1];
         for ( int i=0; i<ns; i++ ) temp[i] = s[i];
         delete [] s;
         s = temp;
      } else s = (Solid*) new Solid[1];
      ns++;
   }

public:
   Models(void) {
      ns = 0;
      s = NULL;
   }

   int  NS(void)    { return ns; }
   void NS(int val) { ns = val; }

   Solid &operator [] (int idx) {
      return s[idx];
   }

   Solid &operator [] (const char *str) {
      int idx;
//      printf("ns %d\n",ns);
      for ( idx=0; idx<ns?strcmp(s[idx].Name(),str):0; idx++ );
//         printf("s[%d : %d].Nome(): %s\tstr: %s\n", idx, ns, s[idx].Name(),str);
      if (idx>=ns) CkAbort("[Models.operator[]] There isn't solid named %s!!!\n",str);
      return s[idx];
   }

   int Add(const char *str, int val1, int val2, const char *filename, int val3) {
      IncrementList();
      s[ns-1].Read_Off_File(str, val1, val2, filename, val3);
      return ns-1;
   }

   Point2FaceInfo Impact_Point(XYZ vtx, XYZ vec) {
      Point2FaceInfo pfi, p;
      int i;

      // teste
      p.flag = 0;
      for ( i=0; (i<ns)?(!p.flag):0; i++ )
         p = s[i].Impact_Point(vtx,vec);
      pfi = p;

      for ( ; i<ns; i++ ) {
         p = s[i].Impact_Point(vtx,vec);
         if (p.flag && pfi.dvp>p.dvp) pfi = p;
      }

      return pfi;
   }

   void Print(void) {
      int i;

      printf("[Models]\n");
      printf("\tns: %d\n", ns);

      for ( i=0; i<ns; i++ ) {
         printf("\ts[%d]\n", i);
         s[i].Print();
      }
   }

   void Print(const char *filename) {
      FILE *fp;
      int i;

      fp = fopen(filename,"wt");
         fprintf(fp,"[Models]\n");
         fprintf(fp,"\tns: %d\n", ns);

         for ( i=0; i<ns; i++ ) {
            fprintf(fp,"\ts[%d]\n", i);
            s[i].Print(fp);
         }
      fclose(fp);
   }

   //-----------------------------------
   // PUP
   //-----------------------------------
   void pup(PUP::er &p) {
      p|ns;
      if (p.isUnpacking()) {
         s = (Solid*) new Solid [ns];
      }
      PUParray(p,s,ns);
   }
};
//---------------------------------------------------

#endif
