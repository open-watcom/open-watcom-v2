:H2.Defining a Driver
:I1.defining drivers
:I2 refid='drive'.defining
:P.
A driver definition specifies the control sequences and
methods by which output is
produced.
This includes such things as how to do a font change,
bolding, and underlining.
The :HP2.DRIVER:eHP2.
:I2 refid='drive'.driver block
block is processed by the GENDEV program
to create a driver definition.
The resulting definition is referenced by the device definitions.
:FIG id='drvblk' place=inline frame=box.
&gml.DRIVER
   <attributes>
   <init block>
   <finish block>
   <newline block>
   <newpage block>
   <htab block>
   <boldstart block>
   <boldend block>
   <understart block>
   <underend block>
   <fontswitch block>
   <pageaddress block>
   <absoluteaddress block>
   <hline block>
   <vline block>
   <dbox block>
&gml.eDRIVER.
:FIGCAP.The DRIVER Block
:eFIG.
:P.
A driver block begins with the :HP2.&gml.driver:eHP2.
:IH1 print='&gml.driver tag'.driver tag
:I1 id='ddrive'.driver tag
tag and ends with the :HP2.&gml.edriver:eHP2.
:IH1 print='&gml.edriver tag'.edriver tag
:I1 id='dedrive'.edriver tag
tag.
The attributes of the driver block must all be specified.
The various blocks of information following the attributes
are not all required to define the driver.
When they are specified, they must be in the order
shown in
:FIGREF refid='drvblk'..
:H3.Attributes of the Driver Block
:FIG place=inline frame=box.
defined_name     = 'character string'
member_name      = 'character string'
rec_spec         = 'character string'
fill_char        = number | character
:FIGCAP.Attributes of the DRIVER Block
:eFIG.
:FIG place=inline frame=box.
defined_name     = 'x2700drv'
member_name      = 'x27drv'
rec_spec         = '(f&gml.80)'
fill_char        = 0
:FIGCAP.Example of the DRIVER Block Attributes
:eFIG.
:SET symbol='local' value='driver'.
:INCLUDE file='rfddef'.
:IH1 id='dadrive'.driver attributes
:I2.defined_name
:I1.defined_name attribute
The defined name is used to identify the driver when
creating a device definition.
:INCLUDE file='rfdmem'.
:I2 refid='dadrive'.member_name
:I1.member_name attribute
:H4.REC_SPEC Attribute
:P.
The :HP2.rec_spec:eHP2.
:I1.rec_spec attribute
:I2 refid='dadrive'.rec_spec
attribute specifies a record specification
value (for example, either (f&gml.80) or (f&gml.c&gml.80) are allowed) for
the output device.
The attribute value must be a valid record specification
(see :HDREF refid='files'.).
:H4.FILL_CHAR Attribute
:P.
A fill character is needed when the output records
for the device have a fixed length.
If a record is output which is less than the record length, the
record must be filled out to the required length.
The :HP2.fill_char:eHP2.
:I1.fill_char attribute
:I2 refid='dadrive'.fill_char
attribute specifies
the fill character to be used when doing this record filling.
The attribute value may be a single character value enclosed
in quotes, an integer number, or a hexadecimal number.
A hexadecimal number is preceded by the dollar($) sign.
:H3.INIT Block
:P.
The :HP2.init:eHP2.
:I2 refid='ddrive'.init block
block specifies
the initialization values which are to be output to a device.
If no initialization is required,
the init block may be omitted.
The two value sections of the init block may appear
in any order and as many times as necessary.
:FIG place=inline frame=box.
&gml.INIT
   place= START | DOCUMENT
   &gml.value.
      <device functions>
   &gml.evalue.
   &gml.fontvalue.
      <device functions>
   &gml.efontvalue.
