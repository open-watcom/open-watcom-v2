.section *refid='fn' fn
.ix 'fn'
.ix 'Block' 'fn'
.ix 'Footnote'
.tag begin
.tdesc
Defines a pop-up window that acts as a footnote. The window is activated when the user clicks a link.
.tclose efn
.tattrbs
.tattr id=text
Specifies a unique alphanumeric identifier used as the target of a link tag. This attribute is required.
.tno Conditions:
The index tags (i1, i2, icmd, isyn) are not allowed. Footnotes cannot be nested within other footnotes, nor 
can they contain headings (h1 - h6). The link to a footnote cannot appear in a child window. The text of 
footnotes is not searchable.
.tclass Block
.tcont Text, Formatting, Block (except fn, h1 - h6), List-block
.tseealso
:HDREF refid='link'.
.tag end

