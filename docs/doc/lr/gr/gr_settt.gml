.gfunc settextpath
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _settextpath( short path );
.do end
.el .do begin
subroutine _settextpath( path )
integer*2 path
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine sets the current text path to have the value of the
.arg path
argument.
The text path specifies the writing direction of the text displayed
by the
.kw _grtext
&routine..
The argument can have one of the following values:
.begterm 15
.uterm PATH_RIGHT
subsequent characters are drawn to the right of the previous character
.uterm PATH_LEFT
subsequent characters are drawn to the left of the previous character
.uterm PATH_UP
subsequent characters are drawn above the previous character
.uterm PATH_DOWN
subsequent characters are drawn below the previous character
.endterm
.pp
The default value of the text path is
.id _PATH_RIGHT.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &funcb.
&routine does not return a value.
.return end
.do end
.see begin
.seelist _grtext _gettextsettings
.see end
.grexam begin eg_settx.&langsuff
.grexam output
.picture eg_settx
.grexam end
.class &grfun
.system
