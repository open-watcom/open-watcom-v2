.*
.*
.dirctv EXPORT
.*
.np
The "EXPORT" directive is used to tell the &lnkname which symbols are
available for import by other executables.
.*
.beglevel
.*
.section EXPORT - OS/2, Win16, Win32 only
.*
.np
The "EXPORT" directive can be used to define the names and attributes
of functions in Dynamic Link Libraries that are to be exported.
An "EXPORT" definition must be specified for every Dynamic Link
Library function that is to be made available externally.
.begnote
.note Win16:
.ix 'window function'
An "EXPORT" directive is also required for the "window function".
This function must be defined by all programs and is called
by Windows to provide information to the program.
For example, the window function is called when a window is created,
destroyed or resized, when an item is selected from a menu, or when a
scroll bar is being clicked with a mouse.
.endnote
.np
The format of the "EXPORT" directive (short form "EXP") is as follows.
.mbigbox
     EXPORT export{,export}
         or
     EXPORT =lbc_file

OS/2 only:
     export ::= entry_name[.ordinal][=internal_name]
                [PRIVATE] [RESIDENT] [iopl_bytes]

Win16, Win32 only:
     export ::= entry_name[.ordinal][=internal_name]
                [PRIVATE] [RESIDENT]
.embigbox
.synote
.mnote entry_name
is the name to be used by other applications to call the function.
.mnote ordinal
is an ordinal value for the function.
If the ordinal number is specified, other applications can reference
the function by using this ordinal number.
.mnote internal_name
is the actual name of the function and should only be specified if it
differs from the entry name.
.mnote PRIVATE
(no short form ) specifies that the function's entry name should be
included in the DLL's export table, but not included in any import
library that the linker generates.
.mnote RESIDENT
(short form "RES") specifies that the function's entry name should be
kept resident in memory (i.e., added to the resident names table).
.np
By default, the entry name is always made memory resident if an
ordinal is not specified (i.e., it is implicitly RESIDENT).
For 16-bit Windows, the limit on the size of the resident names table
is 64K bytes.
Memory resident entry names allow the operating system to resolve
calls more efficiently when the call is by entry name rather than by
ordinal.
.np
If an ordinal is specified and RESIDENT is not specified, the entry
name is added to the non-resident names table (i.e., it is implicitly
non-RESIDENT).
If both the ordinal and the RESIDENT keyword are specified, the symbol
is placed in the resident names table.
.np
If you do not want an entry name to appear in either the resident or
non-resident names table, you can use the "ANONYMOUSEXPORT" directive
described in :HDREF refid='xanonym'..
.mnote iopl_bytes
(OS/2 only)
is required for functions that execute with I/O privilege.
.sy iopl_bytes
specifies that total size of the function's arguments in bytes.
When such a function is executed, the specified number of bytes is
copied from the caller's stack to the I/O-privileged function's stack.
The maximum number of bytes allowed is 63.
.mnote lbc_file
is a file specification for the name of a librarian command file.
If no file extension is specified, a file extension of "lbc" is
assumed.
The linker will process the librarian command file and look for commands
to the librarian that are used to create import library entries.
These commands have the following form.
.im wlbimp
.np
All other librarian commands will be ignored.
.esynote
.autonote Notes:
.note
By default, the &company C and C++ compilers append an underscore
('_') to all function names.
This should be considered when specifying
.sy entry_name
and
.sy internal_name
in an "EXPORT" directive.
.note
If the name contains characters that are special to the linker then
the name may be placed inside apostrophes (e.g.,
.mono export 'myfunc@8'
.ct ).
.note
If the
.kw __export
declspec modifier is used in the source code, it is the equivalent of
using the following linker directive:
.millust begin
EXPORT entry_name RESIDENT
.millust end
.endnote
.*
.section EXPORT - ELF only
.*
.np
The "EXPORT" directive is used to tell the &lnkname which symbols are
available for import by other executables.
The format of the "EXPORT" directive (short form "EXP") is as follows.
.mbigbox
    EXPORT entry_name{,entry_name}
.embigbox
.synote
.mnote entry_name
is the name of the exported symbol.
.esynote
.autonote Notes:
.note
By default, the &company C and C++ compilers append an underscore
('_') to all function names.
This should be considered when specifying
.sy entry_name
in an "EXPORT" directive.
.note
If the name contains characters that are special to the linker then
the name may be placed inside apostrophes (e.g.,
.mono export 'myfunc@8'
.ct ).
.endnote
.*
.section EXPORT - Netware only
.*
.np
The "EXPORT" directive is used to tell the &lnkname which symbols are
available for import by other NLMs.
The format of the "EXPORT" directive (short form "EXP") is as follows.
.mbigbox
    EXPORT entry_name{,entry_name}
.embigbox
.synote
.mnote entry_name
is the name of the exported symbol.
.esynote
.autonote Notes:
.note
If the name contains characters that are special to the linker then
the name may be placed inside apostrophes (e.g.,
.mono export 'myfunc@8'
.ct ).
.endnote
.*
.endlevel
