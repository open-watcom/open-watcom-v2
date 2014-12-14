.gfuncw grtext
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _grtext( short x, short y,
                    char _FAR *text );

short _FAR _grtext_w( double x, double y,
                      char _FAR *text );
.do end
.el .do begin
integer*2 function _grtext( x, y, text )
integer*2 x, y
character*(*) text

integer*2 function _grtext_w( x, y, text )
double precision x, y
character*(*) text
.do end
.gsynop end
.desc begin
The
.id &func.
&routines display a character string.
The
.id &func.
&routine uses the view coordinate system.
The
.id &func2.
&routine uses the window coordinate system.
.np
The character string
.arg text
is displayed at the point
.coord x y .
The string must be terminated by a null character
.if '&lang' eq 'C' .do begin
('\0').
.do end
.el .do begin
(char(0)).
.do end
The text is displayed in the current color using the current text
settings.
.im gr_text
.desc end
.return begin
.im gr_retdr text
.return end
.see begin
.seelist _outtext _outmem _outgtext _setcharsize _settextalign _settextpath _settextorient _setcharspacing
.see end
.grexam begin eg_grtxt.&langsuff
.grexam output
.picture eg_grtxt
.grexam end
.class &grfun
.system
