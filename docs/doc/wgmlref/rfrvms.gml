:H2.Command Lines with &decvax.
:P.
The format of the &WGML. command line conforms to the
:I2 refid='grcmdl'.&decvax.
standard &decvax format.
Documentation available with the system gives a more detailed
explanation of this format.
:P.
Each option on the WGML command line starts with a slash(/)
character.
If the option has an associated value, the option and its value
are separated by an equal sign.
For example,
    :XMP.
WGML book/DEVICE=qume
    :eXMP.
:PC.
Some characters, such as slashes and single quotes('),
have special meanings for the Digital Command Language(DCL) processor.
If the option value contains a special character,
it should be enclosed in double quotes(").
For example,
    :XMP.
WGML book/DEVICE=qume/DELIM="!"
    :eXMP.
:P.
If the options will not fit on one line, they may be
continued to a new input line by entering a
hyphen(-) at the end of the line.
More options may then be entered on a new line.
For example,
    :XMP.
WGML book/DEVICE=qume -
/DELIM="!"
    :eXMP.
:P.
Options such as the font option require more than one value
for each font being specified.
These options accept a list of values.
The value list starts with a left parenthesis and ends with
a right parenthesis.
Each value in the list is separated by a comma.
An option value list may be continued to a new input line,
as in the following example&gml.
    :XMP.
WGML book/DEVICE=qume/DELIM="!"/FONT=(0,mono10,-
plain,"","",1,mono12,bold,"","")
    :eXMP.
:PC.
There are five values for each font specified.
The example above overrides two of the default fonts.
Both of the fonts are specified in the same list.
:H3.Command Files
:P.
If a number of options must be used to process a GML document,
:I2 refid='grcmdf'.&decvax.
they can be placed in a command file to reduce the
amount of typing each time you process the document.
Each line in the command file is entered as if specified at the terminal.
The default file type for a &WGML. command file
is :HP2.opt:eHP2..
(For more information, see :HDREF refid='rfrfile'.).
    :XMP.
/FONT=(0,mono10,plain,-
"","",1,mono12,bold,"","")
    :eXMP.
:PC.
If the lines in the above example are in the file
"setfont.opt", the following example shows
how the command file is referenced.
    :XMP.
WGML book/DELIM="!"/FILE=setfont -
/DEVICE=qume
    :eXMP.
:PC.
The options in the file "setfont.opt" are processed with
those options specified on the command line.
Note that with the exception of the last line,
all lines in the command file must be continued
by ending them with a hyphen.
:H3.&decvax. Specifics
:DL compact.
:DTHD.File Type
:DDHD.Usage
:DT.GML
:DD.document source files
:DT.LAY
:DD.layout files created with the :HP2.&gml.save:eHP2. tag
:DT.OPT
:DD.command files
:DT.VAL
:DD.value files specified by the VALUESET command line option
:eDL.
