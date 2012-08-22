# SRC Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <DEVDIR>/clib/startup/a/cstrt386.asm        <OWRELROOT>/src/startup/386/cstrt386.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrtw32.asm        <OWRELROOT>/src/startup/386/cstrtw32.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrtwnt.asm        <OWRELROOT>/src/startup/386/cstrtwnt.asm
#    <CPCMD> <DEVDIR>/clib/startup/a/cstrtwwt.asm        <OWRELROOT>/src/startup/386/cstrtwwt.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrtx32.asm        <OWRELROOT>/src/startup/386/cstrtx32.asm
    <CPCMD> <DEVDIR>/clib/startup/c/cmain386.c          <OWRELROOT>/src/startup/386/cmain386.c
    <CPCMD> <DEVDIR>/clib/startup/c/wildargv.c          <OWRELROOT>/src/startup/wildargv.c
    <CPCMD> <DEVDIR>/clib/h/initarg.h                   <OWRELROOT>/src/startup/initarg.h
    <CPCMD> <DEVDIR>/clib/startup/a/cstrt086.asm        <OWRELROOT>/src/startup/dos/cstrt086.asm
    <CPCMD> <DEVDIR>/clib/startup/a/dos16m.asm          <OWRELROOT>/src/startup/dos/dos16m.asm
    <CPCMD> <DEVDIR>/clib/startup/a/dllstart.asm        <OWRELROOT>/src/startup/386/dllstart.asm
    <CPCMD> <DEVDIR>/clib/startup/a/dstrt386.asm        <OWRELROOT>/src/startup/386/dstrt386.asm
    <CPCMD> <DEVDIR>/clib/startup/c/cmain086.c          <OWRELROOT>/src/startup/dos/cmain086.c
    <CPCMD> <DEVDIR>/clib/startup/c/cmain086.c          <OWRELROOT>/src/startup/os2/cmain086.c
    <CPCMD> <DEVDIR>/clib/startup/c/hpgrw086.c          <OWRELROOT>/src/startup/dos/hpgrw086.c
    <CPCMD> <DEVDIR>/clib/startup/c/dmaino16.c          <OWRELROOT>/src/startup/os2/dmaino16.c
    <CPCMD> <DEVDIR>/clib/startup/c/libmno16.c          <OWRELROOT>/src/startup/os2/libmno16.c
    <CPCMD> <DEVDIR>/clib/startup/c/maino16.c           <OWRELROOT>/src/startup/os2/maino16.c
    <CPCMD> <DEVDIR>/clib/startup/c/main2o32.c          <OWRELROOT>/src/startup/386/main2o32.c
    <CPCMD> <DEVDIR>/clib/startup/c/libmno32.c          <OWRELROOT>/src/startup/386/libmno32.c
    <CPCMD> <DEVDIR>/clib/startup/c/dmaino32.c          <OWRELROOT>/src/startup/386/dmaino32.c
    <CPCMD> <DEVDIR>/clib/startup/c/main2wnt.c          <OWRELROOT>/src/startup/386/main2wnt.c
    <CPCMD> <DEVDIR>/clib/startup/c/wmainwnt.c          <OWRELROOT>/src/startup/386/wmainwnt.c
    <CPCMD> <DEVDIR>/clib/startup/c/lmainwnt.c          <OWRELROOT>/src/startup/386/lmainwnt.c
    <CPCMD> <DEVDIR>/clib/startup/c/dmainwnt.c          <OWRELROOT>/src/startup/386/dmainwnt.c
    <CPCMD> <DEVDIR>/clib/startup/c/lmn2wnt.c           <OWRELROOT>/src/startup/386/lmn2wnt.c
    <CPCMD> <DEVDIR>/clib/startup/a/cstrto16.asm        <OWRELROOT>/src/startup/os2/cstrto16.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrto32.asm        <OWRELROOT>/src/startup/386/cstrto32.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrtw16.asm        <OWRELROOT>/src/startup/win/cstrtw16.asm
    <CPCMD> <DEVDIR>/clib/startup/a/libentry.asm        <OWRELROOT>/src/startup/win/libentry.asm
    <CPCMD> <DEVDIR>/clib/startup/c/8087cw.c            <OWRELROOT>/src/startup/8087cw.c
    <CPCMD> <DEVDIR>/lib_misc/h/exitwmsg.h              <OWRELROOT>/src/startup/os2/exitwmsg.h
    <CPCMD> <DEVDIR>/clib/startup/h/initfini.h          <OWRELROOT>/src/startup/386/initfini.h
    <CPCMD> <DEVDIR>/clib/startup/h/initfini.h          <OWRELROOT>/src/startup/os2/initfini.h
    <CPCMD> <DEVDIR>/watcom/h/wos2.h                    <OWRELROOT>/src/startup/os2/wos2.h
    <CPCMD> <DEVDIR>/watcom/h/mdef.inc                  <OWRELROOT>/src/startup/mdef.inc
    <CPCMD> <DEVDIR>/watcom/h/xinit.inc                 <OWRELROOT>/src/startup/xinit.inc
    <CPCMD> <DEVDIR>/comp_cfg/h/langenv.inc             <OWRELROOT>/src/startup/langenv.inc
    <CPCMD> <DEVDIR>/clib/startup/a/msgrt16.inc         <OWRELROOT>/src/startup/msgrt16.inc
    <CPCMD> <DEVDIR>/clib/startup/a/msgrt32.inc         <OWRELROOT>/src/startup/msgrt32.inc
    <CPCMD> <DEVDIR>/clib/startup/a/msgcpyrt.inc        <OWRELROOT>/src/startup/msgcpyrt.inc

    <CPCMD> <DEVDIR>/src/misc/*                         <OWRELROOT>/src/

    <CPCMD> <DEVDIR>/cpplib/contain/cpp/*               <OWRELROOT>/src/cpplib/contain/

    <CPCMD> <DEVDIR>/src/clibexam/*                     <OWRELROOT>/samples/clibexam/
    <CPCMD> <DEVDIR>/src/cplbexam/*                     <OWRELROOT>/samples/cplbexam/

    <CPCMD> <DEVDIR>/src/cppexamp/*                     <OWRELROOT>/samples/cppexamp/
    <CPCMD> <DEVDIR>/src/cppexamp/excarea/*             <OWRELROOT>/samples/cppexamp/excarea/
    <CPCMD> <DEVDIR>/src/cppexamp/membfun/*             <OWRELROOT>/samples/cppexamp/membfun/
    <CPCMD> <DEVDIR>/src/cppexamp/rtti/*                <OWRELROOT>/samples/cppexamp/rtti/

    <CPCMD> <DEVDIR>/src/dll/*                          <OWRELROOT>/samples/dll/
    <CPCMD> <DEVDIR>/src/goodies/*                      <OWRELROOT>/samples/goodies/
    <CPCMD> <DEVDIR>/src/os2/*                          <OWRELROOT>/samples/os2/
    <CPCMD> <DEVDIR>/src/os2/dll/*                      <OWRELROOT>/samples/os2/dll/
    <CPCMD> <DEVDIR>/src/os2/pdd/*                      <OWRELROOT>/samples/os2/pdd/
    <CPCMD> <DEVDIR>/src/os2/som/*                      <OWRELROOT>/samples/os2/som/
    <CPCMD> <DEVDIR>/src/os2/som/animals/*              <OWRELROOT>/samples/os2/som/animals/
    <CPCMD> <DEVDIR>/src/os2/som/classes/*              <OWRELROOT>/samples/os2/som/classes/
    <CPCMD> <DEVDIR>/src/os2/som/helloc/*               <OWRELROOT>/samples/os2/som/helloc/
    <CPCMD> <DEVDIR>/src/os2/som/hellocpp/*             <OWRELROOT>/samples/os2/som/hellocpp/
    <CPCMD> <DEVDIR>/src/os2/som/wps/*                  <OWRELROOT>/samples/os2/som/wps/
    <CPCMD> <DEVDIR>/src/win/*                          <OWRELROOT>/samples/win/
    <CPCMD> <DEVDIR>/src/clibexam/kanji/*               <OWRELROOT>/samples/clibexam/kanji/
    <CPCMD> <DEVDIR>/src/clibexam/test/*                <OWRELROOT>/samples/clibexam/test/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/*             <OWRELROOT>/samples/cplbexam/complex/
    <CPCMD> <DEVDIR>/src/cplbexam/contain/*             <OWRELROOT>/samples/cplbexam/contain/
    <CPCMD> <DEVDIR>/src/cplbexam/fstream/*             <OWRELROOT>/samples/cplbexam/fstream/
    <CPCMD> <DEVDIR>/src/cplbexam/ios/*                 <OWRELROOT>/samples/cplbexam/ios/
    <CPCMD> <DEVDIR>/src/cplbexam/iostream/*            <OWRELROOT>/samples/cplbexam/iostream/
    <CPCMD> <DEVDIR>/src/cplbexam/string/*              <OWRELROOT>/samples/cplbexam/string/
    <CPCMD> <DEVDIR>/src/cplbexam/strstrea/*            <OWRELROOT>/samples/cplbexam/strstrea/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/friend/*      <OWRELROOT>/samples/cplbexam/complex/friend/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/pubfun/*      <OWRELROOT>/samples/cplbexam/complex/pubfun/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/relfun/*      <OWRELROOT>/samples/cplbexam/complex/relfun/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/relop/*       <OWRELROOT>/samples/cplbexam/complex/relop/
    <CPCMD> <DEVDIR>/src/cplbexam/fstream/fstream/*     <OWRELROOT>/samples/cplbexam/fstream/fstream/
    <CPCMD> <DEVDIR>/src/cplbexam/fstream/ifstream/*    <OWRELROOT>/samples/cplbexam/fstream/ifstream/
    <CPCMD> <DEVDIR>/src/cplbexam/fstream/ofstream/*    <OWRELROOT>/samples/cplbexam/fstream/ofstream/
    <CPCMD> <DEVDIR>/src/cplbexam/iostream/iostream/*   <OWRELROOT>/samples/cplbexam/iostream/iostream/
    <CPCMD> <DEVDIR>/src/cplbexam/iostream/istream/*    <OWRELROOT>/samples/cplbexam/iostream/istream/
    <CPCMD> <DEVDIR>/src/cplbexam/iostream/ostream/*    <OWRELROOT>/samples/cplbexam/iostream/ostream/
    <CPCMD> <DEVDIR>/src/cplbexam/string/friend/*       <OWRELROOT>/samples/cplbexam/string/friend/
    <CPCMD> <DEVDIR>/src/cplbexam/string/pubfun/*       <OWRELROOT>/samples/cplbexam/string/pubfun/
    <CPCMD> <DEVDIR>/src/cplbexam/strstrea/istrstre/*   <OWRELROOT>/samples/cplbexam/strstrea/istrstre/
    <CPCMD> <DEVDIR>/src/cplbexam/strstrea/ostrstre/*   <OWRELROOT>/samples/cplbexam/strstrea/ostrstre/
    <CPCMD> <DEVDIR>/src/cplbexam/strstrea/strstre/*    <OWRELROOT>/samples/cplbexam/strstrea/strstre/
    <CPCMD> <DEVDIR>/src/win/alarm/*                    <OWRELROOT>/samples/win/alarm/
    <CPCMD> <DEVDIR>/src/win/datactl/*                  <OWRELROOT>/samples/win/datactl/
    <CPCMD> <DEVDIR>/src/win/edit/*                     <OWRELROOT>/samples/win/edit/
    <CPCMD> <DEVDIR>/src/win/generic/*                  <OWRELROOT>/samples/win/generic/
    <CPCMD> <DEVDIR>/src/win/helpex/*                   <OWRELROOT>/samples/win/helpex/
    <CPCMD> <DEVDIR>/src/win/iconview/*                 <OWRELROOT>/samples/win/iconview/
    <CPCMD> <DEVDIR>/src/win/life/*                     <OWRELROOT>/samples/win/life/
    <CPCMD> <DEVDIR>/src/win/shootgal/*                 <OWRELROOT>/samples/win/shootgal/
    <CPCMD> <DEVDIR>/src/win/watzee/*                   <OWRELROOT>/samples/win/watzee/
    <CPCMD> <DEVDIR>/src/win/alarm/win16/*              <OWRELROOT>/samples/win/alarm/win16/
    <CPCMD> <DEVDIR>/src/win/alarm/win32/*              <OWRELROOT>/samples/win/alarm/win32/
    <CPCMD> <DEVDIR>/src/win/alarm/win386/*             <OWRELROOT>/samples/win/alarm/win386/
    <CPCMD> <DEVDIR>/src/win/alarm/winaxp/*             <OWRELROOT>/samples/win/alarm/winaxp/
    <CPCMD> <DEVDIR>/src/win/datactl/win16/*            <OWRELROOT>/samples/win/datactl/win16/
    <CPCMD> <DEVDIR>/src/win/datactl/win32/*            <OWRELROOT>/samples/win/datactl/win32/
    <CPCMD> <DEVDIR>/src/win/datactl/win386/*           <OWRELROOT>/samples/win/datactl/win386/
    <CPCMD> <DEVDIR>/src/win/datactl/winaxp/*           <OWRELROOT>/samples/win/datactl/winaxp/
    <CPCMD> <DEVDIR>/src/win/edit/win16/*               <OWRELROOT>/samples/win/edit/win16/
    <CPCMD> <DEVDIR>/src/win/edit/win32/*               <OWRELROOT>/samples/win/edit/win32/
    <CPCMD> <DEVDIR>/src/win/edit/win386/*              <OWRELROOT>/samples/win/edit/win386/
    <CPCMD> <DEVDIR>/src/win/edit/winaxp/*              <OWRELROOT>/samples/win/edit/winaxp/
    <CPCMD> <DEVDIR>/src/win/generic/win16/*            <OWRELROOT>/samples/win/generic/win16/
    <CPCMD> <DEVDIR>/src/win/generic/win32/*            <OWRELROOT>/samples/win/generic/win32/
    <CPCMD> <DEVDIR>/src/win/generic/win386/*           <OWRELROOT>/samples/win/generic/win386/
    <CPCMD> <DEVDIR>/src/win/generic/winaxp/*           <OWRELROOT>/samples/win/generic/winaxp/
    <CPCMD> <DEVDIR>/src/win/helpex/win16/*             <OWRELROOT>/samples/win/helpex/win16/
    <CPCMD> <DEVDIR>/src/win/helpex/win32/*             <OWRELROOT>/samples/win/helpex/win32/
    <CPCMD> <DEVDIR>/src/win/helpex/win386/*            <OWRELROOT>/samples/win/helpex/win386/
    <CPCMD> <DEVDIR>/src/win/helpex/winaxp/*            <OWRELROOT>/samples/win/helpex/winaxp/
    <CPCMD> <DEVDIR>/src/win/iconview/win16/*           <OWRELROOT>/samples/win/iconview/win16/
    <CPCMD> <DEVDIR>/src/win/iconview/win32/*           <OWRELROOT>/samples/win/iconview/win32/
    <CPCMD> <DEVDIR>/src/win/iconview/win386/*          <OWRELROOT>/samples/win/iconview/win386/
    <CPCMD> <DEVDIR>/src/win/iconview/winaxp/*          <OWRELROOT>/samples/win/iconview/winaxp/
    <CPCMD> <DEVDIR>/src/win/life/win16/*               <OWRELROOT>/samples/win/life/win16/
    <CPCMD> <DEVDIR>/src/win/life/win32/*               <OWRELROOT>/samples/win/life/win32/
    <CPCMD> <DEVDIR>/src/win/life/win386/*              <OWRELROOT>/samples/win/life/win386/
    <CPCMD> <DEVDIR>/src/win/life/winaxp/*              <OWRELROOT>/samples/win/life/winaxp/
    <CPCMD> <DEVDIR>/src/win/shootgal/win16/*           <OWRELROOT>/samples/win/shootgal/win16/
    <CPCMD> <DEVDIR>/src/win/shootgal/win32/*           <OWRELROOT>/samples/win/shootgal/win32/
    <CPCMD> <DEVDIR>/src/win/shootgal/win386/*          <OWRELROOT>/samples/win/shootgal/win386/
    <CPCMD> <DEVDIR>/src/win/shootgal/winaxp/*          <OWRELROOT>/samples/win/shootgal/winaxp/
    <CPCMD> <DEVDIR>/src/win/watzee/win16/*             <OWRELROOT>/samples/win/watzee/win16/
    <CPCMD> <DEVDIR>/src/win/watzee/win32/*             <OWRELROOT>/samples/win/watzee/win32/
    <CPCMD> <DEVDIR>/src/win/watzee/win386/*            <OWRELROOT>/samples/win/watzee/win386/
    <CPCMD> <DEVDIR>/src/win/watzee/winaxp/*            <OWRELROOT>/samples/win/watzee/winaxp/

    <CPCMD> <DEVDIR>/src/directx/c/d3d/*                <OWRELROOT>/samples/directx/c/d3d/
    <CPCMD> <DEVDIR>/src/directx/c/dinput/*             <OWRELROOT>/samples/directx/c/dinput/
    <CPCMD> <DEVDIR>/src/directx/c/dshow/*              <OWRELROOT>/samples/directx/c/dshow/
    <CPCMD> <DEVDIR>/src/directx/c/dsound/*             <OWRELROOT>/samples/directx/c/dsound/
    <CPCMD> <DEVDIR>/src/directx/cpp/d3d/*              <OWRELROOT>/samples/directx/cpp/d3d/
    <CPCMD> <DEVDIR>/src/directx/cpp/dinput/*           <OWRELROOT>/samples/directx/cpp/dinput/
    <CPCMD> <DEVDIR>/src/directx/cpp/dshow/*            <OWRELROOT>/samples/directx/cpp/dshow/
    <CPCMD> <DEVDIR>/src/directx/cpp/dsound/*           <OWRELROOT>/samples/directx/cpp/dsound/
    <CPCMD> <DEVDIR>/src/directx/*                      <OWRELROOT>/samples/directx/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
