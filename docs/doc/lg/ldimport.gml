.*
.*
.dirctv IMPORT
.*
.np
The "IMPORT" directive is used to tell the &lnkname what symbols are
defined externally in other executables.
.*
.beglevel
.*
.section IMPORT - OS/2, Win16, Win32 only
.*
.np
The "IMPORT" directive describes a function that belongs to a Dynamic
Link Library.
The format of the "IMPORT" directive (short form "IMP") is as follows.
.mbigbox
     IMPORT import{,import}

     import ::= internal_name module_name[.entry_name | ordinal]
.embigbox
.synote
.mnote internal_name
is the name the application used to call the function.
.mnote module_name
is the name of the Dynamic Link Library.
Note that this need not be the same as the file name of the executable
file containing the Dynamic Link Library.
This name corresponds to the name specified by the "MODNAME" option
when the Dynamic Link Library was created.
.mnote entry_name
is the actual name of the function as defined in the Dynamic Link
Library.
.mnote ordinal
is the ordinal value of the function.
The ordinal number is an alternate method that can be used to reference
a function in a Dynamic Link Library.
.esynote
.autonote Notes:
.note
By default, the &company C and C++ compilers append an underscore
('_') to all function names.
This should be considered when specifying
.sy internal_name
and
.sy entry_name
in an "IMPORT" directive.
.note
.ix 'apostrophes'
If the name contains characters that are special to the linker then
the name may be placed inside apostrophes (e.g.,
.mono import 'myfunc@8'
.ct ).
.endnote
.np
The preferred method to resolve references to Dynamic Link Libraries
is through the use of import libraries.
See the sections entitled :HDREF refid='os2dll'.,
:HDREF refid='windll'., or :HDREF refid='ntdll'.
for more information on import libraries.
.*
.section IMPORT - ELF only
.*
.np
The "IMPORT" directive is used to tell the &lnkname what symbols are
defined externally in other executables.
The format of the "IMPORT" directive (short form "IMP") is as follows.
.mbigbox
    IMPORT external_name{,external_name}
.embigbox
.synote
.mnote external_name
is the name of the external symbol.
.esynote
.autonote Notes:
.note
By default, the &company C and C++ compilers append an underscore
('_') to all function names.
This should be considered when specifying
.sy external_name
in an "IMPORT" directive.
.note
If the name contains characters that are special to the linker then
the name may be placed inside apostrophes (e.g.,
.mono import 'myfunc@8'
.ct ).
.endnote
.*
.section IMPORT - Netware only
.*
.np
The "IMPORT" directive is used to tell the &lnkname what symbols are
defined externally in other NLMs.
The format of the "IMPORT" directive (short form "IMP") is as follows.
.mbigbox
    IMPORT external_name{,external_name}
.embigbox
.synote
.mnote external_name
is the name of the external symbol.
.esynote
.autonote Notes:
.note
If the name contains characters that are special to the linker then
the name may be placed inside apostrophes (e.g.,
.mono import 'myfunc@8'
.ct ).
.endnote
.np
If an NLM contains external symbols, the NLMs that define the external
symbols must be loaded before the NLM that references the external
symbols is loaded.
.*
.endlevel
