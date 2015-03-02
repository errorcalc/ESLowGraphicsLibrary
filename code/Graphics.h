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

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//====== PROPERTIES ========================
#define USE_SCANLINE_MACRO
#define USE_PRECALC_SCANLINE
#define USE_TRANSLATE
#define USE_SAFE_GRAPHICS
#define SKIP_LAST_PIXEL
//==========================================

#define SWAP(a, b, t)  \
{  \
  t = a;  \
  a = b;  \
  b = t;  \
}

#define MAX(a, b) (((a)>=(b))?(a):(b))
#define MIN(a, b) (((a)<(b))?(a):(b))
#define ABS(value) (((value) < 0)?-(value):(value))

//==========================================

#define Scanline1(w)( ( ((w)+7)/8) )
#define Scanline2(w)( ( ((w)+3)/4) )
#define Scanline4(w)( ( ((w)+1)/2) )

// to do: use it?
// |--------|--------|--------|
//      Left|<- (*)->|Right
// #define NearLeftByte1(x) ((x)/8)
// #define NearRightByte1(x) ((x)/8+1)
// #define FullBytes1(a, b) (NearRightByte1(a) - NearLeftByte1(b)) 

// FUCKED MAKRO
// note: if edit this macro then edit esScanlinePF in "Graphics.c"
#define ES_SCANLINE_PF(Width, pf)                     \
  (((pf == pf1bit)?Scanline1(Width):                   \
   ((pf == pf4bit)?Scanline4(Width):Scanline2(Width))))

#ifdef USE_SCANLINE_MACRO
  #define esScanlinePF ES_SCANLINE_PF   
   
  #ifndef USE_PRECALC_SCANLINE
    #define esScanline(BitMap) (ES_SCANLINE_PF((BitMap)->Width, (BitMap)->PixelFormat))
  #else
    #define esScanline(BitMap) ((BitMap)->Scanline)
  #endif
#endif

#define CLIP_POINT(x, y, x1, y1, x2, y2)  \
{  \
  if(x < x1)x = x1;  \
  if(x > x2)x = x2;  \
  if(y < y1)y = y1;  \
  if(y > y2)y = y2;  \
}

#define NORMALIZE_RECT(x1, y1, x2, y2, t)  \
{  \
  if(x1 > x2)SWAP(x1, x2, t);  \
  if(y1 > y2)SWAP(y1, y2, t);  \
}

#define IS_ENTRY_SEG(a1, a2, b1, b2)  \
(((a1 >= b1)&&(a1 <= b2))||((a2 >= b1)&&(a2 <= b2)))

#define TRANSLATE_POINT(x, y, tx, ty) \
{  \
  (x) += (tx); \
  (y) += (ty); \
}

#define TRANSLATE_RECT(x1, y1, x2, y2, tx, ty) \
{  \
  (x1) += (tx); \
  (y1) += (ty); \
  (x2) += (tx); \
  (y2) += (ty); \
}

#define DECREMENT_POINT(x, y) \
{  \
  (x)--; \
  (y)--; \
}

//==========================================
// Liblary structs and enums
//==========================================

#ifndef _DEFINE_GEOMETRY_TYPES_
#define _DEFINE_GEOMETRY_TYPES_
typedef struct
{
  int X;
  int Y;
} TPoint, *PPoint;

typedef struct
{
  int x1, y1;
  int x2, y2;
} TRect, *PRect;
#endif

typedef unsigned TColor;

typedef enum
{
  alLeft = 1,
  alRight = 2,
  alTop = 4,
  alBottom = 8,
  alLeftTop = alLeft | alTop,
  alRightTop = alRight | alTop,
  alLeftBottom = alLeft | alBottom,
  alRightBottom = alRight | alBottom,
  alCenter = 256,
  alStretch = 512,
  alFit = 1024,
} TAlign;

typedef enum
{
  pfNone = 0,
  pf1bit = 1,
  pf2bit = 2,
  pf4bit = 4,
  pfC2 = 8,
  pfUnsupport = 128,
} TPixelFormat;

typedef enum
{
  afNone = 0,
  afMask = 16,
  afColor = 32
} TAlphaFormat;

typedef enum
{
  ropCopy,
  ropOr,
  ropXor,
  ropAnd,
  ropNotAnd
} TRop;

typedef struct
{
  int Width;
  int Height;
  unsigned char *Pixels;
  unsigned char *Mask;
  TPixelFormat PixelFormat;
  TAlphaFormat AlphaFormat;
  TColor TransparentColor;//todo: add default
  // precalc
  int Scanline;
} TBitMap, *PBitMap;

//==========================================
// Liblary functions
//==========================================

