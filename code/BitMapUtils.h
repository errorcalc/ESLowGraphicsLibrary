#ifndef _BITMAPUTILS_H_
#define _BITMAPUTILS_H_

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

int Convert_pf2bit_To_pf1bit(PBitMap Dest, PBitMap Source);
int Convert_pf4bit_To_pf1bit(PBitMap Dest, PBitMap Source);

void GetBitMapRect(PBitMap BitMap, PRect Rect);

#endif 