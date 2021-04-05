.gfunc getvideoconfig
.gsynop begin
.if '&lang' eq 'C' .do begin
struct videoconfig _FAR * _FAR _getvideoconfig
    ( struct videoconfig _FAR *config );
.do end
.el .do begin
subroutine _getvideoconfig( config )
record /videoconfig/ config
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine returns information about the current video mode
and the hardware configuration.
The information is returned in the
.id videoconfig
structure indicated by the argument
.arg config
.period
The structure contains the following fields (all are
.id &short
fields):
.begterm 15
.termnx numxpixels
number of pixels in x-axis
.termnx numypixels
number of pixels in y-axis
.termnx numtextcols
number of text columns
.termnx numtextrows
number of text rows
.termnx numcolors
number of actual colors
.termnx bitsperpixel
number of bits in a pixel value
.termnx numvideopages
number of video pages
.termnx mode
current video mode
.termnx adapter
adapter type
.termnx monitor
monitor type
.termnx memory
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
The amount of memory reported by
.id &funcb.
will not always
be correct for SuperVGA adapters.
Since it is not always possible to determine the amount of memory,
.id &funcb.
will always report 256K, the minimum amount.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &funcb.
&routine returns information about the current video mode
and the hardware configuration.
.return end
.do end
.see begin
.seelist _setvideomode _setvideomoderows
.see end
.grexam begin eg_getvc.&langsuff
.grexam end
.class &grfun
.system
