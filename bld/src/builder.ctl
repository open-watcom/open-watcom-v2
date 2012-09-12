# SRC Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> <OWSRCDIR>/clib/startup/a/cstrt386.asm        <OWRELROOT>/src/startup/386/cstrt386.asm
    <CPCMD> <OWSRCDIR>/clib/startup/a/cstrtw32.asm        <OWRELROOT>/src/startup/386/cstrtw32.asm
    <CPCMD> <OWSRCDIR>/clib/startup/a/cstrtwnt.asm        <OWRELROOT>/src/startup/386/cstrtwnt.asm
#    <CPCMD> <OWSRCDIR>/clib/startup/a/cstrtwwt.asm        <OWRELROOT>/src/startup/386/cstrtwwt.asm
    <CPCMD> <OWSRCDIR>/clib/startup/a/cstrtx32.asm        <OWRELROOT>/src/startup/386/cstrtx32.asm
    <CPCMD> <OWSRCDIR>/clib/startup/c/cmain386.c          <OWRELROOT>/src/startup/386/cmain386.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/wildargv.c          <OWRELROOT>/src/startup/wildargv.c
    <CPCMD> <OWSRCDIR>/clib/h/initarg.h                   <OWRELROOT>/src/startup/initarg.h
    <CPCMD> <OWSRCDIR>/clib/startup/a/cstrt086.asm        <OWRELROOT>/src/startup/dos/cstrt086.asm
    <CPCMD> <OWSRCDIR>/clib/startup/a/dos16m.asm          <OWRELROOT>/src/startup/dos/dos16m.asm
    <CPCMD> <OWSRCDIR>/clib/startup/a/dllstart.asm        <OWRELROOT>/src/startup/386/dllstart.asm
    <CPCMD> <OWSRCDIR>/clib/startup/a/dstrt386.asm        <OWRELROOT>/src/startup/386/dstrt386.asm
    <CPCMD> <OWSRCDIR>/clib/startup/c/cmain086.c          <OWRELROOT>/src/startup/dos/cmain086.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/cmain086.c          <OWRELROOT>/src/startup/os2/cmain086.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/hpgrw086.c          <OWRELROOT>/src/startup/dos/hpgrw086.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/dmaino16.c          <OWRELROOT>/src/startup/os2/dmaino16.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/libmno16.c          <OWRELROOT>/src/startup/os2/libmno16.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/maino16.c           <OWRELROOT>/src/startup/os2/maino16.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/main2o32.c          <OWRELROOT>/src/startup/386/main2o32.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/libmno32.c          <OWRELROOT>/src/startup/386/libmno32.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/dmaino32.c          <OWRELROOT>/src/startup/386/dmaino32.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/main2wnt.c          <OWRELROOT>/src/startup/386/main2wnt.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/wmainwnt.c          <OWRELROOT>/src/startup/386/wmainwnt.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/lmainwnt.c          <OWRELROOT>/src/startup/386/lmainwnt.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/dmainwnt.c          <OWRELROOT>/src/startup/386/dmainwnt.c
    <CPCMD> <OWSRCDIR>/clib/startup/c/lmn2wnt.c           <OWRELROOT>/src/startup/386/lmn2wnt.c
    <CPCMD> <OWSRCDIR>/clib/startup/a/cstrto16.asm        <OWRELROOT>/src/startup/os2/cstrto16.asm
    <CPCMD> <OWSRCDIR>/clib/startup/a/cstrto32.asm        <OWRELROOT>/src/startup/386/cstrto32.asm
    <CPCMD> <OWSRCDIR>/clib/startup/a/cstrtw16.asm        <OWRELROOT>/src/startup/win/cstrtw16.asm
    <CPCMD> <OWSRCDIR>/clib/startup/a/libentry.asm        <OWRELROOT>/src/startup/win/libentry.asm
    <CPCMD> <OWSRCDIR>/clib/startup/c/8087cw.c            <OWRELROOT>/src/startup/8087cw.c
    <CPCMD> <OWSRCDIR>/lib_misc/h/exitwmsg.h              <OWRELROOT>/src/startup/os2/exitwmsg.h
    <CPCMD> <OWSRCDIR>/clib/startup/h/initfini.h          <OWRELROOT>/src/startup/386/initfini.h
    <CPCMD> <OWSRCDIR>/clib/startup/h/initfini.h          <OWRELROOT>/src/startup/os2/initfini.h
    <CPCMD> <OWSRCDIR>/watcom/h/wos2.h                    <OWRELROOT>/src/startup/os2/wos2.h
    <CPCMD> <OWSRCDIR>/watcom/h/mdef.inc                  <OWRELROOT>/src/startup/mdef.inc
    <CPCMD> <OWSRCDIR>/watcom/h/xinit.inc                 <OWRELROOT>/src/startup/xinit.inc
    <CPCMD> <OWSRCDIR>/comp_cfg/h/langenv.inc             <OWRELROOT>/src/startup/langenv.inc
    <CPCMD> <OWSRCDIR>/clib/startup/a/msgrt16.inc         <OWRELROOT>/src/startup/msgrt16.inc
    <CPCMD> <OWSRCDIR>/clib/startup/a/msgrt32.inc         <OWRELROOT>/src/startup/msgrt32.inc
    <CPCMD> <OWSRCDIR>/clib/startup/a/msgcpyrt.inc        <OWRELROOT>/src/startup/msgcpyrt.inc

    <CPCMD> <OWSRCDIR>/cpplib/contain/cpp/*               <OWRELROOT>/src/cpplib/contain/

    <CPCMD> clibexam/*                                  <OWRELROOT>/samples/clibexam/
    <CPCMD> clibexam/kanji/*                            <OWRELROOT>/samples/clibexam/kanji/
    <CPCMD> clibexam/test/*                             <OWRELROOT>/samples/clibexam/test/

    <CPCMD> cplbexam/*                                  <OWRELROOT>/samples/cplbexam/
    <CPCMD> cplbexam/complex/friend/*                   <OWRELROOT>/samples/cplbexam/complex/friend/
    <CPCMD> cplbexam/complex/friend/*                   <OWRELROOT>/samples/cplbexam/complex/friend/
    <CPCMD> cplbexam/complex/pubfun/*                   <OWRELROOT>/samples/cplbexam/complex/pubfun/
    <CPCMD> cplbexam/complex/relfun/*                   <OWRELROOT>/samples/cplbexam/complex/relfun/
    <CPCMD> cplbexam/complex/relop/*                    <OWRELROOT>/samples/cplbexam/complex/relop/
    <CPCMD> cplbexam/contain/*                          <OWRELROOT>/samples/cplbexam/contain/
    <CPCMD> cplbexam/fstream/fstream/*                  <OWRELROOT>/samples/cplbexam/fstream/fstream/
    <CPCMD> cplbexam/fstream/ifstream/*                 <OWRELROOT>/samples/cplbexam/fstream/ifstream/
    <CPCMD> cplbexam/fstream/ofstream/*                 <OWRELROOT>/samples/cplbexam/fstream/ofstream/
    <CPCMD> cplbexam/ios/*                              <OWRELROOT>/samples/cplbexam/ios/
    <CPCMD> cplbexam/iostream/iostream/*                <OWRELROOT>/samples/cplbexam/iostream/iostream/
    <CPCMD> cplbexam/iostream/istream/*                 <OWRELROOT>/samples/cplbexam/iostream/istream/
    <CPCMD> cplbexam/iostream/ostream/*                 <OWRELROOT>/samples/cplbexam/iostream/ostream/
    <CPCMD> cplbexam/string/friend/*                    <OWRELROOT>/samples/cplbexam/string/friend/
    <CPCMD> cplbexam/string/pubfun/*                    <OWRELROOT>/samples/cplbexam/string/pubfun/
    <CPCMD> cplbexam/strstrea/istrstre/*                <OWRELROOT>/samples/cplbexam/strstrea/istrstre/
    <CPCMD> cplbexam/strstrea/ostrstre/*                <OWRELROOT>/samples/cplbexam/strstrea/ostrstre/
    <CPCMD> cplbexam/strstrea/strstre/*                 <OWRELROOT>/samples/cplbexam/strstrea/strstre/

    <CPCMD> cppexamp/*                                  <OWRELROOT>/samples/cppexamp/
    <CPCMD> cppexamp/excarea/*                          <OWRELROOT>/samples/cppexamp/excarea/
    <CPCMD> cppexamp/membfun/*                          <OWRELROOT>/samples/cppexamp/membfun/
    <CPCMD> cppexamp/rtti/*                             <OWRELROOT>/samples/cppexamp/rtti/

    <CPCMD> directx/*                                   <OWRELROOT>/samples/directx/
    <CPCMD> directx/c/d3d/*                             <OWRELROOT>/samples/directx/c/d3d/
    <CPCMD> directx/c/dinput/*                          <OWRELROOT>/samples/directx/c/dinput/
    <CPCMD> directx/c/dshow/*                           <OWRELROOT>/samples/directx/c/dshow/
    <CPCMD> directx/c/dsound/*                          <OWRELROOT>/samples/directx/c/dsound/
    <CPCMD> directx/cpp/d3d/*                           <OWRELROOT>/samples/directx/cpp/d3d/
    <CPCMD> directx/cpp/dinput/*                        <OWRELROOT>/samples/directx/cpp/dinput/
    <CPCMD> directx/cpp/dshow/*                         <OWRELROOT>/samples/directx/cpp/dshow/
    <CPCMD> directx/cpp/dsound/*                        <OWRELROOT>/samples/directx/cpp/dsound/

    <CPCMD> dll/*                                       <OWRELROOT>/samples/dll/

    <CPCMD> fortran/*                                   <OWRELROOT>/src/fortran/
    <CPCMD> fortran/win/*                               <OWRELROOT>/src/fortran/win/
    <CPCMD> fortran/dos/*                               <OWRELROOT>/src/fortran/dos/
    <CPCMD> fortran/os2/*                               <OWRELROOT>/src/fortran/os2/

    <CPCMD> ftnexamp/os2/*                              <OWRELROOT>/samples/fortran/os2/
    <CPCMD> ftnexamp/os2/rexx/*                         <OWRELROOT>/samples/fortran/os2/rexx/
    <CPCMD> ftnexamp/mtdll/*                            <OWRELROOT>/samples/fortran/mtdll/
    <CPCMD> ftnexamp/mthread/mthrdos2.for               <OWRELROOT>/samples/fortran/os2/mthread.for
    <CPCMD> ftnexamp/mthread/makefile.os2               <OWRELROOT>/samples/fortran/os2/makefile
    <CPCMD> ftnexamp/mthread/mthrdnt.for                <OWRELROOT>/samples/fortran/win32/mthread.for
    <CPCMD> ftnexamp/mthread/makefile.nt                <OWRELROOT>/samples/fortran/win32/makefile
    <CPCMD> ftnexamp/win/*                              <OWRELROOT>/samples/fortran/win/
    <CPCMD> ftnexamp/windll/*                           <OWRELROOT>/samples/fortran/win/dll/
    <CPCMD> ftnexamp/dll/*                              <OWRELROOT>/samples/fortran/os2/dll/
    <CPCMD> ftnexamp/dll/*                              <OWRELROOT>/samples/fortran/win32/dll/
    <CPCMD> ftnexamp/dll/os2/makefile                   <OWRELROOT>/samples/fortran/os2/dll/
    <CPCMD> ftnexamp/dll/win32/makefile                 <OWRELROOT>/samples/fortran/win32/dll/

    <CPCMD> goodies/*                                   <OWRELROOT>/samples/goodies/

    <CPCMD> misc/*                                      <OWRELROOT>/src/

    <CPCMD> os2/*                                       <OWRELROOT>/samples/os2/
    <CPCMD> os2/dll/*                                   <OWRELROOT>/samples/os2/dll/
    <CPCMD> os2/pdd/*                                   <OWRELROOT>/samples/os2/pdd/
    <CPCMD> os2/som/*                                   <OWRELROOT>/samples/os2/som/
    <CPCMD> os2/som/animals/*                           <OWRELROOT>/samples/os2/som/animals/
    <CPCMD> os2/som/classes/*                           <OWRELROOT>/samples/os2/som/classes/
    <CPCMD> os2/som/helloc/*                            <OWRELROOT>/samples/os2/som/helloc/
    <CPCMD> os2/som/hellocpp/*                          <OWRELROOT>/samples/os2/som/hellocpp/
    <CPCMD> os2/som/wps/*                               <OWRELROOT>/samples/os2/som/wps/

    <CPCMD> win/*                                       <OWRELROOT>/samples/win/
    <CPCMD> win/alarm/*                                 <OWRELROOT>/samples/win/alarm/
    <CPCMD> win/datactl/*                               <OWRELROOT>/samples/win/datactl/
    <CPCMD> win/edit/*                                  <OWRELROOT>/samples/win/edit/
    <CPCMD> win/generic/*                               <OWRELROOT>/samples/win/generic/
    <CPCMD> win/helpex/*                                <OWRELROOT>/samples/win/helpex/
    <CPCMD> win/iconview/*                              <OWRELROOT>/samples/win/iconview/
    <CPCMD> win/life/*                                  <OWRELROOT>/samples/win/life/
    <CPCMD> win/shootgal/*                              <OWRELROOT>/samples/win/shootgal/
    <CPCMD> win/watzee/*                                <OWRELROOT>/samples/win/watzee/
    <CPCMD> win/alarm/win16/*                           <OWRELROOT>/samples/win/alarm/win16/
    <CPCMD> win/alarm/win32/*                           <OWRELROOT>/samples/win/alarm/win32/
    <CPCMD> win/alarm/win386/*                          <OWRELROOT>/samples/win/alarm/win386/
    <CPCMD> win/alarm/winaxp/*                          <OWRELROOT>/samples/win/alarm/winaxp/
    <CPCMD> win/datactl/win16/*                         <OWRELROOT>/samples/win/datactl/win16/
    <CPCMD> win/datactl/win32/*                         <OWRELROOT>/samples/win/datactl/win32/
    <CPCMD> win/datactl/win386/*                        <OWRELROOT>/samples/win/datactl/win386/
    <CPCMD> win/datactl/winaxp/*                        <OWRELROOT>/samples/win/datactl/winaxp/
    <CPCMD> win/edit/win16/*                            <OWRELROOT>/samples/win/edit/win16/
    <CPCMD> win/edit/win32/*                            <OWRELROOT>/samples/win/edit/win32/
    <CPCMD> win/edit/win386/*                           <OWRELROOT>/samples/win/edit/win386/
    <CPCMD> win/edit/winaxp/*                           <OWRELROOT>/samples/win/edit/winaxp/
    <CPCMD> win/generic/win16/*                         <OWRELROOT>/samples/win/generic/win16/
    <CPCMD> win/generic/win32/*                         <OWRELROOT>/samples/win/generic/win32/
    <CPCMD> win/generic/win386/*                        <OWRELROOT>/samples/win/generic/win386/
    <CPCMD> win/generic/winaxp/*                        <OWRELROOT>/samples/win/generic/winaxp/
    <CPCMD> win/helpex/win16/*                          <OWRELROOT>/samples/win/helpex/win16/
    <CPCMD> win/helpex/win32/*                          <OWRELROOT>/samples/win/helpex/win32/
    <CPCMD> win/helpex/win386/*                         <OWRELROOT>/samples/win/helpex/win386/
    <CPCMD> win/helpex/winaxp/*                         <OWRELROOT>/samples/win/helpex/winaxp/
    <CPCMD> win/iconview/win16/*                        <OWRELROOT>/samples/win/iconview/win16/
    <CPCMD> win/iconview/win32/*                        <OWRELROOT>/samples/win/iconview/win32/
    <CPCMD> win/iconview/win386/*                       <OWRELROOT>/samples/win/iconview/win386/
    <CPCMD> win/iconview/winaxp/*                       <OWRELROOT>/samples/win/iconview/winaxp/
    <CPCMD> win/life/win16/*                            <OWRELROOT>/samples/win/life/win16/
    <CPCMD> win/life/win32/*                            <OWRELROOT>/samples/win/life/win32/
    <CPCMD> win/life/win386/*                           <OWRELROOT>/samples/win/life/win386/
    <CPCMD> win/life/winaxp/*                           <OWRELROOT>/samples/win/life/winaxp/
    <CPCMD> win/shootgal/win16/*                        <OWRELROOT>/samples/win/shootgal/win16/
    <CPCMD> win/shootgal/win32/*                        <OWRELROOT>/samples/win/shootgal/win32/
    <CPCMD> win/shootgal/win386/*                       <OWRELROOT>/samples/win/shootgal/win386/
    <CPCMD> win/shootgal/winaxp/*                       <OWRELROOT>/samples/win/shootgal/winaxp/
    <CPCMD> win/watzee/win16/*                          <OWRELROOT>/samples/win/watzee/win16/
    <CPCMD> win/watzee/win32/*                          <OWRELROOT>/samples/win/watzee/win32/
    <CPCMD> win/watzee/win386/*                         <OWRELROOT>/samples/win/watzee/win386/
    <CPCMD> win/watzee/winaxp/*                         <OWRELROOT>/samples/win/watzee/winaxp/

    rm -f <OWRELROOT>/samples/clibexam/file.dat
    rm -f <OWRELROOT>/samples/cplbexam/file.dat
    rm -f <OWRELROOT>/samples/cppexamp/file.dat
    rm -f <OWRELROOT>/samples/directx/file.dat
    rm -f <OWRELROOT>/samples/dll/file.dat
    rm -f <OWRELROOT>/samples/goodies/file.dat
    rm -f <OWRELROOT>/samples/os2/file.dat
    rm -f <OWRELROOT>/samples/win/file.dat
    rm -f <OWRELROOT>/src/file.dat
    rm -f <OWRELROOT>/src/fortran/file.dat

[ BLOCK . . ]
#============
cdsay <PROJDIR>
