:H2.Command Lines with &ibmvm. and &ibmpc.
:P.
The options are separated from the file name by a
:I2 refid='grcmdl'.&ibmvm.
:I2 refid='grcmdl'.&ibmpc.
left parenthesis.
Options and their values are separated from each other by a
space character.
For example,
    :XMP.
WGML book ( DEVICE qume
    :eXMP.
:PC.
If an option value contains a space,
it should be enclosed in double(") or single(') quotes.
For example,
    :XMP.
WGML book ( DEVICE "qume device" DELIM !
    :eXMP.
:P.
If the options will not fit on one line, they may be
continued to a new input line by omitting the source file name.
&WGML. will request command line input until a file name is
specified.
The left parenthesis must precede the options on
each of the entered command lines to differentiate them
from a file name.
    :XMP.
WGML ( DEVICE qume
book ( DELIM !
    :eXMP.
:P.
Options such as the font option require more than one value
for each font being specified.
These options accept a list of values.
Each value in the list is separated by a space.
An option value list may be continued to a new input line.
    :XMP.
WGML ( DEVICE qume DELIM ! FONT 0 mono10
book ( bold FONT 1 mono12 bold
    :eXMP.
:PC.
The above example overrides two of the default fonts.
:H3.Command Files
:P.
If a number of options must be used to process a GML document,
:I2 refid='grcmdf'.&ibmvm.
:I2 refid='grcmdf'.&ibmpc.
they can be placed in a command file to reduce the
amount of typing each time you process the document.
Each line in the command file is entered as if specified at the terminal.
The default file type for a &WGML. command file
is :HP2.gmlcmd:eHP2. with &ibmvm., and
:HP2.opt:eHP2. with &ibmpc..
(For more information, see :HDREF refid='rfrfile'.).
    :XMP.
( FONT 0 mono10 plain
( FONT 1 mono12 bold
    :eXMP.
:PC.
If the lines in the above example are in a file
with the name "setfont" (with a file type of gmlcmd or opt),
the following example shows
how the command file is referenced.
    :XMP.
WGML ( DEVICE qume FILE setfont
book ( DELIM !
    :eXMP.
:PC.
The options in the file "setfont" are processed with
those options specified on the command line.
Note that each line in the command file begins with a left
parenthesis.
This allows the specification of a file name on the &WGML. command line.
:H3.&ibmvm. Specifics
:P.
The device library must be specified as a global maclib
(see :HDREF refid='vmlib'.).
The following default file types are use by &WGML.:
:DL compact.
:DTHD.File Type
:DDHD.Usage
:DT.GML
:DD.document source files
:DT.LAYOUT
:DD.layout files created with the :HP2.&gml.save:eHP2. tag
:DT.GMLCMD
:DD.command files
:DT.VALUES
:DD.value files specified by the VALUESET command line option
:eDL.
:H3.&ibmpc. Specifics
:P.
The DOS environment symbol :HP2.GMLLIB:eHP2. must be defined for
locating device information used by the WGML and WGMLUI programs.
(see :HDREF refid='pcldef'.).
:P.
When &WGML. is processing a document, some internal data
may be store on disk.
Setting the DOS symbol :HP2.GMLPAG:eHP2. with a path list directs
the disk locations used to store this information.
:XMP.
SET GMLPAG=E:\;C:\
:eXMP.
:P.
The set in the previous example directs &WGML. to use the
&mono.E:\&emono. disk and root directory.
When the disk is full, it will start using the &mono.C:\&emono. disk
location.
If the &mono.E:&emono. disk drive is a RAM disk, &WGML. will process
the document in less time.
:P.
The WGMLUI program will accept two parameters.
Both parameters may be quoted and empty.
The first parameter is the name of the document file.
The second parameter is the name of the an options file.
:P.
The WGMLUI program will locate and invoke the &mono.wedit.exe&emono.
program when an edit request is made.
If the DOS symbol :HP2.EDITNAME:eHP2. is defined, WGMLUI will use
the symbol value as the program name to invoke.
:P.
The following default file types are used by &WGML.:
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
