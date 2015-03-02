/*******************************************************************************
**                     ErrorSoftLowGraphicsLibrary v0.8.1                     **
**         Free using and editing in non commercial project.                  **
**                                                                            **
**    errorsoft@protonmail.ch or errorsoft@mail.ru or Enter256@yandex.ru      **
**                             - for information of use in commerital project **
**                                                                            **
** Supported pixel formats: 1, 2*, 4 bit for pixel.                           **
** Supported only little-endian, other in future                              **
** * - Support in future                                                      **
**                                                                            **
**                          2009-2014 ErrorSoft(c)                            **
*******************************************************************************/

#include "Graphics.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"
#include "IntMath.h"

static int IsCheckCoord = 1;// yes
static int IsTranslate = 0;// no
static int CenterX = 0;
static int CenterY = 0;
static int IsSLP = 0;// no

//******************************************************************************
// F**** MACROS
//******************************************************************************

#ifndef USE_SAFE_GRAPHICS
#define UNSAFE_SYSTEM_GRAPHICS
#endif

//******************************************************************************
// Getters and setters
//******************************************************************************

void esSetCheckCoord(int _Check)
{
  IsCheckCoord = _Check;
}

int esGetCheckCoord(void)
{
  return IsCheckCoord;
}

void esSetTranslate(int _Translate)
{
  IsTranslate = _Translate;
}

int esGetTranslate(void)
{
  return IsTranslate;
}

void esSetCenter(int x, int y)
{
  CenterX = x;
  CenterY = y;
}

void esGetCenter(int *x, int *y)
{
  *x = CenterX;
  *y = CenterY;
}

void esTranslateCenter(int x, int y)
{
  CenterX += x;
  CenterY += y;
}

void esSetSkipLastPixel(int _IsSLP)
{
  IsSLP = _IsSLP; 
}

int esGetSkipLastPixel(void)
{
  return IsSLP;
}

//******************************************************************************
// Utils
//******************************************************************************


// is entry segments 
int esIsEntrySeg(int a1, int a2, int b1, int b2)
{
  // a1 ---|--- a2
  //       b1 --|-------- b2
  return ((a1 >= b1)&&(a1 <= b2))||((a2 >= b1)&&(a2 <= b2));
}

// 1-Ok, 0-NoEntry
//
// x1, y1 +--------------+
//   --   |              |
//     \-----------      |   x2, y2
//        |        \--------
//        +--------------+
//  f(x) = f(x0) + (f(x1)-f(x0))/(x1-x0) * (x-x0)       
//
// x1, y1 - cliped point
int esClipPoint(int w, int h, int *x1, int *y1, int *x2, int *y2)
{
  if(*x1 < 0 && *x2 >= 0)
  {
    *y1 = *y1 + (*y2-*y1)/(float)(*x2-*x1) * (float)(0-*x1);
    *x1 = 0;
  }

  if(*x1 >= w && *x2 < w)
  {
    *y1 = *y1 + (*y2-*y1)/(float)(*x2-*x1) * (float)(w-*x1-1);
    *x1 = w-1;
  }

  if(*y1 < 0 && *y2 >= 0)
  {
    *x1 = *x1 + (*x2-*x1)/(float)(*y2-*y1) * (float)(0-*y1);
    *y1 = 0;
  }

  if(*y1 >= h && *y2 < h)
  {
    *x1 = *x1 + (*x2-*x1)/(float)(*y2-*y1) * (float)(h-*y1-1);
    *y1 = h-1;
  }
  return 0;
}
// FUCKED MACROS
// x1, y1 - cliped point
#define ES_CLIP_POINT(w, h, x1, y1, x2, y2)    \
{                                              \
  if((x1) < 0 && (x2) >= 0)                    \
  {                                            \
    y1 = ( y1 + (float)(y2-y1)/(float)(x2-x1) * (float)(0-x1) ); \
    x1 = 0;                                    \
  }                                            \
                                               \
  if(x1 >= w && x2 < w)                        \
  {                                            \
    y1 = ( y1 + (float)(y2-y1)/(float)(x2-x1) * (float)(w-x1-1) ); \
    x1 = w-1;                                  \
  }                                            \
                                               \
  if(y1 < 0 && y2 >= 0)                        \
  {                                            \
    x1 = ( x1 + (float)(x2-x1)/(float)(y2-y1) * (float)(0-y1) ); \
    y1 = 0;                                    \
  }                                            \
                                               \
  if(y1 >= h && y2 < h)                        \
  {                                            \
    x1 = ( x1 + (float)(x2-x1)/(float)(y2-y1) * (float)(h-y1-1) ); \
    y1 = h-1;                                  \
  }                                            \
} 

// to do: govnocode
// return:
// -1: no cliped
// 0: all cliped
// 1: cliped point one
// 2: cliped point two
// 3: cliped all points
int esClipLine(int w, int h, int *x1, int *y1, int *x2, int *y2)
{
  int p1, p2;

  p2 = (*x2 >= 0 && *x2 < w)&&
    (*y2 >= 0 && *y2 < h);
  p1 = (*x1 >= 0 && *x1 < w)&&
    (*y1 >= 0 && *y1 < h);

  if(p1 && p2)return -1;// ok, no cliped

  p1 = !p1;
  p2 = !p2;

  //esClipPoint(w, h, x1, y1, x2, y2);
  //esClipPoint(w, h, x2, y2, x1, y1);
  if(p1)ES_CLIP_POINT(w, h, *x1, *y1, *x2, *y2);
  if(p2)ES_CLIP_POINT(w, h, *x2, *y2, *x1, *y1);

  if(
    (*x1 < 0 || *x1 >= w)||
    (*y1 < 0 || *y1 >= h)||
    (*x2 < 0 || *x2 >= w)||
    (*y2 < 0 || *y2 >= h)
    )return 0;// error, all cliped

  //return 1;// ok, cliped
  if(p2)
    if(p1)return 3;
      else return 2;
  else
    return 1;
}


unsigned char esFillByte(TColor Color, TPixelFormat pf)
{
  switch(pf)
  {
    case pf1bit:
      return Color?0xFF:0x00;

    case pf2bit:;
    case pfC2:// fix me: add ???
      switch(Color)
      {
        case 0:
          return 0x00;
        case 1:
          return 0x55;
        case 2:
          return 0xAA;
        case 3:
          return 0xFF;
      }
      
    case pf4bit:
      return (Color&0x0F)|((Color<<4)&0xF0); 
    default:;
  }  
}

// to do: test
int esIsRealColor(TColor Color, TPixelFormat pf)
{
  int isReal = 1;
  switch(pf)
  {
    case pf1bit:
      if(Color > 1)isReal = 0;
      break;
    case pf2bit:;
    case pfC2:
      if(Color > 3)isReal = 0;
      break; 
    case pf4bit:
      if(Color > 15)isReal = 0;
      break;    
  }
  return isReal;
}

// note: if edit this function then edit ES_SCANLINE_PF in "Graphics.h"
#ifndef USE_SCANLINE_MACRO
int esScanlinePF(int Width, TPixelFormat pf)
{
  switch(pf)
  {
    case pf1bit:
      return Scanline1(Width);
    case pf2bit:;
    case pfC2:
      return Scanline2(Width);    
    case pf4bit:
      return Scanline4(Width);
  }
}

int esScanline(PBitMap BitMap)
{
  return ES_SCANLINE_PF(BitMap->Width, BitMap->PixelFormat);
}
#endif

TRect esGetBitMapRect(PBitMap BitMap)
{
  TRect R;
  
  R.x1 = 0;
  R.y1 = 0;
  R.x2 = BitMap->Width;
  R.y2 = BitMap->Height;
  
  return R;
}

int esPFToBitCount(TPixelFormat pf)
{
  switch(pf)
  {
    case pf1bit:
      return 1;
    case pf2bit:;
    case pfC2:
      return 2;    
    case pf4bit:
      return 4;
    default:
      return -1;
  }
}

TPixelFormat esBitCountToPF(int Count)
{
  switch(Count)
  {
    case 1:
      return pf1bit;
    case 2:
      return pf2bit;    
    case 4:
      return pf4bit;
    default: 
      return pfUnsupport;
  }
}

//******************************************************************************
// System graphics
//******************************************************************************

// 1 bit pf

/*
// Need refactor
void esSetPixel1(PBitMap BitMap, int x, int y, TColor color)
{
  const unsigned char byte[] = { 128, 64, 32, 16, 8, 4, 2, 1};
  unsigned char *Pointer;
 
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif
  //1 method: BitMap.Pixels[ (BitMap.Width/8)*y + (x/8)] |= b;
  //2 method: BitMap.Pixels[ (BitMap.Width/8)*y + (x/8)] |= (0x80>>(x%8));
  //3 method: BitMap.Pixels[WidthToScanY(BitMap.Width)*y + (x/8)] |= 128;
  //Pixel = BitMap->Pixels + Scanline1(BitMap->Width)*y + (x/8);
  #ifdef USE_PRECALC_SCANLINE
  Pointer = BitMap->Pixels + BitMap->Scanline*y + (x/8);
  #else
  Pointer = BitMap->Pixels + Scanline1(BitMap->Width)*y + (x/8);
  #endif

  switch(color)
  {
    case 0:
      switch(x%8)
      {
        case 0: *Pointer &= 0x7F; break;
        case 1: *Pointer &= 0xBF; break;
        case 2: *Pointer &= 0xDF; break;
        case 3: *Pointer &= 0xEF; break;
        case 4: *Pointer &= 0xF7; break;
        case 5: *Pointer &= 0xFB; break;
        case 6: *Pointer &= 0xFD; break;
        case 7: *Pointer &= 0xFE; break;
      }
      break;   
    case 1:
      switch(x%8)
      {
        case 0: *Pointer |= 128; break;
        case 1: *Pointer |= 64; break;
        case 2: *Pointer |= 32; break;
        case 3: *Pointer |= 16; break;
        case 4: *Pointer |= 8; break;
        case 5: *Pointer |= 4; break;
        case 6: *Pointer |= 2; break;
        case 7: *Pointer |= 1; break;
      }
      break;
    case 2:
      switch(x%8)
      {
        case 0: *Pointer ^= 128; break;
        case 1: *Pointer ^= 64; break;
        case 2: *Pointer ^= 32; break;
        case 3: *Pointer ^= 16; break;
        case 4: *Pointer ^= 8; break;
        case 5: *Pointer ^= 4; break;
        case 6: *Pointer ^= 2; break;
        case 7: *Pointer ^= 1; break;
      }
      break; 
  }
}
*/

// Need refactor
void esSetPixel1(PBitMap BitMap, int x, int y, TColor color)
{
  //const unsigned char byte[] = { 128, 64, 32, 16, 8, 4, 2, 1};
  unsigned char *p;
 
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif
  //1 method: BitMap.Pixels[ (BitMap.Width/8)*y + (x/8)] |= b;
  //2 method: BitMap.Pixels[ (BitMap.Width/8)*y + (x/8)] |= (0x80>>(x%8));
  //3 method: BitMap.Pixels[WidthToScanY(BitMap.Width)*y + (x/8)] |= 128;
  //Pixel = BitMap->Pixels + Scanline1(BitMap->Width)*y + (x/8);
  #ifdef USE_PRECALC_SCANLINE
  p = BitMap->Pixels + BitMap->Scanline * y + x / 8;
  #else
  p = BitMap->Pixels + Scanline1(BitMap->Width)*y + x / 8;
  #endif

  switch(color)
  {
    case 0:
      *p &= ~(0x80 >> (x%8));
      break;   
    case 1:
      *p |= (0x80 >> (x%8));
      break;
    case 2:
      *p ^= (0x80 >> (x%8));
      break; 
  }
}

// Need refactor
void esDrawPixel1_Or(PBitMap BitMap, int x, int y, TColor color)
{
  //const unsigned char byte[] = { 128, 64, 32, 16, 8, 4, 2, 1};
  unsigned char *p;
 
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  p = BitMap->Pixels + BitMap->Scanline * y + x / 8;
  #else
  p = BitMap->Pixels + Scanline1(BitMap->Width)*y + x / 8;
  #endif

  *p |= color << (7 - x%8);
}

// Need refactor
void esDrawPixel1_Xor(PBitMap BitMap, int x, int y, TColor color)
{
  //const unsigned char byte[] = { 128, 64, 32, 16, 8, 4, 2, 1};
  unsigned char *p;
 
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  p = BitMap->Pixels + BitMap->Scanline * y + x / 8;
  #else
  p = BitMap->Pixels + Scanline1(BitMap->Width)*y + x / 8;
  #endif

  *p ^= color << (7 - x%8);
} 

