:set symbol="read_err"  value="Refer to the notes on Read Errors at the start of this section.".
:set symbol="write_err" value="Refer to the notes on Write Errors at the start of this section.".
.np
This section describes the various warning and error messages that may
be issued by the &wrcname..
.*
.begnote
.note Read Errors
Messages concerning "read errors" indicate than an error occurred while
the &wrcname was reading the specified file. The reason, if present,
indicates the type of error. The message may indicate the type of file
that was being read, such as a bitmap or icon. If the message says
'temporary file' then the file is one created by the &wrcname for use
while it is running. This is not a file specified by the user.
.endnote
.*
.begnote
.note Write Errors
Messages concerning "write errors" indicate than an error occurred while
the &wrcname was writing to the specified file. The reason, if present,
indicates the type of error. The message may indicate the type of file
that was being written, such as a bitmap or icon. If the message says
"temporary file" then the file is one created by the &wrcname for use
while it is running. This is not a file specified by the user.
.endnote
.*
.errsect
.*
.keep
.errnote 6 Unable to open file "<file>": <reason>.
.np
The &wrcname was unable to open the specified file for the specified
reason.
.*
.keep
.errnote 7 Error renaming temporary file "<file>" to "<file>": <reason>
.np
When creating an executable or resource file the &wrcname first writes
the information to a temporary file then renames that file. This
messages indicates that an error occurred while renaming the specified
temporary file.
.*
.keep
.errnote 8 "<file>" is not a valid icon file.
.np
The file specified in an ICON statement is either not a valid icon file
or is corrupted. You can do two things:
.begbull $compact
.bull
Try recreating the file
.bull
Make sure the correct file is specified
.endbull
.*
.keep
.errnote 9 "<file>" is not a valid cursor file.
.np
The file specified in a CURSOR statement is either not a valid cursor
file or is corrupted. You can do two things:
.begbull $compact
.bull
Try recreating the file
.bull
Make sure the correct file is specified
.endbull
.*
.keep
.errnote 10 "<file>" is not a valid bitmap file.
.np
The file specified in a BITMAP statement is either not a valid bitmap
file or is a corrupted. You can do two things:
.begbull $compact
.bull
Try recreating the file
.bull
Make sure the correct file is specified
.endbull
.*
.keep
.errnote 11 Trying to add a Win16 or OS/2 RES file "<file>" to a Win32 executable "<file>".
.np
This error occurs when you attempt to run pass two of the &wrcname
with a Win16 or OS/2
.fi .RES
file and a Win32 executable file. You can do two
things:
.begbull $compact
.bull
Recreate the
.fi .RES
file using the -bt=nt switch
.bull
Make sure the correct file is specified.
.endbull
.*
.keep
.errnote 12 Trying to add a Win32 or OS/2 RES file "<file>" to a Win16 executable "<file>".
.np
This error occurs when you attempt to run pass two of the &wrcname
with a Win32 or OS/2
.fi .RES
file and a Win16 executable file. You can do two things:
.begbull $compact
.bull
Recreate the
.fi .RES
file using the -bt=windows switch
.bull
Make sure the correct file is specified.
.endbull
.*
.keep
.errnote 13 Object alignment increase required. Relink with larger object alignment.
.np
Each Win32 executable file contains an object alignment value that determines
the maximum size of the file. This error indicates that in adding
resources to a Win32 executable, the size of the executable increased enough
that its object alignment must be increased. The solution is to
re-link the executable file with a larger object alignment. (Refer to the
&lnkname OBJALIGN option) then run the &wrcname again.
.*
.keep
.errnote 14 EXE contained old resources that could not be removed.
.np
The Win32 executable that the &wrcname is operating on already contains
resources that the compiler cannot remove. As a result, the produced
executable will be larger than it needs to be but it will still work.
The recommended solution is to relink the EXE and run the &wrcname
again. This is a warning only.
.*
.keep
.errnote 15 Error reading EXE file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 16 Error reading file "<file>": Unexpected end of file.
.np
&read_err
.np
This message probably indicates that the given file is corrupt. Try
regenerating the file.
.*
.keep
.errnote 17 Error writing to "<file>": <reason>.
.np
&write_err
.*
.keep
.errnote 18 Error reading icon file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 19 Error reading cursor file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 20 Error reading bitmap file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 21 Error reading font file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 22 Error reading data file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 23 Error reading RES file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 24 Error reading temporary file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 25 Error writing to temporary file "<file>": <reason>.
.np
&write_err
.*
.keep
.errnote 26 Unable to open temporary file "<file>": <reason>.
.np
The &wrcname is unable to create a temporary file that is needed to
perform the specified task. The reason gives an explanation of why the
operation failed.
.*
.keep
.errnote 27 Error reading from file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 28 Unable to read RES file "<file>": wrong version.
.np
The &wrcname is unable to read the specified
.fi .RES
file because its version is incompatible with this version of the
compiler.
The solution is to recreate the
.fi .RES
file using this version of the &wrcname.
.*
.keep
.errnote 29 "<file>" is not a valid RES file.
.np
This error occurs if the specified file is not a valid RES file.
You can do two things:
.begbull $compact
.bull
Regenerate the
.fi .RES
file.
.bull
Make sure the correct file is specified.
.endbull
.*
.keep
.errnote 30 Internal compiler error <internal_error_num>.
.np
This message indicates that an unexpected error occurred within the
&wrcname . Report any occurrence of this message to &company Technical
Support.
.*
.keep
.errnote 31 Error writing resources to temporary file "<file>": <resource>.
.np
&write_err
.*
.keep
.errnote 32 Error while closing file "<file>" or flushing buffers to it: <reason>.
.np
This indicates an error occurred while closing or writing to the given
file.
.*
.keep
.errnote 33 Error while closing temporary file "<file>" or flushing buffers to it: <reason>.
.np
This indicates an error occurred while closing or writing to the given
file. The given file was a temporary file created by the &wrcname for
its use while it is running.
.*
.keep
.errnote 35 RCINCLUDE keyword not currently supported. Use #include.
.np
This message indicates the rcinclude keyword was used and it is not
supported. The solution is to convert rcinclude statements to #include
statements in the following manner: convert
.mono RCINCLUDE foo.h
to
.mono #include "foo.h"
.*
.keep
.errnote 36 '<src_char>' is not valid at this point.
.np
This message indicates that a syntactic error occurred while parsing a
resource script file.
.*
.keep
.errnote 37 String may not cross newline boundary.
.np
While compiling a
.fi .RC
file a string literal was encountered that did not have a closing
quotation before the end of the line it was on.
For example,
.millust begin
"this string
.millust end
.pc
would cause the error, while
.millust begin
"this one would not"
.millust end
.*
.keep
.errnote 38 Expecting '<expected_str>' but found '<src_char>'
.np
This message indicates that a syntactic error occurred while parsing a
resource script file.
.*
.keep
.errnote 39 Too many arguments: <extra_name>
.np
This error occurs when more than two filenames are specified on the
command line.
.*
.keep
.errnote 40 Filename required on command line.
.np
This message indicates that no filename was specified on the command
line.
.*
.keep
.errnote 41 Unknown option '<option>'
.np
This error occurs when the stated option is specified on the command
line but is not a valid &wrcname option.
.*
.keep
.errnote 42 No options specified after options character.
.np
This error occurs when the character / or - is specified on the
command line with no option specified after it.
.*
.keep
.errnote 43 Out of memory
.np
The &wrcname does not have enough memory to complete the requested
operation.
.*
.keep
.errnote 44 No type given for accelerator <source_num>
.np
This error is reported when the &wrcname encounters a definition
for a character or virtual-key accelerator that lacks a type.
.*
.keep
.errnote 45 Keyword <accel_type> ignored for type ASCII accelerator <source_num>
.np
This error occurs when the keyword specified by <accel_type> is
specified for an ASCII accelerator. This keyword is not valid for
ASCII accelerators and will be ignored.
.*
.keep
.errnote 46 Duplicate resource: <resource_name>
.np
This error occurs when more than one resource with the same name,
type, and language is encountered in a
.fi .RC
file.
Only the first resource encountered is properly included in the
.fi .RES
file. This is a warning only.
.*
.keep
.errnote 47 Internal parser error.
.np
This indicates that a syntactic error occurred while parsing a resource
script file.
.*
.keep
.errnote 49 Syntax error near "<src_str>"
.np
This indicates that a syntactic error occurred while parsing a resource
script file.
.*
.keep
.errnote 50 <source_num> is a duplicate string identifier.
.np
This message indicates that more than one string table string is
defined with the given numeric identifier. Numeric identifiers for
strings in string tables must be unique.
.*
.keep
.errnote 51 Raw data item <source_num> is too big. Max <max_num>
.np
This message indicates that the given value is specified as an element
of an RCDATA resource and is greater than the largest allowable value
for an RCDATA resource element.
.*
.keep
.errnote 52 Raw data item <source_num> is too small. Min <max_num>
.np
This message indicates that the given value is specified as an element
of an RCDATA resource and is less than the least allowable value for
an RCDATA resource element.
.*
.keep
.errnote 53 Font name must be a number.
.np
This error occurs when an ASCII string is used as a name for a FONT
resource. Font resource names must be numeric.
.*
.keep
.errnote 54 Can't #include more than <max_num> levels.
.np
There is a limit on the number of levels deep you can nest #include
statements. <max_num> specifies the maximum number of levels of
nesting allowed. This message occurs when a file contains more levels
of nesting than are allowed.
.*
.keep
.errnote 55 Unknown preprocessor directive
.np
This error occurs when a command that is not a valid preprocessor
directive is prefixed by a #.
.*
.keep
.errnote 56 Invalid #line directive
.np
This error is reported when a #line directive is encountered in a
resource script file but the parameters after it are not valid.
.*
.keep
.errnote 57 Unknown option '<option>'
.np
This error is reported when a command line option is specified that
the &wrcname does not understand.
.*
.keep
.errnote 58 "<file>" is not a valid EXE file.
.np
This error occurs when the &wrcname expects the given file to be an
EXE file but it is either not an EXE file or it is a corrupted EXE
file. You can do two things:
.begbull $compact
.bull
Make sure the correct file is specified.
.bull
Regenerate the EXE file by relinking.
.endbull
.*
.keep
.errnote 59 Unable to produce fastload section.
.np
This error occurs when the &wrcname is unable to create a fastload
section in the given EXE because one of the segments in the EXE is
larger than 64K. This message is only a warning; a working executable
including resources is still produced. This message does not appear if
the -k option is used.
.*
.keep
.errnote 60 Can't find file "<file>".
.np
This message indicates that the &wrcname is unable to locate an input
file needed to complete the operation. This file may be one of the
files specified on the command line or one included in the
.fi .RC
file by a statement such as ICON, BITMAP, RCDATA, or CURSOR.
You can do two things:
.begbull $compact
.bull
For files specified on the command line, make sure the file was
specified correctly and that it exists in the current directory or in
the specified directory if a path for it is given on the command line.
.bull
For files included by a
.fi .RC
file statement, make sure the file is specified correctly and exists
in the current directory or a directory specified by the INCLUDE
environment variable.
.endbull
.*
.keep
.errnote 61 Unknown target OS: '<os>'
.np
This message indicates that on the &wrcname command line the -bt=<os>
option is specified and the compiler does not recognize the specified
<os>. The valid values for <os> are windows, nt and os2.
.*
.keep
.errnote 62 <preproc_msg>
.np
This indicates an error occurred while preprocessing a
.fi .RC
file.
The text of the message contains information about the error.
.*
.keep
.errnote 63 Keyword "<keyword>" is only valid for Win32.
.np
While creating a non-Win32
.fi .RES
file, the &wrcname encountered a keyword that is only valid for Win32.
.*
.keep
.errnote 64 Keyword "<keyword>" not currently being supported.
.np
A keyword was used in a
.fi .RC
file that is not supported by the &wrcname.
.*
.keep
.errnote 65 Resource strings not found.
.np
This message indicates that your copy of the &wrcname has been
corrupted. Try reinstalling the compiler from your original
installation disks. If this fails, report the problem to &company
Technical Support.
.*
.keep
.errnote 66 Error can't find character translation file "<file>".
.np
Character translation files are data files that the &wrcname uses when
creating a Win32
.fi .RES
file to convert text into UNICODE. This message indicates that the
&wrcname cannot locate the given translation file that it requires.
The solution is to find the file and make sure it is in your path.
.*
.keep
.errnote 67 Error opening character translation file "<file>": <reason>.
.np
Character translation files are data files that the &wrcname uses when
creating a Win32
.fi .RES
file to convert text into UNICODE. This message indicates that the
&wrcname found the character translation file but was unable to access
it for the given reason.
.*
.keep
.errnote 68 Error reading character translation file "<file>": <reason>.
.np
&read_err
.*
.keep
.errnote 69 "<file>" is not a valid character translation file.
.np
Character translation files are data files that the &wrcname uses when
creating a Win32
.fi .RES
file to convert text into UNICODE. This error occurs when the &wrcname
finds a file with the same name as the expected character translation
file but this file is either not a character translation file or is a
corrupted character translation file. You can do two things:
.begbull $compact
.bull
Find the correct character translation file (the one that came with
this version of the &wrcname) and put it in a directory that appears
in your path before any other directory that contains a file of that
name.
.bull
Try reinstalling the file from your original distribution disks.
.endbull
.*
.keep
.errnote 70 Unable to read character translation file "<file>": wrong version.
.np
Character translation files are data files that the &wrcname uses when
creating a Win32
.fi .RES
file to convert text into UNICODE. This error occurs when the &wrcname
finds a file with the same name as the expected character translation
file but is unable to read it because its version is incompatible with
this version of the compiler. You can do two things:
.begbull $compact
.bull
Find the correct character translation file (the one that came with
this version of the &wrcname) and put it in a directory that appears
in your path before any other directory that contains a file of that
name.
.bull
Try reinstalling the file from your original distribution disks.
.endbull
.*
.keep
.errnote 71 Option '<option>' not valid for a Win32 .RES file.
.np
This indicates that while producing a Win32
.fi .RES
file, a command line parameter was specified that is not valid for Win32
.fi .RES
files.
.*
.keep
.errnote 72 L"..." construct is not valid for Win16.
.np
This error occurs when the user enters a string prefixed by the letter
(i.e. L"Bob") in an
.fi .RC
file being turned into a Win16
.fi .RES
file.
The L specifier is meaningless for Win16 and will be ignored.
This is a warning only.
.*
.keep
.errnote 73 Options '<option>' and '<option>' may not be specified together.
.np
This error occurs when two options specified on the command line are
not compatible with one another. You should remove one of the specified
options.
.keep
.errnote 74 Environment variable '<var>' is not set.
.np
An environment variable specified on the command line using the @
option had no value set for it.
.*
.keep
.errnote 76 MENUITEM must have ID value.
.np
A MENUITEM in a MENU resource must contain a menu id. This message is
issued when a MENUITEM in a menu resource does not have a menu id
specified for it.
.exam begin
mymenu MENU
BEGIN
    MENUITEM "my item"
