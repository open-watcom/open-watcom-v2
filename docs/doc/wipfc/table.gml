.section *refid='table' table
.ix 'table'
.ix 'Block' 'table'
.tag begin
.tdesc
Begins a table. Tables use a mono-spaced font to ensure that the column width are respected. If you change 
the font (using the font tag) or change make the font bold, the spacing will be thrown off.
.tattrbs
.tattr cols='number number...'
Specifies both the width of each column ('number') and the number of columns (the number of 'number's).
.tattr rules=both | horiz | vert | none
Specify whether the table will have horizontal or vertical rules. If the rule is not drawn, blank space 
appears in it place. The default is 'both'.
.tattr frame=rule|box|none
Specifies the frame around the table. If no frame is drawn, blank space appears in its place. The default 
is 'box'.
.tclass Block
.tcont :HDREF refid='row'.
.tag end
.*
.beglevel
.*
.section *refid='row' row
.ix 'row'
.ix 'Block' 'row'
.ix 'Table' 'row'
.tag begin
.tdesc
A table row. If the number of cells supplied for a row is greater or less than the number specified in 
the table tag, a warning is generated.
.tclass Block
.tcont :HDREF refid='c'.
.tag end
.*
.section *refid='c' c
.ix 'c'
.ix 'Block' 'c'
.ix 'Table' 'column'
.tag begin
.tdesc
A table cell. If the width of the text exceeds the width of the cell, the text is wrapped. If the length of 
a single word exceeds the width of the cell, that word will be truncated to the width of the cell.
.tclass Block
.tcont Text, :HDREF refid='color'., :HDREF refid='font'., :HDREF refid='hpn'., :HDREF refid='lines'., :HDREF refid='link'.
.tno Note:
Using the lines tag does not change the font to a proportional spaced font. It simply makes the whitespace and 
new lines significant so that the cell is formatted as entered. All attributes for the lines tag are ignored.
.tag end
.*
.endlevel
.*