// Need refactor !!!!!!!!!
void esDrawPixel1_And(PBitMap BitMap, int x, int y, TColor color)
{
  //const unsigned char byte[] = { 128, 64, 32, 16, 8, 4, 2, 1};
  unsigned char *p;
 
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  p = BitMap->Pixels + BitMap->Scanline * y + x / 8;
  #else
  p = BitMap->Pixels + Scanline1(BitMap->Width)*y + x / 8;
  #endif

  *p &= ~(0x80 >> (x%8)) | color << (7 - x%8);
}

// Need refactor
TColor esGetPixel1(PBitMap BitMap, int x, int y)
{
  unsigned char b;
 
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return 0;
  #endif
  #ifdef USE_PRECALC_SCANLINE
  b = *( BitMap->Pixels + BitMap->Scanline*y + (x/8) );
  #else
  b = *( BitMap->Pixels + Scanline1(BitMap->Width)*y + (x/8) );
  #endif
  // return (b >> (7-x%8))&0x01;

  return (b>>(7-x%8))&0x01;
}

void esSetPixel2(PBitMap BitMap, int x, int y, TColor color)
{
  unsigned char *Pixel;
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif
  
  #ifdef USE_PRECALC_SCANLINE
  Pixel = BitMap->Pixels + BitMap->Scanline*y + (x>>2);
  #else
  Pixel = BitMap->Pixels + Scanline2(BitMap->Width)*y + (x>>2);
  #endif
  
  switch(x%4)
  {
    case 0: *Pixel = (*Pixel & 0x3F)|(color<<6); break;
    case 1: *Pixel = (*Pixel & 0xCF)|(color<<4); break;
    case 2: *Pixel = (*Pixel & 0xF3)|(color<<2); break;
    case 3: *Pixel = (*Pixel & 0xFC)|(color<<0); break;
  }
}

// Need refactor
// test
TColor esGetPixel2(PBitMap BitMap, int x, int y)
{
  unsigned char b;
 
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return 0;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  b = *( BitMap->Pixels + BitMap->Scanline*y + (x/4) );
  #else
  b = *( BitMap->Pixels + Scanline2(BitMap->Width)*y + (x/4) );
  #endif

  // return (b >> (3-x%4))&0x03;
  switch(x%4)
  {
    case 0: return (b>>3)&0x03;
    case 1: return (b>>2)&0x03;
    case 2: return (b>>1)&0x03;
    case 3: return (b>>0)&0x03;
  }
}

// 4 bit pf

/*
void esSetPixel4(PBitMap BitMap, int x, int y, TColor color)
{
  unsigned char *Pixel;
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  Pixel = BitMap->Pixels + BitMap->Scanline*y + (x>>1);
  #else
  Pixel = BitMap->Pixels + Scanline4(BitMap->Width)*y + (x>>1);
  #endif

  switch(x%2)
  {
    case 0: *Pixel = (*Pixel & 0x0F)|(color<<4); break;
    case 1: *Pixel = (*Pixel & 0xF0)|(color<<0); break;
  }
}*/

void esSetPixel4(PBitMap BitMap, int x, int y, TColor color)
{
  unsigned char *Byte;

  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  Byte = BitMap->Pixels + BitMap->Scanline * y + (x >> 1);
  #else
  Byte = BitMap->Pixels + Scanline4(BitMap->Width) * y + (x >> 1);
  #endif

  switch(x%2)
  {
    case 0: *Byte = (*Byte & 0x0F) | (color << 4); break;
    case 1: *Byte = (*Byte & 0xF0) | (color << 0); break;
  }
}

void esDrawPixel4_And(PBitMap BitMap, int x, int y, TColor color)
{
  unsigned char *Byte;

  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  Byte = BitMap->Pixels + BitMap->Scanline * y + (x >> 1);
  #else
  Byte = BitMap->Pixels + Scanline4(BitMap->Width) * y + (x >> 1);
  #endif

  switch(x%2)
  {
    case 0: *Byte = *Byte & (0x0F | (color << 4)); break;
    case 1: *Byte = *Byte & (0xF0 | (color << 0)); break;
  }
}

void esDrawPixel4_Or(PBitMap BitMap, int x, int y, TColor color)
{
  unsigned char *Byte;

  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  Byte = BitMap->Pixels + BitMap->Scanline * y + (x >> 1);
  #else
  Byte = BitMap->Pixels + Scanline4(BitMap->Width) * y + (x >> 1);
  #endif

  switch(x%2)
  {
    case 0: *Byte = *Byte | (color << 4); break;
    case 1: *Byte = *Byte | (color << 0); break;
  }
}

void esDrawPixel4_Xor(PBitMap BitMap, int x, int y, TColor color)
{
  unsigned char *Byte;

  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  Byte = BitMap->Pixels + BitMap->Scanline * y + (x >> 1);
  #else
  Byte = BitMap->Pixels + Scanline4(BitMap->Width) * y + (x >> 1);
  #endif

  switch(x%2)
  {
    case 0: *Byte = *Byte ^ (color << 4); break;
    case 1: *Byte = *Byte ^ (color << 0); break;
  }
}

/*
// Need refactor
TColor esGetPixel1(PBitMap BitMap, int x, int y)
{
  unsigned char b;
 
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return 0;
  #endif
  #ifdef USE_PRECALC_SCANLINE
  b = *( BitMap->Pixels + BitMap->Scanline*y + (x/8) );
  #else
  b = *( BitMap->Pixels + Scanline1(BitMap->Width)*y + (x/8) );
  #endif
  // return (b >> (7-x%8))&0x01;
  switch(x%8)
  {
    case 0: return (b>>7)&0x01;
    case 1: return (b>>6)&0x01;
    case 2: return (b>>5)&0x01;
    case 3: return (b>>4)&0x01;
    case 4: return (b>>3)&0x01;
    case 5: return (b>>2)&0x01;
    case 6: return (b>>1)&0x01;
    case 7: return (b>>0)&0x01;
  }
}
*/

// Need refactor
// test
TColor esGetPixel4(PBitMap BitMap, int x, int y)
{
  unsigned char b;
 
  #ifndef UNSAFE_SYSTEM_GRAPHICS
  if(IsCheckCoord)
    if(x < 0 || x >= BitMap->Width || y < 0 || y >= BitMap->Height)return 0;
  #endif

  #ifdef USE_PRECALC_SCANLINE
  b = *( BitMap->Pixels + BitMap->Scanline*y + (x/2) );
  #else
  b = *( BitMap->Pixels + Scanline4(BitMap->Width)*y + (x/2) );
  #endif

  // return (b >> (1-x%2))&0x07;
  switch(x%2)
  {
    case 0: return (b>>4)&0x0F;
    case 1: return (b>>0)&0x0F;
  }
}

void esFillPixels(unsigned char *pStart, unsigned char *pEnd, TColor Color)
{
  unsigned char *p;
  p = pStart;
  while(p <= pEnd)
    *(p++) = Color;
}

// UnSafe
void esDrawHLine1(PBitMap BitMap, int x1, int x2, int y, TColor Color)
{
  int x, t;
  unsigned char *scanline, *p, *pEnd;
  unsigned char b;
   
  if(x1 > x2)SWAP(x1, x2, t);
  
  if(esIsRealColor(Color, BitMap->PixelFormat) && x2 - x1 > 7)
  {
    // ...->|
    for(; x1%8; x1++)
      esSetPixel1(BitMap, x1, y, Color);
    // |<-...    
    for(; (x2+1)%8; x2--)
      esSetPixel1(BitMap, x2, y, Color); 
      
    // bytes draw   
    b = esFillByte(Color, BitMap->PixelFormat);// set color to byte    
    
    #ifdef USE_PRECALC_SCANLINE
    scanline = BitMap->Scanline*y + BitMap->Pixels;
    #else
    scanline = esScanlinePF(BitMap->Width, BitMap->PixelFormat)*y + BitMap->Pixels;
    #endif

    p = scanline + x1/8;
    pEnd = scanline + (x2+1)/8;
    
    while(p < pEnd)
      *(p++) = b;
  }else
  {
    for(x = x1; x <= x2; x++)
      esSetPixel1(BitMap, x, y, Color);  
  } 
}

// UnSafe
void esDrawHLine2(PBitMap BitMap, int x1, int x2, int y, TColor Color)
{
  int x, t;
  unsigned char *scanline, *p, *pEnd;
  unsigned char b;
   
  if(x1 > x2)SWAP(x1, x2, t);
  if(esIsRealColor(Color, BitMap->PixelFormat) && x2 - x1 > 3)
  {
    // ...->|
    for(; x1%4; x1++)
      esSetPixel2(BitMap, x1, y, Color);
    // |<-...    
    for(; (x2+1)%4; x2--)
      esSetPixel2(BitMap, x2, y, Color); 
      
    // bytes draw   
    b = esFillByte(Color, BitMap->PixelFormat);// set color to byte    
    
    #ifdef USE_PRECALC_SCANLINE
    scanline = BitMap->Scanline*y + BitMap->Pixels;
    #else
    scanline = esScanlinePF(BitMap->Width, BitMap->PixelFormat)*y 
      + BitMap->Pixels;
    #endif
    
    p = scanline + x1/4;//(x1-1)/4 + 1;
    pEnd = scanline + (x2+1)/4;//(x2+1)/4;
    
    while(p < pEnd)
      *(p++) = b;
  }else
  {
    for(x = x1; x <= x2; x++)
      esSetPixel2(BitMap, x, y, Color);  
  } 
}

// UnSafe
void esDrawHLine4(PBitMap BitMap, int x1, int x2, int y, TColor Color)
{
  int x, t;
  unsigned char *scanline, *p, *pEnd;
  unsigned char b;
   
  if(x1 > x2)SWAP(x1, x2, t);
  if(esIsRealColor(Color, BitMap->PixelFormat) && x2 - x1 > 1)
  {
    // ...->|
    for(; x1%2; x1++)
      esSetPixel4(BitMap, x1, y, Color);
    // |<-...    
    for(; (x2+1)%2; x2--)
      esSetPixel4(BitMap, x2, y, Color); 
      
    // bytes draw   
    b = esFillByte(Color, BitMap->PixelFormat);// set color to byte

    #ifdef USE_PRECALC_SCANLINE
    scanline = BitMap->Scanline*y + BitMap->Pixels;
    #else
    scanline = esScanlinePF(BitMap->Width, BitMap->PixelFormat)*y + BitMap->Pixels;
    #endif

    p = scanline + x1/2;
    pEnd = scanline + (x2+1)/2;
    
    while(p < pEnd)
      *(p++) = b;
  }else
  {
    for(x = x1; x <= x2; x++)
      esSetPixel4(BitMap, x, y, Color);  
  } 
}

// UnSafe
void esDrawHLine(PBitMap BitMap, int x1, int x2, int y, TColor Color)
{
  switch(BitMap->PixelFormat)
  {
    case pf1bit:
      esDrawHLine1(BitMap, x1, x2, y, Color);
      break;
    case pf2bit:;
    case pfC2:
      esDrawHLine2(BitMap, x1, x2, y, Color);
      break;
    case pf4bit:
      esDrawHLine4(BitMap, x1, x2, y, Color);
      break;
  }
}

