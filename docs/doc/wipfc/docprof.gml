.section *refid='docprof' docprof
.ix 'docprof'
.ix 'Document header' 'docprof'
.tag begin
.tdesc
Describes the document profile, which includes which (if any) dynamic link librarys to load, what level 
headers trigger new pages, where controls are displayed. The tag must follow the userdoc tag and (if 
present) the title tag.
.tattrbs
.tattr toc=numbers
Controls which heading levels are included in the table of contents. For example, '123' (the default) places 
headings defined by the h1, h2, and h3 tags in the table of contents. The numbers must be consecutive, beginning 
with 1.
.tattr dll='text'
Specifies a dll to load. This dll can act as a communication object and alter the behavior of the viewer.
.tattr objectname='text'
The dll entry point.
.tattr objectinfo='text'
Parameters to pass to the dll.
.tattr ctrlarea=none | coverpage | page | both
Specifies where the control area that contains push buttons is located. If 'page' is specified, place the control 
area on the text window. If 'coverpage' is specified, place the control area on the cover page. If 'both' is specified, 
place the control are in both locations. If 'none' is specified, suppress the control area.
.tclass Document Header
.tcont None
.tag end

