.*
.*
.dirctv SORT
.*
.np
The "SORT" directive is used to sort the symbols in the "Memory Map"
section of the map file.
By default, symbols are listed on a per module basis in the order the
modules were encountered by the linker.
That is, a module header is displayed followed by the symbols defined
by the module.
.np
The format of the "SORT" directive (short form "SO") is as follows.
.mbigbox
    SORT [GLOBAL] [ALPHABETICAL]
.embigbox
.np
If the "SORT" directive is specified without any options, as in the
following example, the module headers will be displayed each followed
by the list of symbols it defines sorted by address.
.millust begin
sort
.millust end
.np
If only the "GLOBAL" sort option (short form "GL") is specified, as in
the following example, the module headers will not be displayed and
all symbols will be sorted by address.
.millust begin
sort global
.millust end
.np
If only the "ALPHABETICAL" sort option (short form "ALP") is
specified, as in the following example, the module headers will be
displayed each followed by the list of symbols it defines sorted
alphabetically.
.millust begin
sort alphabetical
.millust end
.np
If both the "GLOBAL" and "ALPHABETICAL" sort options are specified, as
in the following example, the module headers will not be displayed and
all symbols will be sorted alphabetically.
.millust begin
sort global alphabetical
.millust end
.np
.ix 'mangled names in C++'
If you are linking a &cmppname application, mangled names are sorted
by using the base name.
The base name is the name of the symbol as it appeared in the source
file.
See the section entitled :HDREF refid='xmangle'. for more information on
mangled names.
