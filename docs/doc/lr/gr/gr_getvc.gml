.gfunc getvideoconfig
.if '&lang' eq 'C' .do begin
struct videoconfig _FAR * _FAR _getvideoconfig
    ( struct videoconfig _FAR *config );
.do end
.el .do begin
subroutine _getvideoconfig( config )
record /videoconfig/ config
.do end
.gfuncend
.desc begin
The &func &routine returns information about the current video mode
and the hardware configuration.
The information is returned in the
.id videoconfig
structure indicated by the argument
.arg config
.ct .li .
The structure contains the following fields (all are
.id &short
fields):
.begterm 15
.note numxpixels
number of pixels in x-axis
.note numypixels
number of pixels in y-axis
.note numtextcols
number of text columns
.note numtextrows
number of text rows
.note numcolors
number of actual colors
.note bitsperpixel
number of bits in a pixel value
.note numvideopages
number of video pages
.note mode
current video mode
.note adapter
adapter type
.note monitor
monitor type
.note memory
number of kilobytes (1024 characters) of video memory
.endterm
.keep begin
.pp
The
.mono adapter
field will contain one of the following values:
.begterm 15
.uterm NODISPLAY
no display adapter attached
.uterm UNKNOWN
unknown adapter/monitor type
.uterm MDPA
Monochrome Display/Printer Adapter
.uterm CGA
Color Graphics Adapter
.uterm HERCULES
Hercules Monochrome Adapter
.uterm MCGA
Multi-Color Graphics Array
.uterm EGA
Enhanced Graphics Adapter
.uterm VGA
Video Graphics Array
.uterm SVGA
SuperVGA Adapter
.endterm
.keep end
.keep begin
.pp
The
.mono monitor
field will contain one of the following values:
.begterm 15
.uterm MONO
regular monochrome
.uterm COLOR
regular color
.uterm ENHANCED
enhanced color
.uterm ANALOGMONO
analog monochrome
.uterm ANALOGCOLOR
analog color
.endterm
.keep end
.pp
The amount of memory reported by &func will not always
be correct for SuperVGA adapters.
Since it is not always possible to determine the amount of memory,
&func will always report 256K, the minimum amount.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The &func &routine returns information about the current video mode
and the hardware configuration.
.return end
.do end
.see begin
.seelist &function. _setvideomode _setvideomoderows
.see end
.grexam begin eg_getvc.&langsuff
.grexam end
.class &grfun
.system
