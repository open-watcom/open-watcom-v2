.*
.*
.option UNDEFSOK
.*
.np
The "UNDEFSOK" option tells the &lnkname to generate an executable
file even if undefined symbols are present.
By default, no executable file will be generated if undefined symbols
are present.
.np
The format of the "UNDEFSOK" option (short form "U") is as follows.
.mbigbox
    OPTION UNDEFSOK
.embigbox
.np
.ix 'NOUNDEFSOK option'
.ix 'options' 'NOUNDEFSOK'
The "NOUNDEFSOK" option tells the &lnkname to not generate an
executable file if undefined symbols are present.
This is the default behaviour.
.np
The format of the "NOUNDEFSOK" option (short form "NOU") is as
follows.
.mbigbox
    OPTION NOUNDEFSOK
.embigbox
