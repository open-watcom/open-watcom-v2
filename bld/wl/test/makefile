OBJS = ovl1.obj ovl2.obj ovl3.obj

.c.obj:
    wcc $[@/ml/d1/oaxf/s/w4/zq

main.exe : main.obj $(OBJS)
    %create tmp.lnk
    %append tmp.lnk name main debug all sys dos option quiet,map,dynamic
    %append tmp.lnk file main
    %append tmp.lnk begin autosection
    @for %i in ($(OBJS)) do %append tmp.lnk file %i
    %append tmp.lnk end
    #%append tmp.lnk lib ..\ovlldr\wovl.lib
    %append tmp.lnk option area=0
    wlink @tmp.lnk
