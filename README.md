ESLowGraphicsLibrary - Low level software graphics library.
====================
ESLGL – this is low-end graphics library, optimized to 4-bit and 1-bit per pixel graphics.
Library written on “C”, but imitate PLO.

Library supports these operations:
• BitMap`s
o esCreateBitMap – Create new BitMap
o esCreateStaticBitMap – Create new BitMap using a static map pixels
o esCreateStaticMaskBitMap – Create new masked BitMap using a static map pixels
o esResizeBitMap – Resize BitMap
o esCloneBitMap – Clone this BitMap
o esFreeBitMap – Free BitMap
o esFreeStaticBitMap – Free static BitMap
• BitMap effects
o esCopyBitMap
o esInverseBitMap
o esFlipVBitMap
o esFlipHBitMap
o esRotate90BitMap
• Main graphics
o esClear
o esSetPixel
o esGetPixel
o esFillRect
o esDrawRect
o esDrawLine
o esDrawElipse
o esFillElipse
• The BitBlt functions 
o 1 - bit
▪ esBitBlt01_Copy
▪ …
▪ esBitBlt01_Mask
▪ esBitBlt01
▪ esBitBltRop01
o 4 – bit
▪ esBitBlt04_Copy
▪ …
▪ esBitBlt04_Color
▪ esBitBltRop04
▪ esBitBlt04
o Universal
▪ esBitBlt
▪ esBitBltRop
• Strech draw functions
o 1 – bit
▪ esStrechDraw01_Copy
▪ esStrechDraw01_Or
▪ esStrechDraw01_Xor
▪ esStrechDraw01_And
▪ esStrechDraw01_Mask
▪ -
▪ esStrechDraw01
▪ esStrechDrawRop01
o 4 – bit
▪ esStrechDraw04_Copy
▪ esStrechDraw04_Or
▪ esStrechDraw04_Xor
▪ esStrechDraw04_And
▪ esStrechDraw04_Mask
▪ esStrechDraw04_Transparent
▪ esStrechDraw04_Color
▪ -
▪ esStrechDraw04
▪ esStrechDrawRop04
o Universal
▪ esStrechDraw
▪ esStrechDrawRop
• other …….

Free using and editing in non commercial project, errorsoft@mail.ru or Enter256@yandex.ru for information of use in commerital project
