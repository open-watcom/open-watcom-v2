.func movedata
#include <string.h>
void movedata( unsigned int src_segment,
               unsigned int src_offset,
               unsigned int tgt_segment,
               unsigned int tgt_offset,
               size_t length );
.ixfunc2 '&Copy' &func
.synop end
.desc begin
The &func function copies
.arg length
bytes from the far pointer calculated as
.mono (src_segment:src_offset)
to a target location determined as a far pointer
.mono (tgt_segment:tgt_offset).
.pp
Overlapping data may not be correctly copied.
When the source and target areas may overlap, copy the areas one
character at a time.
.pp
The function is useful to move data when the near address(es) of the
source and/or target areas are not known.
.desc end
.return begin
No value is returned.
.return end
.see begin
.seelist movedata FP_SEG FP_OFF memcpy segread
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <&doshdr>

void main()
  {
    char buffer[14] = {
        '*', 0x17, 'H', 0x17, 'e', 0x17, 'l', 0x17,
        'l', 0x17, 'o', 0x17, '*', 0x17 };
.exmp break
    movedata( FP_SEG( buffer ),
              FP_OFF( buffer ),
              0xB800,
              0x0720,
              14 );
  }
.exmp end
.class WATCOM
.system
