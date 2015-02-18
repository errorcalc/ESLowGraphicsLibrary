#include "Graphics.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "ESGL2.h"
#include "float.h"
#include "IntMath.h"

static PBitMap Disp;
static float glPerspective = 200.0f;
static TPoint3f Center = {0.0f, 0.0f, 0.0f};
static TPoint Scr = {0, 0};
static int UseZ = 0;
static int UseCullFace = 0;
static fixed *WBuffer = NULL;

void glSetCullFace(int IsUse)
{
  UseCullFace = IsUse;
}

int glGetCullFace(void)
{
  return UseCullFace;
}

void glSetZBuffer(int IsUse)
{
  int size;

  UseZ = IsUse;

  if(Disp == NULL)
    return;

  if(IsUse)
  {
    size = Disp->Width * Disp->Height * sizeof(float);
    if(WBuffer == NULL)
      WBuffer = (fixed*)malloc(size);
    else
      WBuffer = (fixed*)realloc(WBuffer, size);
  }
  else
    if(WBuffer != NULL)
      free(WBuffer);
}

int glGetZBuffer(void)
{
  return UseZ;
}

void glSetDisp(PBitMap _Disp)
{
  Disp = _Disp;
  Scr.X = Disp->Width / 2;
  Scr.Y = Disp->Height / 2;

  if(UseZ)
    glSetZBuffer(1);
}

PBitMap glGetDisp(void)
{
  return Disp;
}

void glSetPerspective(float Perspective)
{
  glPerspective = Perspective;
}

float glGetPerspective(void)
{
  return glPerspective;
}

void glSetCenter(TPoint3f _Center)
{
  Center = _Center;
}

TPoint3f glGetCenter(void)
{
  return Center;
}

//////////////////////

#define SortVertexY(a, b, c, t)    \
{                                  \
  if((a)->sy > (b)->sy)            \
    SWAP(a, b, t);                 \
  if((a)->sy > (c)->sy)            \
    SWAP(a, c, t);                 \
  if((b)->sy > (c)->sy)            \
    SWAP(b, c, t);                 \
}

#define SortVertexZ(a, b, c, t)    \
{                                  \
  if((a)->z > (b)->z)            \
    SWAP(a, b, t);                 \
  if((a)->z > (c)->z)            \
    SWAP(a, c, t);                 \
  if((b)->z > (c)->z)            \
    SWAP(b, c, t);                 \
}

void glZoom(PVertex Vertex, float x, float y, float z)
{
  Vertex->x = Vertex->x * x;
  Vertex->y = Vertex->y * y;
  Vertex->z = Vertex->z * z; 
}

void glArrayZoom(PVertex pVertex, int Count, float x, float y, float z)
{
  int i;
  for(i = 0; i != Count; i++)
  {
    pVertex[i].x *= x;
    pVertex[i].y *= y;
    pVertex[i].z *= z;
  }
}

void glRotate(PVertex Vertex, float ax, float ay, float az)
{
  float x, y, z;
  float CosX, CosY, CosZ, SinX, SinY, SinZ;  

  SinX = sin(ax); CosX = cos(ax);
  SinY = sin(ay); CosY = cos(ay);
  SinZ = sin(az); CosZ = cos(az);

  if(az != 0)
  {
    x = Vertex->x; y = Vertex->y;
    Vertex->x = x * CosZ - y * SinZ;
    Vertex->y = x * SinZ + y * CosZ;
  }
  if(ay != 0)
  {
    x = Vertex->x; z = Vertex->z;
    Vertex->x = x * CosY - z * SinY;
    Vertex->z = x * SinY + z * CosY;
  }
  if(ax != 0)
  {
    y = Vertex->y; z = Vertex->z;
    Vertex->y = y * CosX - z * SinX;
    Vertex->z = y * SinX + z * CosX;
  }
}

