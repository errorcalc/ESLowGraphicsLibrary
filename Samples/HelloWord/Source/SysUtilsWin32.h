#ifndef SYSUTILS32_H
#define SYSUTILS32_H

#include "windows.h"

#define CS_ZOOM2 0x00000001
#define CS_ZOOM3 0x00000002
#define CS_ZOOM4 0x00000003

int SysCreateScreen(unsigned int Width, unsigned int Height, int PF, unsigned long Flags);
void SysFreeScreen(void);
void SysProcessMessages(void);

void SysDrawBuffer(unsigned char *p);
long SysGetTime(void);
void SysSleep(int MS);

void SysSetPixelFormat(int pf);
int SysGetPixelFormat(void);

int SysGetScreenWidth(void);
int SysGetScreenHeight(void);

#endif