.func MK_FP16
.synop begin
#include <windows.h>
DWORD MK_FP16( void far * fp32 );
.synop end
.desc begin
The &func function converts a 32-bit far pointer to a 16-bit far
pointer.
The 16-bit pointer is created by simply removing the high word of the
offset of the 32-bit pointer.
.np
The 32-bit far pointer must be one that was obtained by using
.kw MK_FP32
to extend a 16-bit pointer.
.np
This is useful whenever it is necessary to pass a 16-bit far pointer a
parameter to a Windows function though an _16 function.
.desc end
.return begin
The &func returns a 16-bit far pointer.
.return end
.see begin
.seelist MK_LOCAL32 MK_FP32
.see end
.exmp begin
#include <windows.h>

  DRAWITEMSTRUCT    FAR *lpdis;
  RECT  rc;
  DWORD alias;
  /*
   * The drawitem struct was passed as a long, so we
   * have to convert it to a 32 bit far pointer.
   * Then, we want the 16 bit far pointer of the rcItem
   * element so we can pass it to CopyRect (_16CopyRect
   * is a version of CopyRect that does not convert
   * the pointers it was given).
   */
  case WM_DRAWITEM:
    lpdis = MK_FP32( (void *) lParam );
    alias = AllocAlias16( &rc );
    _16CopyRect( (LPRECT) alias,
                 (LPRECT) MK_FP16( &lpdis->rcItem ) );
    FreeAlias16( alias );
.exmp end
.class WIN386
