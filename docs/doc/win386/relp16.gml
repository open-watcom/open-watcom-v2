.func ReleaseProc16
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       subroutine ReleaseProc16( cbp )
       integer*4 cbp
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
void ReleaseProc16( CALLBACKPTR cbp );
.do end
.funcend
.desc begin
&func releases the callback function allocated by
.kw GetProc16
.ct .li .
Since the callback routines are a limited resource, it is important to
release the routines when they are no longer required.
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
.seelist &function. GetProc16
.see end
.if '&lang' eq 'FORTRAN 77' .do begin
.exmp begin
c$include winapi.fi

       integer*4 cbp
       integer*4 lpProcAbout
       ! get a 16-bit callback routine to point at
       ! our About dialogue procedure, then create
       ! the dialogue.
       cbp = GetProc16( About, GETPROC_CALLBACK )
       lpProcAbout = MakeProcInstance( cbp, hInst )
       call DialogBox( hInst, 'AboutBox'c,
     &                 hWnd, lpProcAbout )
       call FreeProcInstance( lpProcAbout )
       call ReleaseProc16( cbp )
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>

  CALLBACKPTR cbp;
  FARPROC lpProcAbout;
  /*
   * Get a 16-bit callback routine to point at
   * our About dialogue procedure, then create
   * the dialogue.  We use _16 versions of
   * MakeProcInstance, DialogBox, and
   * FreeProcInstance because they do not do
   * any magic work on the callback routines.
   */
   cbp = GetProc16( (PROCPTR) About,
                    GETPROC_CALLBACK );

   lpProcAbout = _16MakeProcInstance( cbp, hInst );

   _16DialogBox( hInst,
              "AboutBox",
              hWnd,
              lpProcAbout );

   _16FreeProcInstance( lpProcAbout );
   ReleaseProc16( cbp );
.exmp end
.do end
.class WIN386
