:H2.Defining a Device
:I1.defining a device
:I2 refid='device'.defining
:P.
The device definition combines the specification of device
setup characteristics, driver definition, and font definitions.
The :HP2.DEVICE:eHP2.
:I2 refid='device'.device block
block is processed by the GENDEV program
to create a device definition.
The device definition to be used for the processing of a document
is specified on the &WGML. command line.
:FIG id='devblk' place=inline frame=box.
&gml.DEVICE
   <attributes>
   <pause block>
   <devicefont block>
   <defaultfont block>
   <fontpause block>
   <rule block>
   <box block>
   <underscore block>
   <pagestart block>
&gml.eDEVICE.
:FIGCAP.The DEVICE Block
:EFIG.
:P.
A device block begins with the :HP2.&gml.device:eHP2.
:IH1 id='ddevice' print='&gml.device tag'.device tag
:I1.device tag
tag and
ends with the :HP2.&gml.edevice:eHP2.
:IH1 id='dedevic' print='&gml.edevice tag'.edevice tag
:I1.edevice tag
tag.
The attributes of the device block must all be specified.
The various blocks of information following the attributes
are not all required to define the device.
When they are specified, they must be in the order shown in
:FIGREF refid='devblk'..
:H3.Attributes of the Device Block
:FIG place=inline frame=box.
defined_name          = 'character string'
member_name           = 'character string'
driver_name           = 'character string'
output_name           = 'character string'
output_suffix         = 'character string'
page_width            = horizontal-base-units
page_depth            = vertical-base-units
horizontal_base_units = number
vertical_base_units   = number
:FIGCAP.Device Attributes
:EFIG.
:FIG place=inline frame=box.
defined_name          = 'x2700'
member_name           = 'x2700'
driver_name           = 'x2700drv'
output_name           = ''
output_suffix         = 'x2700'
page_width            = 2400
page_depth            = 3300
horizontal_base_units = 300
vertical_base_units   = 300
:FIGCAP.Example of the Device Attributes
:EFIG.
:SET symbol='local' value='device'.
:INCLUDE file='rfddef'.
:IH1 id='dadevic'.device attributes
:I2.defined_name
The defined name is used to identify the device on the
&WGML. command line.
:INCLUDE file='rfdmem'.
:I2 refid='dadevic'.member_name
:H4.DRIVER_NAME Attribute
:P.
The :HP2.driver_name:eHP2.
:I1.driver_name attribute
:I2 refid='dadevic'.driver_name
attribute specifies a character value that is
the defined name of a driver definition.
The driver definition contains the control sequences
used to produce the output for the device.
:H4 id='devout'.OUTPUT_NAME Attribute
:P.
If the output is to be directed to a specific device, such as a printer,
the :HP2.output_name:eHP2.
:I1.output_name attribute
:I2 refid='dadevic'.output_name
attribute specifies the name of that device.
A record specification may be part of the output name
(see :HDREF refid='files'.).
If a device does not exist with the specified name,
the attribute value will be used to create an output file name.
If the null string('') is specified as the attribute value, the file
name of the input source document will be used.
:H4.OUTPUT_SUFFIX Attribute
:P.
The character value of the :HP2.output_suffix:eHP2.
:I1.output_suffix attribute
:I2 refid='dadevic'.output_suffix
attribute will be
suffixed to the output name.
If the null string('') is specified as the attribute value, no
file suffix will be added.
:H4.PAGE_WIDTH Attribute
:P.
The :HP2.page_width:eHP2.
:I1.page_width attribute
:I2 refid='dadevic'.page_width
attribute specifies
the physical page width of the output page in horizontal base units.
The page width defined in the document layout may be smaller than
this value.
:H4.PAGE_DEPTH Attribute
:P.
The :HP2.page_depth:eHP2.
:I1.page_depth attribute
:I2 refid='dadevic'.page_depth
attribute specifies
the physical page depth of the output page in vertical base units.
:I1.vertical_base_units
The page depth defined in the document layout may be smaller or
larger than this value.
If the page depth in the document layout is larger than this value,
&WGML. will produce one document page over several of the device
pages.
:H4.HORIZONTAL_BASE_UNITS Attribute
:P.
The value of the :HP2.horizontal_base_units:eHP2.
:I1.horizontal_base_units attribute
:I2 refid='dadevic'.horizontal_base_units
:I1.horizontal_base_units
attribute is a positive integer number.
A horizontal base unit is the smallest unit of space that the
output device can advance in a horizontal direction.
The attribute value specifies the number of horizontal base units
which are equivalent to one inch of horizontal space.
:H4.VERTICAL_BASE_UNITS Attribute
:P.
The value of the :HP2.vertical_base_units:eHP2.
:I1.vertical_base_units attribute
:I2 refid='dadevic'.vertical_base_units
attribute is a positive integer number.
A vertical base unit is the smallest unit of space that the
output device can advance in a vertical direction.
The attribute value specifies the number of vertical base units
:I1.vertical_base_units
which are equivalent to one inch of vertical space.
:H3.PAUSE Block
:P.
The :HP2.pause:eHP2.
:I1.pausing
:I2 refid='ddevice'.pause block
block is used to
cause various actions to occur at the
terminal while &WGML. is processing the document.
The issuing of messages and pausing while changes to the output device
are made can be controlled through the pause block.
If pausing or messages are not required, the pause block may be omitted.
:FIG place=inline frame=box.
&gml.PAUSE
   place                = START|DOCUMENT
                          |DOCUMENT_PAGE
                          |DEVICE_PAGE
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.ePAUSE.
:FIGCAP.The PAUSE Block
:EFIG.
:P.
A pause block begins with the :HP2.&gml.pause:eHP2.
:IH1 print='&gml.pause tag'.pause tag
:I1 id='dapause'.pause tag
tag and ends with the :HP2.&gml.epause:eHP2.
:IH1 print='&gml.epause tag'.epause tag
:I1.epause tag
tag.
The place attribute and value section must both be specified.
:FIG place=inline frame=box.
&gml.PAUSE
   place = document
   &gml.value.
      %text( "Press enter to start the document." )
      %recordbreak()%wait()%clear3270()
   &gml.evalue.
