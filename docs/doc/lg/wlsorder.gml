.*
.*
.ix 'memory layout'
.ix 'segment ordering'
.autonote
.if '&exeformat' = 'phar' .do begin
.if '&target' ne 'WIN32' .do begin
.note
.ix 'USE16 segments'
all "USE16" segments.
These segments are present in applications that execute in both real
mode and protected mode.
They are first in the segment ordering so that the "REALBREAK" option
of the "RUNTIME" directive can be used to separate the real-mode part
of the application from the protected-mode part of the application.
Currently, the "RUNTIME" directive is valid for Phar Lap executables
only.
.do end
.do end
:cmt. .note
:cmt. all segments not belonging to group "DGROUP" with class "BEGCODE"
.note
all segments not belonging to group "DGROUP" with class "CODE"
.note
all other segments not belonging to group "DGROUP"
.note
all segments belonging to group "DGROUP" with class "BEGDATA"
.note
all segments belonging to group "DGROUP" not with class "BEGDATA", "BSS" or
"STACK"
.note
all segments belonging to group "DGROUP" with class "BSS"
.note
all segments belonging to group "DGROUP" with class "STACK"
.endnote
.if '&exeformat' ne 'phar' .do begin
.np
A special segment belonging to class "BEGDATA" is defined when linking with
&company run-time libraries.
This segment is initialized with the hexadecimal byte pattern "01" and is
the first segment in group "DGROUP" so that storing data at location 0
can be detected.
.do end
.np
Segments belonging to class "BSS" contain uninitialized data.
Note that this only includes uninitialized data in segments belonging to
group "DGROUP".
Segments belonging to class "STACK" are used to define the size of the
stack used for your application.
Segments belonging to the classes "BSS" and "STACK" are last in the
segment ordering so that uninitialized data need not take space in the
executable file.