&gml.eINIT.
:FIGCAP.The INIT Block
:eFIG.
:P.
The init block begins with the :HP2.&gml.init:eHP2.
:IH1 print='&gml.init tag'.init tag
:I1 id='dainit'.init tag
tag and ends with the :HP2.&gml.einit:eHP2.
:IH1 print='&gml.einit tag'.einit tag
:I1.einit tag
tag.
The place attribute and at least one of the value
or fontvalue sections must be specified.
:FIG place=inline frame=box.
&gml.INIT
   place= start
   &gml.fontvalue.
      %text(%font_outname2())%binary1(0)
      %text(%font_outname1())%recordbreak()
   &gml.efontvalue.
   &gml.value.
      %binary1($27)
      %text('+P,X2700 -- &WGML. -- ')
      %binary1($15)%recordbreak()
   &gml.evalue.
&gml.eINIT.
:FIGCAP.Example of the INIT Block
:eFIG.
:H4.PLACE Attribute
:P.
The :HP2.place:eHP2.
:I2 refid='dainit'.place attribute
attribute indicates
where in the output the specified
initialization sequences are to appear.
The attribute value may be the
keyword :HP1.START:eHP1. or :HP1.DOCUMENT:eHP1..
A separate init block may be specified for each of these two values.
An automatic font select of font zero is performed at the beginning
of a document as part of the initialization.
:DL.
:DT.START
:DD.The init block is
    evaluated when &WGML. starts processing the input source.
:DT.DOCUMENT
:DD.The init block is
    evaluated when &WGML. starts processing a document.
:eDL.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies the general initialization
sequence to be output,
:INCLUDE file='rfdvalue'.
The value section may be specified more than once, and may precede
and/or follow a fontvalue section.
:H4.FONTVALUE Section
:P.
The :HP2.fontvalue:eHP2.
:I2 refid='dainit'.fontvalue section
section is
used to perform font initialization,
and is started with the :HP2.&gml.fontvalue:eHP2.
:IH1 print='&gml.fontvalue tag'.fontvalue tag
:I1.fontvalue tag
:I1.fonts
:IH2 print='&gml.fontvalue tag'.fontvalue tag
:I2.fontvalue tag
tag.
Device functions are then entered after the &gml.fontvalue tag, and
may be specified on more than one line.
&WGML. selects
the fonts being used in the document.
For each of the selected fonts, the fontvalue section is evaluated.
Device functions, such %default_width, will return the values
appropriate for the selected font.
The :HP2.&gml.efontvalue:eHP2.
:IH1 print='&gml.efontvalue tag'.efontvalue tag
:I1.efontvalue tag
tag delimits the end of a fontvalue section,
and must be the first non-space characters in the line.
The fontvalue section may be specified more than once, and may precede
and/or follow a value section.
:H3.FINISH Block
:P
The :HP2.finish:eHP2.
:I2 refid='ddrive'.finish block
block specifies
the finalization values which are to be output.
If no finalization is required,
the finish block may be omitted.
:FIG place=inline frame=box.
&gml.FINISH
   place= DOCUMENT | END
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eFINISH.
:FIGCAP.The FINISH Block
:eFIG.
:P.
The finish block begins with the :HP2.&gml.finish:eHP2.
:IH1 print='&gml.finish tag'.finish tag
:I1 id='dafinis'.finish tag
tag and ends
with the :HP2.&gml.efinish:eHP2.
:IH1 print='&gml.efinish tag'.efinish tag
:I1.efinish tag
tag.
The place attribute and the value section must both be specified.
:FIG place=inline frame=box.
&gml.FINISH
   place=end
   &gml.value.
      %binary1(21)%binary1($27)%text('+X')
      %binary1(21)%recordbreak()
   &gml.evalue.
&gml.eFINISH.
:FIGCAP.Example of the FINISH Block
:eFIG.
:H4.PLACE Attribute
:P.
The :HP2.place:eHP2.
:I2 refid='dafinis'.place attribute
attribute
indicates where in the output the specified
finalization sequence is to appear.
The attribute value may
be the word :HP1.DOCUMENT:eHP1. or :HP1.END:eHP1..
A separate finish block may be specified for each of these two values.
:DL.
:DT.DOCUMENT
:DD.The finish block is
    evaluated when &WGML. finishes processing a document.