&gml.ePAUSE.
:FIGCAP.Example of the PAUSE Block
:EFIG.
:H4.PLACE Attribute
:P.
The :HP2.place:eHP2.
:I2 refid='dapause'.place attribute
attribute specifies the point during processing
when &WGML. should evaluate the pause block.
The attribute value may be one of the keywords
:HP1.START:eHP1.,
:HP1.DOCUMENT:eHP1.,
:HP1.DOCUMENT_PAGE:eHP1.,
or
:HP1.DEVICE_PAGE:eHP1..
A separate pause block may be specified for each of the different
attribute values.
:DL break.
:DT.START
:DD.The pause block is evaluated when &WGML. begins processing
    the source input data.
:DT.DOCUMENT
:DD.The pause block is evaluated when &WGML. begins processing
    the document text.
:DT.DOCUMENT_PAGE
:DD.The pause block is evaluated at the beginning of each document
    page.
    A document page is the amount of output that &WGML.
    formats for a page in the document.
    The document page may be smaller or larger than the physical
    page produced by the output device.
    If the page being printed is both the document page and the
    device page, the document page pause block takes precedence
    over the device page pause block.
:DT.DEVICE_PAGE
:DD.The pause block is evaluated when &WGML. begins a new page
    on the output device.
:eDL.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies the pausing control sequences
to be output,
:INCLUDE file='rfdvalue'.
:H3.DEVICEFONT Block
:P.
A number of different device definitions may use the same fonts.
In some cases, the fonts available to one device may be a subset
of the fonts available to another device.
The device font block
:I2 refid='ddevice'.device font block
:I1.fonts
is used to define the fonts available to the
device definition.
One device font block is specified for each of the available fonts.
:FIG place=inline frame=box.
&gml.DEVICEFONT
   fontname    = 'character string'
   fontswitch  = 'character string'
   fontpause   = 'character string'
   resident    = YES | NO
