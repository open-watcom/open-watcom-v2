.erase

objs = msdump.obj

.c.obj:
    $(comp286) $[* -ox -d1 -w3 -zq

msdump.exe : $(objs) msdump.lnk
    wlink @$]@

msdump.lnk : makefile
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ OPTION quiet
    @%append $^@ DEBUG line
    @for %i in ( $(objs) ) do @%append $^@ FILE %i
