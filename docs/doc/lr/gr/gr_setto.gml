.gfunc settextorient
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _settextorient( short vecx, short vecy );
.do end
.el .do begin
subroutine _settextorient( vecx, vecy )
integer*2 vecx, vecy
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets the current text orientation to the
vector specified by the arguments
.coord vecx vecy .
The text orientation specifies the direction of the base-line vector
when a text string is
displayed with the
.kw _grtext
&routine..
The default text orientation, for normal left-to-right text,
is the vector
.coord 1 0 .
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
.grexam begin eg_setto.&langsuff
.grexam output
.picture eg_setto
.grexam end
.class &grfun
.system