&gml.eDEVICEFONT.
:FIGCAP.The DEVICEFONT Block
:EFIG.
:P.
A devicefont block begins with the :HP2.&gml.devicefont:eHP2.
:IH1 print='&gml.devicefont tag'.devicefont tag
:I1 id='dadevfo'.devicefont tag
tag and ends with the :HP2.&gml.edevicefont:eHP2.
:IH1 print='&gml.edevicefont tag'.edevicefont tag
:I1.edevicefont tag
tag.
All of the attributes of a device font block must be specified.
:FIG place=inline frame=box.
&gml.DEVICEFONT
        fontname   = 'vintage12'
        fontswitch = "x27 font switch"
        fontpause  = ""
        resident   = no
&gml.eDEVICEFONT.
:FIGCAP.Example of the DEVICEFONT Block
:EFIG.
:H4.FONTNAME Attribute
:P.
The :HP2.fontname:eHP2.
:I2 refid='dadevfo'.fontname attribute
attribute specifies a character value.
This value is the defined name value in a font definition.
:H4.FONTSWITCH Attribute
:P.
The :HP2.fontswitch:eHP2.
:I2 refid='dadevfo'.fontname attribute
attribute specifies a character value which
is the font switch method to be used when switching into the
font.
The attribute value must be an identifier defined by a font
switch block in the driver definition.
If the value is the null('') string, no font switching is performed.
:H4.FONTPAUSE Attribute
:P.
The :HP2.fontpause:eHP2.
:I2 refid='dadevfo'.fontpause attribute
attribute specifies a character value which
is the font pausing method to be used when switching into the
font.
The attribute value must be an identifier defined by a font
pause block in the current device definition.
If the value is the null('') string, no font pausing is performed.
:H4.RESIDENT Attribute
:P.
The :HP2.resident:eHP2.
:I2 refid='dadevfo'.resident attribute
attribute defines the resident status of the font
in the output device.
With some devices, information must be sent to the device for
each of the fonts being used.
To reduce the time required for printing a document, information
about the most commonly used fonts often 'reside'
on the device permanently.
The keywords :HP1.YES:eHP1.
or :HP1.NO:eHP1. may be specified as the attribute value.
:DL.
:DT.YES
:DD.The font resides permanently on the device.
:DT.NO
:DD.The font does not reside on the device.
:eDL.
:H3.DEFAULTFONT Block
:P.
The :HP2.defaultfont:eHP2.
:I2 refid='ddevice'.default font block
:I1.fonts
block specifies a font to be used with this
device when processing a document.
A default font block is specified for each default font to be
defined.
:FIG place=inline frame=box.
&gml.DEFAULTFONT
   font                 = number
   fontname             = 'character string'
   font_height          = number
   font_space           = number
   fontstyle            = PLAIN|UNDERSCORE
                          |BOLD|USBOLD
                          |UNDERLINE|ULBOLD
&gml.eDEFAULTFONT.
:FIGCAP.The DEFAULTFONT Block
:EFIG.
:P.
A default font block begins with the :HP2.&gml.defaultfont:eHP2.
:IH1 print='&gml.defaultfont tag'.defaultfont tag
:I1 id='dadeffo'.defaultfont tag
tag and ends with the :HP2.&gml.edefaultfont:eHP2.
:IH1 print='&gml.edefaultfont tag'.edefaultfont tag
:I1.edefaultfont tag
tag.
Most of the attributes in the default font block must be specified.
The attributes font_height and font_space are only necessary if
the font is scaled.
:FIG place=inline frame=box.
&gml.DEFAULTFONT
   font         = 1
   fontname     = 'times-italic'
   font_height  = 10
   font_space   = 2
   fontstyle    = plain
