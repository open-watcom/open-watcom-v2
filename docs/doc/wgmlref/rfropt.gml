:H2.Options
:P.
The
:IREF refid='gropt'.
:IH1 seeid='gropt'.options
following options may be specified on the &WGML. command line.
The options are illustrated with an example showing the format
for each of the computer systems supported by &WGML..
With each option, upper case letters are used to
indicate the minimum number of characters that must be specified.
:H3.ALTEXTension
:XMP.
&ibmvm. and &ibmpc.
    ALTEXT zgm
&decvax.
    /ALTEXT=zgm
:EXMP.
:PC.
When
:I2 refid='gropt'.altextension
:I1.altextension option
a GML source file is specified on the WGML command line,
or as an include file, the file type can be omitted.
If a source file with the default file type cannot be found,
&WGML. will search for a file with the file type
supplied by the alternate extension option.
:H3.Bind
:XMP.
&ibmvm. and &ibmpc.
    BIND odd-margin even_margin
&decvax.
    /BIND=(odd-margin,even-margin)
:EXMP.
:PC.
The
:I2 refid='gropt'.bind
:I1.bind option
two option values specify the default page margin values for
the odd and even pages.
If the value for even margin is not specified, the first value
applies to both odd and even pages.
The initial default value is zero.
:H3.CPInch
:XMP.
&ibmvm. and &ibmpc.
    CPI cpi-number
&decvax.
    /CPI=cpi-number
:EXMP.
:PC.
The
:I2 refid='gropt'.cpinch
:I1.cpinch option
characters per inch option specifies the base for determining how
much space in the output an integer value represents when used as
a horiztontal space unit.
The initial value is '10'.
:H3.DELim
:XMP.
&ibmvm. and &ibmpc.
    DEL #
&decvax.
    /DEL=#
:EXMP.
:PC.
The
:I2 refid='gropt'.delim
:I1.delim option
value of the delimiter option is a single character.
The delimiter value is used in the document as the GML tag delimiter in place
of the colon character.
:H3.DESCription
:XMP.
&ibmvm. and &ibmpc.
    DESC "Layout for Producing a Manual"
&decvax.
    /DESC="Layout for Producing a Manual"
:EXMP.
:PC.
The
:I2 refid='gropt'.description
:I1.description option
DESCRIPTION option specifies a comment in the option file.
:H3.DEVice
:XMP.
&ibmvm. and &ibmpc.
    DEV qume
&decvax.
    /DEV=qume
:EXMP.
:PC.
The
:I2 refid='gropt'.device
:I1.device option
DEVICE option must be specified.
It determines how the source document is processed to create
a formatted document
appropriate for the output device being used.
Any font definitions previously defined will be deleted.
:P.
When working on a PC/DOS system, the DOS environment symbol
:HP2.GMLLIB:eHP2. is used to locate the device information
(see :HDREF refid='pcldef'.).
If the device information is not found, the document include
path is searched (see :HDREF refid='incltag'.).
:H3.DUPlex/NODUPlex
:XMP.
&ibmvm. and &ibmpc.
    DUP
    NODUP
&decvax.
    /DUP
    /NODUP
:EXMP.
:PC.
The
:I2 refid='gropt'.duplex
:I1.duplex option
:I2 refid='gropt'.noduplex
:I1.noduplex option
duplex option sets the variable &amp.SYSDUPLEX. to the value "ON".
Noduplex will set the value to "OFF".
:H3 id='rfrfile'.FILE
:XMP.
&ibmvm. and &ibmpc.
    FILE doqume
&decvax.
    /FILE=doqume
:EXMP.
:PC.
The
:I2 refid='gropt'.file
:I1.file option
specified file is processed as a &WGML. command file.
:I1.command file
This file is composed of options normally specified
on the command line.
Command files are most useful when many options must be specified,
since they provide a way to specify these options without entering
them individually each time &WGML. is run.
Command line records in a command file may
include other command file invocations.
The default file type for a command file depends on the specific
computer system on which &WGML. is being run.
:P.
When working on a PC/DOS system, the DOS environment symbol
:HP2.GMLLIB:eHP2. is used to locate the command file if it
is not in the current directory
(see :HDREF refid='pcldef'.).
If it is still not found, the document include
path is searched (see :HDREF refid='incltag'.).
:H3.FONT
:XMP.
&ibmvm. and &ibmpc.
    FONT font-number font-name font-attribute font-space font-height
