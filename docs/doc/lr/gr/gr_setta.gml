.gfunc settextalign
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _settextalign( short horiz, short vert );
.do end
.el .do begin
subroutine _settextalign( horiz, vert )
integer*2 horiz, vert
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets the current text alignment to the values
specified by the arguments
.arg horiz
and
.arg vert
.ct .li .
When text is displayed with the
.kw _grtext
&routine, it is aligned (justified) horizontally and vertically
about the given point according to the
current text alignment settings.
.pp
The horizontal component of the
alignment can have one of the following values:
.begterm 15
.uterm NORMAL
use the default horizontal alignment for the current setting of the
text path
.uterm LEFT
the text string is left justified at the given point
.uterm CENTER
the text string is centred horizontally about the given point
.uterm RIGHT
the text string is right justified at the given point
.endterm
.pp
The vertical component of the
alignment can have one of the following values:
.begterm 15
.uterm NORMAL
use the default vertical alignment for the current setting of the
text path
.uterm TOP
the top of the text string is aligned at the given point
.uterm CAP
the cap line of the text string is aligned at the given point
.uterm HALF
the text string is centred vertically about the given point
.uterm BASE
the base line of the text string is aligned at the given point
.uterm BOTTOM
the bottom of the text string is aligned at the given point
.endterm
.pp
The default is to use
.id _LEFT
alignment for the horizontal component unless the text path is
.id _PATH_LEFT,
in which case
.id _RIGHT
alignment is used.
The default value for the vertical component is
.id _TOP
unless the text path is
.id _PATH_UP,
in which case
.id _BOTTOM
alignment is used.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routine does not return a value.
.return end
.do end
.see begin
.seelist &function. _grtext _gettextsettings
.see end
.grexam begin eg_setta.&langsuff
.grexam output
.picture eg_setta
.grexam end
.class &grfun
.system
