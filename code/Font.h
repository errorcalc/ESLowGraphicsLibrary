#ifndef _FONT_H_
#define _FONT_H_

typedef enum
{
  ftMonospace,
  ftProportional,// to do: writeln me!
} TFontType;

typedef enum
{
  fmColor,
  fmCopy,
  fmXor
} TFontMode;

typedef struct
{
  int Width;
  int Height;
  TFontType Type;
  TFontMode Mode;
  TPixelFormat PixelFormat;
  TColor Color;
  unsigned char *Data;
} *PFont, TFont;



PFont esSetFont(PFont Font);

int esDrawSymbol01_Color(PBitMap Dst, int x, int y, unsigned char S, TColor Color);
int esDrawSymbol01_Copy(PBitMap Dst, int x, int y, unsigned char S);
#define esDrawSymbol01_Xor(Dst, x, y, S) esDrawSymbol01_Color((Dst), (x), (y), (S), 2)

int esDrawSymbol04_Color(PBitMap Dst, int x, int y, unsigned char S, TColor Color);
int esDrawSymbol04_Xor(PBitMap Dst, int x, int y, unsigned char S);
int esDrawSymbol04_Copy(PBitMap Dst, int x, int y, unsigned char S);

int esDrawText(PBitMap Dst, int x, int y, const char *Text);
int esDrawColorText(PBitMap Dst, int x, int y, const char *Text, TColor Color);

PFont esCreateMonospaceFont(int Width, int Height, TPixelFormat pf, const unsigned char *FontData);
PFont esFreeFont(PFont Font);

#endif