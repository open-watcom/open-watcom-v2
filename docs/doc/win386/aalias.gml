.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="dword"      value="INTEGER*4".
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="dword"      value="DWORD".
.do end
.func AllocAlias16
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       integer*4 function AllocAlias16( ptr )
       integer*4 ptr
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
DWORD AllocAlias16( void *ptr );
.do end
.synop end
.desc begin
The &func function obtains a 16-bit far pointer equivalent of a 32-bit
near pointer.
These pointers are used when passing data pointers to Windows through
functions that have &dword arguments, and for any pointers within data
structures passed this way.
.desc end
.return begin
The &func function returns a 16-bit far pointer
.if '&lang' eq 'FORTRAN 77' .do begin
(as an INTEGER*4)
.do end
usable by Windows,
or returns 0 if the alias cannot be allocated.
.return end
.see begin
.seelist &function. FreeAlias16
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
