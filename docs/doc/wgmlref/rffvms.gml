:H2.Files with &decvax.
:P.
This section describes the specification of a file
on the &decvax. system.
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
:H4.File Type
:I1.file type
:I3 refid='recsub'.file type
:DL.
:DT.Carriage
:DD.The :HP1.carriage:eHP1.
:I1.carriage control file
:IH1.file type
:I2.carriage control
    attribute specifies the
    file as having ASA (American Standards Association)
    carriage control characters.
    A control character begins each record and is used for
    vertical spacing control.
    The VAX VMS system saves the carriage control
    attribute with the file.
    The attribute is later used when printing the file
    to indicate to the system that the carriage control characters
    should be used for vertical spacing.
    The valid characters and their interpretation are:
    :DL compact.
    :DT.'1'
    :DD.Advance to the top of the next page.
    :DT.'+'
    :DD.Advance zero lines (overprint).
    :DT.' '
    :DD.Advance one line.
    :DT.'0'
    :DD.Advance two lines.
    :DT.'-'
    :DD.Advance three lines.
        This carriage control character is not supported by some
        printers.
        It should only be used when the printer is known
        to support ASA triple spacing.
    :eDL.
:eDL.
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
by the VMS Record Management Services (RMS).
In general, a file designation looks like:
:XMP.
node::device:\(directory\)filename.type;version
:eXMP.
:DL.
:DT.node::
:DD.The :HP1.node:::eHP1.
:I3 refid='filedes'.node name
    is the DECnet node name
    (This feature may not be installed on
    your VAX/VMS system).
:DT.device:
:DD.The :HP1.device::eHP1. is the
    :HP1.device:eHP1. name.
:I3 refid='filedes'.device name
    The device name is optional and defaults to the
    disk containing your current directory.
    The device name is followed by a ":".
    Usually, the device name is not present because most files are
    located on the default disk. Some of the device names on the
    VAX/VMS system are:
    :DL compact.
    :DT.DRA1:
    :DD.A disk drive.
    :DT.MSA0:
    :DD.A magtape drive.
    :DT.TT:
    :DD.The screen/keyboard.
    :DT.LPA0:
    :DD.The printer.
    :eDL.
    :PC.
    Devices may have logical names.
    It is usually more convenient to
    refer to a disk by its logical name.
    Some examples of logical names are:
    :SL compact.
    :LI.SYS$SYSDEVICE:
    :LI.SYS$SYSROOT:
    :LI.DISK$CUSTOMER:
    :eSL.
    :PC.
    The logical name can be 1 to 63 characters (letters A-Z, $,
    and/or numbers 0-9 only) in length.
:DT.\(directory\)
:DD.The file is located in the specified "directory".
:I3 refid='filedes'.directory
    The default is the current directory; consequently it is often
    not specified.
    Directory names have up to nine characters.
    If a file is contained in a subdirectory of another directory,
    the subdirectory name is specified following the directory name.
    These names are separated from each other by a period.
    A minus sign at the start of a directory specification indicates
    the parent directory of the current directory.
    The directory name, including any subdirectories, is enclosed by
    square brackets (\(\)).
    Some examples of directory names are:
    :XMP.
\(USERFILES\)
\(EXAMPLE.PROGRAMS\)
\(-.reports\)
:eXMP.
:DT.filename
:DD.The :HP1.filename:eHP1.
:I3 refid='filedes'.file name
    is the main part of the file's name.
    File names have one to thirty-nine
    characters (letters A-Z and/or numbers 0-9 only).
    Some examples of file names are:
    :XMP.
TEST1
setup
MSGBOARD
    :eXMP.
:DT..type
:DD.The :HP1.type:eHP1.
:I3 refid='filedes'.file type
    is an optional file type
    consisting of one to thirty-nine
    characters (letters A-Z and/or numbers 0-9 only).
    The extension may only be used with disk file names.
    If it is specified, the period(.) character separates
    the extension from the filename.
    The file extension may be omitted from a
    disk file name, but the experienced user will probably always use it.
    The files of source text for &WGML. usually have :HP2.gml:eHP2.
    as the file extension.
:DT.;version
:DD.The :HP1.version:eHP1.
:I3 refid='filedes'.file version
    is the version number of the file.
    Any time you create a new file
    that has the same name as a file already in the directory, the
    system automatically selects a version number that is one greater
    than the last version number.
    If you have more than one file with
    the same name, you can explicitly identify which file you want by
    its version number.
    The latest version of a file is used if the version number is
    not specified.
    The version number is preceded by a semicolon(;).
    Some examples
    of file designations with their version numbers are:
    :XMP.
SAMPLE.DAT;1
SAMPLE.DAT;2
REPORT.GML;15
TRIAL.;4
\(LIBRARY\)WEDIT.EXE;1
:eXMP.
:eDL.
:P.
Most of the fields are optional.
For example, a file under the
current default directory can be referenced without specifying a
node name, device name, or directory, and often without a version
number.
Devices such as a printer can be referenced without
specifying any fields other than the device name.
:P.
In general, the different components of a file designation are
made up of the letters A through Z and the digits 0 through 9.
Component specifications are case insensitive.
Upper and lower case letters are treated identically.
For example, the file designations
:XMP.
Sample.Dat
SAMPLE.DAT
sample.dat
:eXMP.
:PC.
are equivalent.
:H3.Writing to the Printer
:P.
Output can be written to a printer by using the printer device
name as a file name.
A better way of doing this (from the point
of view of document portability) is to define the name "printer"
using the VMS DEFINE command prior to invoking the software, and
then to use "printer" as a device name.
The device name "printer"
is used by the software on most computer systems to designate the
system printer.
The required DEFINE could appear in a "LOGIN"
command file, and would be similar to:
:XMP.
DEFINE PRINTER LPA0
:eXMP.
:H3.Using the Terminal as a File
:P.
The terminal can be used for input or output by opening a file
whose name is the device name of the terminal.
One alternative is
to use the logical names SYS$INPUT and SYS$OUTPUT to refer to the
terminal.
Another possibility is to define a logical name
"terminal" using the VMS DEFINE command prior to invoking the
software.
Use of the device name "terminal" provides a degree of
portability, since this name is used by the software on most
computer systems to designate the terminal.
The required DEFINE
command could appear in a "LOGIN" command file, and would be
similar to:
:XMP.
DEFINE TERMINAL TT
:eXMP.
:H3.File Specification Examples
:P.
The following are some examples of a valid file specification.
:OL.
:LI.The following file specification refers to the file "data.gml" in
    the current default directory:
    :XMP.
data.gml
    :eXMP.
:LI.If the current default directory is \(USERFILES\), the following
    file specification will refer to the file FILE1.REP in the
    \(EXAMPLES\) directory:
    :XMP.
\(examples\)file1.rep
    :eXMP.
:LI.The file designation below indicates that the file resides on a
    logical device called "source" and in the directory called
    "document.text".
    Version number 5 of the file is specified:
    :XMP.
source:\(document.text\)book.gml;5
    :eXMP.
:eOL.
