.func MK_LOCAL32
#include <windows.h>
void far *MK_LOCAL32( void * fp16 );
.funcend
.desc begin
The &func function converts a 16-bit near pointer to a 32-bit far
pointer.
This is needed whenever Windows returns a 16-bit near pointer that is
to be accessed by the 32-bit program.
.desc end
.return begin
The &func returns a 32-bit far pointer.
.return end
.see begin
.seelist &function. MK_FP32 MK_FP16
.see end
.exmp begin
#include <windows.h>

  WORD ich,cch;
  char *pch;
  char far *fpch;
  HANDLE hT;

  /*
   * Request the data from an edit window; copy it
   * into a local buffer so that it can be passed
   * to TextOut
   */
  ich = (WORD) SendMessage( hwndEdit,
                            EM_LINEINDEX,
                            iLine,
                            0L );
  cch = (WORD) SendMessage( hwndEdit,
                            EM_LINELENGTH,
                            ich,
                            0L );
  fpch = MK_LOCAL32( LocalLock( hT ) ) ;
  pch = alloca( cch );
  _fmemcpy( pch, fpch + ich, cch );

  TextOut( hdc, 0, yExtSoFar, (LPSTR) pch, cch );
  LocalUnlock( hT );
.exmp end
.class WIN386