END
.exam end
.np
Menu items in menuex resources do not need a menu id.
.*
.keep
.errnote 77 Id/Type/State/HelpId not allowed for MENU POPUP controls.
.np
A POPUP statement in a MENU resource may not be followed by any
modifiers (such as an ID). This message is issued when a POPUP
statement in a MENU resource is followed by one or more modifiers.
.exam begin
mymenu MENU
BEGIN
    POPUP "my popup", MY_ID
END
.exam end
.np
POPUP statements in menuex resources may be followed by modifiers.
.*
.keep
.errnote 78 MENUITEM options are not allowed for MENUEX POPUP controls.
.np
A POPUP statement in a MENUEX resource may not use optional modifiers
such as CHECKED or GREYED. This message is issued when a POPUP
statement in a MENUEX resource contains one of these optional
modifiers. In a MENUEX resource these modifiers are replaced by MFT_*
and MFS_* flags.
.*
.keep
.errnote 79 Type/State not valid for MENU MENUITEM controls.
.np
A MENUITEM statement in MENU resource must not specify state and type
information using the MFT_* or MFS_* flags. This message is issued
when type or state information is specified for a MENUITEM in a MENU
resource.
.exam begin
mymenu MENU
BEGIN
    MENUITEM "item", MY_ID, MFT_STRING, MFS_GREYED
