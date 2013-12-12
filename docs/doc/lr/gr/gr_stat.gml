.gfunc grstatus
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _grstatus( void );
.do end
.el .do begin
integer*2 function _grstatus()
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns the status of the most recently called
graphics library &routine..
The &routine can be called after any graphics &routine to determine
if any errors or warnings occurred.
The &routine returns 0 if the previous &routine was successful.
Values less than 0 indicate an error occurred; values greater than
0 indicate a warning condition.
.np
The following values can be returned:
.uindex GROK
.uindex GRERROR
.uindex GRMODENOTSUPPORTED
.uindex GRNOTINPROPERMODE
.uindex GRINVALIDPARAMETER
.uindex GRINSUFFICIENTMEMORY
.uindex GRNOOUTPUT
.uindex GRCLIPPED
.uindex GRFONTFILENOTFOUND
.uindex GRINVALIDFONTFILE
.millust begin
Constant              Value  Explanation

_GROK                   0    no error
_GRERROR               -1    graphics error
_GRMODENOTSUPPORTED    -2    video mode not supported
_GRNOTINPROPERMODE     -3    &routine n/a in this mode
_GRINVALIDPARAMETER    -4    invalid parameter(s)
_GRINSUFFICIENTMEMORY  -5    out of memory
_GRFONTFILENOTFOUND    -6    can't open font file
_GRINVALIDFONTFILE     -7    font file has invalid format
_GRNOOUTPUT             1    nothing was done
_GRCLIPPED              2    output clipped
.millust end
.desc end
.return begin
The
.id &func.
&routine returns the status of the most recently called
graphics library &routine..
.return end
.grexam begin eg_stat.&langsuff
.grexam end
.class &grfun
.system
