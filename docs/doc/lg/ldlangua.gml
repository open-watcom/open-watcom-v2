.*
.*
.dirctv LANGUAGE
.*
.np
The "LANGUAGE" directive is used to specify the language in which
strings in the &lnkname directives are specified.
The format of the "LANGUAGE" directive (short form "LANG") is as
follows.
.mbigbox
    LANGUAGE lang

    lang ::= JAPANESE | CHINESE | KOREAN
.embigbox
.begpoint $break
.point JAPANESE
.ix 'LANGUAGE options' 'JAPANESE'
.ix 'DBCS' 'Japanese'
(short form "JA") specifies that strings are to be handled as if they
contained characters from the Japanese Double-Byte Character Set
(DBCS).
.point CHINESE
.ix 'LANGUAGE options' 'CHINESE'
.ix 'DBCS' 'Chinese'
(short form "CH") specifies that strings are to be handled as if they
contained characters from the Chinese Double-Byte Character Set
(DBCS).
.point KOREAN
.ix 'LANGUAGE options' 'KOREAN'
.ix 'DBCS' 'Korean'
(short form "KO") specifies that strings are to be handled as if they
contained characters from the Korean Double-Byte Character Set (DBCS).
.endpoint
