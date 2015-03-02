#include "stdlib.h"
#include "Graphics.h"
#include "Font.h"

// monospace font:
//  ...
//   A
//   B
//   C
//  ...

static PFont ActiveFont = NULL;
static TBitMap RealFontBitMap;
static PBitMap BitMap = &RealFontBitMap;
static unsigned char *FontData;
static int SymbolSize;

PFont esSetFont(PFont Font)
{
  ActiveFont = Font;

  BitMap->PixelFormat = Font->PixelFormat;
  BitMap->Height = Font->Height;
  BitMap->Width = Font->Width;
  BitMap->Scanline = ES_SCANLINE_PF(BitMap->Width, BitMap->PixelFormat);// fuck
  SymbolSize = esScanline(BitMap) * BitMap->Height;

  return Font;
}

/*
int esDrawSymbol01(PBitMap Dst, int x, int y, unsigned char S, PFont Font)
{
  if(Font != ActiveFont)esSetFont(Font);

  if(Font == NULL || Dst->PixelFormat != BitMap->PixelFormat)
    return 0;

  BitMap->Pixels = FontData + SymbolSize * S;
  
  switch(Font->Mode)
  {
    case 0:
      esBitBlt01_
      return 1;
  }
}
*/

int esDrawSymbol01_Color(PBitMap Dst, int x, int y, unsigned char S, TColor Color)
{
  BitMap->Pixels = ActiveFont->Data + SymbolSize * S;
  switch(Color)
  {
    case 0:
      esBitBlt01_NotAnd(Dst, x, y, BitMap);
      break;
    case 1:
      esBitBlt01_Or(Dst, x, y, BitMap);
      break;
    case 2:
      esBitBlt01_Xor(Dst, x, y, BitMap);
      break;
  }

  return 1;
}

int esDrawSymbol01_Copy(PBitMap Dst, int x, int y, unsigned char S)
{
  BitMap->Pixels = ActiveFont->Data + SymbolSize * S;

  return esBitBlt01_Copy(Dst, x, y, BitMap); 

  return 1;
}

int esDrawSymbol04_Color(PBitMap Dst, int x, int y, unsigned char S, TColor Color)
{
  BitMap->Pixels = ActiveFont->Data + SymbolSize * S;

  return esBitBlt04_Color(Dst, x, y, BitMap, Color); 

  return 1;
}

int esDrawSymbol04_Xor(PBitMap Dst, int x, int y, unsigned char S)
{
  BitMap->Pixels = ActiveFont->Data + SymbolSize * S;

  return esBitBlt04_Xor(Dst, x, y, BitMap); 

  return 1;
}

int esDrawSymbol04_Copy(PBitMap Dst, int x, int y, unsigned char S)
{
  BitMap->Pixels = ActiveFont->Data + SymbolSize * S;

  return esBitBlt04_Copy(Dst, x, y, BitMap); 

  return 1;
}


int esDrawTextEx(PBitMap Dst, int x, int y, const char *Text)
{
  int sx;
  int i = 0;
  int IsDraw = 0;

  if(x >= Dst->Width || y >= Dst->Height || y <= -ActiveFont->Height)
    return 0;

  if(x <= -ActiveFont->Width)
  {
    i = (-x) / ActiveFont->Width;
    sx =  - (-x) % ActiveFont->Width;
  }
  else
    sx = x;
  
  switch(Dst->PixelFormat)
  {
    case pf1bit:
      switch(ActiveFont->Mode)
      {
        case fmCopy:
          for(;sx < Dst->Width && Text[i] != 0; sx += ActiveFont->Width)
          {
            esDrawSymbol01_Copy(Dst, sx, y, Text[i++]);
            IsDraw = 1;
          }
          break;

        case fmColor:
          for(;sx < Dst->Width && Text[i] != 0; sx += ActiveFont->Width)
          {
            esDrawSymbol01_Color(Dst, sx, y, Text[i++], ActiveFont->Color);
            IsDraw = 1;
          }
          break;

        case fmXor:
          for(;sx < Dst->Width && Text[i] != 0; sx += ActiveFont->Width)
          {
            esDrawSymbol01_Xor(Dst, sx, y, Text[i++]);
            IsDraw = 1;
          }
          break;
      }
      break;

    case pf4bit:
      switch(ActiveFont->Mode)
      {
        case fmCopy:
          for(;sx < Dst->Width && Text[i] != 0; sx += ActiveFont->Width)
          {
            esDrawSymbol04_Copy(Dst, sx, y, Text[i++]);
            IsDraw = 1;
          }
          break;

        case fmColor:
          for(;sx < Dst->Width && Text[i] != 0; sx += ActiveFont->Width)
          {
            esDrawSymbol04_Color(Dst, sx, y, Text[i++], ActiveFont->Color);
            IsDraw = 1;
          }
          break;

        case fmXor:
          for(;sx < Dst->Width && Text[i] != 0; sx += ActiveFont->Width)
          {
            esDrawSymbol04_Xor(Dst, sx, y, Text[i++]);
            IsDraw = 1;
          }
          break;
      }
      break;
  }

  return IsDraw;
}


int esDrawText(PBitMap Dst, int x, int y, const char *Text, TColor Color)
{
  int sx;
  int i = 0;
  int IsDraw = 0;

  if(x >= Dst->Width || y >= Dst->Height || y <= -ActiveFont->Height)
    return 0;

  if(x <= -ActiveFont->Width)
  {
    i = (-x) / ActiveFont->Width;
    sx =  - (-x) % ActiveFont->Width;
  }
  else
    sx = x;
  
  switch(Dst->PixelFormat)
  {
    case pf1bit:
        for(;sx < Dst->Width && Text[i] != 0; sx += ActiveFont->Width)
        {
          esDrawSymbol01_Color(Dst, sx, y, Text[i++], Color);
          IsDraw = 1;
        }
      break;

    case pf4bit:
        for(;sx < Dst->Width && Text[i] != 0; sx += ActiveFont->Width)
        {
          esDrawSymbol04_Color(Dst, sx, y, Text[i++], Color);
          IsDraw = 1;
        }
      break;
  }

  return IsDraw;
}


PFont esCreateMonospaceFont(int Width, int Height, TPixelFormat pf, const unsigned char *FontData)
{
  PFont Font;

  Font = (PFont)malloc(sizeof(TFont));

  Font->Width = Width;
  Font->Height = Height;
  Font->Type = ftMonospace;
  Font->Mode = fmColor;
  Font->Color = 0;
  Font->PixelFormat = pf;
  Font->Data = (unsigned char*)FontData;

  return Font;
}

PFont esFreeFont(PFont Font)
{
  free(Font);
  return NULL;
}