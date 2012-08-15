# SRC Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
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
    <CPCMD> <DEVDIR>/clib/startup/a/cstrt386.asm        <RELROOT>/src/startup/386/cstrt386.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrtw32.asm        <RELROOT>/src/startup/386/cstrtw32.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrtwnt.asm        <RELROOT>/src/startup/386/cstrtwnt.asm
#    <CPCMD> <DEVDIR>/clib/startup/a/cstrtwwt.asm        <RELROOT>/src/startup/386/cstrtwwt.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrtx32.asm        <RELROOT>/src/startup/386/cstrtx32.asm
    <CPCMD> <DEVDIR>/clib/startup/c/cmain386.c          <RELROOT>/src/startup/386/cmain386.c
    <CPCMD> <DEVDIR>/clib/startup/c/wildargv.c          <RELROOT>/src/startup/wildargv.c
    <CPCMD> <DEVDIR>/clib/h/initarg.h                   <RELROOT>/src/startup/initarg.h
    <CPCMD> <DEVDIR>/clib/startup/a/cstrt086.asm        <RELROOT>/src/startup/dos/cstrt086.asm
    <CPCMD> <DEVDIR>/clib/startup/a/dos16m.asm          <RELROOT>/src/startup/dos/dos16m.asm
    <CPCMD> <DEVDIR>/clib/startup/a/dllstart.asm        <RELROOT>/src/startup/386/dllstart.asm
    <CPCMD> <DEVDIR>/clib/startup/a/dstrt386.asm        <RELROOT>/src/startup/386/dstrt386.asm
    <CPCMD> <DEVDIR>/clib/startup/c/cmain086.c          <RELROOT>/src/startup/dos/cmain086.c
    <CPCMD> <DEVDIR>/clib/startup/c/cmain086.c          <RELROOT>/src/startup/os2/cmain086.c
    <CPCMD> <DEVDIR>/clib/startup/c/hpgrw086.c          <RELROOT>/src/startup/dos/hpgrw086.c
    <CPCMD> <DEVDIR>/clib/startup/c/dmaino16.c          <RELROOT>/src/startup/os2/dmaino16.c
    <CPCMD> <DEVDIR>/clib/startup/c/libmno16.c          <RELROOT>/src/startup/os2/libmno16.c
    <CPCMD> <DEVDIR>/clib/startup/c/maino16.c           <RELROOT>/src/startup/os2/maino16.c
    <CPCMD> <DEVDIR>/clib/startup/c/main2o32.c          <RELROOT>/src/startup/386/main2o32.c
    <CPCMD> <DEVDIR>/clib/startup/c/libmno32.c          <RELROOT>/src/startup/386/libmno32.c
    <CPCMD> <DEVDIR>/clib/startup/c/dmaino32.c          <RELROOT>/src/startup/386/dmaino32.c
    <CPCMD> <DEVDIR>/clib/startup/c/main2wnt.c          <RELROOT>/src/startup/386/main2wnt.c
    <CPCMD> <DEVDIR>/clib/startup/c/wmainwnt.c          <RELROOT>/src/startup/386/wmainwnt.c
    <CPCMD> <DEVDIR>/clib/startup/c/lmainwnt.c          <RELROOT>/src/startup/386/lmainwnt.c
    <CPCMD> <DEVDIR>/clib/startup/c/dmainwnt.c          <RELROOT>/src/startup/386/dmainwnt.c
    <CPCMD> <DEVDIR>/clib/startup/c/lmn2wnt.c           <RELROOT>/src/startup/386/lmn2wnt.c
    <CPCMD> <DEVDIR>/clib/startup/a/cstrto16.asm        <RELROOT>/src/startup/os2/cstrto16.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrto32.asm        <RELROOT>/src/startup/386/cstrto32.asm
    <CPCMD> <DEVDIR>/clib/startup/a/cstrtw16.asm        <RELROOT>/src/startup/win/cstrtw16.asm
    <CPCMD> <DEVDIR>/clib/startup/a/libentry.asm        <RELROOT>/src/startup/win/libentry.asm
    <CPCMD> <DEVDIR>/clib/startup/c/8087cw.c            <RELROOT>/src/startup/8087cw.c
    <CPCMD> <DEVDIR>/lib_misc/h/exitwmsg.h              <RELROOT>/src/startup/os2/exitwmsg.h
    <CPCMD> <DEVDIR>/clib/startup/h/initfini.h          <RELROOT>/src/startup/386/initfini.h
    <CPCMD> <DEVDIR>/clib/startup/h/initfini.h          <RELROOT>/src/startup/os2/initfini.h
    <CPCMD> <DEVDIR>/watcom/h/wos2.h                    <RELROOT>/src/startup/os2/wos2.h
    <CPCMD> <DEVDIR>/watcom/h/mdef.inc                  <RELROOT>/src/startup/mdef.inc
    <CPCMD> <DEVDIR>/watcom/h/xinit.inc                 <RELROOT>/src/startup/xinit.inc
    <CPCMD> <DEVDIR>/comp_cfg/h/langenv.inc             <RELROOT>/src/startup/langenv.inc
    <CPCMD> <DEVDIR>/clib/startup/a/msgrt16.inc         <RELROOT>/src/startup/msgrt16.inc
    <CPCMD> <DEVDIR>/clib/startup/a/msgrt32.inc         <RELROOT>/src/startup/msgrt32.inc
    <CPCMD> <DEVDIR>/clib/startup/a/msgcpyrt.inc        <RELROOT>/src/startup/msgcpyrt.inc

    <CPCMD> <DEVDIR>/src/misc/*                         <RELROOT>/src/

    <CPCMD> <DEVDIR>/cpplib/contain/cpp/*               <RELROOT>/src/cpplib/contain/

    <CPCMD> <DEVDIR>/src/clibexam/*                     <RELROOT>/samples/clibexam/
    <CPCMD> <DEVDIR>/src/cplbexam/*                     <RELROOT>/samples/cplbexam/

    <CPCMD> <DEVDIR>/src/cppexamp/*                     <RELROOT>/samples/cppexamp/
    <CPCMD> <DEVDIR>/src/cppexamp/excarea/*             <RELROOT>/samples/cppexamp/excarea/
    <CPCMD> <DEVDIR>/src/cppexamp/membfun/*             <RELROOT>/samples/cppexamp/membfun/
    <CPCMD> <DEVDIR>/src/cppexamp/rtti/*                <RELROOT>/samples/cppexamp/rtti/

    <CPCMD> <DEVDIR>/src/dll/*                          <RELROOT>/samples/dll/
    <CPCMD> <DEVDIR>/src/goodies/*                      <RELROOT>/samples/goodies/
    <CPCMD> <DEVDIR>/src/os2/*                          <RELROOT>/samples/os2/
    <CPCMD> <DEVDIR>/src/os2/dll/*                      <RELROOT>/samples/os2/dll/
    <CPCMD> <DEVDIR>/src/os2/pdd/*                      <RELROOT>/samples/os2/pdd/
    <CPCMD> <DEVDIR>/src/os2/som/*                      <RELROOT>/samples/os2/som/
    <CPCMD> <DEVDIR>/src/os2/som/animals/*              <RELROOT>/samples/os2/som/animals/
    <CPCMD> <DEVDIR>/src/os2/som/classes/*              <RELROOT>/samples/os2/som/classes/
    <CPCMD> <DEVDIR>/src/os2/som/helloc/*               <RELROOT>/samples/os2/som/helloc/
    <CPCMD> <DEVDIR>/src/os2/som/hellocpp/*             <RELROOT>/samples/os2/som/hellocpp/
    <CPCMD> <DEVDIR>/src/os2/som/wps/*                  <RELROOT>/samples/os2/som/wps/
    <CPCMD> <DEVDIR>/src/win/*                          <RELROOT>/samples/win/
    <CPCMD> <DEVDIR>/src/clibexam/kanji/*               <RELROOT>/samples/clibexam/kanji/
    <CPCMD> <DEVDIR>/src/clibexam/test/*                <RELROOT>/samples/clibexam/test/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/*             <RELROOT>/samples/cplbexam/complex/
    <CPCMD> <DEVDIR>/src/cplbexam/contain/*             <RELROOT>/samples/cplbexam/contain/
    <CPCMD> <DEVDIR>/src/cplbexam/fstream/*             <RELROOT>/samples/cplbexam/fstream/
    <CPCMD> <DEVDIR>/src/cplbexam/ios/*                 <RELROOT>/samples/cplbexam/ios/
    <CPCMD> <DEVDIR>/src/cplbexam/iostream/*            <RELROOT>/samples/cplbexam/iostream/
    <CPCMD> <DEVDIR>/src/cplbexam/string/*              <RELROOT>/samples/cplbexam/string/
    <CPCMD> <DEVDIR>/src/cplbexam/strstrea/*            <RELROOT>/samples/cplbexam/strstrea/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/friend/*      <RELROOT>/samples/cplbexam/complex/friend/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/pubfun/*      <RELROOT>/samples/cplbexam/complex/pubfun/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/relfun/*      <RELROOT>/samples/cplbexam/complex/relfun/
    <CPCMD> <DEVDIR>/src/cplbexam/complex/relop/*       <RELROOT>/samples/cplbexam/complex/relop/
    <CPCMD> <DEVDIR>/src/cplbexam/fstream/fstream/*     <RELROOT>/samples/cplbexam/fstream/fstream/
    <CPCMD> <DEVDIR>/src/cplbexam/fstream/ifstream/*    <RELROOT>/samples/cplbexam/fstream/ifstream/
    <CPCMD> <DEVDIR>/src/cplbexam/fstream/ofstream/*    <RELROOT>/samples/cplbexam/fstream/ofstream/
    <CPCMD> <DEVDIR>/src/cplbexam/iostream/iostream/*   <RELROOT>/samples/cplbexam/iostream/iostream/
    <CPCMD> <DEVDIR>/src/cplbexam/iostream/istream/*    <RELROOT>/samples/cplbexam/iostream/istream/
    <CPCMD> <DEVDIR>/src/cplbexam/iostream/ostream/*    <RELROOT>/samples/cplbexam/iostream/ostream/
    <CPCMD> <DEVDIR>/src/cplbexam/string/friend/*       <RELROOT>/samples/cplbexam/string/friend/
    <CPCMD> <DEVDIR>/src/cplbexam/string/pubfun/*       <RELROOT>/samples/cplbexam/string/pubfun/
    <CPCMD> <DEVDIR>/src/cplbexam/strstrea/istrstre/*   <RELROOT>/samples/cplbexam/strstrea/istrstre/
    <CPCMD> <DEVDIR>/src/cplbexam/strstrea/ostrstre/*   <RELROOT>/samples/cplbexam/strstrea/ostrstre/
    <CPCMD> <DEVDIR>/src/cplbexam/strstrea/strstre/*    <RELROOT>/samples/cplbexam/strstrea/strstre/
    <CPCMD> <DEVDIR>/src/win/alarm/*                    <RELROOT>/samples/win/alarm/
    <CPCMD> <DEVDIR>/src/win/datactl/*                  <RELROOT>/samples/win/datactl/
    <CPCMD> <DEVDIR>/src/win/edit/*                     <RELROOT>/samples/win/edit/
    <CPCMD> <DEVDIR>/src/win/generic/*                  <RELROOT>/samples/win/generic/
    <CPCMD> <DEVDIR>/src/win/helpex/*                   <RELROOT>/samples/win/helpex/
    <CPCMD> <DEVDIR>/src/win/iconview/*                 <RELROOT>/samples/win/iconview/
    <CPCMD> <DEVDIR>/src/win/life/*                     <RELROOT>/samples/win/life/
    <CPCMD> <DEVDIR>/src/win/shootgal/*                 <RELROOT>/samples/win/shootgal/
    <CPCMD> <DEVDIR>/src/win/watzee/*                   <RELROOT>/samples/win/watzee/
    <CPCMD> <DEVDIR>/src/win/alarm/win16/*              <RELROOT>/samples/win/alarm/win16/
    <CPCMD> <DEVDIR>/src/win/alarm/win32/*              <RELROOT>/samples/win/alarm/win32/
    <CPCMD> <DEVDIR>/src/win/alarm/win386/*             <RELROOT>/samples/win/alarm/win386/
    <CPCMD> <DEVDIR>/src/win/alarm/winaxp/*             <RELROOT>/samples/win/alarm/winaxp/
    <CPCMD> <DEVDIR>/src/win/datactl/win16/*            <RELROOT>/samples/win/datactl/win16/
    <CPCMD> <DEVDIR>/src/win/datactl/win32/*            <RELROOT>/samples/win/datactl/win32/
    <CPCMD> <DEVDIR>/src/win/datactl/win386/*           <RELROOT>/samples/win/datactl/win386/
    <CPCMD> <DEVDIR>/src/win/datactl/winaxp/*           <RELROOT>/samples/win/datactl/winaxp/
    <CPCMD> <DEVDIR>/src/win/edit/win16/*               <RELROOT>/samples/win/edit/win16/
    <CPCMD> <DEVDIR>/src/win/edit/win32/*               <RELROOT>/samples/win/edit/win32/
    <CPCMD> <DEVDIR>/src/win/edit/win386/*              <RELROOT>/samples/win/edit/win386/
    <CPCMD> <DEVDIR>/src/win/edit/winaxp/*              <RELROOT>/samples/win/edit/winaxp/
    <CPCMD> <DEVDIR>/src/win/generic/win16/*            <RELROOT>/samples/win/generic/win16/
    <CPCMD> <DEVDIR>/src/win/generic/win32/*            <RELROOT>/samples/win/generic/win32/
    <CPCMD> <DEVDIR>/src/win/generic/win386/*           <RELROOT>/samples/win/generic/win386/
    <CPCMD> <DEVDIR>/src/win/generic/winaxp/*           <RELROOT>/samples/win/generic/winaxp/
    <CPCMD> <DEVDIR>/src/win/helpex/win16/*             <RELROOT>/samples/win/helpex/win16/
    <CPCMD> <DEVDIR>/src/win/helpex/win32/*             <RELROOT>/samples/win/helpex/win32/
    <CPCMD> <DEVDIR>/src/win/helpex/win386/*            <RELROOT>/samples/win/helpex/win386/
    <CPCMD> <DEVDIR>/src/win/helpex/winaxp/*            <RELROOT>/samples/win/helpex/winaxp/
    <CPCMD> <DEVDIR>/src/win/iconview/win16/*           <RELROOT>/samples/win/iconview/win16/
    <CPCMD> <DEVDIR>/src/win/iconview/win32/*           <RELROOT>/samples/win/iconview/win32/
    <CPCMD> <DEVDIR>/src/win/iconview/win386/*          <RELROOT>/samples/win/iconview/win386/
    <CPCMD> <DEVDIR>/src/win/iconview/winaxp/*          <RELROOT>/samples/win/iconview/winaxp/
    <CPCMD> <DEVDIR>/src/win/life/win16/*               <RELROOT>/samples/win/life/win16/
    <CPCMD> <DEVDIR>/src/win/life/win32/*               <RELROOT>/samples/win/life/win32/
    <CPCMD> <DEVDIR>/src/win/life/win386/*              <RELROOT>/samples/win/life/win386/
    <CPCMD> <DEVDIR>/src/win/life/winaxp/*              <RELROOT>/samples/win/life/winaxp/
    <CPCMD> <DEVDIR>/src/win/shootgal/win16/*           <RELROOT>/samples/win/shootgal/win16/
    <CPCMD> <DEVDIR>/src/win/shootgal/win32/*           <RELROOT>/samples/win/shootgal/win32/
    <CPCMD> <DEVDIR>/src/win/shootgal/win386/*          <RELROOT>/samples/win/shootgal/win386/
    <CPCMD> <DEVDIR>/src/win/shootgal/winaxp/*          <RELROOT>/samples/win/shootgal/winaxp/
    <CPCMD> <DEVDIR>/src/win/watzee/win16/*             <RELROOT>/samples/win/watzee/win16/
    <CPCMD> <DEVDIR>/src/win/watzee/win32/*             <RELROOT>/samples/win/watzee/win32/
    <CPCMD> <DEVDIR>/src/win/watzee/win386/*            <RELROOT>/samples/win/watzee/win386/
    <CPCMD> <DEVDIR>/src/win/watzee/winaxp/*            <RELROOT>/samples/win/watzee/winaxp/

    <CPCMD> <DEVDIR>/src/directx/c/d3d/*                <RELROOT>/samples/directx/c/d3d/
    <CPCMD> <DEVDIR>/src/directx/c/dinput/*             <RELROOT>/samples/directx/c/dinput/
    <CPCMD> <DEVDIR>/src/directx/c/dshow/*              <RELROOT>/samples/directx/c/dshow/
    <CPCMD> <DEVDIR>/src/directx/c/dsound/*             <RELROOT>/samples/directx/c/dsound/
    <CPCMD> <DEVDIR>/src/directx/cpp/d3d/*              <RELROOT>/samples/directx/cpp/d3d/
    <CPCMD> <DEVDIR>/src/directx/cpp/dinput/*           <RELROOT>/samples/directx/cpp/dinput/
    <CPCMD> <DEVDIR>/src/directx/cpp/dshow/*            <RELROOT>/samples/directx/cpp/dshow/
    <CPCMD> <DEVDIR>/src/directx/cpp/dsound/*           <RELROOT>/samples/directx/cpp/dsound/
    <CPCMD> <DEVDIR>/src/directx/*                      <RELROOT>/samples/directx/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