&gml.eDEFAULTFONT.
:FIGCAP.Example of the DEFAULTFONT Block
:EFIG.
:H4.FONT Attribute
:P.
The value of the :HP2.font:eHP2.
:I2 refid='dadeffo'.font attribute
attribute must be a
non-negative integer number value.
The font numbers zero through three correspond to the
highlight phrase tags in the GML document.
They also correspond directly to the font numbers used in the
layout definition.
The default font with a font number of zero must be specified.
All font numbers used in the document
which are not defined with the default font block
or the font option on the &WGML. command line will be
assigned the values specified in the font zero default font block.
:H4.FONTNAME Attribute
:P.
The :HP2.fontname:eHP2.
:I2 refid='dadeffo'.fontname attribute
attribute specifies a character value.
This value is a font name defined by a device font block
in the device definition.
The font name must be specified in a devicefont block within
the current device definition.
:H4.FONT_HEIGHT Attribute
:P.
The :HP2.font_height:eHP2.
:I2 refid='dadeffo'.font_height attribute
attribute specifies a number value.
This value is the point size of the characters in a scaled font.
The attribute does not have to be specified if the font is not scaled.
:H4.FONT_SPACE Attribute
:P.
The :HP2.font_space:eHP2.
:I2 refid='dadeffo'.font_space attribute
attribute specifies a number value.
This value is the space between lines of the font, and is specified
in points.
The attribute does not have to be specified if the font is not scaled.
:H4.FONTSTYLE Attribute
:P.
The font style values may be applied to any font.
The value of the :HP2.fontstyle:eHP2.
:I2 refid='dadeffo'.fontstyle attribute
attribute value is a
keyword value which defines the style
of the font.
The keyword value may be one of
:HP1.PLAIN:eHP1.,
:HP1.UNDERSCORE:eHP1.,
:HP1.BOLD:eHP1.,
:HP1.USBOLD:eHP1.,
:HP1.UNDERLINE:eHP1.,
or :HP1.ULBOLD:eHP1..
:DL.
:DT.PLAIN
:DD.The characters of the font are sent to the output device
    without modification.
:DT.UNDERSCORE
:DD.With the exception of space characters, all characters
    of the font are underlined.
:DT.BOLD
:DD.The characters of the font are bolded.
:DT.USBOLD
:DD.With the exception of space characters, all characters
    of the font are underlined and bolded.
:DT.UNDERLINE
:DD.The characters of the font are underlined.
:DT.ULBOLD
:DD.The characters of the font are underlined and bolded.
:eDL.
:H3.FONTPAUSE Block
:P.
A change from one font to another font often requires &WGML.
to send a control sequence to the output device.
In some cases, the font switch may require physical
intervention at the output device by the operator.
Examples of such an intervention would be changing a print wheel
or color ribbon.
The :HP2.fontpause:eHP2.
:I2 refid='ddevice'.fontpause block
:I1.pausing
:I1.fonts
block defines a pausing method to use, and
is referenced by the device font block.
The font pause block is not required if font pausing
is not needed.
:FIG place=inline frame=box.
&gml.FONTPAUSE
   type  = 'character string'
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eFONTPAUSE.
:FIGCAP.The FONTPAUSE Block
:EFIG.
:P.
A fontpause block begins with the :HP2.&gml.fontpause:eHP2.
:IH1 print='&gml.fontpause tag'.fontpause tag
:I1 id='dafontp'.fontpause tag
tag and ends with the :HP2.&gml.efontpause:eHP2.
:IH1 print='&gml.efontpause tag'.efontpause tag
:I1.efontpause tag
tag.
The type attribute and the value section must both be specified.
:FIG place=inline frame=box.
&gml.FONTPAUSE
   type  = '12-pitch wheel'
   &gml.value.
      %text( "Please attach the 12 pitch wheel" )
      %recordbreak()%wait()
   &gml.evalue.
&gml.eFONTPAUSE.
:FIGCAP.Example of the FONTPAUSE Block
:EFIG.
:H4.TYPE Attribute
:P.
The :HP2.type:eHP2.
:I2 refid='dafontp'.type attribute
attribute specifies a character value.
The specified value is referenced by the device font block when
a font pause method is required.
The attribute value must be unique among the font pause blocks in
the device definition.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies the pausing control sequences
to be output before the font switch,
:INCLUDE file='rfdvalue'.
:H3.RULE Block
:P.
The :HP2.rule:eHP2. block specifies the
information necessary for &WGML.
to create a rule line, and must be specified.
:FIG place=inline frame=box.
&gml.RULE
   font         = number | 'character string'
   rule_value   = number | 'character'
&gml.eRULE.
:FIGCAP.The RULE Block
:EFIG.
:P.
A rule block
:I2 refid='ddevice'.rule block
begins with the :HP2.&gml.rule:eHP2.
:IH1 print='&gml.rule tag'.rule tag
:I1 id='darule'.rule tag
tag and ends with the :HP2.&gml.erule:eHP2.
:IH1 print='&gml.erule tag'.erule tag
:I1.erule tag
tag.
Both of the attribute values in the rule block must be specified.
:FIG place=inline frame=box.
&gml.RULE
   font         = 0
   rule_value   = '-'
