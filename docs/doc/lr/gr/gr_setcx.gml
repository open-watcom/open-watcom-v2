.gfuncw setcharspacing
.if '&lang' eq 'C' .do begin
void _FAR _setcharspacing( short space );

void _FAR _setcharspacing_w( double space );
.do end
.el .do begin
subroutine _setcharspacing( space )
integer*2 space

subroutine _setcharspacing_w( space )
double precision space
.do end
.gfuncend
.desc begin
The &func &routines set the current character spacing to have the value
of the argument
.arg space
.ct .li .
For the &func &routine,
.arg space
represents a number of pixels.
For the &func2 &routine,
.arg space
represents a length along the x-axis in the window coordinate system.
.np
The character spacing specifies the additional space to
leave between characters when a text string is displayed with the
.kw _grtext
&routine..
A negative value can be specified to cause the characters to be
drawn closer together.
The default value of the character spacing is 0.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The &func &routines do not return a value.
.return end
.do end
.see begin
.seelist &function. _grtext _gettextsettings
.see end
.grexam begin eg_setcs.&langsuff
.grexam output
.picture eg_setcs
.grexam end
.class &grfun
.system
