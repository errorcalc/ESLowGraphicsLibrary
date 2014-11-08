#ifndef _ESGL2_H_
#define _ESGL2_H_

#include "Graphics.h"

typedef struct
{
  float x, y, z;// 3d coord
  float s, t;// tex coord
  float Color;
  float w;
  int sx, sy;//project coord
} TVertex, *PVertex;

typedef struct
{
  float x, y, z;
} TPoint3f, *PPoint3f;

//
void glSetCullFace(int IsUse);
int glGetCullFace(void);
void glSetZBuffer(int IsUse);
int glGetZBuffer(void);
void glSetDisp(PBitMap _Disp);
PBitMap glGetDisp(void);
void glSetPerspective(float Perspective);
float glGetPerspective(void);
void glSetCenter(TPoint3f _Center);
TPoint3f glGetCenter(void);
//
void glZoom(PVertex Vertex, float x, float y, float z);
void glArrayZoom(PVertex pVertex, int Count, float x, float y, float z);
void glRotate(PVertex Vertex, float ax, float ay, float az);
void glArrayRotate(PVertex pVertex, int Count, float ax, float ay, float az);
void glTranslate(PVertex Vertex, float x, float y, float z);
void glArrayTranslate(PVertex pVertex, int Count, float x, float y, float z);
//
void glArrayCopy(PVertex Dst, const PVertex Src, int Count);
PVertex glArrayCreate(int Count);
PVertex glArrayFree(PVertex p);
//
void glClearZBuffer(void);
void glProjectVertex(PVertex V);
int isCullFace(PVertex a, PVertex b, PVertex c);
//
void glColorTriangle(PVertex a, PVertex b, PVertex c, TColor Color);
void glTextureTriangle(PVertex a, PVertex b, PVertex c, PBitMap Tex);
void glGradientTriangle(PVertex a, PVertex b, PVertex c);
//

#endif