&gml.eRULE.
:FIGCAP.Example of the RULE Block
:EFIG.
:H4.FONT Attribute
:P.
The :HP2.font:eHP2.
:I2 refid='darule'.font attribute
attribute value
may be either a non-negative integer
number or a character value.
If a number is specified, the font of the rule line will
be the default font with the corresponding font number.
A character attribute value must be a font name defined in a
device font block.
The font style PLAIN will be used in this case.
Using a specific font name will ensure that the rule line will
be drawn with a particular font regardless of the selected
default fonts.
:H4.RULE_VALUE Attribute
:P.
The :HP2.rule_value:eHP2.
:I2 refid='darule'.rule_value attribute
attribute may be either a
number or a character value.
The number value may be either a non-negative integer
or a hexadecimal number beginning with a dollar($) sign.
The number represents a character available in the font being
used for the rule line, and is usually used when the character
cannot be entered at the terminal.
If a character value is specified, it must be delimited by quotes
and only one character in length.
The attribute value is used to construct the rule line.
:H3.BOX Block
:P.
The :HP2.box:eHP2.
:I2 refid='ddevice'.box block
:I1.framing
block specifies
the information necessary for &WGML.
to create a box, and must be specified.
:FIG place=inline frame=box.
&gml.BOX
   font               = number | 'character string'
   top_line           = number | 'character'
   bottom_line        = number | 'character'
   left_side          = number | 'character'
   right_side         = number | 'character'
   top_left           = number | 'character'
   top_right          = number | 'character'
   bottom_left        = number | 'character'
   bottom_right       = number | 'character'
&gml.eBOX.
:FIGCAP.The BOX Block
:EFIG.
:P.
A box block begins with the :HP2.&gml.box:eHP2.
:IH1 print='&gml.box tag'.box tag
:I1.box tag
tag and ends with the :HP2.&gml.ebox:eHP2.
:IH1 print='&gml.ebox tag'.ebox tag
:I1.ebox tag
tag.
All of the box block attributes must be specified.
With the exception of the font attribute, all of the attribute
values may be either a
number or a character value.
The number value may be either a non-negative integer
or a hexadecimal number starting with a dollar($) sign.
The number represents a character available in the font being
used for the box, and is usually used when the value
cannot be entered as a character at the terminal.
If a character value is specified, it must be delimited by quotes
and only one character in length.
:FIG place=inline frame=box.
&gml.BOX
   font         = 0
   top_line     = '-'
   bottom_line  = '-'
   left_side    = '!'
   right_side   = '!'
   top_left     = '+'
   top_right    = '+'
   bottom_left  = '+'
   bottom_right = '+'
&gml.eBOX.
:FIGCAP.Example of the BOX Block
:EFIG.
:H4.FONT Attribute
:P.
The :HP2.font:eHP2.
:IH1 id='dabox' print='&gml.box attributes'.boxattr
:I2 refid='dabox'.font attribute
attribute may be either a non-negative integer
number or a character value.
If a number is specified, the font of the box will be
the default font with the corresponding font number.
A character attribute value must be a font name defined in a
device font block.
Using a specific font name will ensure that the box will
be drawn with a particular font regardless of the selected
default fonts.
:H4.TOP_LINE Attribute
:P.
The :HP2.top_line:eHP2.
:I2 refid='dabox'.top_line attribute
attribute value specifies the character used
to create the top line of the box.
:H4.BOTTOM_LINE Attribute
:P.
The :HP2.bottom_line:eHP2.
:I2 refid='dabox'.bottom_line attribute
attribute value specifies the character used
to create the bottom line of the box.
:H4.LEFT_SIDE Attribute
:P.
The :HP2.left_side:eHP2.
:I2 refid='dabox'.left_side attribute
attribute value specifies the character used
to create the left side line of the box.
:H4.RIGHT_SIDE Attribute
:P.
The :HP2.right_side:eHP2.
:I2 refid='dabox'.right_side attribute
attribute value specifies the character used
to create the right side line of the box.
:H4.TOP_LEFT Attribute
:P.
The :HP2.top_left:eHP2.
:I2 refid='dabox'.top_left attribute
attribute value specifies the character used
to create the top left corner of the box.
:H4.TOP_RIGHT Attribute
:P.
The :HP2.top_right:eHP2.
:I2 refid='dabox'.top_right attribute
attribute value specifies the character used
to create the top right corner of the box.
:H4.BOTTOM_LEFT Attribute
:P.
The :HP2.bottom_left:eHP2.
:I2 refid='dabox'.bottom_left attribute
attribute value specifies the character used
to create the bottom left corner of the box.
:H4.BOTTOM_RIGHT Attribute
:P.
The :HP2.bottom_right:eHP2.
:I2 refid='dabox'.bottom_right attribute
attribute value specifies the character used
to create the bottom right corner of the box.
:H3.UNDERSCORE Block
:P.
The :HP2.underscore:eHP2.
:I2 refid='ddevice'.underscore block
:I1.underscoring
:I1.underlining
block specifies
the character information needed by &WGML.
to perform underscoring.
If the underscore block is not specified, the standard
underscore character will be used.
:FIG place=inline frame=box.
&gml.UNDERSCORE
   font         = number | 'character string'
   score_value  = number | 'character'