void glArrayRotate(PVertex pVertex, int Count, float ax, float ay, float az)
{
  int i;
  float x, y, z;
  float CosX, CosY, CosZ, SinX, SinY, SinZ;  

  SinX = sin(ax); CosX = cos(ax);
  SinY = sin(ay); CosY = cos(ay);
  SinZ = sin(az); CosZ = cos(az);

  for(i = 0; i < Count; i++)
  {
    if(az != 0)
    {
      x = pVertex[i].x; y = pVertex[i].y;
      pVertex[i].x = x * CosZ - y * SinZ;
      pVertex[i].y = x * SinZ + y * CosZ;
    }

    if(ay != 0)
    {
      x = pVertex[i].x; z = pVertex[i].z;
      pVertex[i].x = x * CosY - z * SinY;
      pVertex[i].z = x * SinY + z * CosY;
    }

    if(ax != 0)
    {
      y = pVertex[i].y; z = pVertex[i].z;
      pVertex[i].y = y * CosX - z * SinX;
      pVertex[i].z = y * SinX + z * CosX;
    }
  }
}

void glTranslate(PVertex Vertex, float x, float y, float z)
{
  Vertex->x += x;
  Vertex->y += y;
  Vertex->z += z;
}

void glArrayTranslate(PVertex pVertex, int Count, float x, float y, float z)
{
  int i;
  for(i = 0; i != Count; i++)
  {
    pVertex[i].x += x;
    pVertex[i].y += y;
    pVertex[i].z += z;
  }
}

void glArrayCopy(PVertex Dst, const PVertex Src, int Count)
{
  memcpy(Dst, Src, sizeof(TVertex)*Count);
}

PVertex glArrayCreate(int Count)
{
  return (PVertex)malloc(sizeof(TVertex)*Count);
}

PVertex glArrayFree(PVertex p)
{
  free(p);
  return NULL;
}

//////////////////////

void glClearZBuffer(void)
{
  int i;
  for(i = 0; i < Disp->Height * Disp->Width; i++)
    WBuffer[i] = IntToFixed((int)(-glPerspective-1));//-990;//FLT_MIN; 
}

void glProjectVertex(PVertex V)
{
  V->sx = (int)(0.0 + Disp->Width / 2 + ((V->x + Center.x) * glPerspective) / (V->z + glPerspective));
  V->sy = (int)(0.0 + Disp->Height / 2 + ((V->y + Center.y) * glPerspective) / (V->z + glPerspective));

  // V->sx = (int)(0.5 + V->px);
  // V->sy = (int)(0.5 + V->py);

  V->w = 1000.0 / (float)(V->z + glPerspective);
}

int isCullFace(PVertex a, PVertex b, PVertex c)
{
  if( (b->sx - a->sx) * (c->sy - b->sy) -
    (c->sx - b->sx) * (b->sy - a->sy) < 0)return 0;
  return 1;
}

//////////////////////
/*
void glColorTriangle_uu(PVertex a, PVertex b, PVertex c, TColor Color)
{
  PVertex t;
  int y, Start_y, End_y, sx1, sx2, it, x;
  float x1, x2, w1, w2, ft, z;
  float dx1, dx2, dw1, dw2;
  float dw, w, sw1, sw2;
  fixed _w1, _w2, _dw, _w, _sw1, _sw2;
  int scan_w;

  SortVertexY(a, b, c, t);

  x1 = a->sx + (float)(c->sx - a->sx) / (float)(c->sy - a->sy) * 0.5;
  x2 = a->sx + (float)(b->sx - a->sx) / (float)(b->sy - a->sy) * 0.5;

  //w1 = a->w + (float)(c->w - a->w) / (float)(c->sy - a->sy) * 0.5;
  //w2 = a->w + (float)(b->w - a->w) / (float)(b->sy - a->sy) * 0.5;

  dx1 = (float)(c->sx - a->sx) / (float)(c->sy - a->sy);
  dx2 = (float)(b->sx - a->sx) / (float)(b->sy - a->sy);

  //dw1 = (float)(c->w - a->w) / (float)(c->sy - a->sy);
  //dw2 = (float)(b->w - a->w) / (float)(b->sy - a->sy);

  //scan_w = a->sy * Disp->Width;

  for(y = a->sy; y < c->sy; y++)
  {
    if(y == b->sy)
    { 
      x2 = b->sx + (float)(c->sx - b->sx) / (float)(c->sy - b->sy) * 0.5;
      dx2 = (float)(c->sx - b->sx) / (float)(c->sy - b->sy);

      //w2 = b->w + (float)(c->w - b->w) / (float)(c->sy - b->sy) * 0.5;
      //dw2 = (float)(c->w - b->w) / (float)(c->sy - b->sy);
    }

    if(y >= 0 && y < Disp->Height)
    {
      //sw1 = w1;
      //sw2 = w2;
      if((sx1 = x1) > (sx2 = x2))
      {
        SWAP(sx1, sx2, it);
        //SWAP(sw1, sw2, ft);
      }

      if(sx1 < Disp->Width && sx2 > 0)
      {
        if(sx1 < 0)sx1 = 0;
        if(sx2 >= Disp->Width)sx2 = Disp->Width - 1;

        //esDrawHLine(Disp, (int)sx1, (int)sx2, y, Color);
        //w = sw1 + (float)(sw2 - sw1) / (float)(sx2 - sx1) * 0.5;
        ///dw = (float)(sw2 - sw1) / (float)(sx2 - sx1);

        //_w = FloatToFixed(w);
        //_dw = FloatToFixed(dw);

        for(x = sx1; x < sx2; x++)
        {
          //if(WBuffer[scan_w + x] < _w )
          {
          //  WBuffer[scan_w + x] = _w;
            esSetPixel4(Disp, x, y, Color);
          }
          // w += dw;
          //_w += _dw;
        }
      }
    }

    x1 += dx1;
    x2 += dx2;

    //w1 += dw1;
    //w2 += dw2;

    //scan_w += Disp->Width;
  }
}*/