// Getters and setters
void esSetCheckCoord(int _Check);
int esGetCheckCoord(void);
void esSetTranslate(int _Translate);
int esGetTranslate(void);
void esSetCenter(int x, int y);
void esGetCenter(int *x, int *y);
void esTranslateCenter(int x, int y);
void esSetSkipLastPixel(int _IsSLP);
int esGetSkipLastPixel(void);

//******************************************************************************
// Utils
//******************************************************************************
int esIsEntrySeg(int a1, int a2, int b1, int b2);
int esClipPoint(int w, int h, int *x1, int *y1, int *x2, int *y2);
int esClipLine(int w, int h, int *x1, int *y1, int *x2, int *y2);

unsigned char esFillByte(TColor Color, TPixelFormat pf);
int esIsRealColor(TColor Color, TPixelFormat pf);
#ifndef USE_SCANLINE_MACRO
int esScanlinePF(int Width, TPixelFormat pf);
int esScanline(PBitMap BitMap);
#endif

TRect esGetBitMapRect(PBitMap BitMap);

int esPFToBitCount(TPixelFormat pf);
TPixelFormat esBitCountToPF(int Count);

//******************************************************************************
// System graphics
//******************************************************************************
// Pixels opertions for pf1bit
void esSetPixel1(PBitMap BitMap, int x, int y, TColor color);
void esDrawPixel1_Or(PBitMap BitMap, int x, int y, TColor color);
void esDrawPixel1_Xor(PBitMap BitMap, int x, int y, TColor color);
void esDrawPixel1_And(PBitMap BitMap, int x, int y, TColor color);
TColor esGetPixel1(PBitMap BitMap, int x, int y);

// Pixels opertions for pf2bit
void esSetPixel2(PBitMap BitMap, int x, int y, TColor color);
TColor esGetPixel2(PBitMap BitMap, int x, int y);

// Pixels opertions for pf4bit
void esSetPixel4(PBitMap BitMap, int x, int y, TColor color);
void esDrawPixel4_And(PBitMap BitMap, int x, int y, TColor color);
void esDrawPixel4_Or(PBitMap BitMap, int x, int y, TColor color);
void esDrawPixel4_Xor(PBitMap BitMap, int x, int y, TColor color);
TColor esGetPixel4(PBitMap BitMap, int x, int y);

// other (UNSAFE!!!)
void esFillPixels(unsigned char *pStart, unsigned char *pEnd, TColor Color);
void esDrawHLine1(PBitMap BitMap, int x1, int x2, int y, TColor Color);
void esDrawHLine2(PBitMap BitMap, int x1, int x2, int y, TColor Color);
void esDrawHLine4(PBitMap BitMap, int x1, int x2, int y, TColor Color);
void esDrawHLine(PBitMap BitMap, int x1, int x2, int y, TColor Color);
void esDrawVLine(PBitMap BitMap, int y1, int y2, int x, TColor Color);
void esDrawLineU(PBitMap BitMap, int x1, int y1, int x2, int y2, TColor Color);

//******************************************************************************
// Main graphics
//******************************************************************************
void esClear(PBitMap BitMap, TColor Color);
void esSetPixel(PBitMap BitMap, int x, int y, TColor color);
TColor esGetPixel(PBitMap BitMap, int x, int y);
void esFillRect(PBitMap BitMap, int x1, int y1, int x2, int y2, TColor Color);
void esDrawRect(PBitMap BitMap, int x1, int y1, int x2, int y2, TColor Color);
void esDrawLine(PBitMap BitMap, int x1, int y1, int x2, int y2, TColor Color);
void esDrawElipse(PBitMap BitMap, int x1, int y1, int x2, int y2, unsigned char Color);
void esFillElipse(PBitMap BitMap, int x1, int y1, int x2, int y2, unsigned char Color);

