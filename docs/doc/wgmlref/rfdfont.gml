:H2.Defining a Font
:I1.defining fonts
:I2 refid='fonts'.defining
:P.
Information about the character sets available for a particular
device is needed by &WGML. to properly process a document.
The :HP2.FONT:eHP2. block
:I2 refid='fonts'.font block
is processed by the GENDEV program
to create a font definition.
The resulting definition is referenced on the &WGML. command
line and by the device definitions.
:FIG frame = box place = inline.
&gml.FONT
   <attributes>
   <width block>
   <intrans block>
   <outtrans block>
&gml.eFONT.
:FIGCAP.The FONT Block
:EFIG.
:P.
A font block begins with the :HP2.&gml.font:eHP2.
:IH1 id='dfont' print='&gml.font tag'.font tag
:I1.font tag
tag and ends with
the :HP2.&gml.efont:eHP2.
:IH1 print='&gml.efont tag'.efont
:I1.efont
tag.
The attributes of the font block must all be specified.
:P.
The width, intrans and outtrans character definition
:I2 refid='fonts'.character definition
blocks are used to specify
information which defines the characters of the font.
Each possible character in a font is represented by a number
between 0 and 255 inclusive (the representation depends
on the machine system being used).
Information about each of these characters may
be specified in the character definition blocks, with one character
definition per input line.
If a character definition line is not specified for a particular
character value, default values are supplied by the WATCOM GENDEV
program.
:P.
Each of the character definition blocks are optional,
and may be specified more
that once and in any order.
If specified, they must follow the font attributes.
:H3.Attributes of the Font Block
:FIG frame = box place = inline.
defined_name     = 'character string'
member_name      = 'character string'
font_out_name1   = 'character string'
font_out_name2   = 'character string'
line_height      = number
line_space       = number
scale_basis      = number
scale_min        = number
scale_max        = number
char_width       = number
mono_space_width = YES | NO
:FIGCAP.Attributes of the FONT Block
:EFIG.
:FIG frame = box place = inline.
defined_name     = 'times-roman'
member_name      = 'PSTR'
font_out_name1   = 'Times-Roman'
line_height      = 1000
line_space       = 113
scale_basis      = 72000
scale_min        = 1000
scale_max        = 72000
char_width       = 250
mono_space_width = no
:FIGCAP.Example of the FONT Block Attributes
:EFIG.
:SET symbol='local' value='font'.
:INCLUDE file='rfddef'.
:IH1 id='dafont'.font attributes
:I2.defined_name
:I1.defined_name attribute
The defined name is used to identify a font on the
&WGML. command line and is referred to in
a device definition.
:INCLUDE file='rfdmem'.
:I2 refid='dafont'.member_name
:I1.member_name attribute
:H4.FONT_OUT_NAME Attributes
:P.
The optional attributes :HP2.font_out_name1:eHP2.
:I1.font_out_name1 attribute
:I2 refid='dafont'.font_out_name1
and
:HP2.font_out_name2:eHP2.
:I1.font_out_name2 attribute
:I2 refid='dafont'.font_out_name2
specify additional naming information for the font.
The attribute value must be a valid character string.
:P.
Some devices require the specification of the font names
or font characteristics during
the initialization sequence or when a switch to a different
font is made within the document.
The font_out_name attributes may be used to supply these names
to &WGML..
If these values are not needed with a device, the null string('')
may be specified.
:H4.LINE_HEIGHT Attribute
:P.
The :HP2.line_height:eHP2.
:I1.line_height attribute
:I2 refid='dafont'.line_height
attribute specifies the height of
the characters that are in the font being defined.
The attribute value is a positive integer number which represents
the height of the characters in terms of the vertical base units
:I1.vertical base units
specified in the device definition if the font is not scaled.
If the font is scaled, the value is in terms of the scale basis
specified by the :HP1.scale_basis:eHP1. attribute.
This value added to the line space value is the total amount
of space from one line to the next.
:H4.LINE_SPACE Attribute
:P.
The :HP2.line_space:eHP2.
:I1.line_space attribute
:I2 refid='dafont'.line_space
attribute specifies the amount of space between two
lines of characters that are in the font being defined.
The attribute value is a positive integer number which represents
the line space in terms of the vertical base units
:I1.vertical base units
specified in the device definition if the font is not scaled.
If the font is scaled, the value is in terms of the scale basis
specified by the :HP1.scale_basis:eHP1. attribute.
This value added to the line height value is the total amount
of space from one line to the next.
:H4.SCALE_BASIS Attribute
:P.
The :HP2.scale_basis:eHP2.
:I1.scale_basis attribute
:I2 refid='dafont'.scale_basis
attribute specifies the number of base units per inch
for scale operations.
This attribute is not specified when the font characters have a fixed
size.
:H4.SCALE_MIN Attribute
:P.
The :HP2.scale_min:eHP2.
:I1.scale_min attribute
:I2 refid='dafont'.scale_min
attribute specifies the minimum size a font may be
scaled to.
The attribute value is a number in terms of the
:HP1.scale_basis:eHP1. attribute.
This attribute must be specified if the font characters are scaled.
:H4.SCALE_MAX Attribute
:P.
The :HP2.scale_max:eHP2.
:I1.scale_max attribute
:I2 refid='dafont'.scale_max
attribute specifies the maximim size a font may be
scaled to.
The attribute value is a number in terms of the
:HP1.scale_basis:eHP1. attribute.
This attribute must be specified if the font characters are scaled.
:H4.CHAR_WIDTH Attribute
:P.
The :HP2.char_width:eHP2.
:I1.char_width attribute
:I2 refid='dafont'.char_width
attribute specifies the default width of
the characters that are in the font being defined.
The attribute value is a positive integer number which represents
the width of a character in terms of the horizontal base units
:I1.horizontal base units
specified in the device definition if the font is not scaled.
For example, if there are ten characters per inch with a particular font,
and there are 300 horizontal base units per inch, then the number
30 would be the default character width value for the font.
If the font is scaled, the value is in terms of the scale basis
specified by the :HP1.scale_basis:eHP1. attribute.
:H4.MONO_SPACE_WIDTH Attribute
:P.
The optional :HP2.mono_space_width:eHP2.
:I1.mono_space_width attribute
:I2 refid='dafont'.mono_space_width
attribute determines the way in which
the character widths of the font are used.
The attribute value may be one of the keywords YES and NO.
:DL.
:DT.YES
:DD.All of the characters in the font are treated as having
    the same width.