typedef struct
{
  fixed x;
  fixed dx;
  fixed w;
  fixed dw;
} TColorEgde;

typedef struct
{
  fixed x;
  fixed dx;
  fixed w;
  fixed dw;
  fixed s;
  fixed ds;
  fixed t;
  fixed dt;
} TTextureEgde;

typedef struct
{
  fixed x;
  fixed dx;
  fixed w;
  fixed dw;
  fixed c;
  fixed dc;
} TGradientEgde;

//                    
#define CalcFixEgde_x(Egde, a, b, t) \
  {  \
    t = (b->sx - a->sx) / (float)(b->sy - a->sy);  \
    Egde.dx = FloatToFixed(t);  \
    Egde.x = FloatToFixed(a->sx + t * 0.5f);  \
  }

#define CalcFixEgde_w(Egde, a, b, t) \
  {  \
    t = (b->w - a->w) / (float)(b->sy - a->sy);  \
    Egde.dw = FloatToFixed(t);  \
    Egde.w = FloatToFixed(a->w + t * 0.5f);  \
  }

#define CalcFixEgde_t(Egde, a, b, temp) \
  {  \
    temp = (b->s - a->s) / (float)(b->sy - a->sy);  \
    Egde.ds = FloatToFixed(temp);  \
    Egde.s = FloatToFixed(a->s + temp * 0.5f);  \
    temp = (b->t - a->t) / (float)(b->sy - a->sy);  \
    Egde.dt = FloatToFixed(temp);  \
    Egde.t = FloatToFixed(a->t + temp * 0.5f);  \
  }

#define CalcFixEgde_c(Egde, a, b, t) \
  {  \
    t = (b->Color - a->Color) / (float)(b->sy - a->sy);  \
    Egde.dc = FloatToFixed(t);  \
    Egde.c = FloatToFixed(a->Color + t * 0.5f);  \
  }