:DT.END
:DD.The finish block is
    evaluated when &WGML. finishes processing the input source.
:eDL.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies the general finalization
sequence to be output,
:INCLUDE file='rfdvalue'.
:H3.NEWLINE Block
:P.
When &WGML. starts a new line in the output,
it must know the method by which to start the new line.
The :HP2.newline:eHP2.
:I2 refid='ddrive'.newline block
block specifies this method.
To provide for different spacing mechanisms, the newline block
may be specified any number of times to set different sequences
for advancing lines with different line spacing.
:FIG place=inline frame=box.
&gml.NEWLINE
   advance=number
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eNEWLINE.
:FIGCAP.The NEWLINE Block
:eFIG.
:P.
The newline block begins with
the :HP2.&gml.newline:eHP2.
:IH1 print='&gml.newline tag'.newline tag
:I1 id='danewli'.newline tag
tag and ends with the :HP2.&gml.enewline:eHP2.
:IH1 print='&gml.enewline tag'.enewline tag
:I1.enewline tag
tag.
The advance attribute and the value section must both be specified.
:FIG place=inline frame=box.
&gml.NEWLINE
   advance=1
   &gml.value.
      %binary1(13)%binary1(10)
   &gml.evalue.
&gml.eNEWLINE.
:FIGCAP.Example of the NEWLINE Block
:eFIG.
:H4.ADVANCE Attribute
:P.
The :HP2.advance:eHP2.
:I2 refid='danewli'.advance attribute
attribute specifies the number of lines that
will be advanced when the given sequence is output.
The attribute value must be a non-negative integer.
A particular advance number may be used only once in a newline block.
The value zero gives the return to beginning of line sequence.
This must be supplied if underlining and/or bolding is accomplished
with overstriking.
A newline block with an advance value of one must be specified.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies
the sequence to be output to obtain
the required number of line advances,
:INCLUDE file='rfdvalue'.
:H3.NEWPAGE Block
:P.
The :HP2.newpage:eHP2.
:I2 refid='ddrive'.newpage block
block defines the
method by which &WGML. will start
a new page in the output, and must be specified.
:FIG place=inline frame=box.
&gml.NEWPAGE
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eNEWPAGE.
:FIGCAP.The NEWPAGE Block
:eFIG.
:P.
The newpage block begins
with the :HP2.&gml.newpage:eHP2.
:IH1 print='&gml.newpage tag'.newpage tag
:I1 id='danewpa'.newpage tag
tag and ends with the :HP2.&gml.enewpage:eHP2.
:IH1 print='&gml.enewpage tag'.enewpage tag
:I1.enewpage tag
tag.
The value section must be specified.
:FIG place=inline frame=box.
&gml.NEWPAGE
   &gml.value.
      %binary1(12)
   &gml.evalue.