END
.exam end
In a MENU resource this information is specified using option
modifiers such as GREYED or CHECKED.
.*
.keep
.errnote 80 MENUITEM options not valid for MENUEX MENUITEM controls.
.np
A MENUITEM statement in a MENUEX resource may not use optional
modifiers such as CHECKED or GREYED. This message is issued when a
MENUITEM statement in a MENUEX resource contains one of these optional
modifiers. In a MENUEX resource these modifiers are replaced by MFT_*
and MFS_* flags.
.*
.keep
.errnote 81 HELPID not valid for DIALOG.
.np
You may not specify a help id for a DIALOG resource. This message is
issued when a help id is found with a DIALOG resource.
.exam begin
my_dlg DIALOG 0, 0, 100, 100, MY_HELPID
.exam end
.np
A help id may be specified for a DIALOGEX resource.
.*
.keep
.errnote 82 Font Italic setting not allowed for DIALOG.
.np
You may not specify an italic font for a DIALOG resource. This message
is issued when the italic flag has been specified in a FONT statement
in a DIALOG resource.
.exam begin
my_dlg DIALOG 0, 0, 100, 100
FONT "helvetica", 8,  FW_BOLD, 1
.exam end
.np
The italic flag may be specified for DIALOGEX resources.
.*
.keep
.errnote 83 Font Weight setting not allowed for DIALOG.
.np
You may not specify a font weight for a DIALOG resource. This message
is issued when a font weight has been specified in a FONT statement
in a DIALOG resource.
.exam begin
my_dlg DIALOG 0, 0, 100, 100
FONT "helvetica", 8,  FW_BOLD
.exam end
.np
A font weight may be specified for DIALOGEX resources.
.*
.keep
.errnote 84 HELPID not valid for DIALOG controls.
.np
You may not specify a help id for a control in a DIALOG resource. This
message is issued when a help id is specified for a control in a
DIALOG resource.
.exam begin
my_dlg DIALOG 0, 0, 100, 100
BEGIN
    CONTROL "None",  MY_ID, "BUTTON",
             BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE
            12, 40, 30, 10,  0, MY_HELPID