void glColorTriangle_z(PVertex a, PVertex b, PVertex c, TColor Color)
{
  PVertex vt;
  int y, Start_y, End_y, x, it;
  TColorEgde e1, e2;
  int scan_w, x1, x2;
  float w1, w2, ft;
  fixed w, dw;

  SortVertexY(a, b, c, vt);

  CalcFixEgde_x(e1, a, c, ft);
  CalcFixEgde_x(e2, a, b, ft);
  CalcFixEgde_w(e1, a, c, ft);
  CalcFixEgde_w(e2, a, b, ft);

  scan_w = a->sy * Disp->Width;
  for(y = a->sy; y < c->sy; y++)
  {
    if(y == b->sy)
    { 
      CalcFixEgde_x(e2, b, c, ft);
      CalcFixEgde_w(e2, b, c, ft);
    }

    if(y >= 0 && y < Disp->Height)
    {
      x1 = FixedToInt(e1.x);
      x2 = FixedToInt(e2.x);

      w1 = FixedToFloat(e1.w);
      w2 = FixedToFloat(e2.w);

      if((x1 = x1) > (x2 = x2))
      {
        SWAP(x1, x2, it);
        SWAP(w1, w2, ft);
      }

      if(x1 < Disp->Width && x2 > 0)
      {
        ft = (float)(w2 - w1) / (float)(x2 - x1);
        dw = FloatToFixed(ft);
        w = FloatToFixed(w1 + ft * 0.5f);

        if(x1 < 0)
        {
          w += (dw * -x1);
          x1 = 0;
        }
        if(x2 >= Disp->Width)x2 = Disp->Width;

        for(x = x1; x < x2; x++)
        {
          if(WBuffer[scan_w + x] < w)
          {
            WBuffer[scan_w + x] = w;
            esSetPixel(Disp, x, y, Color);
          }
          w += dw;
        }
      }
    }

    e1.x += e1.dx;
    e2.x += e2.dx;
    e1.w += e1.dw;
    e2.w += e2.dw;

    scan_w += Disp->Width;
  }
}

void glColorTriangle_u(PVertex a, PVertex b, PVertex c, TColor Color)
{
  PVertex t;
  int y, Start_y, End_y, x, it;
  TColorEgde e1, e2;
  int scan_w, x1, x2;
  float w1, w2, w, dw, ft;

  SortVertexY(a, b, c, t);

  CalcFixEgde_x(e1, a, c, ft);
  CalcFixEgde_x(e2, a, b, ft);
  // CalcFixEgde_w(e1, a, c, ft);
  // CalcFixEgde_w(e2, a, b, ft);

  // scan_w = a->sy * Disp->Width;
  for(y = a->sy; y < c->sy; y++)
  {
    if(y == b->sy)
    { 
      CalcFixEgde_x(e2, b, c, ft);
      // CalcFixEgde_w(e2, b, c, ft);
    }

    if(y >= 0 && y < Disp->Height)
    {
      x1 = FixedToInt(e1.x);
      x2 = FixedToInt(e2.x);

      // w1 = FixedToFloat(e1.w);
      // w2 = FixedToFloat(e2.w);

      if((x1 = x1) > (x2 = x2))
      {
        SWAP(x1, x2, it);
        // SWAP(w1, w2, ft);
      }

      if(x1 < Disp->Width && x2 > 0)
      {
        // dw = FloatToFixed(ft);
        // w = FloatToFixed(w1 + ft * 0.5f);

        if(x1 < 0)
        {
          x1 = 0;
        }
        if(x2 >= Disp->Width)x2 = Disp->Width;

        for(x = x1; x < x2; x++)
        {
          // if(WBuffer[scan_w + x] < w )
          {
            // WBuffer[scan_w + x] = w;
            esSetPixel(Disp, x, y, Color);
          }
          // w += dw;
        }
      }
    }

    e1.x += e1.dx;
    e2.x += e2.dx;
    // e1.w += e1.dw;
    // e2.w += e2.dw;

    // scan_w += Disp->Width;
  }
}