&gml.eNEWPAGE.
:FIGCAP.Example of the NEWPAGE Block
:eFIG.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies
the sequence to be output to obtain
the new page,
:INCLUDE file='rfdvalue'.
:H3.HTAB Block
:P.
The :HP2.htab:eHP2.
:I2 refid='ddrive'.htab block
:I1.relative tabbing
:I1.horizontal addressing
block defines a relative horizontal tabbing sequence.
It must define a method of moving in a forward horizontal direction
based upon the horizontal base unit measurement of the device.
This block is not required, but will allow the production of
a more professional looking document.
If a proportional font is used, and this block is not specified,
it may not be possible to properly align the words to the right
hand side of the column when the text is to be justified.
:FIG place=inline frame=box.
&gml.HTAB
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eHTAB.
:FIGCAP.The HTAB Block
:eFIG.
:P.
The htab block begins with the :HP2.&gml.htab:eHP2.
:IH1 print='&gml.htab tag'.htab tag
:I1 id='dahtab'.htab tag
tag and ends with the :HP2.&gml.ehtab:eHP2.
:IH1 print='&gml.ehtab tag'.ehtab tag
:I1.ehtab tag
tag.
The value section must be specified.
:FIG place=inline frame=box.
&gml.CMT.Set up for horizontal tabbing.
&gml.CMT.Issue tab control codes.
&gml.CMT.Add tab width value over 256 to the '@' character
&gml.CMT.Divide the tab width value between 16 and 256
&gml.CMT.by 16 and add to the '@' character.
&gml.CMT.Add the tab width value between 0 and 16
&gml.CMT.to the '@' character.
&gml.HTAB
   &gml.value.
      %binary1(27)%text('H')
      %binary1(%add(%divide(%tab_width(),256),$40))
      %binary1(%add(%divide(%remainder(
               %tab_width(),256),16),$40))
      %binary1(%add(%remainder(%tab_width(),16),$40))
   &gml.evalue.
&gml.eHTAB.
:FIGCAP.Example of the HTAB Block
:eFIG.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies
the sequence to be output to obtain
the horizontal tab,
:INCLUDE file='rfdvalue'.
The device function TAB_WIDTH will contain the amount of
horizontal space &WGML. will needs to tab over.
:H3.BOLDSTART Block
:P.
The :HP2.boldstart:eHP2.
:I2 refid='ddrive'.boldstart block
:I1.bolding
block defines
the method by which &WGML. will
cause text to appear in boldface in the output.
If this block is not specified, bold text is obtained by overprinting
the output line.
If bolding is accomplished with
overprinting, a newline block with an advance
of zero must be specified.
:FIG place=inline frame=box.
&gml.BOLDSTART
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eBOLDSTART.
:FIGCAP.The BOLDSTART Block
:eFIG.
:P.
The boldstart block begins with the :HP2.&gml.boldstart:eHP2.
:IH1 print='&gml.boldstart tag'.boldstart tag
:I1 id='dabolds'.boldstart tag
tag and ends with the :HP2.&gml.eboldstart:eHP2.
:IH1 print='&gml.eboldstart tag'.eboldstart tag
:I1.eboldstart tag
tag.
The value section must be specified.
:FIG place=inline frame=box.
&gml.BOLDSTART
   &gml.value.
      %binary1($27)%text('b')
   &gml.evalue.
&gml.eBOLDSTART.
:FIGCAP.Example of the BOLDSTART Block
:eFIG.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies
the sequence to be output to obtain
the bolding,
:INCLUDE file='rfdvalue'.
:H3.BOLDEND Block
:P.
The :HP2.boldend:eHP2.
:I2 refid='ddrive'.boldend block
block defines the sequences needed to stop the bolding of text.
The boldend block is required if a boldstart block is specified.
:FIG place=inline frame=box.
&gml.BOLDEND
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eBOLDEND.
:FIGCAP.The BOLDEND Block
:eFIG.
:P.
The boldend block begins with the :HP2.&gml.boldend:eHP2.
:IH1 print='&gml.boldend tag'.boldend tag
:I1 id='dabolde'.boldend tag
tag and ends with the :HP2.&gml.eboldend:eHP2.
:IH1 print='&gml.eboldend tag'.eboldend tag
:I1.eboldend tag
tag.
The value section must be specified.
:FIG place=inline frame=box.
&gml.BOLDEND
   &gml.value.
      %binary1($27)%text('p')
   &gml.evalue.
