.section *refid='font' font
.ix 'font'
.ix 'Formatting' 'font'
.tag begin
.tdesc
Change the font face, size, or encoding for the text in the page (fonts reset at the next header defining a 
new page).
.tattrbs
.tattr facename='text'
Sets the name of the font to use. If set to 'default', resets the font to the default face and size. This 
attribute is required.
.tattr size=HxW
Sets the height and width of the font. If either value is 0, resets the font to the default face and size. 
This attribute is required. The 'x' separating the numbers is required.
.tattr codepage=number
Sets the encoding of the displayed text. The value is a 3 digit number.
.tclass Formatting
.tcont Text, Formatting, In-line, Block, List-block
.tag end

