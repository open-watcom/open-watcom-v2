.*
.*
.section The &stripname Command Line
.*
.np
The &stripname command line syntax is:
.ix '&stripcmdup.' 'command line format'
.ix 'command line format' '&stripcmdup'
.mbigbox
&stripcmdup [options] input_file [output_file] [info_file]
.embigbox
.begnote where:
.note []
The square brackets denote items which are optional.
.note options
.begpoint
.point &sw.n
(noerrors) Do not issue any diagnostic message.
.point &sw.q
(quiet) Do not print any informational messages.
.point &sw.r
(resources) Process resource information rather than debugging
information.
.point &sw.a
(add) Add information rather than remove information.
.endpoint
.note input_file
is a file specification for the name of an executable file.
If no file extension is specified, the &stripname will assume one of
the following extensions: "exe", "dll", "exp", "rex", "nlm", "dsk",
"lan", "nam", "msl", "cdm", "ham", "qnx" or no file extension.
Note that the order specified in the list of file extensions is the
order in which the &stripname will select file extensions.
.note output_file
is an optional file specification for the output file.
If no file extension is specified, the file extension specified in the
input file name will be used for the output file name.
If "." is specified, the input file name will be used.
.note info_file
is an optional file specification for the file in which the debugging
or resource information is to be stored (when removing information) or
read (when adding information).
If no file extension is specified, a file extension of "sym" is
assumed for debugging information and "res" for resource information.
To specify the name of the information file but not the name of an
output file, a "." may be specified in place of
.sy output_file.
.endnote
.autonote Description:
.note
If the "r" (resource) option is not specified then the default action
is to add/remove symbolic debugging information.
.note
If the "a" (add) option is not specified then the default action is to
remove information.
.note
If
.sy output_file
is not specified, the debugging or resource information is added to or
removed from
.sy input_file.
.note
If
.sy output_file
is specified,
.sy input_file
is copied to
.sy output_file
and the debugging or resource information is added to or removed from
.sy output_file.
.sy input_file
remains unchanged.
.note
If
.sy info_file
is specified then the debugging or resource information that is added
to or removed from the executable file is read from or written to this
file.
The debugging or resource information may be appended to the
executable by specifying the "a" (add) option.
Also, the debugging information may be appended to the executable by
concatenating the debugging information file to the end of the
executable file (the files must be treated as binary files).
.note
During processing, the &stripname will create a temporary file,
ensuring that a file by the chosen name does not already exist.
.endnote
