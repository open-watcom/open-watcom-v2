.ERASE

off=
line_or_all = all
objs = &
        quoi.obj &
        fetch.obj &
        set_true.obj &
        do_it.obj  &
        set_fals.obj &
        check_if.obj &
        show.obj &
        dont_do.obj

cc = wcc

.BEFORE
        set include=$(lang_root)\h\os21x;$(%INCLUDE)

.c.obj:
        $(cc) $[@ /zq /d2 /od /ml /s /d_MARK_ON$(off)

all : .SYMBOLIC pt.exe ptd.exe pt1.exe pt2.exe pt3.exe pt4.exe threaded.exe
        @%null

pt.exe : $(objs) pt.lnk
        wlink NAME $^* DEBUG $(line_or_all) @$]@
        wsample /r=1 /b=20 pt.exe  2

ptd.exe : $(objs) ptd.lnk
        wlink NAME $^* DEBUG $(line_or_all) @$]@
        wsample /r=1 /b=20 ptd.exe 2

pt1.exe : $(objs) pt1.lnk
        wlink NAME $^* DEBUG $(line_or_all) @$]@
        wsample /r=1 /b=20 pt1.exe 2

pt2.exe : $(objs) pt2.lnk
        wlink NAME $^* DEBUG $(line_or_all) @$]@
        wsample /r=1 /b=20 pt2.exe 2

pt3.exe : $(objs) pt3.lnk
        wlink NAME $^* DEBUG $(line_or_all) @$]@
        wsample /r=1 /b=20 pt3.exe 2

pt4.exe : $(objs) pt4.lnk
        wlink NAME $^* DEBUG $(line_or_all) @$]@
        wsample /r=1 /b=20 pt4.exe 2

threaded.exe : threaded.obj threaded.lnk
        wlink NAME $^* DEBUG $(line_or_all) @$]@