void glTextureTriangle_z(PVertex a, PVertex b, PVertex c, PBitMap Tex)
{
  PVertex vt; 
  int y, Start_y, End_y, x, it;
  TTextureEgde e1, e2;
  int scan_w, x1, x2;
  float w1, w2, ft;
  fixed w, dw;
  float t1, t2;
  fixed t, dt;
  float s1, s2;
  fixed s, ds;

  SortVertexY(a, b, c, vt);

  CalcFixEgde_x(e1, a, c, ft);
  CalcFixEgde_x(e2, a, b, ft);
  CalcFixEgde_w(e1, a, c, ft);
  CalcFixEgde_w(e2, a, b, ft);
  CalcFixEgde_t(e1, a, c, ft);
  CalcFixEgde_t(e2, a, b, ft);

  scan_w = a->sy * Disp->Width;
  for(y = a->sy; y < c->sy; y++)
  {
    if(y == b->sy)
    { 
      CalcFixEgde_x(e2, b, c, ft);
      CalcFixEgde_w(e2, b, c, ft);
      CalcFixEgde_t(e2, b, c, ft);
    }

    if(y >= 0 && y < Disp->Height)
    {
      x1 = FixedToInt(e1.x);
      x2 = FixedToInt(e2.x);

      s1 = FixedToFloat(e1.s);
      s2 = FixedToFloat(e2.s);
      t1 = FixedToFloat(e1.t);
      t2 = FixedToFloat(e2.t);

      w1 = FixedToFloat(e1.w);
      w2 = FixedToFloat(e2.w);
      if((x1 = x1) > (x2 = x2))
      {
        SWAP(x1, x2, it);
        SWAP(s1, s2, ft);
        SWAP(t1, t2, ft);
        SWAP(w1, w2, ft);
      }

      if(x1 < Disp->Width && x2 > 0)
      {
        ft = (float)(s2 - s1) / (float)(x2 - x1);
        ds = FloatToFixed(ft);
        s = FloatToFixed(s1 + ft * 0.5f);
        ft = (float)(t2 - t1) / (float)(x2 - x1);
        dt = FloatToFixed(ft);
        t = FloatToFixed(t1 + ft * 0.5f);

        ft = (float)(w2 - w1) / (float)(x2 - x1);
        dw = FloatToFixed(ft);
        w = FloatToFixed(w1 + ft * 0.5f);

        if(x1 < 0)
        {
          w += (dw * -x1);
          s += (ds * -x1);
          t += (dt * -x1);
          x1 = 0;
        }
        if(x2 >= Disp->Width)x2 = Disp->Width;

        for(x = x1; x < x2; x++)
        {
          if(WBuffer[scan_w + x] < w )
          {
            WBuffer[scan_w + x] = w;
            esSetPixel(Disp, x, y, esGetPixel(Tex, FixedToInt(s), FixedToInt(t)));
          }
          s += ds;
          t += dt;
          w += dw;
        }
      }
    }

    e1.x += e1.dx;
    e2.x += e2.dx;
    e1.s += e1.ds;
    e2.s += e2.ds;
    e1.t += e1.dt;
    e2.t += e2.dt;
    e1.w += e1.dw;
    e2.w += e2.dw;

    scan_w += Disp->Width;
  }
}

void glTextureTriangle_u(PVertex a, PVertex b, PVertex c, PBitMap Tex)
{
  PVertex vt;
  int y, Start_y, End_y, x, it;
  TTextureEgde e1, e2;
  int scan_w, x1, x2;
  float t1, t2, ft;
  fixed t, dt;
  float s1, s2;
  fixed s, ds;

  SortVertexY(a, b, c, vt);

  CalcFixEgde_x(e1, a, c, ft);
  CalcFixEgde_x(e2, a, b, ft);
  CalcFixEgde_t(e1, a, c, ft);
  CalcFixEgde_t(e2, a, b, ft);

  for(y = a->sy; y < c->sy; y++)
  {
    if(y == b->sy)
    { 
      CalcFixEgde_x(e2, b, c, ft);
      CalcFixEgde_t(e2, b, c, ft);
    }

    if(y >= 0 && y < Disp->Height)
    {
      x1 = FixedToInt(e1.x);
      x2 = FixedToInt(e2.x);

      s1 = FixedToFloat(e1.s);
      s2 = FixedToFloat(e2.s);
      t1 = FixedToFloat(e1.t);
      t2 = FixedToFloat(e2.t);

      if((x1 = x1) > (x2 = x2))
      {
        SWAP(x1, x2, it);
        SWAP(s1, s2, ft);
        SWAP(t1, t2, ft);
      }

      if(x1 < Disp->Width && x2 > 0)
      {
        ft = (float)(s2 - s1) / (float)(x2 - x1);
        ds = FloatToFixed(ft);
        s = FloatToFixed(s1 + ft * 0.5f);
        ft = (float)(t2 - t1) / (float)(x2 - x1);
        dt = FloatToFixed(ft);
        t = FloatToFixed(t1 + ft * 0.5f);

        if(x1 < 0)
        {
          s += (ds * -x1);
          t += (dt * -x1);
          x1 = 0;
        }
        if(x2 >= Disp->Width)x2 = Disp->Width;

        for(x = x1; x < x2; x++)
        {
          esSetPixel(Disp, x, y, esGetPixel(Tex, FixedToInt(s), FixedToInt(t)));
          s += ds;
          t += dt;
        }
      }
    }

    e1.x += e1.dx;
    e2.x += e2.dx;
    e1.s += e1.ds;
    e2.s += e2.ds;
    e1.t += e1.dt;
    e2.t += e2.dt;
  }
}