&gml.eBOLDEND.
:FIGCAP.Example of the BOLDEND Block
:eFIG.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies the sequence to be output to stop
the bolding of text,
:INCLUDE file='rfdvalue'.
:H3.UNDERSTART Block
:P.
The :HP2.understart:eHP2.
:I2 refid='ddrive'.understart block
:I1.underscoring
:I1.underlining
block defines
the method by which &WGML. will
cause text to be underscored in the output.
If this block is not specified,
underscoring of text is obtained by overprinting
the output line.
If overprinting is used, a newline block with an advance
of zero must be specified.
:FIG place=inline frame=box.
&gml.UNDERSTART
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eUNDERSTART.
:FIGCAP.The UNDERSTART Block
:eFIG.
:P.
The understart block begins with the :HP2.&gml.understart:eHP2.
:IH1 print='&gml.understart tag'.understart tag
:I1.understart tag
tag and ends with the :HP2.&gml.eunderstart:eHP2.
:IH1 print='&gml.eunderstart tag'.eunderstart tag
:I1.eunderstart tag
tag.
The value section must be specified.
:FIG place=inline frame=box.
&gml.UNDERSTART
   &gml.value.
      %binary1($27)%text('u')
   &gml.evalue.
&gml.eUNDERSTART.
:FIGCAP.Example of the UNDERSTART Block
:eFIG.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies
the sequence to be output to obtain
the underscoring,
:INCLUDE file='rfdvalue'.
:H3.UNDEREND Block
:P.
The :HP2.underend:eHP2.
:I2 refid='ddrive'.underend block
block defines the
sequences needed to stop the underscoring
of text.
The underend block is required if an understart block is specified.
:FIG place=inline frame=box.
&gml.UNDEREND
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eUNDEREND.
:FIGCAP.The UNDEREND Block
:eFIG.
:P.
The underend block begins with the :HP2.&gml.underend:eHP2.
:IH1 print='&gml.underend tag'.underend tag
:I1.underend tag
tag and ends with the :HP2.&gml.eunderend:eHP2.
:IH1 print='&gml.eunderend tag'.eunderend tag
:I1.eunderend tag
tag.
The value section must be specified.
:FIG place=inline frame=box.
&gml.UNDEREND
   &gml.value.
      %binary1($27)%text('w')
   &gml.evalue.
&gml.eUNDEREND.
:FIGCAP.Example of the UNDEREND Block
:eFIG.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies the sequence to be output to stop
the underscoring of text,
:INCLUDE file='rfdvalue'.
:H3.FONTSWITCH Block
:P.
The :HP2.fontswitch:eHP2.
:I2 refid='ddrive'.fontswitch block
block identifies a method for switching fonts.
With some output devices, different fonts are available when
control sequences are used to switch from the default font.
A separate fontswitch block may be specified for each type
of font switch method available with the device.
:FIG place=inline frame=box.
&gml.FONTSWITCH
   type=string
   &gml.startvalue.
      <device functions>
   &gml.estartvalue.
   &gml.endvalue.
      <device functions>
   &gml.eendvalue.
&gml.eFONTSWITCH.
:FIGCAP.The FONTSWITCH Block
:eFIG.
:P.
The fontswitch block begins with the :HP2.&gml.fontswitch:eHP2.
:IH1 print='&gml.fontswitch tag'.fontswitch tag
:I1 id='dafonts'.fontswitch tag
tag and ends with the :HP2.&gml.efontswitch:eHP2.
:IH1 print='&gml.efontswitch tag'.efontswitch tag
:I1.efontswitch tag
tag.
The type attribute and the value section must be specified.
:FIG place=inline frame=box.
&gml.FONTSWITCH
   type='qume proportional on'
   &gml.startvalue.
      %binary1(27)%text('$')
   &gml.estartvalue.
   &gml.endvalue.
      %binary1(27)%text('x')
      %recordbreak()
   &gml.eendvalue.