&decvax.
    /FONT=(font-number,font-name,font-attribute,font-space,font-height)
:EXMP.
:PC.
The
:I2 refid='gropt'.font
:I1.font option
specified font-number is assigned a particular font.
The font numbers zero through three correspond to the
highlight-phrase tags :HP2.&gml.hp0:eHP2. through :HP2.&gml.hp3:eHP2..
:I2 refid='ethp0'.hp0
:I2 refid='ethp1'.hp1
:I2 refid='ethp2'.hp2
:I2 refid='ethp3'.hp3
Font
:I2 refid='etsf'.sf
numbers greater than three (up to a maximum of 255)
may be used in the layout section
or with the :HP2.&gml.sf:eHP2. tag.
:P.
Each device has a list of available fonts defined with it.
The font-name value is selected from these defined fonts, and must
be specified.
:P.
The font-attribute value specifies an attribute for the defined font.
If the font attribute is not specified, the attribute PLAIN is set.
The possible values for the font attribute are:
:IH2.font
:I3.attributes
:DL compact.
:DT.BOLD
:DD.The defined font is bolded.
:DT.PLAIN
:DD.The font is used as defined.
    This attribute is the default.
:DT.ULBOLD
:DD.The defined font is bolded and underlined.
    Spaces are also underlined.
:DT.ULINE
:DD.The defined font is underlined.
    Spaces are also underlined.
:DT.USBOLD
:DD.The defined font is bolded and underscored.
    Spaces are not underscored.
:DT.USCORE
:DD.The defined font is underscored.
    Spaces are not underscored.
:eDL.
:P.
When a font is selected for output, the total line height for the
font has two components.
The first component is the height of the characters in the font,
and is fixed by the design of the character set.
The second component is a value to create space between lines of a
font.
Although the line spacing has an optimal value for each font height,
it can be modified to adjust the overall look of the document.
Both of the line height components are specified as point values
(there are 72 points in one inch),
with a decimal portion in hundredths of a point (ie. 10.25).
:P.
The font-space attribute is optional, and overrides the default
space value defined for the font.
This attribute may be specified for any type of character font.
:P.
The font-height attribute is specified with scaled fonts.
Scaled fonts have no predetermined character heights,
and must be defined with a height before they can be used.
The font-space attribute must also be specified, but can be
a null('') value to set the default font spacing.
:H3 id='runform'.FORMat
:XMP.
&ibmvm. and &ibmpc.
    FORM format-type
&decvax.
    /FORM=format-type
:EXMP.
:PC.
The
:I2 refid='gropt'.format
:I1.format option
FORMAT option specifies the type of GML document to be processed.
The format types have different sets of GML tags
and, to some extent, a different syntax.
The format types available are:
:DL compact.
:DT.STANDARD
:DD.This format provides the standard set of GML tags
    and is the default.
:DT.LETTER
:DD.The letter format is designed for processing letters.
    Refer to :HDREF refid='lettag'. for more information.
:eDL.
:H3 id='rfrfroo'.FROM
:XMP.
&ibmvm. and &ibmpc.
    FROM page-number
&decvax.
    /FROM=page-number
:EXMP.
:PC.
The
:I2 refid='gropt'.from
:I1.from option
FROM option
will cause &WGML. to print the document starting
at the specified page number within the body of the document.
The number is specified as an integer, and does not depend on
the format of the numbers on the output pages
(or even printed on the page).
For example, if the page numbers are formatted as roman numerals,
the page number you specify would be "4", not "iv".
See :HDREF refid='rfrtoo'. for more information.
:H3.INCList/NOINCList
:XMP.
&ibmvm. and &ibmpc.
    INCL
    NOINCL
&decvax.
    /INCL
    /NOINCL
:EXMP.
:PC.
The
:I2 refid='gropt'.inclist
:I2 refid='gropt'.noinclist
:I1.inclist option
:I1.noinclist option
INCLIST option
causes &WGML. to display on the
terminal the name of each source file as
it is included in the document.
When the NOINCLIST option is specified,
the include file names are not displayed on the terminal
as they are included.
NOINCLIST is the default when in line mode (see :HDREF refid=rfrlino.).
:H3 id='runindx'.INDex/NOINDex
:XMP.
&ibmvm. and &ibmpc.
    IND
    NOIND
&decvax.
    /IND
    /NOIND