void glGradientTriangle_z(PVertex a, PVertex b, PVertex c)
{
  PVertex vt;
  int y, Start_y, End_y, x, it;
  TGradientEgde e1, e2;
  int scan_w, x1, x2;
  float w1, w2, ft;
  fixed w, dw;
  float c1, c2;
  fixed color, dc;

  SortVertexY(a, b, c, vt);

  CalcFixEgde_x(e1, a, c, ft);
  CalcFixEgde_x(e2, a, b, ft);
  CalcFixEgde_w(e1, a, c, ft);
  CalcFixEgde_w(e2, a, b, ft);
  CalcFixEgde_c(e1, a, c, ft);
  CalcFixEgde_c(e2, a, b, ft);

  scan_w = a->sy * Disp->Width;
  for(y = a->sy; y < c->sy; y++)
  {
    if(y == b->sy)
    { 
      CalcFixEgde_x(e2, b, c, ft);
      CalcFixEgde_w(e2, b, c, ft);
      CalcFixEgde_c(e2, b, c, ft);
    }

    if(y >= 0 && y < Disp->Height)
    {
      x1 = FixedToInt(e1.x);
      x2 = FixedToInt(e2.x);

      c1 = FixedToFloat(e1.c);
      c2 = FixedToFloat(e2.c);

      w1 = FixedToFloat(e1.w);
      w2 = FixedToFloat(e2.w);
      if((x1 = x1) > (x2 = x2))
      {
        SWAP(x1, x2, it);
        SWAP(c1, c2, ft);
        SWAP(w1, w2, ft);
      }

      if(x1 < Disp->Width && x2 > 0)
      {
        ft = (float)(c2 - c1) / (float)(x2 - x1);
        dc = FloatToFixed(ft);
        color = FloatToFixed(c1 + ft * 0.5f);

        ft = (float)(w2 - w1) / (float)(x2 - x1);
        dw = FloatToFixed(ft);
        w = FloatToFixed(w1 + ft * 0.5f);

        if(x1 < 0)
        {
          w += (dw * -x1);
          color += (dc * -x1);
          x1 = 0;
        }
        if(x2 >= Disp->Width)x2 = Disp->Width;

        for(x = x1; x < x2; x++)
        {
          if(WBuffer[scan_w + x] < w )
          {
            WBuffer[scan_w + x] = w;
            esSetPixel(Disp, x, y, FixedToInt(color));
          }
          color += dc;
          w += dw;
        }
      }
    }

    e1.x += e1.dx;
    e2.x += e2.dx;
    e1.c += e1.dc;
    e2.c += e2.dc;
    e1.w += e1.dw;
    e2.w += e2.dw;

    scan_w += Disp->Width;
  }
}


void glGradientTriangle_u(PVertex a, PVertex b, PVertex c)
{
  PVertex vt;
  int y, Start_y, End_y, x, it;
  TGradientEgde e1, e2;
  int x1, x2;
  fixed w, dw;
  float c1, c2, ft;
  fixed color, dc;

  SortVertexY(a, b, c, vt);

  CalcFixEgde_x(e1, a, c, ft);
  CalcFixEgde_x(e2, a, b, ft);
  CalcFixEgde_c(e1, a, c, ft);
  CalcFixEgde_c(e2, a, b, ft);

  for(y = a->sy; y < c->sy; y++)
  {
    if(y == b->sy)
    { 
      CalcFixEgde_x(e2, b, c, ft);
      CalcFixEgde_c(e2, b, c, ft);
    }

    if(y >= 0 && y < Disp->Height)
    {
      x1 = FixedToInt(e1.x);
      x2 = FixedToInt(e2.x);

      c1 = FixedToFloat(e1.c);
      c2 = FixedToFloat(e2.c);

      if((x1 = x1) > (x2 = x2))
      {
        SWAP(x1, x2, it);
        SWAP(c1, c2, ft);
      }

      if(x1 < Disp->Width && x2 > 0)
      {
        ft = (float)(c2 - c1) / (float)(x2 - x1);
        dc = FloatToFixed(ft);
        color = FloatToFixed(c1 + ft * 0.5f);

        if(x1 < 0)
        {
          w += (dw * -x1);
          color += (dc * -x1);
          x1 = 0;
        }
        if(x2 >= Disp->Width)x2 = Disp->Width;

        for(x = x1; x < x2; x++)
        {
          esSetPixel(Disp, x, y, FixedToInt(color));
          color += dc;
        }
      }
    }

    e1.x += e1.dx;
    e2.x += e2.dx;
    e1.c += e1.dc;
    e2.c += e2.dc;
  }
}


