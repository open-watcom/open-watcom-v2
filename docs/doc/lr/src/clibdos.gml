.chap DOS Considerations
.*
.np
.ix 'DOS considerations'
For the most part, DOS (Disk Operating System) for your personal
computer can be ignored, unless an application is highly dependent
upon the hardware or uses specialized functions from the operating
system.
In this section, some of these aspects will be addressed.
For a more detailed explanation, the technical
documentation for the DOS that you are using should be consulted.
.*
.section DOS Devices
.*
.np
.ix 'DOS devices'
.ix 'devices'
Most of the hardware devices attached to your computer have names
which are recognized by DOS.
These names cannot be used as the names of files.
Some examples are:
.begnote $compact $setptnt 8
.note CON
the console (screen)
.note AUX
the serial (auxiliary) port
.note COM1
serial port 1
.note COM2
serial port 2
.note PRN
the printer on the parallel port
.note LPT1
the printer on the first parallel port
.note LPT2
the printer on the second parallel port
.note LPT3
the printer on the third parallel port
.note NUL
a non-existent device, which accepts (and discards) output
.endnote
.np
Disks (such as diskette drives and hard disks) are specified as single
letters, starting with the letter A.
A colon character (:) follows the letter for the drive.
Either uppercase or lowercase letters can be used.
Some examples are:
.begnote $compact $setptnt 8
.note  A:
the first disk drive
.note  a:
the first disk drive
.note  e:
the fifth disk drive
.endnote
.*
.section DOS Directories
.*
.np
.ix 'DOS directory'
.ix 'directory'
Each disk drive is conceptually divided into directories.
Each directory is capable of containing files and/or other directories.
The initial directory, called the
.us root directory,
is not named; all other directories are named and can be accessed with a
.us path
specification.
A path is either absolute or relative to the current working directory.
.ix 'path'
.ix 'DOS path'
Some examples are:
.begnote
.note b:&pc.
the root directory of the second disk drive
.note &pc.
the root directory of the current disk drive
.note &pc.outer&pc.middle&pc.inner
.sk 0
directory
.mono inner
which is contained within directory
.mono middle
which is contained within directory
.mono outer
which is contained within the root directory of the current disk
drive.
.endnote
.np
Directory names are separated by backslash characters (\).
The initial backslash character informs DOS that the path starts with
the root directory.
When the first character is not a backslash, the path starts with the
current working directory on the indicated device.
.np
The
.doscmd CHDIR (CD)
can be used to change the current working directory for a device.
Suppose that the following DOS commands were issued:
.millust begin
chdir a:&pc.apps&pc.payroll
chdir c:&pc.mydir
.millust end
.pc
Then, the following path specifications are:
.begpoint $setptnt 15 $compact
.termhd1 Relative Path
.termhd2 Absolute Path
.point a:xxx&pc.y
a:&pc.apps&pc.payroll&pc.xxx&pc.y
.point c:zzzzz
c:&pc.mydir&pc.zzzzz
.endpoint
.pc
When no drive is specified, DOS uses the current disk drive.
.*
.section DOS File Names
.*
.np
The name of a file within a directory has the format
.monoon
filename.ext
.monooff
where the required
.mono filename
portion is up to eight characters in length and the optional
.mono ext
portion is up to three characters in length.
A period character (.) separates the two names when the
.mono ext
portion is present.
.np
More than eight characters can be given in the
.mono filename.
DOS truncates the name to eight characters when a longer
.mono filename
is given.
This may lead to erroneous results in some cases, since the files
.mono MYBIGDATAFILE
and
.mono MYBIGDATES
both refer to the file
.mono MYBIGDAT.
.np
The characters used in file names may be letters, digits as well as
some other characters documented in your DOS technical documentation.
Most people restrict their file names to contain only letters and
digits.
Uppercase and lowercase letters are treated as being equivalent
(file names are case insensitive).
Thus, the files
.millust begin
MYDATA.NEW
mydata.new
MyData.New
.millust end
.pc
all refer to the same file.
.np
You cannot use a DOS device name (such as
.kw CON
or
.kw PRN
.ct,
for example) for a file name.
See the section
.us DOS Devices
for a list of these reserved names.
.np
A complete file designation has the following format:
.millust begin
drive:&pc.path&pc.filename.ext
.millust end
.pc
where:
.begnote $setptnt 13
.cc 6
.note drive:
is an optional disk drive specification.
If omitted, the default drive is used.
Some examples are:
.millust begin
A:  (first disk drive)
c:  (third disk drive)
.millust end
.note &pc.path&pc.
is the path specification for the directory containing the desired file.
Some examples are:
.millust begin
&pc.mylib&pc.
&pc.apps&pc.payroll&pc.
.millust end
.note filename.ext
is the name of the file.
.endnote
.np
Suppose that the current working directories are as follows:
.begpoint $setptnt 8 $compact
.termhd1 Drive
.termhd2 Directory
.point A:
&pc.payroll
.point B:
&pc.~b~b~b~b~b~b~b~b~b~b~b(root directory)
.point C:
&pc.source&pc.c
.endpoint
.pc
and that the default disk drive is
.mono C:.
Then, the following file designations will result in the indicated
file references:
.begpoint $setptnt 18 $compact
.termhd1 Designation
.termhd2 Actual File
.point pgm.c
C:&pc.SOURCE&pc.C&pc.PGM.C
.point &pc.basic.dat
C:&pc.BASIC.DAT
.point paypgm&pc.outsep.c
C:&pc.SOURCE&pc.C&pc.PAYPGM&pc.OUTSEP.C
.point b:data
B:&pc.DATA
.point a:employee
A:&pc.PAYROLL&pc.EMPLOYEE
.point a:&pc.deduct&pc.yr1988
A:&pc.DEDUCT&pc.YR1988
.endpoint
.*
.section DOS Files
.*
.np
.ix 'DOS file'
DOS files are stored within directories on disk drives.
Most software, including &company C/C++, treats files in two
representations:
.begnote $setptnt 10
.note BINARY
These files can contain arbitrary data.
It is the responsibility of the software to recognize records within
the file if they exist.
.note TEXT
These files contain lines of "printable" characters.
Each line is delimited by a carriage return character followed
by a linefeed character.
.endnote
.np
Since the conceptual view of text files in the C and C++ languages is
that lines are terminated by only linefeed characters, the &company C
library will remove carriage returns on input and add them on output,
provided the mode is set to be
.us text.
This mode is set upon opening the file or with the
.kw setmode
function.
.*
.section DOS Commands
.*
.np
.ix 'DOS commands'
DOS commands are documented in the technical documentation for your
DOS system.
These may be invoked from a C or C++ program with the
.kw system
function.
.*
.section DOS Interrupts
.*
.np
.ix 'DOS Interrupts'
DOS interrupts and 8086 interrupts are documented in the technical
documentation for your DOS system.
These may be generated from a C or C++ program by calling the
.kw bdos
.ct,
.kw intdos
.ct,
.kw intdosx
.ct,
.kw intr
.ct,
.kw int386
.ct,
.kw int386x
.ct,
.kw int86
and
.kw int86x
functions.
.*
.section DOS Processes
.*
.np
.ix 'Process Functions'
Currently, DOS has the capability to execute only one process at a
time.
Thus, when a process is initiated with the
.kw spawn&grpsfx
parameter
.kw P_WAIT
.ct,
the new process will execute to completion before control returns to
the initiating program.
Otherwise, the new task replaces the initial task.
Tasks can be started by using the
.kw system
.ct,
.kw exec&grpsfx
and
.kw spawn&grpsfx
functions.
