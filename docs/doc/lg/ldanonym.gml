.*
.*
.dirctv ANONYMOUSEXPORT
.*
.np
The "ANONYMOUSEXPORT" directive is an alternative to the "EXPORT"
directive described in :HDREF refid='xexport'..
The symbol associated with this name will not appear in either the
resident or the non-resident names table.
The entry point is, however, still available for ordinal linking.
.*
.np
The format of the "ANONYMOUSEXPORT" directive (short form "ANON") is
as follows.
.mbigbox
     ANONYMOUSEXPORT export{,export}
         or
     ANONYMOUSEXPORT =lbc_file

     export ::= entry_name[.ordinal][=internal_name]
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
in an "ANONYMOUSEXPORT" directive.
.note
If the name contains characters that are special to the linker then
the name may be placed inside apostrophes (e.g.,
.mono anonymousexport 'myfunc@8'
.ct ).
.note
The symbol associated with the entry name will not appear in either
the resident or the non-resident names table.
The entry point is, however, still available for ordinal linking.
This directive is important when you wish to reduce the number of
entries that are placed in the resident and non-resident names table.
.endnote