#define Interpolation( val_b, val_e, step, div ) \
( (div != 0) ? (( (val_b) * ((div) - (step) )) + ( (val_e) * (step) ) ) / (div) : (val_b) )

//f(x) = f(x0) + (f(x1)-f(x0))/(x1-x0) * (x-x0) 
#define fInt(f1, f2, x1, x2, x) \
  (f1 + (f2 - f1)/(x2-x1) * (x-x1))

void glColorTriangle(PVertex a, PVertex b, PVertex c, TColor Color)
{
  TVertex _t1, _t2;
  PVertex t, t1 = &_t1, t2 = &_t2;
  float ZCyt;

  glProjectVertex(a);
  glProjectVertex(b);
  glProjectVertex(c);
  if(UseCullFace && !isCullFace(a, b, c))return;

  ZCyt = 20.1 - glPerspective + 0;

  SortVertexZ(a, b, c, t);

  if(c->z > ZCyt)
  {
    if(a->z > ZCyt)
    {
      if(UseZ)
        glColorTriangle_z(a, b, c, Color);
      else
        glColorTriangle_u(a, b, c, Color);
    }
    else
    {
      if(b->z > ZCyt)
      {
        t1->x = fInt(b->x, a->x, b->z, a->z, ZCyt);
        t1->y = fInt(b->y, a->y, b->z, a->z, ZCyt);
        t1->z = ZCyt;

        t2->x = fInt(c->x, a->x, c->z, a->z, ZCyt);
        t2->y = fInt(c->y, a->y, c->z, a->z, ZCyt);
        t2->z = ZCyt;

        glProjectVertex(t2);
        glProjectVertex(t1);

        if(UseZ)
        {
          glColorTriangle_z(b, t1, c, Color);
          glColorTriangle_z(t1, t2, c, Color);
        }
        else
        {
          glColorTriangle_u(b, t1, c, Color);
          glColorTriangle_u(t1, t2, c, Color);
        }
      }
      else
      {
        t1->x = fInt(c->x, a->x, c->z, a->z, ZCyt);
        t1->y = fInt(c->y, a->y, c->z, a->z, ZCyt);
        t1->z = ZCyt;

        t2->x = fInt(c->x, b->x, c->z, b->z, ZCyt);
        t2->y = fInt(c->y, b->y, c->z, b->z, ZCyt);
        t2->z = ZCyt;

        glProjectVertex(t2);
        glProjectVertex(t1);

        if(UseZ)
          glColorTriangle_z(t1, t2, c, Color);
        else
          glColorTriangle_u(t1, t2, c, Color);
      }
    }
  }
}