static const unsigned char PixelMask1[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
static const unsigned char PixelMask2[4] = {0xC0, 0x30, 0x0C, 0x03};
static const unsigned char PixelMask4[2] = {0xF0, 0x0F};

// UnSafe
void esDrawVLine(PBitMap BitMap, int y1, int y2, int x, TColor Color)
{
  unsigned char *p, b, bc, bn;
  int scanline, y, t;
  
  if(y1 > y2)
    SWAP(y1, y2, t);


  // if(y1 < 0)y1 = 0;/// !!!!!!!!!!!!!!!
  //if(y2 >= BitMap->Height)y2 = BitMap->Height - 1;// to do: fix me: ВРЕМЕННАЯ ЗАПЛАТКА ЧТОБЫ DrawRect НЕ ПОГАНИЛ КУЧУ!!!!!!!!!!!!!!!!!

  if(esIsRealColor(Color, BitMap->PixelFormat))
  { 
    #ifdef USE_PRECALC_SCANLINE
    scanline = BitMap->Scanline;
    #else
    scanline = esScanlinePF(BitMap->Width, BitMap->PixelFormat);
    #endif
    
    switch(BitMap->PixelFormat)
    {
      case pf1bit:
        p = BitMap->Pixels + scanline*y1 + x/8;
        b = PixelMask1[x%8];
        break;
      case pf2bit:;
      case pfC2:
        p = BitMap->Pixels + scanline*y1 + x/4;
        b = PixelMask2[x%4];
        break;
      case pf4bit:
        p = BitMap->Pixels + scanline*y1 + x/2;
        b = PixelMask4[x%2]; 
        break;
    }      
    bc = b & esFillByte(Color, BitMap->PixelFormat);
    bn = ~b;
 
    for(y = y1; y <= y2; y++)
    {
      *p = (*p&(bn)) | bc;
      p = p + scanline;
    }
  }
  else
    switch(BitMap->PixelFormat)
    {
      case pf1bit:
        for(y = y1; y <= y2; y++)
          esSetPixel1(BitMap, x, y, Color);
        break;
      case pf2bit:;
      case pfC2:
        for(y = y1; y <= y2; y++)
          esSetPixel2(BitMap, x, y, Color);
        break;
      case pf4bit:
        for(y = y1; y <= y2; y++)
          esSetPixel4(BitMap, x, y, Color);
        break;
    } 
}

// todo: UP SPEED
void esDrawLineU(PBitMap BitMap, int x1, int y1, int x2, int y2, TColor Color)
{
  int t, dist, x_error, y_error, dx, dy, x_inc, y_inc; 
  
  dx = x2 - x1;
  dy = y2 - y1;
  if(dx > 0)x_inc = 1; else x_inc = -1;
  if(dx == 0)x_inc = 0;
  if(dy > 0)y_inc = 1; else y_inc = -1;
  if(dy == 0)y_inc = 0;
  dx = ABS(dx);
  dy = ABS(dy);
  dist = MAX(dx, dy);
  
  x_error = dist>>1;// dist/2;
  y_error = dist>>1;// dist/2;
 
  for(t = 0; t < dist; t++)
  {
    esSetPixel(BitMap, x1, y1, Color);

    x_error = x_error + dx;
    y_error = y_error + dy;
    if(x_error >= dist)
    {
      x_error = x_error - dist;
      x1 = x1 + x_inc;
    }
    if(y_error >= dist)
    {
      y_error = y_error - dist;
      y1 = y1 + y_inc;
    }
  }
  #ifdef SKIP_LAST_PIXEL
  if(!IsSLP)
    esSetPixel(BitMap, x1, y1, Color);
  #else
  esSetPixel(BitMap, x1, y1, Color);
  #endif

}

//******************************************************************************
// Main graphics
//******************************************************************************

// bw
void esClear(PBitMap BitMap, TColor Color)
{
  unsigned char b;
  switch(BitMap->PixelFormat)
  {
    case pf1bit:
      b = esFillByte(Color, BitMap->PixelFormat);
      memset(BitMap->Pixels, b, Scanline1(BitMap->Width)*BitMap->Height);
      break;
    
    case pf2bit:;
    case pfC2:
      b = esFillByte(Color, BitMap->PixelFormat);
      memset(BitMap->Pixels, b, Scanline2(BitMap->Width)*BitMap->Height);
      break;
      
    case pf4bit:; 
      b = esFillByte(Color, BitMap->PixelFormat); 
      memset(BitMap->Pixels, b, Scanline4(BitMap->Width)*BitMap->Height);
      break;
  }    
}

void esSetPixel(PBitMap BitMap, int x, int y, TColor color)
{ 
  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif
  switch(BitMap->PixelFormat)
  {
    case pf1bit:
      esSetPixel1(BitMap, x, y, color);
      break;
    
    case pf2bit:;
    case pfC2:
      esSetPixel2(BitMap, x, y, color);
      break;
      
    case pf4bit:
      esSetPixel4(BitMap, x, y, color);
      break;
  }  
}

TColor esGetPixel(PBitMap BitMap, int x, int y)
{ 
  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif
  switch(BitMap->PixelFormat)
  {
    case pf1bit:
      return esGetPixel1(BitMap, x, y);
    
    case pf2bit:;
    case pfC2:
      return esGetPixel2(BitMap, x, y);
      
    case pf4bit:
      return esGetPixel4(BitMap, x, y);
  }  
}

void esFillRect(PBitMap BitMap, int x1, int y1, int x2, int y2, TColor Color)
{
  int y, t;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(x1, y1, x2, y2, CenterX, CenterY);
  #endif
  
  #ifdef SKIP_LAST_PIXEL
  if(IsSLP)DECREMENT_POINT(x2, y2);
  #endif

  #ifdef USE_SAFE_GRAPHICS  
  if(IsCheckCoord)
  {
    NORMALIZE_RECT(x1, y1, x2, y2, t);
    if(x1 >= BitMap->Width || y1 >= BitMap->Height || x2 < 0 || y2 < 0 || x1 == x2)
      return;
    CLIP_POINT(x1, y1, 0, 0, BitMap->Width, BitMap->Height);
    CLIP_POINT(x2, y2, 0, 0, BitMap->Width, BitMap->Height);
  }
  #endif

  switch(BitMap->PixelFormat)
  {
    case pf1bit:
      for(y = y1; y <= y2; y++)
        esDrawHLine1(BitMap, x1, x2, y, Color);
      break;

    case pf2bit:;
    case pfC2:
      for(y = y1; y <= y2; y++)
        esDrawHLine2(BitMap, x1, x2, y, Color);
      break;
      
    case pf4bit:
      for(y = y1; y <= y2; y++)
        esDrawHLine4(BitMap, x1, x2, y, Color);
      break;
  } 
}

// SUPER GOVNOKODE FIX ME PLEASE!!!!!!!!!!!!!!!!!
void esDrawRect(PBitMap BitMap, int x1, int y1, int x2, int y2, TColor Color)
{
  int t;
  int IsTop = 1, IsBottom = 1, IsLeft, IsRight;
  int ty1, ty2, tx1, tx2;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(x1, y1, x2, y2, CenterX, CenterY);
  #endif  
  
  #ifdef SKIP_LAST_PIXEL
  if(IsSLP)DECREMENT_POINT(x2, y2);
  #endif
  
  #ifdef USE_SAFE_GRAPHICS 
  if(IsCheckCoord)
  {
    NORMALIZE_RECT(x1, y1, x2, y2, t);
    if(x1 >= BitMap->Width || y1 >= BitMap->Height || x2 < 0 || y2 < 0)
      return;
  }
  #endif

  IsTop = y1 >= 0;
  IsBottom = y2 < BitMap->Height;
  IsLeft = x1 >= 0;
  IsRight = x2 < BitMap->Width;

  if(IsTop)
  {
    //if(x2 - x1 != 1)
    {
      tx1 = x1 >= 0 ? x1 : 0;
      tx2 = x2 < BitMap->Width ? x2 : BitMap->Width - 1;
      esDrawHLine(BitMap, tx1, tx2, y1, Color);
    }
  }

  if(IsBottom)
  {
    //if(x2 - x1 != 1 && y1 != y2)
    {
      tx1 = x1 >= 0 ? x1 : 0;
      tx2 = x2 < BitMap->Width ? x2 : BitMap->Width - 1;
      esDrawHLine(BitMap, tx1, tx2, y2, Color);
    }
  }


  if(IsLeft)
  {
    if(y2 - y1 >= 1 && y1 + 1 < BitMap->Height && y2 - 1 >= 0)
    {
      ty1 = (y1 + 1) >= 0 ? y1 + 1 : 0;
      ty2 = (y2 - 1) < BitMap->Height ? y2 - 1: BitMap->Height - 1;
      //if(ty1 < ty2)
      esDrawVLine(BitMap, ty1, ty2, x1, Color);
    }
  }

  if(IsRight && x1 != x2)
  {
    if(y2 - y1 >= 1 && y1 + 1 < BitMap->Height && y2 - 1 >= 0)
    {
      ty1 = (y1 + 1) >= 0 ? y1 + 1 : 0;
      ty2 = (y2 - 1) < BitMap->Height ? y2 - 1: BitMap->Height - 1;
      //if(ty1 < ty2)
      esDrawVLine(BitMap, ty1, ty2, x2, Color);
    }
  }


  /*
  // Vertical lines
  if(x1 >= 1 || x2 < BitMap->Width)
  {
    if(y1 < 0)
    {
      y1 = 0;
      IsTop = 0;
    }
    if(y2 >= BitMap->Width)
    {
      y2 = BitMap->Width - 1;
      IsBottom = 0;
    }

    if(y1 != y2)
    {
      esDrawVLine(BitMap, y1, y2, x1, Color);
      esDrawVLine(BitMap, y1, y2, x2, Color);
    }
    else
    {
      esDrawVLine(BitMap, y1, y2, x1, Color);
      return;
    }
  }
  */
  /*
  // Horizontal
  if(IS_ENTRY_SEG(x1, x2, 0, BitMap->Width-1))
  {
    if(IsLeft = (x1 < 0))x1 = 0;
    if(IsRight = (x2 > BitMap->Width - 1))x2 = BitMap->Width - 1;
    if(IsTop    = (y1 >= 0 && y1 < BitMap->Height))esDrawHLine(BitMap, x1, x2, y1, Color);
    if(y1 != y2 && (IsBottom = (y2 >= 0 && y2 < BitMap->Height)))esDrawHLine(BitMap, x1, x2, y2, Color);
  }
  else
    return;// ????????????????????????????????????????????????????????????????????????????????
  
  // Vertical
  if(IS_ENTRY_SEG(y1, y2, 0, BitMap->Height-1))
  {
    if(IsTop)y1++;
    if(IsBottom)y2--;
    
    if(y1 < 0)y1 = 0;
    if(y2 > BitMap->Height - 1)y2 = BitMap->Height - 1;
    if(x1 >= 0 && x1 < BitMap->Width && !IsLeft)esDrawVLine(BitMap, y1, y2, x1, Color); 
    if(x1 != x2 && x2 >= 0 && x2 < BitMap->Width && !IsRight)esDrawVLine(BitMap, y1, y2, x2, Color);
  }*/
}

void esDrawLine(PBitMap BitMap, int x1, int y1, int x2, int y2, TColor Color)
{

//  #ifdef SKIP_LAST_PIXEL
//  if(IsSLP)DECREMENT_POINT(x2, y2);
//  #endif

  if(esClipLine(BitMap->Width, BitMap->Height, &x1, &y1, &x2, &y2))
  {
    esDrawLineU(BitMap, x1, y1, x2, y2, Color);
  }
}

/*
// to do: добавить контроль границ
void __esDrawCircle(PBitMap BitMap, int x1, int y1, int x2, int y2, unsigned char Color)
{
  int x = 0;
  int y = x2;
  int d = 2 - 2 * x2;
  int error = 0;

  error = d/2;

  while(y > 0)
  {
    esSetPixel(BitMap, x1 + x, y1 + y, Color);
    error = 2 * (d + y) - 1;
    if((d < 0) && (error <=0))
    {
      d += 2 * ++x + 1;
      continue;
    }

    if((d > 0) && (error > 0))
    {
      d += 1 - 2 * y--;
      continue;
    }

    x++;
    d += 2 * (x - y + 2);
    y--;
  }

}*/

/*
// to do: добавить контроль границ и использовать другой алгоритм
void ddesDrawElipse(PBitMap BitMap, int x1, int y1, int x2, int y2, unsigned char Color)
{
  int height, width;
  float j;
  int td, tdx, tdy, twidth, x, y;
  int cx, cy;
  int d = 0;
  float R;
 // dx = 0; dy = 0;

  width = x2 - x1;
  height = y2 - y1;

  cx = x1 + width / 2;
  cy = y1 + height / 2;

  R = (width > height ? height : height) / 2.0f;

  j = width < height ? width/(float)height : width/(float)height;
  
  for(y = y1; y < (y2+1)/2; y++)
  {
    x = (int)ceil( j*sqrt(R*R - (float)(y*y)));//x1;
    esSetPixel1(BitMap, cx + x, cy + y, Color);
    esSetPixel1(BitMap, cx + x, cy - y, Color);
    if(width % 2 != 0)
      x--;
    esSetPixel1(BitMap, cx - x, cy + y, Color);
    esSetPixel1(BitMap, cx - x, cy - y, Color);
  }

}

// GOLD
void llesDrawElipse(PBitMap BitMap, int x, int y, int a, int b, int color)
{
int col,i,row,bnew;
long a_square,b_square,two_a_square,two_b_square,four_a_square,four_b_square,d;
 

  if(a <= 0 || b <= 0)return;
  b_square=b*b;
  a_square=a*a;
  row=b;
  col=0;
  two_a_square=a_square<<1;
  four_a_square=a_square<<2;
  four_b_square=b_square<<2;
  two_b_square=b_square<<1;
  d=two_a_square*((row-1)*(row))+a_square+two_b_square*(1-a_square);

  while(a_square*(row)>b_square*(col) )
  {
    esSetPixel1(BitMap,col+x,row+y,color);
    esSetPixel1(BitMap,col+x,y-row,color);
    if(col != 0)
    {
      esSetPixel1(BitMap,x-col,row+y,color);
      esSetPixel1(BitMap,x-col,y-row,color);
    }
    if (d>=0)
    {
      row--;
      d-=four_a_square*(row);
    }
    d+=two_b_square*(3+(col<<1));
    col++;
  }

  d=two_b_square*(col+1)*col+two_a_square*(row*(row-2)+1)+(1-two_a_square)*b_square;
 
  while ((row) + 1)
  {
    esSetPixel1(BitMap,col+x,row+y,color);
    esSetPixel1(BitMap,x-col,row+y,color);
    if(row != 0)
    {
      esSetPixel1(BitMap,col+x,y-row,color);
      esSetPixel1(BitMap,x-col,y-row,color);
    }
    if (d<=0)
    {
      col++;
      d+=four_b_square*col;
    }
    row--;
    d+=two_a_square*(3-(row <<1));
  }
}*/


// to do: НЕ ВЕСЬ КОД - МОЙ, РАЗОБРАТЬСЯ\ПЕРЕПИСАТЬ
void esDrawElipse(PBitMap BitMap, int x1, int y1, int x2, int y2, unsigned char Color)
{
  int col, i, row, bnew, dx, dy, a, b, x, y, w, h, t;
  long a_square, b_square, two_a_square, two_b_square, four_a_square, four_b_square, d;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(x1, y1, x2, y2, CenterX, CenterY);
  #endif

  NORMALIZE_RECT(x1, y1, x2, y2, t);
  #ifdef SKIP_LAST_PIXEL
  if(IsSLP)y2--; // decrement x2 does LineU automatically
  if(x2 < x1 || y2 < y1)return;
  #endif

  w = x2 - x1;
  h = y2 - y1;

  if(h < 2)
  {
    if(h == 1)
    {
      esDrawLine(BitMap, x1, y1, x2, y1, Color);
      esDrawLine(BitMap, x1, y2, x2, y2, Color);
    }
    else
      esDrawLine(BitMap, x1, y1, x2, y1, Color);
    return;
  }

  if(w < 2)
  {
    if(w == 1)
    {
      esDrawLine(BitMap, x1, y1, x1, y2, Color);
      esDrawLine(BitMap, x2, y1, x2, y2, Color);
    }
    else
      esDrawLine(BitMap, x1, y1, x1, y2, Color);
    return;
  }

  a = w / 2;
  b = h / 2;
  x = x1 + a;
  y = y1 + b;

  b_square = b * b;
  a_square = a * a;
  row = b;
  col = 0;
  two_a_square = a_square * 2;
  four_a_square = a_square * 4;
  four_b_square = b_square * 4;
  two_b_square = b_square * 2;

  d = two_a_square * ((row - 1) * (row)) + a_square + two_b_square * (1 - a_square);
  while(a_square * (row) > b_square * (col))
  {
    dx = (w % 2) ? col + 1: col;
    dy = (h % 2) ? row + 1: row;
    esSetPixel(BitMap, x + dx, y + dy, Color);
    esSetPixel(BitMap, x + dx, y - row, Color);
    if(col != 0 || w % 2)
    {
      esSetPixel(BitMap, x - col, y + dy, Color);
      esSetPixel(BitMap, x - col, y - row, Color);
    }
    if(d >= 0)
    {
      row--;
      d -= four_a_square * (row);
    }
    d += two_b_square * (3 + (col * 2));
    col++;
  }

  d = two_b_square * (col + 1) * col + two_a_square * (row * (row - 2) + 1) + (1 - two_a_square) * b_square;
  while ((row) + 1)
  {
    dx = (w % 2) ? col + 1: col;
    dy = (h % 2) ? row + 1: row;
    esSetPixel(BitMap, x + dx, y + dy, Color);
    esSetPixel(BitMap, x - col, y + dy, Color);
    if(row != 0 || h % 2)
    {
      esSetPixel(BitMap, x + dx, y - row, Color);
      esSetPixel(BitMap, x - col, y - row, Color);
    }
    if(d <= 0)
    {
      col++;
      d += four_b_square * col;
    }

    d += two_a_square * (3 - (row * 2));
    row--;
  }
}

// to do: НЕ ВЕСЬ КОД - МОЙ, РАЗОБРАТЬСЯ\ПЕРЕПИСАТЬ
void esFillElipse(PBitMap BitMap, int x1, int y1, int x2, int y2, unsigned char Color)
{
  int col, i, row, bnew, dx, dy, a, b, x, y, w, h, t;
  long a_square, b_square, two_a_square, two_b_square, four_a_square, four_b_square, d;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(x1, y1, x2, y2, CenterX, CenterY);
  #endif

  NORMALIZE_RECT(x1, y1, x2, y2, t);
  #ifdef SKIP_LAST_PIXEL
  if(IsSLP)y2--; // decrement x2 does LineU automatically
  if(x2 < x1 || y2 < y1)return;
  #endif

  w = x2 - x1;
  h = y2 - y1;

  if(h < 2)
  {
    if(h == 1)
    {
      esDrawLine(BitMap, x1, y1, x2, y1, Color);
      esDrawLine(BitMap, x1, y2, x2, y2, Color);
    }
    else
      esDrawLine(BitMap, x1, y1, x2, y1, Color);
    return;
  }

  if(w < 2)
  {
    if(w == 1)
    {
      esDrawLine(BitMap, x1, y1, x1, y2, Color);
      esDrawLine(BitMap, x2, y1, x2, y2, Color);
    }
    else
      esDrawLine(BitMap, x1, y1, x1, y2, Color);
    return;
  }

  a = w / 2;
  b = h / 2;
  x = x1 + a;
  y = y1 + b;

  b_square = b * b;
  a_square = a * a;
  row = b;
  col = 0;
  two_a_square = a_square * 2;
  four_a_square = a_square * 4;
  four_b_square = b_square * 4;
  two_b_square = b_square * 2;

  d = two_a_square * ((row - 1) * (row)) + a_square + two_b_square * (1 - a_square);
  while(a_square * (row) > b_square * (col) - 1)
  {
    if(d >= 0)
    {
      dx = (w % 2) ? col + 1: col;
      dy = (h % 2) ? row + 1: row;
      esDrawLine(BitMap, x - col, y - row, x + dx, y - row, Color);
      esDrawLine(BitMap, x - col, y + dy, x + dx, y + dy, Color);
      row--;
      d -= four_a_square * (row);
    }
    d += two_b_square * (3 + (col * 2));
    col++;
  }

  d = two_b_square * (col + 1) * col + two_a_square * (row * (row - 2) + 1) + (1 - two_a_square) * b_square;
  while ((row) + 1)
  {
    dx = (w % 2) ? col + 1: col;
    dy = (h % 2) ? row + 1: row;

    esDrawLine(BitMap, x - col, y - row, x + dx, y - row, Color);
    if(row != 0 || h % 2)
      esDrawLine(BitMap, x - col, y + dy, x + dx, y + dy, Color);
    if(d <= 0)
    {
      col++;
      d += four_b_square * col;
    }

    d += two_a_square * (3 - (row * 2));
    row--;
  }
}


/*
// to do: добавить контроль границ и использовать другой алгоритм
void ellsDrawElipse(PBitMap BitMap, int x1, int y1, int x2, int y2, unsigned char Color)
{
  int x, y;
  int yr2, xr2, d;
  float xm;

  xr2 = (long)sqrt((float)x2) << 1;
  yr2 = (long)sqrt((float)y2) << 1;

  x = 0;
  y = y2;
  d = (yr2 - xr2 * y) + xr2 >> 1;
  xm = xr2 / sqrt((float)((xr2 + yr2) << 1)) - 1;

  while(x < xm)
  {
    if(d > 0)
    {
      y--;
      d = d + yr2 * (x << 1 + 3) - xr2 * y << 1;
    }
    else
      d = d + yr2 * (x << 1 + 3);
    x++;
    esSetPixel1(BitMap, x1 + x, y1 + y, Color);
    esSetPixel1(BitMap, x1 + x, y1 - y, Color);
    esSetPixel1(BitMap, x1 - x, y1 + y, Color);
    esSetPixel1(BitMap, x1 - x, y1 - y, Color);
  }


  //return;
  d = (xr2 - yr2 * x2) + xr2 >> 1;
  x = x2;
  y = 0;
  xm = xm + 2;

  while(x > xm)
  {
    if(d > 0)
    {
      x--;
      d = d + xr2 * (y << 1 + 3) - yr2 * x << 1;
    }
    else
      d = d + xr2 * (y << 1 + 3);
    y++;
    esSetPixel1(BitMap, x1 + x, y1 + y, Color);
    esSetPixel1(BitMap, x1 + x, y1 - y, Color);
    esSetPixel1(BitMap, x1 - x, y1 + y, Color);
    esSetPixel1(BitMap, x1 - x, y1 - y, Color);
  }

}*/

/*
// to do: добавить контроль границ
void esDrawCircle(PBitMap BitMap, int x1, int y1, int R, unsigned char Color)
{
  int height, width;
  int x, y, dx, dy;
  int d, error;
  int cx, cy;
 // dx = 0; dy = 0;

  width = R;//x2 - x1;
  height = R;//y2 - y1;

  cx = x1 + width / 2;
  cy = y1 + height / 2;

  R = width / 2.0f;

  x = 0;
  y = R;
  d = 1 - 2 * R;
  dx = 1 - 2 * width;
  dy = 1 - 2 * height;
  error = 0;
 
  while(y >= 0)
  {
    esSetPixel1(BitMap, cx + x, cy + y, Color);
    esSetPixel1(BitMap, cx + x, cy - y, Color);
    esSetPixel1(BitMap, cx - x, cy + y, Color);
    esSetPixel1(BitMap, cx - x, cy - y, Color);

    error = 2 * (d + y - 1);
    if((d < 0) && (error <= 0))
    {
      d += 2 * ++x + 1;
      continue;
    }
    error = 2 * (d - x) + 1;
    if((d > 0) && (error > 0))
    {
      d += 1 - 2 * --y;
      continue;
    }
    x++;
    d += 2 * (x - y + 2);
    y--;
  }

}

// to do: добавить контроль границ
void nnesDrawCircle(PBitMap BitMap, int x1, int y1, int x2, int y2, unsigned char Color)
{
  int height, width;
  int x, y;
  int R;
  int d, error;
  int cx, cy, a, b;
 // dx = 0; dy = 0;

  a = x2*x2;
  b = y2*y2;

  width = x2 - x1;
  height = y2 - y1;

  cx = x1 + width / 2;
  cy = y1 + height / 2;

  R = width / 2.0f;

  x = 0;
  y = R;
  d = 1 - 2 * R;
  error = 0;
 
  while(y >= 0)
  {
    esSetPixel1(BitMap, cx + x, cy + y, Color);
    esSetPixel1(BitMap, cx + x, cy - y, Color);
    esSetPixel1(BitMap, cx - x, cy + y, Color);
    esSetPixel1(BitMap, cx - x, cy - y, Color);

    error = 2 * (d + y - 1);
    if((d < 0) && (error <= 0))
    {
      d += 2 * ++x + 1;
      continue;
    }
    error = 2 * (d - x) + 1;
    if((d > 0) && (error > 0))
    {
      d += 1 - 2 * --y;
      continue;
    }
    x++;
    d += 2 * (x - y + 2);
    y--;
  }

}*/

//******************************************************************************
// The BitBlt functions
//******************************************************************************
//  esBitBltXX_XXXX
//          ^   ^---- Mode(copy, or, and, xor, use??_)
//          |
// PixelFormat (01, 02, 04, C2)
//
// Idea and design in Шindows nt4 source code
// ntgdi\blt0101.cxx && bitblt.cxx
//
// todo: refactoring!!!
// 14.01.2008
// Говнокод, который я сам плохо понимаю,
// через полгода я уже не вспомню как оно работает.
// |0|1|2|3|4|5|6|7|0|1|2|3|4|5|6|7|0|1|2|3|4|5|6|7|
// |               |               |               |
//            |----+---------------+----|
//          5(a)                      19(b) 
//
// 17.08.2014 НО КАК? КАК ОНО РАБОТАЕТ ???

// view: source/cfx/RightPixels___LeftPixels_Reverse.bmp
const unsigned char RPixels01[] =
{
  0xFF,// 0
  0x7F,// 1
  0x3F,// 2
  0x1F,// 3
  0x0F,// 4
  0x07,// 5
  0x03,// 6
  0x01,// 7
};

const unsigned char LPixels01[] =
{
  0x00,// 8
  0x80,// 9
  0xC0,// 10
  0xE0,// 11
  0xF0,// 12
  0xF8,// 13
  0xFC,// 14
  0xFE,// 15
};

/*
// GOLG, 100% worked
int esBitBlt01_Copy(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, EndOffset, NormalDraw;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd;

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 8;
    Offset = x % 8;
  }
  else
  {
    ix = -x/8;
    Start_x = 0;
    Offset = 8 - abs(x) % 8;
  }

  if((x+Src->Width)/8 < DstScan)
    End_x = (x+Src->Width)/8;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  EndOffset = (x + Src->Width) % 8;
  NormalDraw = Offset + Src->Width > 7 || Offset == 0 || EndOffset == 0; 

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(Offset != 0 && x > 0)
    {
      if(NormalDraw)
        *pDst = ((*pSrc >> Offset) & RPixels01[Offset]) | (~RPixels01[Offset] & *pDst);
      else
        *pDst = ((*pSrc >> Offset) & RPixels01[Offset] & LPixels01[EndOffset]) |
                ((~RPixels01[Offset] | ~LPixels01[EndOffset]) & *pDst);
      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        *pDst = *pSrc <<(8 - Offset) | *(pSrc+1) >>(Offset);
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        *pDst = *pSrc;
        pSrc++;
      }
       
    // |.. |
    // view: Source\Doc\OffsetBitBlt.jpg
    if(EndOffset && (x+Src->Width) / 8 < DstScan && NormalDraw)
    {
      if(Offset)
        if(Offset >= EndOffset)
          *pDst = *pSrc << (8 - Offset) & LPixels01[EndOffset] | *pDst & ~LPixels01[EndOffset];
        else
          *pDst = *pSrc << (8 - Offset) | *(pSrc+1) >> Offset & LPixels01[EndOffset] | *pDst & ~LPixels01[EndOffset];
      else
        *pDst = *pSrc & LPixels01[EndOffset] | *pDst & ~LPixels01[EndOffset];
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}
*/

// GOLG, 100% worked
int esBitBlt01_Copy(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, EndOffset, NormalDraw;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 8;
    Offset = x % 8;
  }
  else
  {
    ix = -x/8;
    Start_x = 0;
    Offset = 8 - abs(x) % 8;
  }

  if((x+Src->Width)/8 < DstScan)
    End_x = (x+Src->Width)/8;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  EndOffset = (x + Src->Width) % 8;
  NormalDraw = Offset + Src->Width > 7 || Offset == 0 || EndOffset == 0; 

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(Offset != 0 && x > 0)
    {
      if(NormalDraw)
      {
        s = (*pSrc >> Offset) & RPixels01[Offset];
        *pDst = s | (~RPixels01[Offset] & *pDst);// -> draw byte
      }
      else
      {
        s = (*pSrc >> Offset) & RPixels01[Offset] & LPixels01[EndOffset];
        *pDst = s | ((~RPixels01[Offset] | ~LPixels01[EndOffset]) & *pDst);// -> draw byte
      }
      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << (8 - Offset) | *(pSrc+1) >> (Offset);
        *pDst = s;// -> draw byte 
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst = s;// -> draw byte
        pSrc++;
      }
       
    // |.. |
    // view: Source\Doc\OffsetBitBlt.jpg
    if(EndOffset && (x+Src->Width) / 8 < DstScan && NormalDraw)
    {
      if(Offset)
        if(Offset >= EndOffset)
        {
          s = *pSrc << (8 - Offset) & LPixels01[EndOffset];
        }
        else
        {
          s = *pSrc << (8 - Offset) | *(pSrc+1) >> Offset & LPixels01[EndOffset];
        }
      else
      {
        s = *pSrc & LPixels01[EndOffset];
      }
      *pDst = s | *pDst & ~LPixels01[EndOffset];// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}


int esBitBlt01_Or(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, EndOffset, NormalDraw;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 8;
    Offset = x % 8;
  }
  else
  {
    ix = -x/8;
    Start_x = 0;
    Offset = 8 - abs(x) % 8;
  }

  if((x+Src->Width)/8 < DstScan)
    End_x = (x+Src->Width)/8;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  EndOffset = (x + Src->Width) % 8;
  NormalDraw = Offset + Src->Width > 7 || Offset == 0 || EndOffset == 0; 

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(Offset != 0 && x > 0)
    {
      if(NormalDraw)
      {
        s = (*pSrc >> Offset) & RPixels01[Offset];
        *pDst |= s;// | (~RPixels01[Offset] & *pDst);// -> draw byte
      }
      else
      {
        s = (*pSrc >> Offset) & RPixels01[Offset] & LPixels01[EndOffset];
        *pDst |= s;// | ((~RPixels01[Offset] | ~LPixels01[EndOffset]) & *pDst);// -> draw byte
      }
      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << (8 - Offset) | *(pSrc+1) >> (Offset);
        *pDst |= s;// -> draw byte 
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst |= s;// -> draw byte
        pSrc++;
      }
       
    // |.. |
    // view: Source\Doc\OffsetBitBlt.jpg
    if(EndOffset && (x+Src->Width) / 8 < DstScan && NormalDraw)
    {
      if(Offset)
        if(Offset >= EndOffset)
        {
          s = *pSrc << (8 - Offset) & LPixels01[EndOffset];
        }
        else
        {
          s = *pSrc << (8 - Offset) | *(pSrc+1) >> Offset & LPixels01[EndOffset];
        }
      else
      {
        s = *pSrc & LPixels01[EndOffset];
      }
      *pDst |= s;// | *pDst & ~LPixels01[EndOffset];// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

int esBitBlt01_And(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, EndOffset, NormalDraw;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 8;
    Offset = x % 8;
  }
  else
  {
    ix = -x / 8;
    Start_x = 0;
    Offset = 8 - abs(x) % 8;
  }

  if((x+Src->Width)/8 < DstScan)
    End_x = (x+Src->Width)/8;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  EndOffset = (x + Src->Width) % 8;
  NormalDraw = Offset + Src->Width > 7 || Offset == 0 || EndOffset == 0; 

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(Offset != 0 && x > 0)
    {
      if(NormalDraw)
      {
        s = (*pSrc >> Offset) | LPixels01[Offset];
        *pDst &= s;// | (~RPixels01[Offset] & *pDst);// -> draw byte
      }
      else
      {
        s = (*pSrc >> Offset) | LPixels01[Offset] | RPixels01[EndOffset];
        *pDst &= s;// | ((~RPixels01[Offset] | ~LPixels01[EndOffset]) & *pDst);// -> draw byte
      }
      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << (8 - Offset) | *(pSrc+1) >> (Offset);
        *pDst &= s;// -> draw byte 
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst &= s;// -> draw byte
        pSrc++;
      }
       
    // |.. |
    // view: Source\Doc\OffsetBitBlt.jpg
    if(EndOffset && (x+Src->Width) / 8 < DstScan && NormalDraw)
    {
      if(Offset)
        if(Offset >= EndOffset)
        {
          s = (*pSrc << (8 - Offset)) | RPixels01[EndOffset];
        }
        else
        {
          s = (*pSrc << (8 - Offset) | *(pSrc+1) >> Offset) | RPixels01[EndOffset];
        }
      else
      {
        s = *pSrc | RPixels01[EndOffset];
      }
      *pDst &= s;// | *pDst & ~LPixels01[EndOffset];// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}


// to do: fix me: ПЕРЕДЕЛАТЬ ВСЕ К ЧЕРТЯМ, Я НАСТАВИЛ КОСТЫЛЕЙ, РАЗОБРАТЬСЯ
int esBitBlt01_NotAnd(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, EndOffset, NormalDraw;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 8;
    Offset = x % 8;
  }
  else
  {
    ix = -x / 8;
    Start_x = 0;
    Offset = 8 - abs(x) % 8;
  }

  if((x+Src->Width)/8 < DstScan)
    End_x = (x+Src->Width)/8;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  EndOffset = (x + Src->Width) % 8;
  NormalDraw = Offset + Src->Width > 7 || Offset == 0 || EndOffset == 0; 

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(Offset != 0 && x > 0)
    {
      if(NormalDraw)
      {
        s = ((~*pSrc) >> Offset) | LPixels01[Offset];
        *pDst &= s;// | (~RPixels01[Offset] & *pDst);// -> draw byte
      }
      else
      {
        s = ((~*pSrc) >> Offset) | LPixels01[Offset] | RPixels01[EndOffset];
        *pDst &= s;// | ((~RPixels01[Offset] | ~LPixels01[EndOffset]) & *pDst);// -> draw byte
      }
      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = (~*pSrc & 0xFF) << (8 - Offset) | (~*(pSrc+1) & 0xFF) >> (Offset);
        *pDst &= s;// -> draw byte 
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst &= ~s;// -> draw byte
        pSrc++;
      }
       
    // |.. |
    // view: Source\Doc\OffsetBitBlt.jpg
    if(EndOffset && (x+Src->Width) / 8 < DstScan && NormalDraw)
    {
      if(Offset)
        if(Offset >= EndOffset)
        {
          s = ((~*pSrc & 0xFF) << (8 - Offset)) | RPixels01[EndOffset];
        }
        else
        {
          s = ((~*pSrc & 0xFF) << (8 - Offset) | (~*(pSrc+1) & 0xFF) >> Offset) | RPixels01[EndOffset];
        }
      else
      {
        s = (~*pSrc & 0xFF) | RPixels01[EndOffset];
      }
      *pDst &= s;// | *pDst & ~LPixels01[EndOffset];// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

int esBitBlt01_Xor(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, EndOffset, NormalDraw;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 8;
    Offset = x % 8;
  }
  else
  {
    ix = -x/8;
    Start_x = 0;
    Offset = 8 - abs(x) % 8;
  }

  if((x+Src->Width)/8 < DstScan)
    End_x = (x+Src->Width)/8;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  EndOffset = (x + Src->Width) % 8;
  NormalDraw = Offset + Src->Width > 7 || Offset == 0 || EndOffset == 0; 

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(Offset != 0 && x > 0)
    {
      if(NormalDraw)
      {
        s = (*pSrc >> Offset) & RPixels01[Offset];
        *pDst ^= s;// | (~RPixels01[Offset] & *pDst);// -> draw byte
      }
      else
      {
        s = (*pSrc >> Offset) & RPixels01[Offset] & LPixels01[EndOffset];
        *pDst ^= s;// | ((~RPixels01[Offset] | ~LPixels01[EndOffset]) & *pDst);// -> draw byte
      }
      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << (8 - Offset) | *(pSrc+1) >> (Offset);
        *pDst ^= s;// -> draw byte 
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst ^= s;// -> draw byte
        pSrc++;
      }
       
    // |.. |
    // view: Source\Doc\OffsetBitBlt.jpg
    if(EndOffset && (x+Src->Width) / 8 < DstScan && NormalDraw)
    {
      if(Offset)
        if(Offset >= EndOffset)
        {
          s = *pSrc << (8 - Offset) & LPixels01[EndOffset];
        }
        else
        {
          s = *pSrc << (8 - Offset) | *(pSrc+1) >> Offset & LPixels01[EndOffset];
        }
      else
      {
        s = *pSrc & LPixels01[EndOffset];
      }
      *pDst ^= s;// | *pDst & ~LPixels01[EndOffset];// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

// GOLG, 100% worked
int esBitBlt01_MaskData(PBitMap Dst, int x, int y, PBitMap Src, unsigned char *MaskData)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, EndOffset, NormalDraw;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, *pMask, *pMaskLine, m, s, d;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 8;
    Offset = x % 8;
  }
  else
  {
    ix = -x/8;
    Start_x = 0;
    Offset = 8 - abs(x) % 8;
  }

  if((x+Src->Width)/8 < DstScan)
    End_x = (x+Src->Width)/8;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pMaskLine = /*Mask->Pixels*/MaskData + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  EndOffset = (x + Src->Width) % 8;
  NormalDraw = Offset + Src->Width > 7 || Offset == 0 || EndOffset == 0; 

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pMask = pMaskLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(Offset != 0 && x > 0)
    {
      if(NormalDraw)
      {
        s = (*pSrc >> Offset) & RPixels01[Offset];// | (~RPixels01[Offset] & *pDst);
        m = (*pMask >> Offset) & RPixels01[Offset];
        *pDst = s & m | *pDst & ~m;
      }
      else
      {
        //*pDst = ((*pSrc >> Offset) & RPixels01[Offset] & LPixels01[EndOffset]) |
        //        ((~RPixels01[Offset] | ~LPixels01[EndOffset]) & *pDst);
        s = (*pSrc >> Offset) & RPixels01[Offset] & LPixels01[EndOffset];
        m = (*pMask >> Offset) & RPixels01[Offset] & LPixels01[EndOffset];
        *pDst = s & m | *pDst & ~m;
      }
      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << (8 - Offset) | *(pSrc+1) >> (Offset);
        m = *pMask << (8 - Offset) | *(pMask+1) >> (Offset);
        *pDst = s & m | *pDst & ~m;// -> draw byte 
        pSrc++;
        pMask++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        m = *pMask;
        *pDst = *pSrc & m | *pDst & ~m;// -> draw byte
        pSrc++;
        pMask++;
      }
       
    // |.. |
    // view: Source\Doc\OffsetBitBlt.jpg
    if(EndOffset && (x+Src->Width) / 8 < DstScan && NormalDraw)
    {
      if(Offset)
        if(Offset >= EndOffset)
        {
          s = *pSrc << (8 - Offset) & LPixels01[EndOffset];
          m = *pMask << (8 - Offset) & LPixels01[EndOffset];
        }
        else
        {
          s = *pSrc << (8 - Offset) | *(pSrc+1) >> Offset & LPixels01[EndOffset];
          m = *pMask << (8 - Offset) | *(pMask+1) >> Offset & LPixels01[EndOffset]; 
        }
      else
      {
        s = *pSrc & LPixels01[EndOffset];
        m = *pMask & LPixels01[EndOffset];
      }
      *pDst = s & m | *pDst & ~m;// -> draw byte // | *pDst & ~LPixels01[EndOffset];
    }

    pSrcLine = pSrcLine + SrcScan;
    pMaskLine = pMaskLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

int esBitBlt01_Mask(PBitMap Dst, int x, int y, PBitMap Src)
{
  return esBitBlt01_MaskData(Dst, x, y, Src, Src->Mask);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GOLG, 100% worked
int esBitBlt04_Copy(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, IsLeftParticle, IsRightParticle;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 2;
    Offset = x % 2;
  }
  else
  {
    ix = -x / 2;
    Start_x = 0;
    Offset = abs(x) % 2;
  }

  if((x + Src->Width) / 2 < DstScan)
    End_x = (x + Src->Width) / 2;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  IsLeftParticle = Offset != 0 && x > 0;
  IsRightParticle = (x + Src->Width) % 2 && (x + Src->Width) / 2 < DstScan;

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(IsLeftParticle)
    {
      s = *pSrc >> 4;
      *pDst = s | (*pDst & 0xF0);// -> draw byte 

      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << 4 | *(pSrc+1) >> 4;
        *pDst = s;// -> draw byte 
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst = s;// -> draw byte
        pSrc++;
      }
       
    // |.. |
    if(IsRightParticle)
    {
      if(Offset)
        s = *pSrc << 4;
      else
      {
        s = *pSrc & 0xF0;
      }
      *pDst = s | *pDst & 0x0F;// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

// GOLG, 100% worked
int esBitBlt04_Or(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, IsLeftParticle, IsRightParticle;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 2;
    Offset = x % 2;
  }
  else
  {
    ix = -x / 2;
    Start_x = 0;
    Offset = abs(x) % 2;
  }

  if((x + Src->Width) / 2 < DstScan)
    End_x = (x + Src->Width) / 2;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  IsLeftParticle = Offset != 0 && x > 0;
  IsRightParticle = (x + Src->Width) % 2 && (x + Src->Width) / 2 < DstScan;

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(IsLeftParticle)
    {
      s = *pSrc >> 4;
      *pDst |= s;// -> draw byte 

      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << 4 | *(pSrc+1) >> 4;
        *pDst |= s;// -> draw byte 
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst |= s;// -> draw byte
        pSrc++;
      }
       
    // |.. |
    if(IsRightParticle)
    {
      if(Offset)
        s = *pSrc << 4;
      else
      {
        s = *pSrc & 0xF0;
      }
      *pDst |= s;// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

// GOLG, 100% worked
int esBitBlt04_Xor(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, IsLeftParticle, IsRightParticle;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 2;
    Offset = x % 2;
  }
  else
  {
    ix = -x / 2;
    Start_x = 0;
    Offset = abs(x) % 2;
  }

  if((x + Src->Width) / 2 < DstScan)
    End_x = (x + Src->Width) / 2;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  IsLeftParticle = Offset != 0 && x > 0;
  IsRightParticle = (x + Src->Width) % 2 && (x + Src->Width) / 2 < DstScan;

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(IsLeftParticle)
    {
      s = *pSrc >> 4;
      *pDst ^= s;// -> draw byte 

      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << 4 | *(pSrc+1) >> 4;
        *pDst ^= s;// -> draw byte 
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst ^= s;// -> draw byte
        pSrc++;
      }
       
    // |.. |
    if(IsRightParticle)
    {
      if(Offset)
        s = *pSrc << 4;
      else
      {
        s = *pSrc & 0xF0;
      }
      *pDst ^= s;// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}


// GOLG, 100% worked
int esBitBlt04_And(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, IsLeftParticle, IsRightParticle;
  unsigned char *pSrc, *pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 2;
    Offset = x % 2;
  }
  else
  {
    ix = -x / 2;
    Start_x = 0;
    Offset = abs(x) % 2;
  }

  if((x + Src->Width) / 2 < DstScan)
    End_x = (x + Src->Width) / 2;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  IsLeftParticle = Offset != 0 && x > 0;
  IsRightParticle = (x + Src->Width) % 2 && (x + Src->Width) / 2 < DstScan;

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(IsLeftParticle)
    {
      s = *pSrc >> 4;
      *pDst &= s | 0xF0;// -> draw byte 

      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << 4 | *(pSrc+1) >> 4;
        *pDst &= s;// -> draw byte 
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst &= s;// -> draw byte
        pSrc++;
      }
       
    // |.. |
    if(IsRightParticle)
    {
      if(Offset)
        s = *pSrc << 4;
      else
      {
        s = *pSrc & 0xF0;
      }
      *pDst &= s | 0x0F;// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

// GOLG, 100% worked
int esBitBlt04_MaskData(PBitMap Dst, int x, int y, PBitMap Src, unsigned char *MaskData)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, IsLeftParticle, IsRightParticle;
  unsigned char *pSrc,*pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;
  unsigned char *pMask, *pMaskLine;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 2;
    Offset = x % 2;
  }
  else
  {
    ix = -x / 2;
    Start_x = 0;
    Offset = abs(x) % 2;
  }

  if((x + Src->Width) / 2 < DstScan)
    End_x = (x + Src->Width) / 2;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pMaskLine = /*Mask->Pixels*/MaskData + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  IsLeftParticle = Offset != 0 && x > 0;
  IsRightParticle = (x + Src->Width) % 2 && (x + Src->Width) / 2 < DstScan;

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pMask = pMaskLine + ix;
    pDst = pDstLine + Start_x;

    // | ..|
    if(IsLeftParticle)
    {
      s = *pSrc >> 4;
      m = *pMask >> 4;
      *pDst = (*pDst & ~m | s & m);// | 0xF0;// -> draw byte s s | 0xF0;// -> draw byte 

      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << 4 | *(pSrc+1) >> 4;
        m = *pMask << 4 | *(pMask+1) >> 4;
        *pDst = *pDst & ~m | s & m;// -> draw byte 
        pSrc++;
        pMask++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        m = *pMask;
        *pDst = *pDst & ~m | s & m;// -> draw byte
        pSrc++;
        pMask++;
      }
       
    // |.. |
    if(IsRightParticle)
    {
      if(Offset)
      {
        s = *pSrc << 4;
        m = *pMask << 4;
      }
      else
      {
        s = *pSrc & 0xF0;
        m = *pMask & 0xF0;
      }
      *pDst = *pDst & ~m | s & m;// -> draw byte 
    }

    pSrcLine = pSrcLine + SrcScan;
    pMaskLine = pMaskLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

int esBitBlt04_Mask(PBitMap Dst, int x, int y, PBitMap Src)
{
  return esBitBlt04_MaskData(Dst, x, y, Src, Src->Mask);
}

// GOLG, 100% worked
int esBitBlt04_Transparent(PBitMap Dst, int x, int y, PBitMap Src)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, IsLeftParticle, IsRightParticle;
  unsigned char *pSrc,*pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, m;
  unsigned char Transp, TranspShift, TranspFill;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  Transp = Src->TransparentColor;
  TranspShift = Transp << 4;
  TranspFill = Transp | TranspShift;

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 2;
    Offset = x % 2;
  }
  else
  {
    ix = -x / 2;
    Start_x = 0;
    Offset = abs(x) % 2;
  }

  if((x + Src->Width) / 2 < DstScan)
    End_x = (x + Src->Width) / 2;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  IsLeftParticle = Offset != 0 && x > 0;
  IsRightParticle = (x + Src->Width) % 2 && (x + Src->Width) / 2 < DstScan;

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // to do: выяснить что дешевле несколько условий или логические операции
    // и => доработать код
    // | ..|
    if(IsLeftParticle)
    {
      s = *pSrc >> 4;

      if(s != Transp)
        *pDst = s | (*pDst & 0xF0);// -> draw byte

      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << 4 | *(pSrc+1) >> 4;
        if(s != TranspFill)
        {
          m = 0x00;
          if((s & 0x0F) != Transp)m |= 0x0F;
          if((s & 0xF0) != TranspShift)m |= 0xF0;
          *pDst = *pDst & ~m | s & m;// -> draw byte
        }
        
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;

        if(s != TranspFill)
        {
          m = 0x00;
          if((s & 0x0F) != Transp)m |= 0x0F;
          if((s & 0xF0) != TranspShift)m |= 0xF0;
          *pDst = *pDst & ~m | s & m;// -> draw byte
        }
        pSrc++;
      }
       
    // |.. |
    if(IsRightParticle)
    {
      if(Offset)
      {
        s = *pSrc << 4;
      }
      else
      {
        s = *pSrc & 0xF0;
      }
      if(s != TranspShift)
        *pDst = s | *pDst & 0x0F;// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

// GOLG, 100% worked
int esBitBlt04_Color(PBitMap Dst, int x, int y, PBitMap Src, TColor Color)
{
  int DstScan, SrcScan, sy, ix, iy;
  int Start_y, End_y, Start_x, End_x;
  int Offset, IsLeftParticle, IsRightParticle;
  unsigned char *pSrc,*pSrcLine, *pDst, *pDstLine, *pDstEnd, d, s, FillColor;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_POINT(x, y, CenterX, CenterY);
  #endif 

  if(x >= Dst->Width || y >= Dst->Height || x + Src->Width < 0 || y + Src->Height < 0)
    return 0;

  FillColor = esFillByte(Color, pf4bit);

  DstScan = esScanline(Dst);
  SrcScan = esScanline(Src);

  if(y >= 0)
  {
    Start_y = y; iy = 0;
  }
  else
  {
    Start_y = 0; iy = -y;
  }

  if(y + Src->Height < Dst->Height)
    End_y = y + Src->Height;
  else
    End_y = Dst->Height;

  if(x >= 0)
  {
    ix = 0;
    Start_x = x / 2;
    Offset = x % 2;
  }
  else
  {
    ix = -x / 2;
    Start_x = 0;
    Offset = abs(x) % 2;
  }

  if((x + Src->Width) / 2 < DstScan)
    End_x = (x + Src->Width) / 2;
  else
    End_x = DstScan;

  pSrcLine = Src->Pixels + SrcScan * iy;
  pDstLine = Dst->Pixels + DstScan * Start_y;

  IsLeftParticle = Offset != 0 && x > 0;
  IsRightParticle = (x + Src->Width) % 2 && (x + Src->Width) / 2 < DstScan;

  // scan y
  for(sy = Start_y; sy < End_y; sy++)
  {
    pSrc = pSrcLine + ix;
    pDst = pDstLine + Start_x;

    // to do: выяснить что дешевле несколько условий или логические операции
    // и => доработать код
    // | ..|
    if(IsLeftParticle)
    {
      s = *pSrc >> 4;

      //*pDst = (s & FillColor) | (*pDst & 0xF0);// -> draw byte
      *pDst = *pDst & ~s | s & FillColor;// -> draw byte

      pDst++;
    }

    // |...|
    pDstEnd = pDstLine + End_x;

    if(Offset != 0)
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc << 4 | *(pSrc+1) >> 4;
        *pDst = *pDst & ~s | s & FillColor;// -> draw byte
        
        pSrc++;
      }
    else
      for(; pDst < pDstEnd; pDst++)
      {
        s = *pSrc;
        *pDst = *pDst & ~s | s & FillColor;// -> draw byte

        pSrc++;
      }
       
    // |.. |
    if(IsRightParticle)
    {
      if(Offset)
      {
        s = *pSrc << 4;
      }
      else
      {
        s = *pSrc & 0xF0;
      }
      //*pDst = (s & FillColor) | *pDst & 0x0F;// -> draw byte
      *pDst = *pDst & ~s | s & FillColor;// -> draw byte
    }

    pSrcLine = pSrcLine + SrcScan;
    pDstLine = pDstLine + DstScan;
  }

  return 1;
}

int esBitBlt01(PBitMap Dst, int x, int y, PBitMap Src)
{
  switch(Src->AlphaFormat)
  {
    case afNone:
      return esBitBlt01_Copy(Dst, x, y, Src);

    case afMask:
      if(Src->Mask != NULL)
        return esBitBlt01_Mask(Dst, x, y, Src);
      else
        return 0;

    case afColor:
      return 0; 
  }
}

int esBitBlt04(PBitMap Dst, int x, int y, PBitMap Src)
{
  switch(Src->AlphaFormat)
  {
    case afNone:
      return esBitBlt04_Copy(Dst, x, y, Src);

    case afMask:
      if(Src->Mask != NULL)
        return esBitBlt04_Mask(Dst, x, y, Src);
      else
        return 0;

    case afColor:
      return esBitBlt04_Transparent(Dst, x, y, Src);
  }
}

// to do: add pixel formats
int esBitBlt(PBitMap Dst, int x, int y, PBitMap Src)
{
  switch(Src->PixelFormat)
  {
    case pf1bit:
      return esBitBlt01(Dst, x, y, Src);
    case pf4bit:
      return esBitBlt04(Dst, x, y, Src);
    case pf2bit:
    case pfC2:
      return 0;
  }
}

int esBitBltRop01(PBitMap Dst, int x, int y, PBitMap Src, TRop Rop)
{
  switch(Rop)
  {
    case ropCopy:
      return esBitBlt01_Copy(Dst, x, y, Src);
    case ropOr:
      return esBitBlt01_Or(Dst, x, y, Src);
    case ropXor:
      return esBitBlt01_Xor(Dst, x, y, Src);
    case ropAnd:
      return esBitBlt01_And(Dst, x, y, Src);
    case ropNotAnd:
      return esBitBlt01_NotAnd(Dst, x, y, Src);
  }
}

int esBitBltRop04(PBitMap Dst, int x, int y, PBitMap Src, TRop Rop)
{
  switch(Rop)
  {
    case ropCopy:
      return esBitBlt04_Copy(Dst, x, y, Src);
    case ropOr:
      return esBitBlt04_Or(Dst, x, y, Src);
    case ropXor:
      return esBitBlt04_Xor(Dst, x, y, Src);
    case ropAnd:
      return esBitBlt04_And(Dst, x, y, Src);
  }
}

// to do: add pixel formats
int esBitBltRop(PBitMap Dst, int x, int y, PBitMap Src, TRop Rop)
{
  switch(Src->PixelFormat)
  {
    case pf1bit:
      return esBitBltRop01(Dst, x, y, Src, Rop);
    case pf4bit:
      return esBitBltRop04(Dst, x, y, Src, Rop);
    case pf2bit:
    case pfC2:
      return 0;
  }
}


/*
// GOLD 100% WORKED
void G_esStrechDraw01(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int x, y, t;
  int sx, sy;
  int w, h;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 
  
  w = s.x2 - s.x1;
  h = s.y2 - s.y1;

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  for(sy = d.y1; sy < d.y2; sy++)
  {
    for(sx = d.x1; sx < d.x2; sx++)
    {
      x = s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * (float)(sx-d.x1+0.5f);   
      y = s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * (float)(sy-d.y1+0.5f); 
      Color = esGetPixel1(Src, x, y);
      esSetPixel1(Dst, sx, sy, Color);
    }
  }
}

void ooG_esStrechDraw01(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int x, y, t;
  int sx, sy;
  int w, h;
  float dx, dy, fx, fy, s_fx;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 
  
  w = s.x2 - s.x1;
  h = s.y2 - s.y1;

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }


  s_fx = s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f;
  fy = s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f;
  dx = (float)(s.x2-s.x1)/(float)(d.x2-d.x1);
  dy = (float)(s.y2-s.y1)/(float)(d.y2-d.y1);


  for(sy = d.y1; sy < d.y2; sy++)
  {
    fx = s_fx;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      x = (int)fx;  
      y = (int)fy;
      Color = esGetPixel1(Src, x, y);
      esSetPixel1(Dst, sx, sy, Color);
      fx = fx + dx;
    }
    fy = fy + dy;
  }
}
*/

// to do: дописать контроль границ
void esStrechDraw01_Copy(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      Color = esGetPixel1(Src, FixedToInt(x), FixedToInt(y));
      esSetPixel1(Dst, sx, sy, Color);
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw01_Or(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      Color = esGetPixel1(Src, FixedToInt(x), FixedToInt(y));
      esDrawPixel1_Or(Dst, sx, sy, Color);
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw01_Xor(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      Color = esGetPixel1(Src, FixedToInt(x), FixedToInt(y));
      esDrawPixel1_Xor(Dst, sx, sy, Color);
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw01_And(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      Color = esGetPixel1(Src, FixedToInt(x), FixedToInt(y));
      esDrawPixel1_And(Dst, sx, sy, Color);
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw01_Mask(PBitMap Dst, TRect d, PBitMap Src, TRect s, PBitMap Mask)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h, ix, iy;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      ix = FixedToInt(x);
      iy = FixedToInt(y);
      if(esGetPixel1(Mask, ix, iy) == 1)
      {
        Color = esGetPixel1(Src, ix, iy);
        esSetPixel1(Dst, sx, sy, Color);
      }
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw04_Copy(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      Color = esGetPixel4(Src, FixedToInt(x), FixedToInt(y));
      esSetPixel4(Dst, sx, sy, Color);
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw04_Or(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      Color = esGetPixel4(Src, FixedToInt(x), FixedToInt(y));
      esDrawPixel4_Or(Dst, sx, sy, Color);
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw04_Xor(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      Color = esGetPixel4(Src, FixedToInt(x), FixedToInt(y));
      esDrawPixel4_Xor(Dst, sx, sy, Color);
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw04_And(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      Color = esGetPixel4(Src, FixedToInt(x), FixedToInt(y));
      esDrawPixel4_And(Dst, sx, sy, Color);
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw04_Mask(PBitMap Dst, TRect d, PBitMap Src, TRect s, PBitMap Mask)
{
  unsigned char Color;
  int sx, sy, t;
  int w, h, ix, iy;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      ix = FixedToInt(x);
      iy = FixedToInt(y);

      if(esGetPixel4(Mask, ix, iy) != 0)
      {
        Color = esGetPixel4(Src, ix, iy);
        esSetPixel4(Dst, sx, sy, Color);
      }
      x += dx;
    }
    y += dy;
  }
}

// to do: дописать контроль границ
void esStrechDraw04_Transparent(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  unsigned char Color, Transp;
  int sx, sy, t;
  int w, h, ix, iy;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  Transp = Src->TransparentColor;

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      ix = FixedToInt(x);
      iy = FixedToInt(y);
      if((Color = esGetPixel4(Src, ix, iy)) != Transp)
      //if(esGetPixel4(Src, ix, iy) != Transp)
      {
        //Color = esGetPixel4(Src, ix, iy);
        esSetPixel4(Dst, sx, sy, Color);
      }
      x += dx;
    }
    y += dy;
  }
}

void esStrechDraw04_Color(PBitMap Dst, TRect d, PBitMap Src, TRect s, TColor FillColor)
{
  unsigned char Color, Mix, DstColor, SrcColor;
  int sx, sy, t;
  int w, h, ix, iy;
  fixed dx, dy, x, y, start_x;

  #ifdef USE_TRANSLATE
  if(IsTranslate)TRANSLATE_RECT(d.x1, d.y1, d.x2, d.y2, CenterX, CenterY);
  #endif 

  if(d.x1 > d.x2)
  {
    SWAP(d.x1, d.x2, t);
    SWAP(s.x1, s.x2, t);
  }

  if(d.y1 > d.y2)
  {
    SWAP(d.y1, d.y2, t);
    SWAP(s.y1, s.y2, t);
  }

  start_x = FloatToFixed( s.x1 + (s.x2-s.x1)/(float)(d.x2-d.x1) * 0.5f );
  y = FloatToFixed( s.y1 + (s.y2-s.y1)/(float)(d.y2-d.y1) * 0.5f );
  dx = FloatToFixed( (float)(s.x2-s.x1)/(float)(d.x2-d.x1) );
  dy = FloatToFixed( (float)(s.y2-s.y1)/(float)(d.y2-d.y1) );


  for(sy = d.y1; sy < d.y2; sy++)
  {
    x = start_x;
    for(sx = d.x1; sx < d.x2; sx++)
    {
      ix = FixedToInt(x);
      iy = FixedToInt(y);

      DstColor = esGetPixel4(Dst, sx, sy);
      SrcColor = esGetPixel4(Src, ix, iy);

      Mix = DstColor & ~SrcColor | FillColor & SrcColor;

      esSetPixel4(Dst, sx, sy, Mix);

      x += dx;
    }
    y += dy;
  }
}

// to do: Add return`s error
int esStrechDraw01(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  TBitMap Mask;
  TAlphaFormat t;

  switch(Src->AlphaFormat)
  {
    case afNone:
      esStrechDraw01_Copy(Dst, d, Src, s);
      return 1;

    case afMask:
      if(Src->Mask != NULL)
      {
        Mask = *Src;
        Mask.Pixels = Mask.Mask;
        esStrechDraw01_Mask(Dst, d, Src, s, &Mask);
        return 1;
      }
      else
        return 0;

    case afColor:
      return 0; 
  }
}

// to do: Add return`s error
int esStrechDraw04(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  TBitMap Mask;
  TAlphaFormat t;

  switch(Src->AlphaFormat)
  {
    case afNone:
      esStrechDraw04_Copy(Dst, d, Src, s);
      return 1;

    case afMask:
      if(Src->Mask != NULL)
      {
        Mask = *Src;
        Mask.Pixels = Mask.Mask;
        esStrechDraw04_Mask(Dst, d, Src, s, &Mask);
        return 1;
      }
      else
        return 0;

    case afColor:
      esStrechDraw04_Transparent(Dst, d, Src, s);
      return 1; 
  }
}

// to do: add pixel formats
int esStrechDraw(PBitMap Dst, TRect d, PBitMap Src, TRect s)
{
  switch(Src->PixelFormat)
  {
    case pf1bit:
      return esStrechDraw01(Dst, d, Src, s);
    case pf4bit:
      return esStrechDraw04(Dst, d, Src, s);
    case pf2bit:
    case pfC2:
      return 0;
  }
}

// to do: Add return`s error
int esStrechDrawRop01(PBitMap Dst, TRect d, PBitMap Src, TRect s, TRop Rop)
{
  switch(Rop)
  {
    case ropCopy:
      esStrechDraw01_Copy(Dst, d, Src, s);
      return 1;
    case ropOr:
      esStrechDraw01_Or(Dst, d, Src, s);
      return 1;
    case ropXor:
      esStrechDraw01_Xor(Dst, d, Src, s);
      return 1;
    case ropAnd:
      esStrechDraw01_And(Dst, d, Src, s);
      return 1;
    default:
      return 0;
  }
}

// to do: Add return`s error
int esStrechDrawRop04(PBitMap Dst, TRect d, PBitMap Src, TRect s, TRop Rop)
{
  switch(Rop)
  {
    case ropCopy:
      esStrechDraw04_Copy(Dst, d, Src, s);
      return 1;
    case ropOr:
      esStrechDraw04_Or(Dst, d, Src, s);
      return 1;
    case ropXor:
      esStrechDraw04_Xor(Dst, d, Src, s);
      return 1;
    case ropAnd:
      esStrechDraw04_And(Dst, d, Src, s);
      return 1;
    default:
      return 0;
  }
}

// to do: add pixel formats
int esStrechDrawRop(PBitMap Dst, TRect d, PBitMap Src, TRect s, TRop Rop)
{
  switch(Src->PixelFormat)
  {
    case pf1bit:
      return esStrechDrawRop01(Dst, d, Src, s, Rop);
    case pf4bit:
      return esStrechDrawRop04(Dst, d, Src, s, Rop);
    case pf2bit:
    case pfC2:
      return 0;
  }
}

// Align
int esAlignDrawRop(PBitMap Dst, PBitMap Src, unsigned int Align, TRop Rop)
{
  int x, y, t;
  TRect d;

  if(Align & alStretch)
    return esStrechDrawRop(Dst, esGetBitMapRect(Dst), Src, esGetBitMapRect(Src), Rop);
  else
    if(Align & alFit)
    {
      t = MIN(Dst->Width, Dst->Height);

      if((float)(Dst->Height / (float)Src->Height) * Src->Width > t)//w > h)
      {
        d.x1 = 0;
        d.x2 = Dst->Width;
        d.y1 = Dst->Height / 2 - Dst->Width / 2;
        d.y2 = d.y1 + Dst->Width;
      }
      else
      {
        d.y1 = 0;
        d.y2 = Dst->Height;
        d.x1 = Dst->Width / 2 - Dst->Height / 2;
        d.x2 = d.x1 + Dst->Height;
      }
      return esStrechDrawRop(Dst, d, Src, esGetBitMapRect(Src), Rop);
    }

  if((Align & (alTop | alBottom)) == 0)
  {
    y = Dst->Height / 2 - Src->Height / 2;
  }
  else
  {
    if(Align & alTop)
      y = 0;
    else
      y = Dst->Height - Src->Height;
  }

  if((Align & (alLeft | alRight)) == 0)
  {
    x = Dst->Width / 2 - Src->Width / 2;
  }
  else
  {
    if(Align & alLeft)
      x = 0;
    else
      x = Dst->Width - Src->Width;
  }
 
  return esBitBltRop(Dst, x, y, Src, Rop);
}

int esAlignDraw(PBitMap Dst, PBitMap Src, unsigned int Align)
{
  return esAlignDrawRop(Dst, Src, Align, ropCopy);
}

//******************************************************************************
// BitMap effects
//******************************************************************************

int esCopyBitMap(PBitMap Dst, PBitMap Src)
{
  unsigned char *pDst, *pSrc;
  int Scanline;

  if(!esIsCompatibleBitMap(Dst, Src))return 0;

  Scanline = esScanline(Dst);

  pSrc = Src->Pixels;
  for(pDst = Dst->Pixels; pDst < Dst->Pixels + Dst->Height * Scanline; pDst++)
  {
    *pDst = *pSrc;
    pSrc++;
  }

  return 1;
}

void esInverseBitMap(PBitMap BitMap)
{
  unsigned char *p, *pEnd;
  int Scanline;

  Scanline = esScanline(BitMap);
  p = BitMap->Pixels;
  pEnd = BitMap->Pixels + BitMap->Height * Scanline;
  
  do
  {
    *p = ~*p;
  }
  while(++p < pEnd);
}

void esFlipVBitMap(PBitMap BitMap)
{
  unsigned char *p, *pEnd, t, *a, *b;
  int Scanline, x, y;

  Scanline = esScanline(BitMap);
  p = BitMap->Pixels;
  pEnd = BitMap->Pixels + BitMap->Height * Scanline;

  for(y = 0; y < BitMap->Height/2; y++)
  {
    a = p + Scanline * y;
    b = p + (BitMap->Height - 1 - y) *  Scanline;
    for(x = 0; x < Scanline; x++)
    {
      // t = *a;
      // *a = *b;
      // *b = t;
      SWAP(*a, *b, t);
      a++;
      b++;
    }
  }
}

void esFlipHBitMap(PBitMap BitMap)
{
  unsigned char *p, *pEnd, t;
  int Scanline, x, y;

  Scanline = esScanline(BitMap);
  p = BitMap->Pixels;
  pEnd = BitMap->Pixels + BitMap->Height * Scanline;

  for(x = 0; x < BitMap->Width/2; x++)
  {
    for(y = 0; y < BitMap->Height; y++)
    {
      t = esGetPixel(BitMap, x, y);
      esSetPixel(BitMap, x, y, esGetPixel(BitMap, BitMap->Width - 1 - x, y));
      esSetPixel(BitMap, BitMap->Width - 1 - x, y, t);
    }
  }
}


void esRotate90BitMap(PBitMap BitMap, int Angle)
{
  PBitMap R, TempBitMap;
  int Scanline, x, y;
  unsigned char t;

  if(Angle != 1 && Angle != 2 && Angle != 3)
    return;

  Scanline = esScanline(BitMap);

  if(Angle == 1 || Angle == 3)
    R = esCreateBitMap(BitMap->Height, BitMap->Width, BitMap->PixelFormat);
  else
    R = esCreateBitMap(BitMap->Width, BitMap->Width, BitMap->PixelFormat);

  switch(Angle)
  {
    case 1:
      for(y = 0; y < BitMap->Height; y++)
      {
        for(x = 0; x < BitMap->Width; x++)
        {
          t = esGetPixel(BitMap, x, BitMap->Height - y - 1);
          esSetPixel(R, y, x, t); 
        }
      }  
      break;
    case 2:
      for(y = 0; y < BitMap->Height; y++)
      {
        for(x = 0; x < BitMap->Width; x++)
        {
          t = esGetPixel(BitMap, BitMap->Width - x - 1, BitMap->Height - y - 1);
          esSetPixel(R, x, y, t); 
        }
      }  
      break;
    case 3:
      for(y = 0; y < BitMap->Height; y++)
      {
        for(x = 0; x < BitMap->Width; x++)
        {
          t = esGetPixel(BitMap, BitMap->Width - x - 1, y);
          esSetPixel(R, y, x, t); 
        }
      } 
      break;
  }

  // it`s magic
  TempBitMap = (PBitMap)malloc(sizeof(TBitMap));
  *TempBitMap = *BitMap;
  esFreeBitMap(TempBitMap);
  *BitMap = *R;
}

//******************************************************************************
// TBitMap
//******************************************************************************
// Not use esScanline in "TBitMap" block!

unsigned char *esCreateBitMapData(int Width, int Height, TPixelFormat pf)
{
  unsigned char *Pixels;
  Pixels = (unsigned char*)malloc(sizeof(unsigned char)*esScanlinePF(Width, pf)*Height);
  // add exept
  return Pixels;
}

unsigned char *esResizeBitMapData(unsigned char *Pixels, int Width, int Height, TPixelFormat pf)
{
  Pixels = (unsigned char*)realloc((void*)Pixels, sizeof(unsigned char)*esScanlinePF(Width, pf)*Height);
  // add exept
  return Pixels;
}

int esLoadBitMapData(PBitMap BitMap, const unsigned char *pSrc)
{
  unsigned char *pDst;
  int Scanline;

  if(pSrc == NULL)
    return 1;

  Scanline = esScanline(BitMap);

  for(pDst = BitMap->Pixels; pDst < BitMap->Pixels + BitMap->Height * Scanline; pDst++)
  {
    *pDst = *pSrc;
    pSrc++;
  }

  return 0;
}

void esFreeBitMapData(unsigned char *Pixels)
{
  free((void*)Pixels);
}

int esIsCompatibleBitMap(PBitMap Dst, PBitMap Src)
{
  return Dst->Width == Src->Width &&
    Dst->Height == Src->Height &&
    Dst->PixelFormat == Src->PixelFormat;
}

//------------------------------------------------------------------------------

PBitMap esCreateBlankBitMap(TPixelFormat pf)
{
  PBitMap BitMap;
  
  BitMap = (PBitMap)malloc(sizeof(TBitMap));

  BitMap->Width = 0;
  BitMap->Height = 0;
  BitMap->PixelFormat = pf;
  BitMap->AlphaFormat = afNone;
  #ifdef USE_PRECALC_SCANLINE
  BitMap->Scanline = 0;
  #endif
  BitMap->Pixels = NULL;
  BitMap->Mask = NULL;

  return BitMap;
}

PBitMap esCreateBitMap(int Width, int Height, TPixelFormat pf)
{
  PBitMap BitMap;
  
  BitMap = (PBitMap)malloc(sizeof(TBitMap));

  BitMap->Width = Width;
  BitMap->Height = Height;
  BitMap->PixelFormat = pf;
  BitMap->AlphaFormat = afNone;

  #ifdef USE_PRECALC_SCANLINE
  BitMap->Scanline = esScanlinePF(BitMap->Width, BitMap->PixelFormat);
  #endif
  
  BitMap->Pixels = esCreateBitMapData(Width, Height, pf);
  BitMap->Mask = 0;
  // add exept
  return BitMap;
}

PBitMap esCreateBitMapOf(int Width, int Height, TPixelFormat pf, const unsigned char *Pixels)
{
  PBitMap BitMap;

  BitMap = esCreateBitMap(Width, Height, pf);

  esLoadBitMapData(BitMap, Pixels);

  return BitMap;
}

PBitMap esCreateStaticBitMap(int Width, int Height, TPixelFormat pf, const unsigned char *Pixels)
{
  PBitMap BitMap;
  
  BitMap = (PBitMap)malloc(sizeof(TBitMap));

  BitMap->Width = Width;
  BitMap->Height = Height;
  BitMap->PixelFormat = pf;
  BitMap->AlphaFormat = afNone;
  BitMap->Pixels = (unsigned char*)Pixels;
  BitMap->Mask = 0;

  #ifdef USE_PRECALC_SCANLINE
  BitMap->Scanline = esScanlinePF(BitMap->Width, BitMap->PixelFormat);
  #endif

  return BitMap;
}

PBitMap esCreateStaticMaskBitMap(int Width, int Height, TPixelFormat pf, const unsigned char *Pixels,
  const unsigned char *Mask)
{
  PBitMap BitMap;
  
  BitMap = esCreateStaticBitMap(Width, Height, pf, Pixels);
  BitMap->AlphaFormat = afMask;
  BitMap->Mask = (unsigned char*)Mask;

  return BitMap;
}

void esResizeBitMap(PBitMap BitMap, int Width, int Height)
{
  BitMap->Pixels = esResizeBitMapData(BitMap->Pixels, Width, Height, BitMap->PixelFormat);
  if(BitMap->Mask != NULL)
    BitMap->Mask = esResizeBitMapData(BitMap->Mask, Width, Height, BitMap->PixelFormat);

  // add exept
  BitMap->Width = Width;
  BitMap->Height = Height;

  #ifdef USE_PRECALC_SCANLINE
  BitMap->Scanline = esScanlinePF(BitMap->Width, BitMap->PixelFormat);
  #endif
}

void esResizeBitMapEx(PBitMap BitMap, int Width, int Height, unsigned char Color, unsigned int Align)
{
  PBitMap Temp;

  Temp = esCloneBitMap(BitMap);

  esResizeBitMap(BitMap, Width, Height);
  
  //if((Align & alStretch) == 0)
  esClear(BitMap, Color);

  esAlignDraw(BitMap, Temp, Align);

  esFreeBitMap(Temp);
}

PBitMap esCloneBitMap(PBitMap BitMap)
{
  PBitMap Clone;

  Clone = esCreateBitMap(BitMap->Width, BitMap->Height, BitMap->PixelFormat);
  esCopyBitMap(Clone, BitMap);

  return Clone;
}

PBitMap esFreeBitMap(PBitMap BitMap)
{
  esFreeBitMapData(BitMap->Pixels);
  if(BitMap->Mask != NULL)
    esFreeBitMapData(BitMap->Mask);
  free(BitMap);
  return 0;
}

PBitMap esFreeStaticBitMap(PBitMap BitMap)
{
  free(BitMap);
  return 0;
}

// ENDFILE






