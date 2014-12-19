.* FAMILY OF PROCESS REFERENCES
.seelist abort atexit _bgetcmd
.if '&machsys' eq 'QNX' .do begin
.seelist close
.do end
.seelist exec&grpsfx
.seelist exit _Exit _exit getcmd getenv main onexit putenv
.if '&machsys' eq 'QNX' .do begin
.seelist sigaction signal
.do end
.seelist spawn&grpsfx
.seelist system
.if '&machsys' eq 'QNX' .do begin
.seelist wait waitpid
.do end