:DT.NO
:DD.The font is treated as a character set with varying widths.
:eDL.
:H3.Width Block
:P.
The :HP2.width:eHP2.
:I2 refid='dfont'.width block
block is an optional section of the font block.
The width block is specified within a font definition
after the font block attributes.
:FIG frame = box place = inline.
&gml.WIDTH.
font-character  character-width
&gml.eWIDTH.
:FIGCAP.The WIDTH Block
:EFIG.
:P.
The :HP2.&gml.width:eHP2.
:IH1 print='&gml.width tag'.width tag
:I1.width tag
tag begins the width block.
The :HP2.&gml.ewidth:eHP2.
:IH1 print='&gml.ewidth tag'.ewidth tag
:I1.ewidth tag
tag delimits the end of
the width block and must be the first
non-space characters on the line.
Hexadecimal values in the width block must begin
with a dollar($) sign.
:FIG frame = box place = inline.
&gml.WIDTH.
1    0
A   20
$C2 20
&gml.eWIDTH.
:FIGCAP.Example of the WIDTH Block
:EFIG.
:P.
The font-character value specifies the character
to define.
This value may be a single character, an integer number, or a hexadecimal
number.
If a single character is specified, it should not be enclosed in quotes.
:P.
The character-width value specifies the width
of the character in terms of the horizontal base units specified
in the device definition if the font is fixed, and in terms of the
scale basis if the font characters are scaled.
This value must be a non-negative integer number.
If a character is not defined in the width block, it is
assigned the width value defined by the char_width attribute
of the font block.
:H3 id='dvintr'.InTrans Block
:P.
The :HP2.intrans:eHP2.
:I1.input translation
:I2 refid='dfont'.intrans block
block is an optional section of the font block.
The intrans block is specified within a font definition
after the font block attributes.
:FIG frame = box place = inline.
&gml.INTRANS.
font-character  input-translation
&gml.eINTRANS.
:FIGCAP.The INTRANS Block
:EFIG.
:P.
The :HP2.&gml.intrans:eHP2.
:IH1 print='&gml.intrans tag'.intrans tag
:I1.intrans tag
tag begins the intrans block.
The :HP2.&gml.eintrans:eHP2.
:IH1 print='&gml.eintrans tag'.eintrans tag
:I1.eintrans tag
tag delimits the end of
the intrans block and must be the first
non-space characters on the line.
:FIG frame = box place = inline.
&gml.INTRANS.
1     1
A   $C1
$C2 $40
&gml.eINTRANS.
:FIGCAP.Example of the INTRANS Block
:EFIG.
:P.
The font-character and input-translation values
may be a single character, an integer number, or a hexadecimal
number.
Hexadecimal values in the intrans block must begin
with a dollar($) sign.
If a single character is specified, it should not be enclosed in quotes.
:P.
The input-translation value defines the character that
the font-character value is translated to when input translation
is performed.
If a character is not defined in the intrans block, the input
translation value of the character will be itself.
Refer to :HDREF refid='intrans'. for more information.
:H3.OutTrans Block
:P.
The :HP2.outtrans:eHP2.
:I1.output translation
:I2 refid='dfont'.outtrans block
block is an optional section of the font block.
The outtrans block is specified within a font definition
after the font block attributes.
:FIG frame = box place = inline.
&gml.OUTTRANS.
font-character  output-translation
&gml.eOUTTRANS.
:FIGCAP.The OUTTRANS Block
:EFIG.
:P.
The :HP2.&gml.outtrans:eHP2.
:IH1 print='&gml.outtrans tag'.outtrans tag
:I1.outtrans tag
tag begins the outtrans block.
The :HP2.&gml.eouttrans:eHP2.
:IH1 print='&gml.eouttrans tag'.eouttrans tag
:I1.eouttrans tag
tag delimits the end of
the outtrans block and must be the first
non-space characters on the line.
:FIG frame = box place = inline.
&gml.OUTTRANS.
1     1
A   $C1
$C2 $40
)   \\  )
175 \\  2  6  7
&gml.eOUTTRANS.
:FIGCAP.Example of the OUTTRANS Block
:EFIG.
:P.
The font-character value specifies the character
to define.
This value may be a single character, an integer number, or a hexadecimal
number.
Hexadecimal values in the outtrans block must begin
with a dollar($) sign.
If a single character is specified, it should not be enclosed in quotes.
:P.
Some output devices represent characters by a different
numeric sequence than the computer used to produce the document.
The output translation value may be specified to translate the
character entered in the input text to the numeric representation
required by the device.
Each output translation character is separated by a space,
and may be a character, an integer number, or a hexadecimal number.
If a character is not defined in the outtrans block, the output
translation value of the character will be itself.
