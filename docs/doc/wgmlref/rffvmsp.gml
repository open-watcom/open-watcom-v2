:H2.Files with &ibmvm.
:P.
This section describes the specification of a file
on the &ibmvm. system.
:H3.Record Attributes
:I1.record attributes
:I2 refid='filespe'.attribute
:P.
The attributes are specified inside a pair of parentheses
and must precede the file designation with no intervening spaces.
The attributes are all optional.
When specified, attributes must be separated by a colon.
Non-numeric attributes can be abbreviated by truncating
characters from the end to a minimum of one character.
:H4.Record Type
:I1.record type
:I3 refid='recsub'.record type
:P.
A file should be viewed as a number of :HP1.records:eHP1.,
where each record is a sequence of one or more characters.
&WGML. supports files with three different types of records.
:DL.
:DT.Text
:DD.A :HP1.text:eHP1.
:I1.text file
:IH1.record type
:I2.text file
    file consists of variable length records.
    Some of the possible character values cannot normally be
    entered at the keyboard.
    Text files are most commonly used for containing document
    source and other human-readable data.
    The record size of a text file specifies the size of the
    largest record which may be read from or
    written to that file.
:DT.Variable
:DD.A :HP1.variable:eHP1.
:I1.variable file
:IH1.record type
:I2.variable file
    file consists of variable length records.
    A variable file may contain any possible character.
    The record size of a variable file specifies the size of the
    largest record which may be read from or
    written to that file.
:DT.Fixed
:DD.A :HP1.fixed:eHP1.
:I1.fixed file
:IH1.record type
:I2.fixed file
    file consists of fixed length records.
    A fixed file may contain any possible character.
    The record size of a fixed file specifies the size of each
    record read from or written to that file.
:eDL.
:PC.
The default record type of a document source file read by &WGML.
is :HP1.text:eHP1..
The default record type of the &WGML. output file or device
is determined by the device definition being used.
:H4.Record Size
:I1.record size
:I3 refid='recsub'.record size
:P.
The record size attribute is a sequence of numeric digits which specifies
the record length for the file.
A record which is longer than the specified record size
will be truncated.
A record size of 132
is the default record size of a document source file read by &WGML..
The default record size of the &WGML. output file or device
is determined by the device definition being used.
:H3.File Designation
:I1.file designation
:I2 refid='filespe'.file designation
:P.
A file designation may be any valid filename recognized
by the &ibmvm. system.
In general, a file designation looks like:
:XMP.
filename filetype filemode
:eXMP.
:DL.
:DT.filename
:DD.The :HP1.filename:eHP1.
:I3 refid='filedes'.file name
    may contain up to eight characters, and
    is the main part of the file's name.
    If more than eight
    characters are used, only the first eight are meaningful.
    This is an important point.
    &ibmvm. does not check that
    the name is too long.
    If you specify more than eight characters
    then you may inadvertently destroy an existing file whose
    name happens to match the first eight characters.
:DT.filetype
:DD.The :HP1.filetype:eHP1.
:I3 refid='filedes'.file type
    is a one to eight character
    value which is a convenience in classifying files.
    The file type may only be used with disk file names.
    If it is specified, the space character separates
    the file type from the filename.
    The file type may be omitted from a
    disk file name, but the experienced user will probably always use it.
    The default file type is :HP2.file:eHP2. if it is not specified.
    The files of source text for &WGML. usually have :HP2.gml:eHP2.
    as the file type.
:DT.filemode
:DD.The :HP1.filemode:eHP1.
:I3 refid='filedes'.file mode
    is an optional two character
    value which specifies the CMS mini-disk containing the file.
    The first character is a letter identifying the mini-disk.
    The second character is a mode number for the file.
    The file mode may only be used with disk file names.
    If it is specified, the space character separates
    the file mode from the filetype.
    :P.
    If the file mode is not specified, then it will default
    to "*".
    For a new file, this will result in a file mode of "A1".
    For an existing file, "*" will match the first mini-disk
    that contains a file with the specified file name and type.
    You should consult the appropriate CMS documentation for
    more information on the file mode.
:eDL.
:P.
The components of a file designation are case insensitive.
Upper and lower case letters are treated identically.
For example, the following file designations are
equivalent and refer to the same file.
:XMP.
Book Gml
BOOK GML
book gml
:eXMP.
:P.
Each component of the file designation is separated
from the next by one or more space characters.
&WGML. also allows a period to separate the components
of the file designation.
The following file designations refer to the same file.
:XMP.
book gml a
book.gml a
book.gml.a
:eXMP.
:H3.Special File Names
:IH1.output devices
:I2.special names
:P.
Some file names have been reserved for devices.
These special names are:
:DL compact.
:DT.TERMINAL
:DD.the terminal screen
:DT.TERM
:DD.the terminal screen
:DT.READER
:DD.the VM virtual reader
:DT.RDR
:DD.the VM virtual reader
:DT.PRINTER
:DD.the VM virtual printer
:DT.PRT
:DD.the VM virtual printer
:DT.PUNCH
:DD.the VM virtual punch
:DT.PUN
:DD.the VM virtual punch
:eDL.
:PC.
When using one of these special file names, no other
part of the file specification may be specified.
:H3.File Specification Examples
:PC.
The following are some examples of a valid file specification.
:OL.
:LI.The following file specification refers to a file on the current
    mini-disk.
    :XMP.
DATA.FIL
    :eXMP.
:LI.The file specification below indicates that the file is to have
    fixed-length records of length 130.
    :XMP.
(F:130)EXAMPLE1.TST
    :eXMP.
:LI.The file specification below indicates that the file is to have
    variable-length records of maximum length 145, and resides
    on the "C" mini-disk.
    :XMP.
(V:145)NOVEMBER.RPT.C
    :eXMP.
:LI.The file specification below indicates that the file resides on the
    ""C" mini-disk.
    :XMP.
testables.dat.c
    :eXMP.
    :PC.
    Note that the trailing "s" in the file name will be ignored.
    Thus the following designation is equivalent.
    :XMP.
testable.dat.c
    :eXMP.
:LI.The file specification below refers to the terminal device.
    :XMP.
TERMINAL
    :eXMP.
:eOL.
