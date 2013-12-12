.gfunc setvideomode
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _setvideomode( short mode );
.do end
.el .do begin
integer*2 function _setvideomode( mode )
integer*2 mode
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets the video mode according to the value of the
.arg mode
argument.
The value of
.arg mode
can be one of the following:
.uindex MAXRESMODE
.uindex MAXCOLORMODE
.uindex DEFAULTMODE
.uindex TEXTBW40
.uindex TEXTC40
.uindex TEXTBW80
.uindex TEXTC80
.uindex MRES4COLOR
.uindex MRESNOCOLOR
.uindex HRESBW
.uindex TEXTMONO
.uindex HERCMONO
.uindex MRES16COLOR
.uindex HRES16COLOR
.uindex ERESNOCOLOR
.uindex ERESCOLOR
.uindex VRES2COLOR
.uindex VRES16COLOR
.uindex MRES256COLOR
.uindex URES256COLOR
.uindex VRES256COLOR
.uindex SVRES16COLOR
.uindex SVRES256COLOR
.uindex XRES16COLOR
.uindex XRES256COLOR
.code begin
Mode           Type    Size    Colors    Adapter

_MAXRESMODE   (graphics mode with highest resolution)
_MAXCOLORMODE (graphics mode with most colors)
_DEFAULTMODE  (restores screen to original mode)
_TEXTBW40      M,T    40 x 25    16   MDPA,HGC,VGA,SVGA
_TEXTC40       C,T    40 x 25    16   CGA,EGA,MCGA,VGA,SVGA
_TEXTBW80      M,T    80 x 25    16   MDPA,HGC,VGA,SVGA
_TEXTC80       C,T    80 x 25    16   CGA,EGA,MCGA,VGA,SVGA
_MRES4COLOR    C,G   320 x 200    4   CGA,EGA,MCGA,VGA,SVGA
_MRESNOCOLOR   C,G   320 x 200    4   CGA,EGA,MCGA,VGA,SVGA
_HRESBW        C,G   640 x 200    2   CGA,EGA,MCGA,VGA,SVGA
_TEXTMONO      M,T    80 x 25    16   MDPA,HGC,VGA,SVGA
_HERCMONO      M,G   720 x 350    2   HGC
_MRES16COLOR   C,G   320 x 200   16   EGA,VGA,SVGA
_HRES16COLOR   C,G   640 x 200   16   EGA,VGA,SVGA
_ERESNOCOLOR   M,G   640 x 350    4   EGA,VGA,SVGA
_ERESCOLOR     C,G   640 x 350  4/16  EGA,VGA,SVGA
_VRES2COLOR    C,G   640 x 480    2   MCGA,VGA,SVGA
_VRES16COLOR   C,G   640 x 480   16   VGA,SVGA
_MRES256COLOR  C,G   320 x 200  256   MCGA,VGA,SVGA
_URES256COLOR  C,G   640 x 400  256   SVGA
_VRES256COLOR  C,G   640 x 480  256   SVGA
_SVRES16COLOR  C,G   800 x 600   16   SVGA
_SVRES256COLOR C,G   800 x 600  256   SVGA
_XRES16COLOR   C,G  1024 x 768   16   SVGA
_XRES256COLOR  C,G  1024 x 768  256   SVGA
.code end
.keep begin
.pp
In the preceding table, the Type column contains the following letters:
.if &e'&dohelp eq 0 .do begin
.in +4
.do end
.begnote $setptnt 10
.note M
indicates monochrome; multiple colors are shades of grey
.note C
indicates color
.note G
indicates graphics mode; size is in pixels
.note T
indicates text mode; size is in columns and rows of characters
.endnote
.if &e'&dohelp eq 0 .do begin
.in -4
.do end
.keep end
.keep begin
.pp
The Adapter column contains the following codes:
.if &e'&dohelp eq 0 .do begin
.in +4
.do end
.begnote $setptnt 10
.term MDPA
IBM Monochrome Display/Printer Adapter
.term CGA
IBM Color Graphics Adapter
.term EGA
IBM Enhanced Graphics Adapter
.term VGA
IBM Video Graphics Array
.term MCGA
IBM Multi-Color Graphics Array
.term HGC
Hercules Graphics Adapter
.term SVGA
SuperVGA adapters
.endnote
.if &e'&dohelp eq 0 .do begin
.in -4
.do end
.keep end
.pp
The modes
.mono _MAXRESMODE
and
.mono _MAXCOLORMODE
will select from among the video modes supported by the current
graphics adapter the one that has the highest resolution or the greatest
number of colors.
The video mode will be selected from the standard modes, not including
the SuperVGA modes.
.pp
Selecting a new video mode resets the current output positions for
graphics and text to be the top left corner of the screen.
The background color is reset to black and the default color value
is set to be one less than the number of colors in the selected mode.
.desc end
.return begin
The
.id &func.
&routine returns the number of text rows when the new mode is
successfully selected; otherwise, zero is returned.
.return end
.see begin
.seelist &function. _getvideoconfig _settextrows _setvideomoderows
.see end
.grexam begin eg_getvc.&langsuff
.grexam end
.class &grfun
.system
