:H2.Options
:P.
The
:I1 id='dropt'.WATCOM GENDEV options
:IH1 seeid='dropt'.options
following options may be specified on the WATCOM GENDEV command line.
The options are illustrated with an example showing the format
for each of the computer systems supported by WATCOM GENDEV.
With each option, upper case letters are used to
indicate the minimum number of characters that must be specified.
Refer to
:HDREF refid='rungml'.
for more information about specifying command lines on your system.
:H3.ALTEXTension
:XMP.
&ibmvm. and &ibmpc.
    ALTEXT dev
&decvax.
    /ALTEXT=dev
:EXMP.
:PC.
When
:I2 refid='dropt'.altextension
a GENDEV source file is specified on the GENDEV command line,
or as an include file, the file type may be omitted.
A default file type will be supplied by WATCOM GENDEV.
If the source file cannot be found with the default file type,
the alternate extension option supplies a second file type to
find with the source file.
:H3.DELim
:XMP.
&ibmvm. and &ibmpc.
    DEL #
&decvax.
    /DEL=#
:EXMP.
:PC.
The
:I2 refid='dropt'.delim
value of the delimiter option is a single character.
The delimiter value is used in the definition
as the tag delimiter in place
of the colon character.
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
The INCLIST option
:I2 refid='dropt'.inclist
:I2 refid='dropt'.noinclist
displays on the terminal the name of each source file as
it is included.
The name of each include file is not displayed on the terminal
as it is included when the NOINCLIST option (the default) is specified.
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
:I2 refid='dropt'.warning
:I2 refid='dropt'.nowarning
WARNING option (the default) causes
WATCOM GENDEV warning messages about possible error situations
to be displayed on the screen.
Processing of the definition is not halted when a warning
message is displayed.
WATCOM
GENDEV warnings about possible error situations
are not displayed on the screen when the NOWARNING option is specified.
