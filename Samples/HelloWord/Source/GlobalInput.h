#ifndef _GLOBALINPUT_H_
#define _GLOBALINPUT_H_

#ifdef WIN32
#include "windows.h"
#endif

#define KEY_UP VK_UP 
#define KEY_DOWN VK_DOWN 
#define KEY_RIGHT VK_RIGHT 
#define KEY_LEFT VK_LEFT 

#define KEY_EXE VK_RETURN 
#define KEY_EXIT VK_BACK 

void SetMousePos(int x, int y);
void SetMouseButton(int Button);

int GetMouseX(void);
int GetMouseY(void);

int GetMouseButton(void);
#endif