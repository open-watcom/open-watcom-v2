:cmt display all sysx, sysxx, sysxxx variables via .ty
.se alfa="abcdefghijklmnopqrstuvwxyz"

:cmt macro for sysX display
.dm x1 begin
.se v1=&'substr(&alfa,&s1,1)
.sr s1=&s1 + 1
.me
.se *a=&v1.
.se *vx=sys&*a.
.if "&'right(&sys&*a.,4)" ne "&*vx." .ty &*vx. = &&*vx.
.dm x1 end

:cmt macro for sysXX display
.dm x2 begin
.se v2=&'substr(&alfa,&s2,1)
.sr s2=&s2 + 1
.me
.se *a=&v1.&v2.
.se *vx=sys&*a.
.if &*a eq "cw" .do begin
.   .ty syscw = &&*vx.
.me
.do end
.if |&'right(&sys&*a.,5)| ne "&*vx." .ty &*vx. = &&*vx.
.dm x2 end

:cmt macro for sysXXX display
.dm x3 begin
.se v3=&'substr(&alfa,&s3,1)
.sr s3=&s3 + 1
.me
.se *a=&v1.&v2.&v3.
.se *vx=sys&*a.
.if "&'right(&sys&*a.,6)" ne "&*vx." .ty &*vx. = &&*vx.
.dm x3 end

:cmt macro for sysXXXX display
.dm x4 begin
.se v4=&'substr(&alfa,&s4,1)
.sr s4=&s4 + 1
.me
.se *a=&v1.&v2.&v3.&v4.
.se *vx=sys&*a.
.if "&'right(&sys&*a.,7)" ne "&*vx." .ty &*vx. = &&*vx.
.dm x4 end

:cmt macro for sysXXXXX display
.dm x5 begin
.se v5=&'substr(&alfa,&s5,1)
.sr s5=&s5 + 1
.se *a=&v1.&v2.&v3.&v4.&v5.
.se *vx=sys&*a.
.if "&'right(&sys&*a.,8)" ne "&*vx." .ty &*vx. = &&*vx.
.dm x5 end

:cmt macro for sysXXXXXX display
.dm x6 begin
.se *a=&v1.&v2.&v3.&v4.&v5.&'substr(&alfa,&s6,1)
.sr s6=&s6 + 1
.se *vx=sys&*a.
.if "&'right(&sys&*a.,9)" ne "&*vx." .ty &*vx. = &&*vx.
.dm x6 end

:gdoc.
.* set e to last index (for z)
.se e=26
.* set sx to first index (for a)
.se s1=16
.ty ------------------ please wait, it can take HOURS
.* no more comments to speed up processing
...l1
.x1
.se s2=1
...l2
.x2
.se s3=1
...l3
.x3
.se s4=1
...l4
.x4
.se s5=1
...l5
.x5
.se s6=1
...l6
.x6
.if &s6 le &e .go l6
.if &s5 le &e .go l5
.if &s4 le &e .go l4
.if &s3 le &e .go l3
.if &s2 le &e .go l2
.if &s1 le &e .go l1
.ty ------------------- that's all
:egdoc.
