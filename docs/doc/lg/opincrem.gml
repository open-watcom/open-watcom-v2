.*
.*
.option INCREMENTAL
.*
.np
The "INCREMENTAL" option can be used to enable incremental linking.
.ix 'incremental linking'
Incremental linking is a process whereby the linker attempts to modify
the existing executable file by changing only those portions for which
new object files are provided.
.np
The format of the "INCREMENTAL" option (short form "INC") is as follows.
.mbigbox
    OPTION INCREMENTAL[=inc_file_name]
.embigbox
.synote
.mnote inc_file_name
is a file specification for the name of the incremental information
file.
If no file extension is specified, a file extension of "ilk" is
assumed.
.esynote
.np
This option engages the incremental linking feature of the linker.
This option must be one of the first options encountered in the list
of directives and options supplied to the linker.
If the option is presented too late, the linker will issue a
diagnostic message.
.np
By default, the incremental information file has the same name as the
program except with an "ilk" extension unless the "NAME" directive has
not been seen yet.
If this is the case then the file is called
.fi __wlink.ilk.
.np
The linker's incremental linking technique is very resistant to
changes in the underlying object files - there are very few cases
where an incremental re-link is not possible.
The options "ELIMINATE" and "VFREMOVAL" cannot be used at the same
time as incremental linking.
.np
It is possible, over time, to accumulate unneeded functions in the
executable by using incremental linking.
To guarantee an executable of minimum size, you can cause a full
relink by deleting the ".ilk" file or by not specifying the
"INCREMENTAL" option.
.np
Do not use a post processor like the &wrcname on the executable file
since this will damage the data structures maintained by the linker.
Add resources to the executable file using the "RESOURCE" option which
is described in :HDREF refid='xresour'..
.remark
Only DWARF debugging information is supported with incremental
linking.
.eremark
