.gfunc registerfonts
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _registerfonts( char _FAR *path );
.do end
.el .do begin
integer*2 function _registerfonts( path )
character*(*) path
.do end
.gsynop end
.desc begin
The
.id &func.
&routine initializes the font graphics system.
Fonts must be registered, and a font selected,
before text can be displayed with the
.kw _outgtext
&routine..
.pp
The argument
.arg path
specifies the location of the font files.
This argument is a file specification, and can contain drive and
directory components and may contain wildcard characters.
The
.id &func.
&routine opens each of the font files specified and
reads the font information.
Memory is allocated to store the characteristics of the font.
These font characteristics are used by the
.kw _setfont
&routine when selecting a font.
.desc end
.return begin
The
.id &func.
&routine returns the number of fonts that were registered if
the &routine is successful; otherwise, a negative number is returned.
.return end
.see begin
.seelist _unregisterfonts _setfont _getfontinfo _outgtext _getgtextextent _setgtextvector _getgtextvector
.see end
.grexam begin eg_reg.&langsuff
.grexam end
.class &grfun
.system