&gml.eFONTSWITCH.
:FIGCAP.Example of the FONTSWITCH Block
:eFIG.
:H4 id='fswtype'.TYPE Attribute
:P.
The character value of the :HP2.type:eHP2.
:I2 refid='dafonts'.type attribute
attribute provides
an identifier for the font switch method.
This identifier is referenced in the device definition when
specifying the fonts available for the device.
The attribute value must be unique among the font switch blocks
in the driver definition.
:H4.STARTVALUE Section
:P.
The :HP2.startvalue:eHP2.
:I2 refid='dafonts'.startvalue section
section specifies
the sequence to be output to perform
the font switch,
and is started
with the :HP2.&gml.startvalue:eHP2.
:IH1 print='&gml.startvalue tag'.startvalue tag
:I1.startvalue tag
tag.
Device functions are then entered after the &gml.startvalue tag, and
may be specified on more than one line.
The :HP2.&gml.estartvalue:eHP2.
:IH1 print='&gml.estartvalue tag'.estartvalue tag
:I1.estartvalue tag
tag delimits the end of a startvalue section,
and must be the first non-space characters in the line.
:P.
When a switch between two fonts is necessary,
the startvalue sections of the two fonts are evaluated.
The font switch is only performed if the results of the
two evaluations are different.
:H4.ENDVALUE Section
:P.
The :HP2.endvalue:eHP2.
:I2 refid='dafonts'.endvalue section
section specifies
the sequence to be output before the font switch
sequence of the new font is performed,
and is started
with the :HP2.&gml.endvalue:eHP2.
:IH1 print='&gml.endvalue tag'.endvalue tag
:I1.endvalue tag
tag.
Device functions are then entered after the &gml.endvalue tag, and
may be specified on more than one line.
The :HP2.&gml.eendvalue:eHP2.
:IH1 print='&gml.eendvalue tag'.eendvalue tag
:I1.eendvalue tag
tag delimits the end of an endvalue section,
and must be the first non-space characters in the line.
:H3.PAGEADDRESS Block
:P.
As text is placed on the output page,
the X and Y components of the address are adjusted to make a new
address.
With some output devices, this adjustment is added (positive) to
the address.
The adjustment is subtracted (negative) with other output devices.
The :HP2.pageaddress:eHP2.
:I2 refid='ddrive'.pageaddress block
block specifies
whether the adjustment is positive or negative.
If the output device does not support page addressing, this
block should not be specified.
(See :HDREF refid='devpgad'. for more information).
:FIG place=inline frame=box.
&gml.PAGEADDRESS
   x_positive = YES | NO
   y_positive = YES | NO
&gml.ePAGEADDRESS.
:FIGCAP.The PAGEADDRESS Block
:eFIG.
:P.
The pageaddress block begins with the :HP2.&gml.pageaddress:eHP2.
:IH1 print='&gml.pageaddress tag'.pageaddress tag
:I1 id='dapagea'.pageaddress tag
tag and ends
with the :HP2.&gml.epageaddress:eHP2.
:IH1 print='&gml.epageaddress tag'.epageaddress tag
:I1.epageaddress tag
tag.
The two attributes must be specified.
:FIG place=inline frame=box.
&gml.PAGEADDRESS
   x_positive = yes
   y_positive = yes
&gml.ePAGEADDRESS.
:FIGCAP.Example of the PAGEADDRESS Block
:eFIG.
:H3.ABSOLUTEADDRESS Block
:P.
When an output device supports page addressing,
the :HP2.absoluteaddress:eHP2.
:I1.absolute addressing
:I2 refid='ddrive'.absoluteaddress block
block specifies the
mechanism for absolute page addressing.
If the output device does not support page addressing, this
block should not be specified.
:FIG place=inline frame=box.
&gml.ABSOLUTEADDRESS
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eABSOLUTEADDRESS.
:FIGCAP.The ABSOLUTEADDRESS Block
:eFIG.
:P.
The absoluteaddress block begins with the :HP2.&gml.absoluteaddress:eHP2.
:IH1 print='&gml.absoluteaddress tag'.absoluteaddress tag
:I1 id='daabsol'.absoluteaddress tag
tag and ends with the :HP2.&gml.eabsoluteaddress:eHP2.
:IH1 print='&gml.eabsoluteaddress tag'.eabsoluteaddress tag
:I1.eabsoluteaddress tag
tag.
The value section must be specified.
:FIG place=inline frame=box.
&gml.ABSOLUTEADDRESS
   &gml.value.
      %binary1($27)%text('a')
      %text(decimal(%x_address()))
      %text(',')%decimal(%y_address())
      %binary1($15)
   &gml.evalue.
