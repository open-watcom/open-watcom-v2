.func _commit
.synop begin
#include <&iohdr>
int _commit( int handle );
.ixfunc2 '&FileOp' &funcb
.synop end
.desc begin
The
.id &funcb.
function commits changes to the file specified by
.arg handle
to disk immediately.
.desc end
.return begin
The
.id &funcb.
function returns &minus.1 if the changes have been successfully committed.  Otherwise, 0 is returned and
.kw errno
is set to indicate the error.
.return end
.class WATCOM
.system