:EXMP.
:PC.
When the
:I2 refid='gropt'.index
:I2 refid='gropt'.noindex
:I1.index option
:I1.noindex option
INDEX option is specified, the indexing
tags in the document are processed.
The :HP2.&gml.index:eHP2. tag must be specified in the back material
:I2 refid='etindex'.index
of the document to produce the index in the output document.
The NOINDEX option (the default) will cause the
indexing tags in the document to be ignored, creating an empty index.
:H3 id='rfrlayo'.LAYout
:XMP.
&ibmvm. and &ibmpc.
    LAY file-name
&decvax.
    /LAY=file-name
:EXMP.
:PC.
A
:I2 refid='gropt' pg=major.layout
:I1.layout option
layout file is included at the beginning of the document.
This option has the same effect
as having an :HP2.&gml.include:eHP2. tag as the
:I2 refid='etinclu'.include
first GML source line.
:H3 id='rfrlino'.LINEmode
:XMP.
&ibmvm. and &ibmpc.
    LINE
&decvax.
    /LINE
:EXMP.
:PC.
The
:I2 refid='gropt'.linemode
:I1.linemode option
presentation of information about the current status of &WGML.
is displayed on the terminal as separate output lines during the
processing of a document.
With the &ibmpc. system, the full area of the terminal screen is
used to display the information.
The LINEMODE option forces the display of information from full screen
mode to line mode.
The options INCLIST, VERBOSE, and STATISTICS are defaulted when
in full screen mode.
The option WARNING is defaulted in both modes.
:H3.LLength
:XMP.
&ibmvm. and &ibmpc.
    LL ll-number
&decvax.
    /LL=ll-number
:EXMP.
:PC.
The
:I2 refid='gropt'.llength
:I1.llength option
line length option specifies the initial value for the line length
used in the document.
:H3.LPInch
:XMP.
&ibmvm. and &ibmpc.
    LPI lpi-number
&decvax.
    /LPI=lpi-number
:EXMP.
:PC.
The
:I2 refid='gropt'.cpinch
:I1.cpinch option
lines per inch option specifies the base for determining how
much space in the output an integer value represents when used as
a vertical space unit.
The initial value is '6'.
:H3 id='mailmrg'.MAILmerge
:XMP.
&ibmvm. and &ibmpc.
    MAIL file-name
&decvax.
    /MAIL=file-name
