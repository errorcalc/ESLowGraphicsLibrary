#include "..\..\..\code\Graphics.h"
#include "..\..\..\code\GraphicFiles.h"
#include "SysUtils.h"
#include "GlobalInput.h"
#include "Math.h"
#include "stdio.h"

void SampleMain(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  SysCreateScreen(600, 600, pf4bit, CS_ZOOM1);
  SampleMain();
}

#define SLIDE_COUNT 30
void SampleMain(void) 
{ 
  PBitMap Disp;
  PBitMap Slides[SLIDE_COUNT];
  char FileName[MAX_PATH] = {0};
  char ExePath[MAX_PATH] = {0};
  int s, i, time;
 
  Disp = esCreateBitMap(SysGetScreenWidth(), SysGetScreenHeight(), SysGetPixelFormat()); 

  // load bmp
  GetModuleFileNameA(NULL, ExePath, MAX_PATH);
  if(strrchr(ExePath, '\\') != NULL)
    *strrchr(ExePath, '\\') = '/0';
  for(i = 0; i < SLIDE_COUNT; i++)
  {
    sprintf(FileName, "%s%s%i.bmp", ExePath, "/../Source/Pictures/", i + 1, ".bmp");
    Slides[i] = esCreateBlankBitMap(pf4bit);
    esLoadFileBmp(Slides[i], FileName, ifoSavePixelFormat);
  }
  
  time = 500;
  s = 0;
  esClear(Disp, 4);
  while(1)
  {
    if(rand() % 2 == 0)
      if(rand() % 3 == 0)
        esClear(Disp, rand() % 16);
      else
        esClear(Disp, 1);

    if(rand() % 10 < 8)
      esBitBltRop(Disp, 0, 0, Slides[s], ropXor);
    else
      if(rand() % 2 == 0)
        esBitBltRop(Disp, 0, 0, Slides[s], ropCopy);
      else
        esBitBltRop(Disp, 0, 0, Slides[s], ropOr);

    if(rand() % 4 == 0)
      esRotate90BitMap(Disp, rand() % 4);

    s++;
    if(s >= SLIDE_COUNT)
      s = 0;

    time = time > 200 ? time - 33 : 200;
    if(GetMouseButton())
      time = 80;
    
    SysDrawBuffer(Disp->Pixels);
    SysSleep(time);
  }

  esFreeBitMap(Disp);
  for(i = 0; i < SLIDE_COUNT; i++)
    esFreeBitMap(Slides[i]);  
} 
