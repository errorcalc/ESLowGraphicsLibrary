#include "GraphicFiles.h"
#include "stdio.h"

#pragma pack(push, 1)

typedef struct
{
  unsigned short Type;
  unsigned long Size;
  unsigned short Reserved1;
  unsigned short Reserved2;
  unsigned long OffBits;
} TBitMapHeader, *PBitMapHeader;

typedef struct
{
  //long Size;
  unsigned short Width;
  unsigned short Height;
  unsigned short Planes;
  unsigned short BitCount;
} TBitMapInfoCore, *PBitMapInfoCore;

typedef struct
{
  //long Size;
  long Width;
  long Height;
  unsigned short Planes;
  unsigned short BitCount;
  unsigned long Compression;
  unsigned long SizeImage;
  long XPelsPerMeter;
  long YPelsPerMeter;
  unsigned long ClrUsed;
  unsigned long ClrImportant;
} TBitMapInfoV3, *PBitMapInfoV3;

#pragma pack(pop)

int esLoadFileBmp(PBitMap BitMap, char *FileName, unsigned int Options)
{
  FILE *f;
  TBitMapHeader Header;
  TBitMapInfoCore Info;
  TBitMapInfoV3 Info3;
  int x, y;
  unsigned long Size;
  unsigned long Offset;
  int Width, Height;
  unsigned char *p, b;
  int IsChangePixelFormat = 0;
  int BitCount;

  f = fopen(FileName, "rb");

  if(f != NULL)
  {
    
    fseek(f, 0x00, SEEK_SET);

    // header
    if(fread(&Header, sizeof(TBitMapHeader), 1, f) == 0)
      goto ErrorCorruptedFile;

    // check bmp file
    if(Header.Type != 0x4d42)
      goto ErrorCorruptedFile;

    // size
    if(fread(&Size, sizeof(long), 1, f) == 0)
      goto ErrorCorruptedFile;

    // info
    if(Size == 12)
    {
      if(fread(&Info, sizeof(TBitMapInfoCore), 1, f) == 0)
        goto ErrorCorruptedFile; 
      BitCount = Info.BitCount;
      Width = Info.Width;
      Height = Info.Height;
    }
    else
    {
      if(fread(&Info3, sizeof(TBitMapInfoV3), 1, f) == 0)
        goto ErrorCorruptedFile; 
      BitCount = Info3.BitCount;
      Width = Info3.Width;
      Height = Info3.Height;
      // to do: добавить поддержку
      if(Info3.Compression != 0)
        goto ErrorUnsupportedPixelFormat;
    }

    // if wrong size
    if(Width < 1 || ABS(Height) < 1)
      goto ErrorUnknownError;
    // if not 4bit or 1bit per pixel
    if(BitCount != 1 && BitCount != 4)
      goto ErrorUnsupportedPixelFormat;

    // ifoSaveSize
    if(Options & ifoSaveSize)
    {
      if(BitMap->Width != Width || BitMap->Height != ABS(Height))
        goto ErrorUnknownError;
    }
    // ifoSavePixelFormat
    if(Options & ifoSavePixelFormat)
    {
      if(esPFToBitCount(BitMap->PixelFormat) != BitCount)
        goto ErrorAnotherPixelFormat;
    }
    else
      if(esPFToBitCount(BitMap->PixelFormat) != BitCount)
      {
        BitMap->PixelFormat = esBitCountToPF(BitCount);
        IsChangePixelFormat = 1;
      }

    // resize
    if(BitMap->Pixels == NULL || IsChangePixelFormat ||
         BitMap->Width != Width || BitMap->Height != ABS(Height))
    {
      esResizeBitMap(BitMap, Width, ABS(Height));
      if(BitMap->Pixels == NULL)
        goto ErrorNotEnoughMemory;
    }

    // begin load
    fseek(f, Header.OffBits, SEEK_SET);

    for(y = 0; y < ABS(Height); y++)
    {
      if(Height < 0)
        p = BitMap->Pixels + esScanline(BitMap) * y;
      else
        p = BitMap->Pixels + esScanline(BitMap) * (Height - y - 1);

      for(x = 0; x < esScanline(BitMap); x++)
      {
        if((b = fgetc(f)) == EOF)
          goto ErrorCorruptedFile;
        *(p++) = b;
      }

      // skip last bytes
      for(; x % 4; x++)
        if(fgetc(f) == EOF)
          goto ErrorCorruptedFile;
    }
    // end load
  
    fclose(f);
    return 0;// is ok
  }
  else
    return errNotOpenFile;

// error`s
ErrorCorruptedFile:
  fclose(f);
  return errCorruptedFile;

ErrorUnsupportedPixelFormat:
  fclose(f);
  return errUnsupportedPixelFormat;

ErrorAnotherPixelFormat:
  fclose(f);
  return errAnotherPixelFormat;

ErrorUnknownError:
  fclose(f);
  return errUnknownError;

ErrorNotEnoughMemory:
  fclose(f);
  return errNotEnoughMemory;
}
