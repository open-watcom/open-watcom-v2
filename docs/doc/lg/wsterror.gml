.*
.*
.section Strip Utility Messages
.*
.np
.ix '&stripcmdup' 'diagnostics'
.ix 'diagnostics' '&stripcmdup'
The following messages may be issued by the &stripname..
.begnote $break
.note Usage: &stripcmdup. [options] input_file [output_file] [info_file]
.bi options: (-option is also accepted)
.br
.bi ~b~b~b~b~b~b/n~b~b~b~b~bdon't print warning messages
.br
.bi ~b~b~b~b~b~b/q~b~b~b~b~bdon't print informational messages
.br
.bi ~b~b~b~b~b~b/r~b~b~b~b~bprocess resource information rather than debugging information
.br
.bi ~b~b~b~b~b~b/a~b~b~b~b~badd information rather than delete information
.br
.bi input_file:  executable file
.br
.bi output_file: optional output executable or '.'
.br
.bi info_file:   optional output debugging or resource information file
.br
.bi ~b~b~b~b~b~b~b~b~b~b~b~b~bor input debugging or resource informational file
.br
The command line was entered with no arguments.
.mnote Too low on memory
There is not enough free memory to allocate file buffers.
.mnote Unable to find '%s'
The specified file could not be located.
.mnote Cannot create temporary file
All the temporary file names are in use.
.mnote Unable to open '%s' to read
The input executable file cannot be opened for reading.
.mnote '%s' is not a valid executable file
The input file has invalid executable file header information.
.mnote '%s' does not contain debugging information
There is nothing to strip from the specified executable file.
.mnote Seek error on '%s'
An error occurred during a seek operation on the specified
file.
.mnote Unable to create output file '%s'
The output file could not be created.
Check that the output disk is not write-protected or that the
specified output file is not marked "read-only".
.mnote Unable to create symbol file '%s'
The symbol file could not be created.
.mnote Error reading '%s'
An error occurred while reading the input executable file.
.mnote Error writing to '%s'
An error occurred while writing the output executable file or
the symbol file.
Check the amount of free space on the output disk.
If the input and output files reside on the same disk, there might
not be enough room for a second copy of the executable file during
processing.
.mnote Cannot erase file '%s'
The input executable file is probably marked "read-only" and
therefore could not be erased (the input file is erased whenever
the output file has the same name).
.mnote Cannot rename file '%s'
The output executable file could not be renamed.
Ordinarily, this should never occur.
.endnote
