.*
.*
.option CASEEXACT
.*
.np
The "CASEEXACT" option tells the &lnkname to respect case when
resolving references to global symbols.
That is, "ScanName" and "SCANNAME" represent two different symbols.
This is the default because the most commonly used languages
(C, C++, FORTRAN) are case sensitive.
The format of the "CASEEXACT" option (short form "C") is as follows.
.mbigbox
    OPTION CASEEXACT
.embigbox
.np
It is possible to override the default by using the "NOCASEEXACT" option.
The "NOCASEEXACT" option turns off case-sensitive linking.
The format of the "NOCASEEXACT" option (short form "NOCASE") is as
follows.
.mbigbox
    OPTION NOCASEEXACT
.embigbox
.np
.ix 'default directive file' 'wlink.lnk'
.ix 'wlink.lnk' 'default directive file'
You can specify the "NOCASEEXACT" option in the default directive
files :FNAME.wlink.lnk:eFNAME. or :FNAME.wlsystem.lnk:eFNAME. if
required.
.*
.im wlinklnk
.*