END
.exam end
.np
A help id may be specified for a control in DIALOGEX resource.
.*
.keep
.errnote 85 Data Elements not valid for DIALOG controls.
.np
This indicates that a list of data values has been found following a
control in a DIALOG resource. Data can only follow control statements
in a DIALOGEX resource.
.*
.keep
.errnote 86 <value> is not a valid resource id.
.np
This message is issued when a numeric identifier has been specified
for a resource but the value is outside the allowable range.
.*
.keep
.errnote 87 User interrupt detected.
.np
Processing was stopped to allow another user action to occur.
.*
.keep
.errnote 88 Resource type for resource "<resource>" is greater than 32767.
.np
When a number is used to identify the type of a resource the number
must be between 0 and 32767. This message is issued when a number has
been specified as a resource type that is outside the allowable range.
.exam begin
MYRES 0xF000 { 0 }
.exam end
.*
.keep
.errnote 89 Resource ID "<id>" is greater than 32767.
.np
When a number is used to identify a resource the number must be
between 0 and 32767. This message is issued when a number has been
specified as a resource identifier that is outside the allowable
range.
.exam begin
0xF000 ICON "t.ico"
.exam end
.*
.keep
.errnote 90 Unmatched quotation mark on command line.
.np
Quotations marks may be used on the WRC command line to delimit file
or path names. This message indicates that a quotation mark was found
on the command line but no matching quotation mark could be found.
.*
.keep
.errnote 100 Trying to add a Win16 or Win32 RES file "<file>" to an OS/2 executable "<file>".
.np
This error occurs when you attempt to run pass two of the &wrcname
with a Win16 or Win32
.fi .RES
file and an OS/2 executable file. You can do two things:
.begbull $compact
.bull
Recreate the
.fi .RES
file using the -bt=os2 switch
.bull
Make sure the correct file is specified.
.endbull
.*
.keep
.errnote 101 <value> is not a valid resource type.
.np
This message is issued when a numeric type has been specified
for a resource but the value is outside the allowable range.
.*
.keep
.errnote 102 Symbol <value> is not defined.
.np
This message is issued when a numeric resource id is expected
but the symbol representing the resource id has not been defined.
.np
This error occurs only for OS/2 resource files.
Other platforms support symbolic and string constant resource ids.
.*
.eerrsect
