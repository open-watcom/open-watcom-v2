.*
.*
.option REDEFSOK
.*
.np
The "REDEFSOK" option tells the &lnkname to ignore redefined symbols
and to generate an executable file anyway.
By default, warning messages are displayed and an executable file is
generated if redefined symbols are present.
.np
The format of the "REDEFSOK" option (short form "RED") is as follows.
.mbigbox
    OPTION REDEFSOK
.embigbox
.np
.ix 'NOREDEFSOK option'
.ix 'options' 'NOREDEFSOK'
The "NOREDEFSOK" option tells the &lnkname to treat redefined symbols
as an error and to not generate an executable file.
By default, warning messages are displayed and an executable file is
generated if redefined symbols are present.
.np
The format of the "NOREDEFSOK" option (short form "NORED") is as follows.
.mbigbox
    OPTION NOREDEFSOK
.embigbox
