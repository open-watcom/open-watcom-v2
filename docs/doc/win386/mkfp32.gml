.func MK_FP32
#include <windows.h>
void far *MK_FP32( void * fp16 );
.funcend
.desc begin
The &func function converts a 16-bit far pointer to a 32-bit far
pointer.
This is needed whenever Windows returns a 16-bit far pointer, and
access to the data is needed by the 32-bit program.
.desc end
.return begin
The &func returns a 32-bit far pointer.
.return end
.see begin
.seelist &function. MK_LOCAL32 MK_FP16
.see end
.exmp begin
#include <windows.h>

  MEASUREITEMSTRUCT far *mis;

  case WM_MEASUREITEM:
    /*
     * Windows has passed us a 16 bit far pointer
     * to the measure item data structure.  We
     * use MK_FP32 to make that pointer a 32-bit far
     * pointer, which enables us to access the data.
     */
    mis = MK_FP32( (void *) lParam );
    mis->itemHeight = MEASUREITEMHEIGHT;
    mis->itemWidth  = MEASUREITEMWIDTH;
    return TRUE;
.exmp end
.class WIN386
