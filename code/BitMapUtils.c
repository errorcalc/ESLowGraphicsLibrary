#include "Graphics.h"
#include "BitMapUtils.h"
#include "stddef.h"

static const unsigned char StdHatch_pf2bit[] =
{
  0x00, 0x55, 0x55, 0xFF,// 0
  0x00, 0xAA, 0xEE, 0xFF,// 1
  0x00, 0x55, 0x55, 0xFF,// 2
  0x00, 0xAA, 0xBB, 0xFF,// 3
  0x00, 0x55, 0x55, 0xFF,// 4
  0x00, 0xAA, 0xEE, 0xFF,// 5
  0x00, 0x55, 0x55, 0xFF,// 6
  0x00, 0xAA, 0xBB, 0xFF,// 7
};

static const unsigned char StdHatch_pf4bit[] =
{
  0x00, 0x00, 0x22, 0x22, 0xAA, 0xAA, 0xAA, 0xAA, 0xEE, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,// 0
  0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x55, 0x55, 0x55, 0x55, 0xDD, 0xDD, 0xFF, 0xFF, 0xFF,// 1
  0x00, 0x88, 0x88, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,// 2
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x55, 0x55, 0x55, 0x55, 0x55, 0x77, 0x77, 0xFF, 0xFF,// 3
  0x00, 0x00, 0x22, 0x22, 0xAA, 0xAA, 0xAA, 0xAA, 0xEE, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,// 4
  0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x55, 0x55, 0x55, 0x55, 0xDD, 0xDD, 0xFF, 0xFF, 0xFF,// 5
  0x00, 0x88, 0x88, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,// 6
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x55, 0x55, 0x55, 0x55, 0x55, 0x77, 0x77, 0xFF, 0xFF,// 7
};


static unsigned char *Hatch_pf2bit = (unsigned char*)StdHatch_pf2bit;
static unsigned char *Hatch_pf4bit = (unsigned char*)StdHatch_pf4bit;

void SetHatch_pf2bit(unsigned char *Hatch)
{
  if(Hatch != NULL)
    Hatch_pf2bit = Hatch;
  else
    Hatch_pf2bit = (unsigned char*)StdHatch_pf2bit;
}

unsigned char *GetHatch_pf2bit(void)
{
  return Hatch_pf2bit;
}

void SetHatch_pf4bit(unsigned char *Hatch)
{
  if(Hatch != NULL)
    Hatch_pf4bit = Hatch;
  else
    Hatch_pf4bit = (unsigned char*)StdHatch_pf4bit;
}

unsigned char *GetHatch_pf4bit(void)
{
  return Hatch_pf4bit;
}

// UnSafe !!!
//                         Source                                         Dest
// [ 0 1 | 2 3 | 4 5 | 6 7 ] + [ 0 1 | 2 3 | 4 5 | 6 7 ] => [ 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 ]
// |           p1          |   |           p2          |    |       p2      |       p1      |
int Convert_pf2bit_To_pf1bit(PBitMap Dest, PBitMap Source)
{
  unsigned char *Hatch = Hatch_pf2bit;
  int x, y, hy;
  unsigned char p1, p2;
  unsigned char *pSource, *pDest, *LineHatch;
  
  pSource = Source->Pixels;
  pDest = Dest->Pixels;
  hy = 0;
  for(y = 0; y < Dest->Height; y++)
  {
    hy = (y%8)*4;
    for(x = 0; x < Scanline1(Dest->Width); x++)
    {
      LineHatch = Hatch + hy;
      p1 = *(pSource++);                 // p1 = Source->Pixels[Scanline2(Dest->Width)*y + x*2];
      p2 = *(pSource++);                 // p2 = Source->Pixels[Scanline2(Dest->Width)*y + x*2 + 1];
      *(pDest++) =                       // Dest->Pixels[Scanline1(Dest->Width)*y + x] = 
      (LineHatch[(p2>>0)&0x03] & BIT0) | // (Hatch[ hy + ((p2>>0)&0x03) ] & bit0) |
      (LineHatch[(p2>>2)&0x03] & BIT1) |
      (LineHatch[(p2>>4)&0x03] & BIT2) |
      (LineHatch[(p2>>6)&0x03] & BIT3) |
      (LineHatch[(p1>>0)&0x03] & BIT4) |
      (LineHatch[(p1>>2)&0x03] & BIT5) |
      (LineHatch[(p1>>4)&0x03] & BIT6) |
      (LineHatch[(p1>>6)&0x03] & BIT7);  
    }
  }
}

// UnSafe !!!
int Convert_pf4bit_To_pf1bit(PBitMap Dest, PBitMap Source)
{
  unsigned char *Hatch = Hatch_pf4bit;
  int x, y, hy;
  unsigned char p1, p2, p3, p4;
  unsigned char *pSource, *pDest, *LineHatch;
  
  pSource = Source->Pixels;
  pDest = Dest->Pixels;
  hy = 0;
  for(y = 0; y < Dest->Height; y++)
  {
    hy = (y%8)*16;
    for(x = 0; x < Scanline1(Dest->Width); x++)
    {
      LineHatch = Hatch + hy;
      p1 = *(pSource++); 
      p2 = *(pSource++); 
      p3 = *(pSource++);
      p4 = *(pSource++);
      *(pDest++) =             
      (LineHatch[(p4>>0)&0x0F] & BIT0) | 
      (LineHatch[(p4>>4)&0x0F] & BIT1) |
      (LineHatch[(p3>>0)&0x0F] & BIT2) |
      (LineHatch[(p3>>4)&0x0F] & BIT3) |
      (LineHatch[(p2>>0)&0x0F] & BIT4) |
      (LineHatch[(p2>>4)&0x0F] & BIT5) |
      (LineHatch[(p1>>0)&0x0F] & BIT6) |
      (LineHatch[(p1>>4)&0x0F] & BIT7);  
    }
  }
}

void GetBitMapRect(PBitMap BitMap, PRect Rect)
{
  Rect->x1 = 0;
  Rect->y1 = 0;
  Rect->x2 = BitMap->Width;
  Rect->y2 = BitMap->Height;
}