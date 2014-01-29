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

pt.exe : $(objs)
    %create pt.lnk
    @%append pt.lnk OPTION map, verbose
    @%append pt.lnk FILE show, dont_do, check_if, do_it, fetch, quoi, set_fals, set_true
    wlink NAME $^* DEBUG $(line_or_all) @pt.lnk
    wsample /r=1 /b=20 pt.exe  2

ptd.exe : $(objs)
    %create ptd.lnk
    @%append ptd.lnk SYS dos
    @%append ptd.lnk OPTION map, verbose
    @%append ptd.lnk OPTION dynamic, area=58k
    #@%append ptd.lnk LIBPATH f:\comp\c\lib286;f:\comp\c\lib286\dos
    # root
    @%append ptd.lnk FILE quoi, check_if, set_fals
    # overlays
    @%append ptd.lnk BEGIN
    @%append ptd.lnk     SECTION FILE fetch
    @%append ptd.lnk     SECTION FILE set_true, show
    @%append ptd.lnk     SECTION FILE dont_do
    @%append ptd.lnk     SECTION FILE do_it
    @%append ptd.lnk END
    wlink NAME $^* DEBUG $(line_or_all) @ptd.lnk
    wsample /r=1 /b=20 ptd.exe 2

pt1.exe : $(objs)
    %create pt1.lnk
    @%append pt1.lnk SYS dos
    @%append pt1.lnk OPTION case, map, verbose
    # root
    @%append pt1.lnk FILE quoi, check_if, set_fals, do_it, dont_do
    # overlays
    @%append pt1.lnk BEGIN
    @%append pt1.lnk     SECTION FILE fetch
    @%append pt1.lnk     SECTION FILE set_true, show
    @%append pt1.lnk END
    wlink NAME $^* DEBUG $(line_or_all) @pt1.lnk
    wsample /r=1 /b=20 pt1.exe 2

pt2.exe : $(objs)
    %create pt2.lnk
    @%append pt2.lnk SYS dos
    @%append pt2.lnk OPTION case, map, verbose
    # root
    @%append pt2.lnk FILE quoi, check_if, set_fals
    # overlays
    @%append pt2.lnk BEGIN
    @%append pt2.lnk     SECTION FILE fetch
    @%append pt2.lnk     SECTION FILE set_true, show
    @%append pt2.lnk END
    @%append pt2.lnk BEGIN 
    @%append pt2.lnk     SECTION FILE dont_do
    @%append pt2.lnk     SECTION FILE do_it
    @%append pt2.lnk END
    wlink NAME $^* DEBUG $(line_or_all) @pt2.lnk
    wsample /r=1 /b=20 pt2.exe 2

pt3.exe : $(objs)
    %create pt3.lnk
    @%append pt3.lnk SYS dos
    @%append pt3.lnk OPTION case, map, verbose
    # root
    @%append pt3.lnk FILE quoi, check_if, set_fals, do_it, dont_do
    # overlays
    @%append pt3.lnk BEGIN
    @%append pt3.lnk     SECTION INTO aaa_3 FILE fetch
    @%append pt3.lnk     SECTION FILE set_true, show
    @%append pt3.lnk END
    wlink NAME $^* DEBUG $(line_or_all) @pt3.lnk
    wsample /r=1 /b=20 pt3.exe 2

pt4.exe : $(objs)
    %create pt4.lnk
    @%append pt4.lnk SYS dos
    @%append pt4.lnk OPTION case, map, verbose
    # root
    @%append pt4.lnk FILE quoi, check_if, set_fals, do_it, dont_do
    # overlays
    @%append pt4.lnk BEGIN
    @%append pt4.lnk     SECTION INTO aaa_4 FILE fetch
    @%append pt4.lnk     SECTION INTO bbb_4 FILE set_true, show
    @%append pt4.lnk END
    wlink NAME $^* DEBUG $(line_or_all) @pt4.lnk
    wsample /r=1 /b=20 pt4.exe 2

threaded.exe : threaded.obj
    %create threaded.lnk
    @%append threaded.lnk OPTION map, verbose
    @%append threaded.lnk SYS os2
    @%append threaded.lnk FILE threaded
    wlink NAME $^* DEBUG $(line_or_all) @threaded.lnk
