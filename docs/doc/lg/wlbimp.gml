.millust begin
++sym.dll_name[.[altsym].export_name][.ordinal]
.millust end
.synote
.mnote sym
is the name of a symbol in a Dynamic Link Library.
.mnote dll_name
is the name of the Dynamic Link Library that defines
.id sym.
.mnote altsym
is the name of a symbol in a Dynamic Link Library.
When omitted, the default symbol name is
.id sym.
.mnote export_name
is the name that an application that is linking to the Dynamic Link
Library uses to reference
.id sym.
When omitted, the default export name is
.id sym.
.mnote ordinal
is the ordinal value that can be used to identify
.id sym
instead of using the name
.id export_name.
.esynote
