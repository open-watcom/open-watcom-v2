.*
.*
.option DOSSEG
.*
.np
The "DOSSEG" option tells the &lnkname to order segments in a special
way.
The format of the "DOSSEG" option (short form "D") is as follows.
.mbigbox
    OPTION DOSSEG
.embigbox
.pc
When the "DOSSEG" option is specified, segments will be ordered in the
following way.
.*
.im wlsorder
.*
.np
When using &company run-time libraries, it is not necessary to specify
the "DOSSEG" option.
One of the object files in the &company run-time libraries contains a
special record that specifies the "DOSSEG" option.
.np
If no "DOSSEG" option is specified,
segments are ordered in the order they are encountered by the &lnkname..
.np
When the "DOSSEG" option is specified, the &lnkname defines two
special variables.
.ix '_edata linker symbol'
.ix '_end linker symbol'
.ix 'linker symbols' '_edata'
.ix 'linker symbols' '_end'
.id _edata
defines the start of the "BSS" class of segments and
.id _end
defines the end of the "BSS" class of segments.
Your program must not redefine these symbols.