&gml.eABSOLUTEADDRESS.
:FIGCAP.Example of the ABSOLUTEADDRESS Block
:eFIG.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies
the sequence to be output to set a new absolute address,
:INCLUDE file='rfdvalue'.
:H3.HLINE Block
:P.
The :HP2.hline:eHP2.
:I1.horizontal lines
:I1.rule lines
:I2 refid='ddrive'.hline block
block specifies the
mechanism for creating horizontal rule lines.
If this block is not specified, rule lines will be created with
characters.
:FIG place=inline frame=box.
&gml.HLINE
   thickness = number
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eHLINE.
:FIGCAP.The HLINE Block
:eFIG.
:P.
The hline block begins with the :HP2.&gml.hline:eHP2.
:IH1 print='&gml.hline tag'.hline tag
:I1 id='dahline'.hline tag
tag and ends with the :HP2.&gml.ehline:eHP2.
:IH1 print='&gml.ehline tag'.ehline tag
:I1.ehline tag
tag.
The thickness attribute and value section must be specified.
The special symbols :HP1.%x_size:eHP1. and :HP1.%thickness:eHP1.
are defined prior to processing the hline block.
The symbol %x_size is set to the width of the horizontal line, from
the left edge to the right edge.
The symbol %thickness is set to the value specified by the
hline blocks thickness attribute.
&WGML. positions to the bottom left corner of the line before
creating the rule line, and assumes the current point of the device
is set to the bottom right corner of the line when finished.
:FIG place=inline frame=box.
&gml.HLINE
   thickness=4
   &gml.value.
      %binary1(27)%text('x')
      %text(%decimal(%x_address()))%text(',')
      %text(%decimal(%y_address()))%text(',')
      %text(%decimal(%x_size()))%text(',')
      %text(%decimal(%thickness()))%text(',')%binary1(10)
   &gml.evalue.
&gml.eHLINE.
:FIGCAP.Example of the HLINE Block
:eFIG.
:H4.THICKNESS Attribute
:P.
The :HP2.thickness:eHP2. attribute specifies
the thickness of the horizontal line.
This value is in terms of the device horizontal base units.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies
the sequence to be output to create a horizontal rule line,
:INCLUDE file='rfdvalue'.
:H3.VLINE Block
:P.
The :HP2.vline:eHP2.
:I1.vertical lines
:I1.rule lines
:I2 refid='ddrive'.vline block
block specifies the
mechanism for creating vertical rule lines.
If this block is not specified, rule lines will be created with
characters.
:FIG place=inline frame=box.
&gml.VLINE
   thickness = number
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eVLINE.
:FIGCAP.The VLINE Block
:eFIG.
:P.
The vline block begins with the :HP2.&gml.vline:eHP2.
:IH1 print='&gml.vline tag'.vline tag
:I1 id='davline'.vline tag
tag and ends with the :HP2.&gml.evline:eHP2.
:IH1 print='&gml.evline tag'.evline tag
:I1.evline tag
tag.
The thickness attribute and value section must be specified.
The special symbols :HP1.%y_size:eHP1. and :HP1.%thickness:eHP1.
are defined prior to processing the vline block.
The symbol %y_size is set to the height of the vertical line,
from the top edge to the bottom edge.
The symbol %thickness is set to the value specified by the
vline blocks thickness attribute.
&WGML. positions to the bottom left corner of the line before
creating the rule line, and assumes the current point of the device
is set to the top left corner of the line when finished.
:FIG place=inline frame=box.
&gml.VLINE
   thickness=4
   &gml.value.
      %binary1(27)%text('y')
      %text(%decimal(%x_address()))%text(',')
      %text(%decimal(%y_address()))%text(',')
      %text(%decimal(%y_size()))%text(',')
      %text(%decimal(%thickness()))%text(',')%binary1(10)
   &gml.evalue.