void glTextureTriangle(PVertex a, PVertex b, PVertex c, PBitMap Tex)
{
  TVertex _t1, _t2;
  PVertex t, t1 = &_t1, t2 = &_t2;
  float ZCyt;

  glProjectVertex(a);
  glProjectVertex(b);
  glProjectVertex(c);
  if(UseCullFace && !isCullFace(a, b, c))return;

  ZCyt = 20.1 - glPerspective;

  SortVertexZ(a, b, c, t);

  if(c->z > ZCyt)
  {
    if(a->z > ZCyt)
    {
      if(UseZ)
        glTextureTriangle_z(a, b, c, Tex);
      else
        glTextureTriangle_u(a, b, c, Tex);
    }
    else
    {
      if(b->z > ZCyt)
      {
        t1->x = fInt(b->x, a->x, b->z, a->z, ZCyt);
        t1->y = fInt(b->y, a->y, b->z, a->z, ZCyt);
        t1->s = fInt(b->s, a->s, b->z, a->z, ZCyt);
        t1->t = fInt(b->t, a->t, b->z, a->z, ZCyt);
        t1->z = ZCyt;

        t2->x = fInt(c->x, a->x, c->z, a->z, ZCyt);
        t2->y = fInt(c->y, a->y, c->z, a->z, ZCyt);
        t2->s = fInt(c->s, a->s, c->z, a->z, ZCyt);
        t2->t = fInt(c->t, a->t, c->z, a->z, ZCyt);
        t2->z = ZCyt;

        glProjectVertex(t2);
        glProjectVertex(t1);

        if(UseZ)
        {
          glTextureTriangle_z(b, t1, c, Tex);
          glTextureTriangle_z(t1, t2, c, Tex);
        }
        else
        {
          glTextureTriangle_u(b, t1, c, Tex);
          glTextureTriangle_u(t1, t2, c, Tex);
        }
      }
      else
      {
        t1->x = fInt(c->x, a->x, c->z, a->z, ZCyt);
        t1->y = fInt(c->y, a->y, c->z, a->z, ZCyt);
        t1->s = fInt(c->s, a->s, c->z, a->z, ZCyt);
        t1->t = fInt(c->t, a->t, c->z, a->z, ZCyt);
        t1->z = ZCyt;

        t2->x = fInt(c->x, b->x, c->z, b->z, ZCyt);
        t2->y = fInt(c->y, b->y, c->z, b->z, ZCyt);
        t2->s = fInt(c->s, b->s, c->z, b->z, ZCyt);
        t2->t = fInt(c->t, b->t, c->z, b->z, ZCyt);
        t2->z = ZCyt;

        glProjectVertex(t2);
        glProjectVertex(t1);

        if(UseZ)
          glTextureTriangle_z(t1, t2, c, Tex);
        else
          glTextureTriangle_u(t1, t2, c, Tex);
      }
    }
  }
}

void glGradientTriangle(PVertex a, PVertex b, PVertex c)
{
  TVertex _t1, _t2;
  PVertex t, t1 = &_t1, t2 = &_t2;
  float ZCyt;

  glProjectVertex(a);
  glProjectVertex(b);
  glProjectVertex(c);
  if(UseCullFace && !isCullFace(a, b, c))return;

  ZCyt = 20.1 - glPerspective;

  SortVertexZ(a, b, c, t);

  if(c->z > ZCyt)
  {
    if(a->z > ZCyt)
    {
      if(UseZ)
        glGradientTriangle_z(a, b, c);
      else
        glGradientTriangle_u(a, b, c);
    }
    else
    {
      if(b->z > ZCyt)
      {
        t1->x = fInt(b->x, a->x, b->z, a->z, ZCyt);
        t1->y = fInt(b->y, a->y, b->z, a->z, ZCyt);
        t1->Color = fInt(b->Color, a->Color, b->z, a->z, ZCyt);
        t1->z = ZCyt;

        t2->x = fInt(c->x, a->x, c->z, a->z, ZCyt);
        t2->y = fInt(c->y, a->y, c->z, a->z, ZCyt);
        t2->Color = fInt(c->Color, a->Color, c->z, a->z, ZCyt);
        t2->z = ZCyt;

        glProjectVertex(t2);
        glProjectVertex(t1);

        if(UseZ)
        {
          glGradientTriangle_z(b, t1, c);
          glGradientTriangle_z(t1, t2, c);
        }
        else
        {
          glGradientTriangle_u(b, t1, c);
          glGradientTriangle_u(t1, t2, c);
        }
      }
      else
      {
        t1->x = fInt(c->x, a->x, c->z, a->z, ZCyt);
        t1->y = fInt(c->y, a->y, c->z, a->z, ZCyt);
        t1->Color = fInt(c->Color, a->Color, c->z, a->z, ZCyt);
        t1->z = ZCyt;

        t2->x = fInt(c->x, b->x, c->z, b->z, ZCyt);
        t2->y = fInt(c->y, b->y, c->z, b->z, ZCyt);
        t2->Color = fInt(c->Color, b->Color, c->z, b->z, ZCyt);
        t2->z = ZCyt;

        glProjectVertex(t2);
        glProjectVertex(t1);

        if(UseZ)
          glGradientTriangle_z(t1, t2, c);
        else
          glGradientTriangle_u(t1, t2, c);
      }
    }
  }
}