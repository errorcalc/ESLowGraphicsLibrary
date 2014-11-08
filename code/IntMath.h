#ifndef _INTMATH_H_
#define _INTMATH_H_

#define fixed int

#define FloatToFixed(v)( ((fixed)((v) * 65536.0f)) )
#define FixedToFloat(v)(((float)((v) / (float)65536.0f)) )

#define FixedToInt(v)( (v)>>16 )
#define IntToFixed(v)( (v)<<16 )

#endif