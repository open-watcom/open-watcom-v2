.section *refid='ctrl' ctrl
.ix 'ctrl'
.ix 'Document header' 'ctrl'
.tag begin
.tdesc
Defines which push-buttons are displayed in the control area, and where. For on-line documents, the default
push-buttons are: Previous, Search, Print, Index, Contents, Back, Forward, Tutorial (if a tutorial is present).
These are displayed in the control area of the cover window. For help windows, the default buttons are: 
Previous, Search, Print, Index, Tutorial (if a tutorial is present).
.tattrbs
.tattr ctrlid=text
Specifies the id by which this control group can be referenced. Alphanumeric. Duplicate ids are not allowed.
.tattr controls='search print index contents esc back forward custom-id'
A space-separated list of the ids of the buttons you want to display, in the order to be displayed. If you 
define your own button (using the pbutton tag), use the id from that tag as the custom-id.
.tattr page
Put the buttons in the control area of a page.
.tattr coverpage
Put the buttons in the control area of the cover (main) page.
.tno Conditions
The ctrl tag must be enclosed in a ctrldef block, and must follow all pbutton tags
.tclass Document Header
.tcont None
.tseealso
:HDREF refid='ctrldef'., :HDREF refid='pbutton'.
.tag end