:EXMP.
:PC.
The
:I2 refid='gropt'.mailmerge
:I1.mailmerge option
MAILMERGE option specifies a
file containing symbol substitution values or the name of a WATFILE database.
This option may be used to create a number of similar documents, such
as a form letter mailing.
The &WGML. processor will inspect the file to determine the file type
(WATFILE or symbol values).
The following two subsections describe the processing of these files.
:H4.WATFILE Database File
:P.
Each field value in a WATFILE input record is assigned to a symbol
name created from the WATFILE field name.
If the field name contains characters which are invalid in GML symbol
names, the characters up to the invalid character are used.
:P.
If the WATFILE file contains the following data:
:XMP.
define name                 = 10   L
define addr1                =  6   L
define addr2                =  5   L
bye
John Doe  StreetCity
:eXMP.
:PC.then the three values could be used in the following way:
:XMP.
&gml.ADDRESS.
&gml.ALINE.&amp.name.
&gml.ALINE.&amp.addr1.
&gml.ALINE.&amp.addr2.
&gml.eADDRESS.
:eXMP.
:H4.Values File
:P.
Each record in the values file must contain
the same number of symbol values.
The document will be produced once for
each record in the file.
If a layout is specified, it is only processed the first
time the document is processed.
:P.
Symbol values are separated by commas.
Each value may be enclosed in
single (') or double (") quotes.
The quotation
marks surrounding the text are not
part of the symbol value. If a
quotation mark of the same type
used to delimit the symbol value is to
be part of the symbol text, it can
be entered by specifying the quote
character twice. Only one quote
character will appear in the resulting
symbol value. A symbol value must be quoted
if it contains a comma.
:P.
The blanks outside quotations, or if the value is not quoted, the
blanks before the first and after
the last nonblank character, are not
considered part of the symbol value.
For example, <,,>, <,\b\b\b,>,
<,end_of_record>, or empty records
all specify empty symbol values.
:P.
Each symbol value in an input record is assigned to a special symbol
name.
The values are assigned to VALUE1, VALUE2, etc.
For example, if the values file contains the following,
:XMP.
"John Doe","13 Country Lane","Canada"
:eXMP.
:PC.then the three values could be used in the following way:
:XMP.
&gml.ADDRESS.
&gml.ALINE.&amp.value1.
&gml.ALINE.&amp.value2.
&gml.ALINE.&amp.value3.
&gml.eADDRESS.
:eXMP.
:H3.OUTput
:XMP.
&ibmvm. and &ibmpc.
    OUT temp1
&decvax.
    /OUT=temp1
:EXMP.
:PC.
The
:I2 refid='gropt'.output
:I1.output option
output will go to the specified file name instead of the
default output file.
The default output file name is determined by the device selected
on the command line.
In some cases the output from &WGML. is sent directly
to the device, while in other cases the output is sent
to a disk file.
:P.
If the file name component of the output name is an asterisk,
the name of the document is used.
For example, an output file specification of &mono.*.ps&emono.
when the document name is &mono.manual.gml&emono. will produce
&mono.manual.ps&emono. as the output file.
Refer to :HDREF refid='devout'. for more information.
:H3 id='runpass'.PASSes
:XMP.
&ibmvm. and &ibmpc.
    PASS 2
&decvax.
    /PASS=2
:EXMP.
:PC.
In
:I2 refid='gropt'.passes
:I1.passes option
some cases &WGML.
must process a document more than once to properly
produce the output document.
The value of the passes option is
the number of times &WGML. must process the document.
&WGML. will issue a warning message if more passes are necessary.
The default passes value is one.
:H3.PAUSE/NOPause
:XMP.
&ibmvm. and &ibmpc.
    PAUSE
    NOP
&decvax.
    /PAUSE
    /NOP
:EXMP.
:PC.
When
:I2 refid='gropt'.pause
:I1.pause option
:I2 refid='gropt'.nopause
:I1.nopause option
some of the output devices are selected, information
messages are displayed and a response from the keyboard is
requested.
An example of this is the terminal device which pauses at the
bottom of the screen to prevent the output from being scrolled
off the screen.
The NOPAUSE option suppresses the display of information and
requests for keyboard input.
PAUSE is the default option.
:H3.PROCess
:XMP.
&ibmvm. and &ibmpc.
    PROC x2700
&decvax.
    /PROC=x2700
:EXMP.
:PC.
The
:I2 refid='gropt'.process
:I1.process option
specified name is an alternate condition for the :HP2.&gml.psc:eHP2. tag.
:I2 refid='etpsc'.psc
:H3.QUIET/NOQuiet
:XMP.
&ibmvm. and &ibmpc.
    QUIET
    NOQ
&decvax.
    /QUIET
    /NOQ
:EXMP.
:PC.
The
:I2 refid='gropt'.quiet
:I1.quiet option
:I2 refid='gropt'.noquiet
:I1.noquiet option
quiet option sets the variable &amp.SYSQUIET. to the value "ON".
Noquiet will set the value to "OFF".
:H3.RESETscreen
:XMP.
&ibmvm. and &ibmpc.
    RESET
&decvax.
    /RESET
:EXMP.
:PC.
The
:I2 refid='gropt'.resetscreen
:I1.resetscreen option
RESETSCREEN option clears the screen before document processing
begins and queries the user when the formatting is complete.
Active only when the screen is used in line mode, this option
is intended for use when &WGML. is invoked from an application program.
:H3.SCRipt/NOSCRipt
:XMP.
&ibmvm. and &ibmpc.
    SCR
    NOSCR
&decvax.
    /SCR
    /NOSCR
:EXMP.
:PC.
The
:I2 refid='gropt'.script
:I1.script option
:I2 refid='gropt'.noscript
:I1.noscript option
SCRIPT option enables recognition of Script control words and the
line separator character.
The default option NOSCRIPT will cause these values to be treated as text.
:H3.SETsymbol
:XMP.
&ibmvm. and &ibmpc.
    SET processor WGML
&decvax.
    /SET=(processor,WGML)
:EXMP.
:PC.
The
:I2 refid='gropt'.setsymbol
:I1.setsymbol option
SETSYMBOL option requires two values.
The first value is the name of the symbol to be set.
The second value is the character string that is to be
assigned to the specified symbol name.
This option is equivalent to using the :HP2.&gml.SET:eHP2.
tag at the beginning of the source document.
Refer to :HDREF refid='symsub'. and :HDREF refid='settag'..
:H3.STATistics/NOSTATistics
:XMP.
&ibmvm. and &ibmpc.
    STAT
    NOSTAT
&decvax.
    /STAT
    /NOSTAT
:EXMP.
:PC.
Statistics
:I2 refid='gropt'.statistics
:I1.statistics option
about the document are displayed
after document processing is completed when in line mode,
and during the document processing when in full screen mode.
Examples of the type of information displayed are the number of input
lines processed, the number of include files, and the number
of pages produced.
NOSTATISTICS is the default when in line mode (see :HDREF refid=rfrlino.).
:H3.TERSE/VERBose
:XMP.
&ibmvm. and &ibmpc.
    TERSE
    VERB
&decvax.
    /TERSE
    /VERB
:EXMP.
:PC.
Headings
:I2 refid='gropt'.terse
:I2 refid='gropt'.verbose
:I1.terse option
:I1.verbose option
are not displayed on the terminal as the document is
processed when the TERSE option is specified.
Headings
are displayed on the terminal as the document is
processed when the VERBOSE option is specified.
TERSE is the default when in line mode (see :HDREF refid=rfrlino.).
:H3 id='rfrtoo'.TO
:XMP.
&ibmvm. and &ibmpc.
    TO page-number
&decvax.
    /TO=page-number
:EXMP.
:PC.
The
:I2 refid='gropt'.to
:I1.to option
TO option
will direct &WGML. to stop printing the document
at the specified page number within the body of the document.
The number is specified as an integer, and does not depend on
the format the numbers on the output pages (or whether or not they
are even printed on the page).
For example, if the page numbers are formatted as roman numerals,
the page number you specify would be "6", not "vi".
See :HDREF refid=rfrfroo. for more information.
:H3.VALUESet
:XMP.
&ibmvm. and &ibmpc.
    VALUES file-name
&decvax.
    /VALUES=file-name
:EXMP.
:PC.
The
:I2 refid='gropt'.valueset
:I1.valueset option
VALUESET option is an equivalent name for
the MAILMERGE option.
See :HDREF refid='mailmrg'. for more information.
:H3.WAIT/NOWAIT
:XMP.
&ibmvm. and &ibmpc.
    WAIT
    NOWAIT
&decvax.
    /WAIT
    /NOWAIT
:EXMP.
:PC.
Certain
:I2 refid='gropt'.wait
:I1.wait option
:I2 refid='gropt'.nowait
:I1.nowait option
errors (such as device not ready) will result in a query from
&WGML. about continuing with the document processing.
&WGML. also waits after processing the document
when in not line mode (see :HDREF refid='rfrlino'.).
The default option WAIT enables these queries.
The option NOWAIT will suppress the query.
:H3.WARNing/NOWARNing
:XMP.
&ibmvm. and &ibmpc.
    WARN
    NOWARN
&decvax.
    /WARN
    /NOWARN
:EXMP.
:PC.
The
:I2 refid='gropt'.warning
:I2 refid='gropt'.nowarning
:I1.warning option
:I1.nowarning option
WARNING option(the default) causes
GML warning messages about possible error conditions
to be displayed on the screen.
Processing of the document is not halted when a warning
message is displayed.
&WGML. warnings about possible error situations and information messages
are not displayed on the screen when the NOWARNING option is specified.
:H3.WSCRipt
:XMP.
&ibmvm. and &ibmpc.
    WSCR
&decvax.
    /WSCR
:EXMP.
:PC.
The
:I2 refid='gropt'.wscript
:I1.wscript option
WSCRIPT option enables recognition of Script control words and the
line separator character.
In addition, it enables several WATCOM extensions over Waterloo Script.
The extensions are:
:OL.
:LI.Lines of input which
:UL.
:LI.are processed when concatenate is OFF
:LI.start with blank space
:LI.the blank space is followed by a GML tag that is not a continuation
:eUL.
have the blank space at the beginning of the line ignored.
:LI.When .CO OFF is set, lines which exceed the line length are
split into two lines.
:LI.Extra blanks between words are suppressed in concatenate mode.
:LI.Full and partial stops are recognized anywhere in the input line
if followed by a space.
:LI.If a macro for .LB, .LT, .NL, or .BL is not defined, a break is
not implicitly performed.
:eOL.
