#include "GlobalInput.h"

static int x = 0, y = 0, Button = 0;

void SetMousePos(int _x, int _y)
{
  x = _x;
  y = _y;
}

void SetMouseButton(int _Button)
{
  Button = _Button;
}

int GetMouseX(void)
{
  return x;
}

int GetMouseY(void)
{
  return y;
}

int GetMouseButton(void)
{
  return Button;
}
