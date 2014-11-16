#ifndef _GRAPHICFILES_H_
#define _GRAPHICFILSE_H_

#include "Graphics.h"

#define errNotOpenFile 100
#define errNotWriteFile 101
#define errCorruptedFile 102
#define errAnotherPixelFormat 103
#define errUnsupportedFormat 104
#define errUnsupportedPixelFormat 105
#define errUnsupportedVersion 106
#define errNotEnoughMemory 107
#define errUnknownError 199

typedef enum 
{
  ifoSaveSize = 1,
  ifoSavePixelFormat = 2,
  ifoEqual = ifoSaveSize | ifoSavePixelFormat,
} TImageFileOptions;

int esLoadFileBmp(PBitMap BitMap, char *FileName, unsigned int Options);

#endif