//******************************************************************************
// The BitBlt functions
//******************************************************************************
// BitBlt for pf1bit
int esBitBlt01_Copy(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt01_Or(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt01_And(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt01_NotAnd(PBitMap Dst, int x, int y, PBitMap Src); // to do: add
int esBitBlt01_Xor(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt01_MaskData(PBitMap Dst, int x, int y, PBitMap Src, unsigned char *MaskData);
// #define esBitBlt01_Mask(Dst, x, y, Src) esBitBlt01_MaskData(Dst, x, y, Src, (Src)->Mask);
int esBitBlt01_Mask(PBitMap Dst, int x, int y, PBitMap Src);

// BitBlt for pf4bit
int esBitBlt04_Copy(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt04_Or(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt04_Xor(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt04_And(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt04_MaskData(PBitMap Dst, int x, int y, PBitMap Src, unsigned char *MaskData);
// #define esBitBlt04_Mask(Dst, x, y, Src) esBitBlt04_MaskData(Dst, x, y, Src, (Src)->Mask);
int esBitBlt04_Mask(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt04_Transparent(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt04_Color(PBitMap Dst, int x, int y, PBitMap Src, TColor Color);

// Universal BitBlt functions
int esBitBlt01(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt04(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBlt(PBitMap Dst, int x, int y, PBitMap Src);
int esBitBltRop01(PBitMap Dst, int x, int y, PBitMap Src, TRop Rop);
int esBitBltRop04(PBitMap Dst, int x, int y, PBitMap Src, TRop Rop);
int esBitBltRop(PBitMap Dst, int x, int y, PBitMap Src, TRop Rop);

//******************************************************************************
// Strech draw functions
//******************************************************************************
// StrechDraw for pf1bit
void esStrechDraw01_Copy(PBitMap Dst, TRect d, PBitMap Src, TRect s);
void esStrechDraw01_Or(PBitMap Dst, TRect d, PBitMap Src, TRect s);
void esStrechDraw01_Xor(PBitMap Dst, TRect d, PBitMap Src, TRect s);
void esStrechDraw01_And(PBitMap Dst, TRect d, PBitMap Src, TRect s);
void esStrechDraw01_Mask(PBitMap Dst, TRect d, PBitMap Src, TRect s, PBitMap Mask);

// StrechDraw for pf4bit
void esStrechDraw04_Copy(PBitMap Dst, TRect d, PBitMap Src, TRect s);
void esStrechDraw04_Or(PBitMap Dst, TRect d, PBitMap Src, TRect s);
void esStrechDraw04_Xor(PBitMap Dst, TRect d, PBitMap Src, TRect s);
void esStrechDraw04_And(PBitMap Dst, TRect d, PBitMap Src, TRect s);
void esStrechDraw04_Mask(PBitMap Dst, TRect d, PBitMap Src, TRect s, PBitMap Mask);
void esStrechDraw04_Transparent(PBitMap Dst, TRect d, PBitMap Src, TRect s);
void esStrechDraw04_Color(PBitMap Dst, TRect d, PBitMap Src, TRect s, TColor Color);

// Uneversal Strech draw functions
int esStrechDraw01(PBitMap Dst, TRect d, PBitMap Src, TRect s);
int esStrechDraw04(PBitMap Dst, TRect d, PBitMap Src, TRect s);
int esStrechDraw(PBitMap Dst, TRect d, PBitMap Src, TRect s);
int esStrechDrawRop01(PBitMap Dst, TRect d, PBitMap Src, TRect s, TRop Rop);
int esStrechDrawRop04(PBitMap Dst, TRect d, PBitMap Src, TRect s, TRop Rop);
int esStrechDrawRop(PBitMap Dst, TRect d, PBitMap Src, TRect s, TRop Rop);
// Align
int esAlignDrawRop(PBitMap Dst, PBitMap Src, unsigned int Align, TRop Rop);
int esAlignDraw(PBitMap Dst, PBitMap Src, unsigned int Align);

//******************************************************************************
// BitMap effects
//******************************************************************************
int esCopyBitMap(PBitMap Dst, PBitMap Src);
void esInverseBitMap(PBitMap BitMap);
void esFlipVBitMap(PBitMap BitMap);
void esFlipHBitMap(PBitMap BitMap);
void esRotate90BitMap(PBitMap BitMap, int Angle); // 0, 1, 2, 3

//******************************************************************************
// TBitMap
//******************************************************************************
unsigned char *esCreateBitMapData(int Width, int Height, TPixelFormat pf);
unsigned char *esResizeBitMapData(unsigned char *Pixels, int Width, int Height, TPixelFormat pf);
int esLoadBitMapData(PBitMap BitMap, const unsigned char *pSrc);
void esFreeBitMapData(unsigned char *Pixels);
int esIsCompatibleBitMap(PBitMap Dst, PBitMap Src);
//---
PBitMap esCreateBlankBitMap(TPixelFormat pf); 
PBitMap esCreateBitMap(int Width, int Height, TPixelFormat pf); 
PBitMap esCreateBitMapOf(int Width, int Height, TPixelFormat pf, const unsigned char *Pixels); 
PBitMap esCreateStaticBitMap(int Width, int Height, TPixelFormat pf, const unsigned char *Pixels);
PBitMap esCreateStaticMaskBitMap(int Width, int Height, TPixelFormat pf, const unsigned char *Pixels,
  const unsigned char *Mask);
void esResizeBitMap(PBitMap BitMap, int Width, int Height); 
void esResizeBitMapEx(PBitMap BitMap, int Width, int Height, unsigned char Color, unsigned int Align);
PBitMap esCloneBitMap(PBitMap BitMap);
PBitMap esFreeBitMap(PBitMap BitMap);
PBitMap esFreeStaticBitMap(PBitMap BitMap);


#endif