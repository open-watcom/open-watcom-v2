ideactiv.exe : ideactiv.obj
        wlink sys windows debug all op symf op map file ideactiv.obj

ideactiv.obj : ideactiv.c
        wcc -w4 -d1 -bt=windows -zw -otexan -s -fpc -ms -dSTAND_ALONE $[@
