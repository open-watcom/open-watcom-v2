.section *refid='link' link
.ix 'link'
.ix 'Links' 'text'
.ix 'In-line' 'link'
.tag begin
.tdesc
A link to additional information. Activating the link may jump to a different location in the same or a different 
document, open a footnote, launch an application, or send a notification to another running process.
.tclose elink
Unless the link is contained in an artlink tag, or if reftype is 'inform'.
.tattrbs
.tattr reftype=hd | fn | launch | inform
If reftype is 'hd', then it links to a heading and the refid attribute must be specified. If the heading is
in an external file, the database must also be set and the heading must have its 'global' attribute set.
If reftype is 'fn', the link is to a footnote and the 'refid' attribute must be set. Note that a split window 
cannot contain a link to a footnote.
If reftype is 'launch', the link starts the external program specified by the 'object' attribute with parameters 
specified in the 'data' attribute.
If reftype is 'inform', the value of the 'res' attribute is sent to the application.
.tattr res=number
The resource id of the header tag.
.tattr refid=text
The id of the header. Alphanumeric.
.tattr database='text'
The name of the external INF or HLP file.
.tattr object='text'
The name of a program to execute.
.tattr data='text'
The parameters to pass to program being started.
.tattr auto
Automatically trigger this link when the page that contains it is displayed. Footnotes cannot be opened automatically.
The link tag must follow a heading tag before other tags and text.
.tattr viewport
Open a secondary window when the link that refers to it is opened.
.tattr dependent
Make the automatically opened window close when the secondary window that opened it is closed.
.tattr split
Open a secondary window when the link is activated. Note that the primary window can only contain links to 
secondary windows. Each of the secondary windows must have a different group number if they are to be displayed 
at the same time.
.tattr child
Open the page being linked to as a child of the current page, clipped to fit within the current page. The child 
is always on top, is closed when the parent is closed, resized when the parent's size is changed. 
If only the child is minimized, its icon appears within the parent's border.
.tattr group=number
The group number of the window (1 to 64000). All pages in the same group are displayed in the same window, 
one replacing the other.
.tattr vpx=measurement
The x origin of the window. See :HDREF refid='units'.. Not valid for footnotes.
.tattr vpy=measurement
The y origin of the window. See :HDREF refid='units'.. Not valid for footnotes.
.tattr vpcx=measurement
The width of the window. Absolute or dynamic units only. See :HDREF refid='units'.. Not valid for footnotes.
.tattr vpcy=measurement
The height of the window. Absolute or dynamic units only. See :HDREF refid='units'.. Not valid for footnotes.
.tattr titlebar=yes|sysmenu|minmax|both|none
Set what contents appear in the title bar. The default is 'both'. Useful for secondary windows.
.tattr scroll=horizontal|vertical|both|none
Set which scroll bars appear. The default is 'both'. Useful for secondary windows.
.tattr rules=border|sizeborder|none
Set the type of border on the window. The default is 'sizeborder'. Useful for secondary windows.
.tattr x=number
The x origin of the active area of the graphic. Valid only if the link is in an artlink.
.tattr y=number
The y origin of the active area of the graphic. Valid only if the link is in an artlink.
.tattr cx=number
The width of the active area of the graphic. Valid only if the link is in an artlink.
.tattr cy=number
The height of the active area of the graphic. Valid only if the link is in an artlink.
.tno Note:
If the link is to contain an image, the link must be enclosed in an artlink tag block.
.tclass In-line
.tcont Text, Formatting, In-line
.tseealso
:HDREF refid='artlink'., :HDREF refid='hdref'.
.tag end