&gml.eUNDERSCORE.
:FIGCAP.The UNDERSCORE Block
:EFIG.
:P.
An underscore block begins with the :HP2.&gml.underscore:eHP2.
:IH1 print='&gml.underscore tag'.underscore tag
:I1 id='daunder'.underscore tag
tag and ends with the :HP2.&gml.eunderscore:eHP2.
:IH1 print='&gml.eunderscore tag'.eunderscore tag
:I1.eunderscore tag
tag.
All of the attributes in the underscore block must be specified.
:FIG place=inline frame=box.
&gml.UNDERSCORE
   font         = 0
   score_value  = '_'
&gml.eUNDERSCORE.
:FIGCAP.Example of the UNDERSCORE Block
:EFIG.
:H4.FONT Attribute
:P.
The :HP2.font:eHP2.
:I2 refid='daunder'.font attribute
attribute value
may be either a non-negative integer
number or a character value.
If a number is specified, the font of the underscore character will
the default font with the corresponding font number.
A character attribute value must be a font name defined in a
device font block.
The font style PLAIN will be used in this case.
Using a specific font name will ensure that the underscore character will
be used with a particular font regardless of the selected
default fonts.
:H4.SCORE_VALUE Attribute
:P.
The :HP2.score_value:eHP2.
:I2 refid='daunder'.score_value attribute
attribute may be either a
number or a character value.
The number value may be either a non-negative integer
or a hexadecimal number beginning with a dollar($) sign.
The number represents a character available in the font being
used for the underscore character, and is usually used when the character
cannot be entered at the terminal.
If a character value is specified, it must be delimited by quotes
and only one character in length.
The specified attribute is used to underscore text.
:H3.PAGESTART Block
:P.
The :HP2.pagestart:eHP2.
:I2 refid='ddevice'.pagestart block
block specifies the
address of the first line on the output page.
:FIG place=inline frame=box.
&gml.PAGESTART
   x_start  = number
   y_start  = number
&gml.ePAGESTART.
:FIGCAP.The PAGESTART Block
:EFIG.
:P.
A pagestart block begins with the :HP2.&gml.pagestart:eHP2.
:IH1 print='&gml.pagestart tag'.pagestart tag
:I1 id='dapages'.pagestart tag
tag and ends with the :HP2.&gml.epagestart:eHP2.
:IH1 print='&gml.epagestart tag'.epagestart tag
:I1.epagestart tag
tag.
All of the attributes in the pagestart block must be specified.
:FIG place=inline frame=box.
&gml.PAGESTART
   x_start  = 300
   y_start  = 500
&gml.ePAGESTART.
:FIGCAP.Example of the PAGESTART Block
:EFIG.
:H4.X_START Attribute
:P.
The :HP2.x_start:eHP2.
:I2 refid='dapages'.x_start attribute
attribute value
is a non-negative integer number.
The value is in horizontal base units.
:H4.Y_START Attribute
:P.
The :HP2.y_start:eHP2.
:I2 refid='dapages'.y_start attribute
attribute value
is a non-negative integer number.
The value is in vertical base units.
