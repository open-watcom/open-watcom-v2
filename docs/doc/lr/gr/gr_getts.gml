.gfunc gettextsettings
.gsynop begin 
.if '&lang' eq 'C' .do begin
struct textsettings _FAR * _FAR _gettextsettings
    ( struct textsettings _FAR *settings );
.do end
.el .do begin
subroutine _gettextsettings( settings )
record /textsettings/ settings
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns information about the current text settings
used when text is displayed by the
.kw _grtext
&routine..
The information is stored in the
.id textsettings
structure indicated by the argument
.arg settings
.ct .li .
The structure contains the following fields (all are
.id &short
fields):
.begterm 15
.note basevectorx
x-component of the current base vector
.note basevectory
y-component of the current base vector
.note path
current text path
.note height
current text height (in pixels)
.note width
current text width (in pixels)
.note spacing
current text spacing (in pixels)
.note horizalign
horizontal component of the current text alignment
.note vertalign
vertical component of the current text alignment
.endterm
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routine returns information about the current graphics text settings.
.return end
.do end
.see begin
.seelist &function. _grtext _setcharsize _setcharspacing _settextalign _settextpath _settextorient
.see end
.grexam begin eg_getts.&langsuff
.grexam end
.class &grfun
.system
