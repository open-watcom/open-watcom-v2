.section *refid='hpn' hp1 - hp9
.ix 'hp1'
.ix 'hp2'
.ix 'hp3'
.ix 'hp4'
.ix 'hp5'
.ix 'hp6'
.ix 'hp7'
.ix 'hp8'
.ix 'hp9'
.ix 'Formatting' 'hp1'
.ix 'Formatting' 'hp2'
.ix 'Formatting' 'hp3'
.ix 'Formatting' 'hp4'
.ix 'Formatting' 'hp5'
.ix 'Formatting' 'hp6'
.ix 'Formatting' 'hp7'
.ix 'Formatting' 'hp8'
.ix 'Formatting' 'hp9'
.ix 'Highlighted phrase'
.tag begin
.tdesc
Change the style of the text. The style can be a combination of bold, italic, or underlined, or a color change.
hp1 is italic. hp2 is bold. hp3 is bold italic. hp4 is blue. hp5 is underlined. hp6 is underlined italic. hp7 
is underlined bold. hp8 is red. hp9 is pink.
.tclose matching tag
.tno Conditions
Highlighting tags cannot nest. Note that the &ipfname allows this as an extension, but still issues a warning. 
In this case the opening and closing tags must match at each nesting level. Also note that nested highlighting 
tags are not additive: an hp1 (italic) inside hp2 (bold) does not make an hp3 (bold italic).
.tclass Formatting
.tcont Text, Formatting, In-line, Block, List-block
.tag end

