.gfunc _gettextsettings
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
.id &funcb.
&routine returns information about the current text settings
used when text is displayed by the
.reffunc _grtext
&routine..
The information is stored in the
.id textsettings
structure indicated by the argument
.arg settings
.period
The structure contains the following fields (all are
.id &short
fields):
.begterm 15
.termnx basevectorx
x-component of the current base vector
.termnx basevectory
y-component of the current base vector
.termnx path
current text path
.termnx height
current text height (in pixels)
.termnx width
current text width (in pixels)
.termnx spacing
current text spacing (in pixels)
.termnx horizalign
horizontal component of the current text alignment
.termnx vertalign
vertical component of the current text alignment
.endterm
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &funcb.
&routine returns information about the current graphics text settings.
.return end
.do end
.see begin
.seelist _grtext _setcharsize _setcharspacing _settextalign _settextpath _settextorient
.see end
.grexam begin eg_getts.&langsuff
.grexam end
.class &grfun
.system
