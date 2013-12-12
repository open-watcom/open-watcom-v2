.func FreeAlias16
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       subroutine FreeAlias16( fp16 )
       integer*4 fp16
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
void FreeAlias16( DWORD fp16 );
.do end
.synop end
.desc begin
&func frees a 16-bit far pointer alias for a 32-bit near pointer that
was allocated with
.kw AllocAlias16
.ct .li .
This is important to do when there is no further use for the pointer
since there are a limited number of 16-bit aliases available (due to
limited space in the local descriptor table).
.desc end
.return begin
.if '&lang' eq 'FORTRAN 77' .do begin
&func is a subroutine.
.do end
.el .do begin
The &func function returns nothing.
.do end
.return end
.see begin
.seelist &function. AllocAlias16
.see end
.if '&lang' eq 'FORTRAN 77' .do begin
.exmp begin
integer*4 mcs_16
record /MDICREATESTRUCT/ mcs
mcs.szTitle = AllocAlias16( loc( 'Title'c ) )
mcs.szClass = AllocAlias16( loc( 'mdichild'c ) )
mcs.hOwner = hInst
mcs.x = mcs.cx = CW_USEDEFAULT
mcs.y = mcs.cy = CW_USEDEFAULT
mcs.style = 0
! Send a message to an MDI client to create a window.
! Since the pointer to the structure is passed in an
! argument that may not be a pointer (depending on the
! type of message), there is no implicit 32 to 16-bit
! conversion done so the conversion must be done by the
! programmer.
mcs_16 = AllocAlias16( loc( mcs ) )
hwnd = SendMessage( hwndMDI, WM_MDICREATE, 0, mcs_16 )
FreeAlias16( mcs_16 )
FreeAlias16( mcs.szClass )
FreeAlias16( mcs.szTitle )
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>

  DWORD mcs_16;
  /*
   * Send a message to a MDI client to create a window.
   * _16SendMessage is used for this example, since it will
   * not do any pointer conversions automatically.
   */
  MDICREATESTRUCT mcs;
  mcs.szTitle = (LPSTR) AllocAlias16( "c:\\foo.bar" );
  mcs.szClass = (LPSTR) AllocAlias16( "mdichild" );
  mcs.hOwner  = hInst;
  mcs.x = mcs.cx = (int) CW_USEDEFAULT;
  mcs.y = mcs.cy = (int) CW_USEDEFAULT;
  mcs.style = 0;

  /* tell the MDI Client to create the child */
  mcs_16 = AllocAlias16( &mcs );
  hwnd = (WORD) _16SendMessage( hwndMDIClient,
                             WM_MDICREATE,
                             0,
                             (LONG) mcs_16 );
  FreeAlias16( mcs_16 );
  FreeAlias16( (DWORD) mcs.szClass );
  FreeAlias16( (DWORD) mcs.szTitle );
.exmp end
.do end
.class WIN386
