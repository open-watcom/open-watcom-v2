!ifdef on_build_machine
clibdir_s = $(build_root)\clib\library\msdos.086\ms\clibs.lib
clibdir_3 = $(build_root)\clib\library\msdos.386\ms_r\clibs.lib
clibdir_q = $(build_root)\clib\library\qnx.286\ms\clibs.lib
!else
clibdir_s = $(%watcom)\lib286\dos\clibs.lib
clibdir_3 = $(%watcom)\lib386\dos\clib3r.lib
clibdir_q = $(%watcom)\lib286\qnx\clibs.lib
!endif

libs :: .symbolic
libs :: graph.lib graph386.lib graph98.lib graph983.lib graphq.lib graphq3r.lib graphq3s.lib graphp.2bj
!ifndef on_build_machine
    @!echo all done
!endif

graph.lib : ..\o.s\graphs.lib $(build_root)\pgchart\o.s\charts.lib
    set srclib=graphs
    set targlib=graph
    set chartlib=$(build_root)\pgchart\o.s\charts.lib
    set objdir=..\o.s\
    set clib=$(clibdir_s)
    set fix_seg=yes
    %make fixlib

graph386.lib : ..\o.3s\graph3s.lib ..\o.3r\graph3r.lib &
               $(build_root)\pgchart\o.3r\chart3r.lib $(build_root)\pgchart\o.3s\chart3s.lib
    copy ..\o.3s\graph3s.lib ..\o.3r\tmp.lib
    wlib /b /q ..\o.3r\tmp -globals -bit -sqrtf -std -hercfont -cgautils -egautils -vgautils -hgcutils -svgautil -font8x8
    wlib /b /q ..\o.3r\tmp +..\o.3r\graph3r.lib
    copy $(build_root)\pgchart\o.3r\chart3r.lib chart.lib
    wlib /b /q chart.lib + $(build_root)\pgchart\o.3s\chart3s.lib
    set srclib=tmp
    set targlib=graph386
    set chartlib=chart.lib
    set objdir=..\o.3r\
    set clib=$(clibdir_3)
    set fix_seg=no
    %make fixlib
    del ..\o.3r\tmp.lib
    del chart.lib

graph98.lib : ..\o.nec\graphs.lib $(build_root)\pgchart\o.s\charts.lib
    set srclib=graphs
    set targlib=graph98
    set chartlib=$(build_root)\pgchart\o.s\charts.lib
    set objdir=..\o.nec\
    set clib=$(clibdir_s)
    set fix_seg=yes
    %make fixlib

graph983.lib : ..\o.n3s\graph3s.lib ..\o.n3r\graph3r.lib &
               $(build_root)\pgchart\o.3r\chart3r.lib $(build_root)\pgchart\o.3s\chart3s.lib
    copy ..\o.n3s\graph3s.lib ..\o.n3r\tmp.lib
    wlib /b /q ..\o.n3r\tmp -globals -bit -sqrtf -std -necutils -font8x8
    wlib /b /q ..\o.n3r\tmp +..\o.n3r\graph3r.lib
    copy $(build_root)\pgchart\o.3r\chart3r.lib chart.lib
    wlib /b /q chart.lib + $(build_root)\pgchart\o.3s\chart3s.lib
    set srclib=tmp
    set targlib=graph983
    set chartlib=chart.lib
    set objdir=..\o.n3r\
    set clib=$(clibdir_3)
    set fix_seg=no
    %make fixlib
    del ..\o.n3r\tmp.lib
    del chart.lib

graphq.lib : ..\o.qs\graphs.lib $(build_root)\pgchart\o.s\charts.lib
    set srclib=graphs
    set targlib=graphq
    set chartlib=$(build_root)\pgchart\o.s\charts.lib
    set objdir=..\o.qs\
    set clib=$(clibdir_q)
    set fix_seg=yes
    %make fixlib

graphq3r.lib : ..\o.q3r\graph3r.lib $(build_root)\pgchart\o.3r\chart3r.lib
    copy ..\o.q3r\graph3r.lib graphq3r.lib
    wlib /b /q graphq3r.lib + $(build_root)\pgchart\o.3r\chart3r.lib
    objstrip /n graphq3r.lib

graphq3s.lib : ..\o.q3r\graph3r.lib $(build_root)\pgchart\o.3s\chart3s.lib
    copy ..\o.q3s\graph3s.lib graphq3s.lib
    wlib /b /q graphq3s.lib + $(build_root)\pgchart\o.3s\chart3s.lib
    objstrip /n graphq3s.lib

fixlib : .procedure
# get library - add fsmath, dummy, chart library
    copy $(%objdir)$(%srclib).lib $(%targlib).lib
    wlib /b /q $(%targlib).lib +$(%objdir)dummy +$(%objdir)fsmath +$(%chartlib)
# find external references - and check for double precision usage
    objxref /e=except $(%targlib).lib | sort >$(%targlib).ref
    wgrep -f -l __FD $(%targlib).ref >x.x
    cmp -s x.x fd.ok
    @if errorlevel 1 echo ERROR - double precision floating point used
    erase x.x
# find modules, extract, change _TEXT segment, and add to lib
# fix_seg symbol controls zapping of segment names (not for 32-bit)
    objfind @$(%targlib).ref $(%clib) >$(%targlib).mod
    sed 's/^^/*/' $(%targlib).mod >tmp.lbc
    wlib /b /q $(%clib) @tmp.lbc
    del tmp.lbc
    @if $(%fix_seg) == yes objlnam _TEXT=GRAPH_TEXT /n *.obj
    wlib /b /q $(%targlib).lib @$(%targlib).mod
# add _GR to names of all symbols in extracted files and fsmath, dummy
    objxdef *.obj $(%objdir)fsmath.obj $(%objdir)dummy.obj >$(%targlib).stb
    del *.obj
    objnames _GR* /i=$(%targlib).stb /n $(%targlib).lib
    objstrip /n $(%targlib).lib

!include ..\o.s\makefile

graphp.2bj : ..\c\seginit.c
    $(compiler) $[* @gr_extra $(cflags) /dDOSX286 /fo=graphp.2bj -bt=dos