&gml.eVLINE.
:FIGCAP.Example of the VLINE Block
:eFIG.
:H4.THICKNESS Attribute
:P.
The :HP2.thickness:eHP2. attribute specifies
the thickness of the vertical line.
This value is in terms of the device horizontal base units.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies
the sequence to be output to create a vertical rule line,
:INCLUDE file='rfdvalue'.
:H3.DBOX Block
:P.
The :HP2.dbox:eHP2.
:I1.framing
:I1.boxes
:I1.rule lines
:I2 refid='ddrive'.dbox block
block specifies the
mechanism for creating a box.
If this block is not specified, rule lines will be created with
the hline and vline block definitions.
:FIG place=inline frame=box.
&gml.DBOX
   thickness = number
   &gml.value.
      <device functions>
   &gml.evalue.
&gml.eDBOX.
:FIGCAP.The DBOX Block
:eFIG.
:P.
The dbox block begins with the :HP2.&gml.dbox:eHP2.
:IH1 print='&gml.dbox tag'.dbox tag
:I1 id='dadbox'.dbox tag
tag and ends with the :HP2.&gml.edbox:eHP2.
:IH1 print='&gml.edbox tag'.edbox tag
:I1.edbox tag
tag.
The thickness attribute and value section must be specified.
The special symbols :HP1.%x_size:eHP1.,
:HP1.%y_size:eHP1. and :HP1.%thickness:eHP1.
are defined prior to processing the dbox block.
The symbol %x_size is set to the width of the
horizontal component of the box,
from the left edge to the right edge.
The symbol %y_size is set to the height of the vertical component of the box,
from the top edge to the bottom edge.
The symbol %thickness is set to the value specified by the
dbox thickness attribute.
&WGML. positions to the bottom left corner of the box before
creating the box lines, and assumes the current point of the device
is set to the bottom right corner of the box when finished.
:FIG place=inline frame=box.
&gml.DBOX
   thickness=10
   &gml.value.
      %recordbreak()
      %text(%decimal(%divide(%thickness(),2)))
      %text(' ')
      %text(%decimal(%divide(%thickness(),2)))
      %text(' rmoveto')
      %recordbreak()
      %text('0 ')%text(%decimal(%y_size()))%text(' rlineto ')
      %recordbreak()
      %text(%decimal(%subtract(%x_size(),%thickness())))
      %text(' 0 rlineto ')
      %recordbreak()
      %text('0 -')%text(%decimal(%y_size()))%text(' rlineto ')
      %recordbreak()
      %text('-')%text(%decimal(%subtract(%x_size(),%thickness())))
      %text(' 0 rlineto')
      %recordbreak()
      %text('closepath ')
      %text(%decimal(%thickness()))
      %text(' setlinewidth stroke')
      %recordbreak()
      %text(%decimal(%add(%x_address(),%x_size())))
      %text(' ')%text(%decimal(%y_address()))
      %text(' moveto')
      %recordbreak()
   &gml.evalue.
&gml.eDBOX.
:FIGCAP.Example of the DBOX Block
:eFIG.
:H4.THICKNESS Attribute
:P.
The :HP2.thickness:eHP2. attribute specifies
the thickness of the box lines.
This value is in terms of the device horizontal base units.
:H4.VALUE Section
:P.
The :HP2.value:eHP2. section specifies
the sequence to be output to create a box,
:INCLUDE file='rfdvalue'.
