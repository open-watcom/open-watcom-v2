.section *refid='pbutton' pbutton
.ix 'pbutton'
.ix 'Document header' 'pbutton'
.tag begin
.tdesc
Defines a custom (author-defined) push button.
.tattrbs
.tattr id=text
An alphanumeric identifier that will be referenced by the ctrl tag when using the button.
.tattr res=number
The resource identifier for the button (returned by window messages). Must be greater 256; values of 256 or
less are reserved.
.tattr text='text'
The button text. May include spaces. May not include entity references. Must not conflict with any pre-defined
buttons.
.tno Note:
All pbutton tags must appear inside a ctrldef block and before any ctrl tags.
.tclass Document Header
.tcont None
.tseealso
:HDREF refid='ctrl'., :HDREF refid='ctrldef'.
.tag end

