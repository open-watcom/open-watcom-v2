.section *refid='hn' h1 - h6
.ix 'h1'
.ix 'h2'
.ix 'h3'
.ix 'h4'
.ix 'h5'
.ix 'h6'
.ix 'Block' 'h1'
.ix 'Block' 'h2'
.ix 'Block' 'h3'
.ix 'Block' 'h4'
.ix 'Block' 'h5'
.ix 'Block' 'h6'
.ix 'Headings'
.tag begin
.tdesc
A heading tag defines a new page (window), provided the heading level is less than the maximum value set
using the docprof tag. The text of the header immediately follows the close of the tag, either on the same 
or the next line. The attributes of the heading tag can be used to set the properties of the window 
that displays the page of information. Heading tags must appear in consecutive ascending (but not descending) 
order. That is, you cannot go from h1 to h3 without an intervening h2, but you can go from h3 to h1.
.tattrbs
.tattr res=number
Specify the resource id of the header. It must be in the range of 1 to 64000. This attribute is required for 
HLP files.
.tattr id=text
The id of the header. Alphanumeric.
.tattr name=text
The name of the header. Alphanumeric.
.tattr global
If set, this page can be linked to by an external HLP or INF file.
.tattr tutorial=text
Specifies the file name of the tutorial, and adds the tutorial button to the control area.
.tattr x=measurement
The x origin of the window in the parent window. See :HDREF refid='units'..
.tattr y=measurement
The y origin of the window in the parent window. See :HDREF refid='units'..
.tattr width=measurement
The width of the window in the parent window. See :HDREF refid='units'..
.tattr height=measurement
The height of the window in the parent window. See :HDREF refid='units'..
.tno Note:
You cannot mix absolute units with dynamic or relative units when specifying x and width, or y and height.
.tattr group=number
The group number of the window (1 to 64000). All pages in the same group are displayed in the same window, 
one replacing the other.
.tattr viewport
Force the opening of a new window for this page.
.tattr clear
Close any open windows before opening a new one to display this page.
.tattr titlebar=yes | sysmenu | minmax | both | none
Set what contents appear in the title bar. The default is 'both'. Useful for secondary windows.
.tattr scroll=horizontal | vertical | both | none (default: both)
Set which scroll bars appear. The default is 'both'. Useful for secondary windows.
.tattr rules=border | sizeborder | none
Set the type of border on the window. The default is 'sizeborder'. Useful for secondary windows.
.tattr nosearch
When searching, do not jump to this secondary window. Instead, jump to the parent window that contains it.
.tattr noprint
Do not print the contents of a secondary window separately. Print it as part of the parent window contents.
.tattr hide
Do not include this header in the table of contents.
.tattr toc=numbers
Controls which heading levels are included in the table of contents. For example, '123' (the default) places 
headings defined by the h1, h2, and h3 tags in the table of contents. The numbers must be consecutive, beginning 
with 1. Valid until the end of the file or until the next header with a toc attribute.
.tattr ctrlarea=page|none
Specifies where the control area that contains push buttons is located. If 'page' is specified, place the control 
area on the text window. If 'none' is specified, suppress the control area on this page. This overrides the value 
set in the docprof tag.
.tattr ctrlrefid=text
An alphanumeric identifier of the set of controls you wish to display.
.tclass Block
.tcont Text, Formatting, In-line, Block, List-block
.tseealso
:HDREF refid='fn'.
.tag end

