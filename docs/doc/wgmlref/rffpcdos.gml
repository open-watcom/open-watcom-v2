:H2.Files with &ibmpc.
:P.
This section describes the specification of a file
on an &ibmpc. system.
The attributes of a file are not stored with
the file on the &ibmpc. system.
A file created with a particular set of attributes
must therefore have those attributes specified when that file
is later referenced.
It should also be noted that there is an upper limit on the number
of open files.
The default system limit can be increased with the FILES command
in the CONFIG.SYS file.
See your DOS manual for more information.
:H3.Record Attributes
:I1.record attributes
:I2 id='recsub' refid='filespe'.attribute
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
where each record is a sequence of zero or more characters.
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
    Two special characters are used to signify the end of a record.
    The CR (carriage return) and LF (line feed) characters
    separate records in a text file.
    These characters are automatically added to the end of each
    record, and should not be accounted for when determining
    the appropriate record size for the file.
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
    A 16 bit number at the beginning of the record specifies the
    length of each record.
    This number is automatically added to the beginning of each
    record, and should not be accounted for when determining
    the appropriate record size for the file.
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
:I2 id='filedes' refid='filespe'.file designation
:P.
A file designation may be any valid filename recognized
by the &ibmpc. system.
In general, a file designation looks like:
:XMP.
drive:\\path\\filename.ext
:eXMP.
:DL.
:DT.drive:
:I3 refid='filedes'.drive name
:DD.If the :HP1.drive name:eHP1. is omitted, the default
    drive is assumed.
    Examples of drive names are:
    :DL compact.
    :DT.A:
    :DD.disk drive A
    :DT.B:
    :DD.disk drive B
    :DT.C:
    :DD.disk drive C
    :eDL.
:DT.\\path\\
:I3 refid='filedes'.path
:DD.If the :HP1.path:eHP1. specification is omitted,
    the current directory is used.
    The path may be used to refer to files that are stored
    in sub-directories of the disk.
    Some examples of path specifications are:
    :XMP.
 \\top\\
 \\gml\\data\\
 ..\\tests\\
    :eXMP.
    :PC.
    Your &ibmpc. manual can tell you more about directories:
    how to create them; how to store files in them; how
    to specify a path; etc.
:DT.filename
:I3 refid='filedes'.file name
:DD.The filename may contain up to eight characters, and
    is the main part of the file's name.
    If more than eight
    characters are used, only the first eight are meaningful.
    This is an important point.
    &ibmpc. does not check that
    the name is too long.
    If you specify more than eight characters
    then you may inadvertently destroy an existing file whose
    name happens to match the first eight characters.
:DT..ext
:I3 refid='filedes'.file extension
:DD.The file :HP1.extension:eHP1. is an optional one to three character
    value which is a convenience in classifying files.
    The extension may only be used with disk file names.
    If it is specified, the period character separates
    the extension from the filename.
    The experienced user will specify the file extension to
    identify the type of information stored in the file.
    The files of source text for &WGML. usually have :HP2.gml:eHP2.
    as the file extension.
:eDL.
:H3.Special Device Names
:IH1.output devices
:I2.special names
:P.
&ibmpc. has reserved some names for devices.
These special device names are:
:DL compact.
:DT.CON
:DD.the console (or terminal)
:DT.AUX
:DD.the serial port
:DT.COM1
:DD.serial port 1
:DT.COM2
:DD.serial port 2
:DT.PRN
:DD.the parallel printer
:DT.LPT1
:DD.the first parallel printer
:DT.LPT2
:DD.the second parallel printer
:DT.LPT3
:DD.the third parallel printer
:DT.NUL
:DD.nonexistent device
:eDL.
:PC.
When using one of these special device names, no other
part of the file specification may be specified.
Earlier versions of DOS allowed a trailing ":" to be specified
after a special device name.
Starting with DOS 2.0, a trailing ":" may not be specified.
For example, "CON" is acceptable but "CON:" is not.
:H3.File Specification Examples
:PC.
The following are some examples of a valid file specification.
:OL.
:LI.The following file specification refers to a file in the current
    directory of the default disk.
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
    on the "C" disk.
    :XMP.
(V:145)C:NOVEMBER.RPT
    :eXMP.
:LI.The file specification below indicates that the file resides in the
    "RECORDS" directory of the "B" disk.
    :XMP.
b:\\records\\bigmanual.gml
    :eXMP.
    :PC.
    Note that the trailing "l" in the file name will be ignored.
    Thus the following designation is equivalent.
    :XMP.
b:\\records\\bigmanua.gml
    :eXMP.
:LI.The file specification below refers to a second parallel printer.
    :XMP.
LPT2
    :eXMP.
:eOL.
