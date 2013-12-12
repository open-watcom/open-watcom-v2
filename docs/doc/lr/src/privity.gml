.if '&machsys' eq 'QNX' .do begin
.np
When you use the
.id &func.
function, your program must be linked for
privity level 1 and the process must be run by the superuser.
See the &company C/C++ User's Guide discussion of privity levels and
the documentation of the &company Linker PRIVILEGE option.
.do end
