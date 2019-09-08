# SRC Builder Control file
# ========================

set PROJNAME=src

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> <OWSRCDIR>/clib/startup/a/cstrtwnt.asm      <OWRELROOT>/src/startup/386/cstrtwnt.asm
#    <CCCMD> <OWSRCDIR>/clib/startup/a/cstrtwwt.asm      <OWRELROOT>/src/startup/386/cstrtwwt.asm
    <CCCMD> <OWSRCDIR>/clib/startup/c/cmain386.c        <OWRELROOT>/src/startup/386/cmain386.c
    <CCCMD> <OWSRCDIR>/clib/h/initarg.h                 <OWRELROOT>/src/startup/initarg.h
    <CCCMD> <OWSRCDIR>/clib/startup/a/dstrt386.asm      <OWRELROOT>/src/startup/386/dstrt386.asm
    <CCCMD> <OWSRCDIR>/clib/startup/c/cmain086.c        <OWRELROOT>/src/startup/dos/cmain086.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/cmain086.c        <OWRELROOT>/src/startup/os2/cmain086.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/libmno16.c        <OWRELROOT>/src/startup/os2/libmno16.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/dmaino16.c        <OWRELROOT>/src/startup/os2/dmaino16.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/main2o32.c        <OWRELROOT>/src/startup/386/main2o32.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/libmno32.c        <OWRELROOT>/src/startup/386/libmno32.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/dmaino32.c        <OWRELROOT>/src/startup/386/dmaino32.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/main2wnt.c        <OWRELROOT>/src/startup/386/main2wnt.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/wmainwnt.c        <OWRELROOT>/src/startup/386/wmainwnt.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/lmainwnt.c        <OWRELROOT>/src/startup/386/lmainwnt.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/dmainwnt.c        <OWRELROOT>/src/startup/386/dmainwnt.c
    <CCCMD> <OWSRCDIR>/clib/startup/c/lmn2wnt.c         <OWRELROOT>/src/startup/386/lmn2wnt.c
    <CCCMD> <OWSRCDIR>/clib/startup/a/cstrto32.asm      <OWRELROOT>/src/startup/386/cstrto32.asm
    <CCCMD> <OWSRCDIR>/clib/startup/c/8087cw.c          <OWRELROOT>/src/startup/8087cw.c
    <CCCMD> <OWSRCDIR>/clib/startup/h/initfini.h        <OWRELROOT>/src/startup/386/initfini.h
    <CCCMD> <OWSRCDIR>/clib/startup/h/initfini.h        <OWRELROOT>/src/startup/os2/initfini.h
    <CCCMD> <OWSRCDIR>/watcom/h/mdef.inc                <OWRELROOT>/src/startup/
    <CCCMD> <OWSRCDIR>/watcom/h/extender.inc            <OWRELROOT>/src/startup/
    <CCCMD> <OWSRCDIR>/watcom/h/tinit.inc               <OWRELROOT>/src/startup/
    <CCCMD> <OWSRCDIR>/watcom/h/xinit.inc               <OWRELROOT>/src/startup/

    <CCCMD> startup/msgcpyrt.inc                        <OWRELROOT>/src/startup/
    <CCCMD> startup/wildargv.c                          <OWRELROOT>/src/startup/
    <CCCMD> startup/cstrt086.asm                        <OWRELROOT>/src/startup/dos/
    <CCCMD> startup/dos16m.asm                          <OWRELROOT>/src/startup/dos/
    <CCCMD> startup/maino16.c                           <OWRELROOT>/src/startup/os2/
    <CCCMD> startup/cstrto16.asm                        <OWRELROOT>/src/startup/os2/
    <CCCMD> startup/cstrtw16.asm                        <OWRELROOT>/src/startup/win/
    <CCCMD> startup/libentry.asm                        <OWRELROOT>/src/startup/win/
    <CCCMD> startup/cstrt386.asm                        <OWRELROOT>/src/startup/386/
    <CCCMD> startup/cstrtw32.asm                        <OWRELROOT>/src/startup/386/
    <CCCMD> startup/dllstart.asm                        <OWRELROOT>/src/startup/386/
    <CCCMD> startup/cstrtx32.asm                        <OWRELROOT>/src/startup/386/

    <CCCMD> <OWSRCDIR>/cpplib/contain/cpp/*             <OWRELROOT>/src/cpplib/contain/

    <CCCMD> clibexam/*                                  <OWRELROOT>/samples/clibexam/
    <CCCMD> clibexam/kanji/*                            <OWRELROOT>/samples/clibexam/kanji/
    <CCCMD> clibexam/test/*                             <OWRELROOT>/samples/clibexam/test/

    <CCCMD> cplbexam/*                                  <OWRELROOT>/samples/cplbexam/
    <CCCMD> cplbexam/complex/friend/*                   <OWRELROOT>/samples/cplbexam/complex/friend/
    <CCCMD> cplbexam/complex/friend/*                   <OWRELROOT>/samples/cplbexam/complex/friend/
    <CCCMD> cplbexam/complex/pubfun/*                   <OWRELROOT>/samples/cplbexam/complex/pubfun/
    <CCCMD> cplbexam/complex/relfun/*                   <OWRELROOT>/samples/cplbexam/complex/relfun/
    <CCCMD> cplbexam/complex/relop/*                    <OWRELROOT>/samples/cplbexam/complex/relop/
    <CCCMD> cplbexam/contain/*                          <OWRELROOT>/samples/cplbexam/contain/
    <CCCMD> cplbexam/fstream/fstream/*                  <OWRELROOT>/samples/cplbexam/fstream/fstream/
    <CCCMD> cplbexam/fstream/ifstream/*                 <OWRELROOT>/samples/cplbexam/fstream/ifstream/
    <CCCMD> cplbexam/fstream/ofstream/*                 <OWRELROOT>/samples/cplbexam/fstream/ofstream/
    <CCCMD> cplbexam/ios/*                              <OWRELROOT>/samples/cplbexam/ios/
    <CCCMD> cplbexam/iostream/iostream/*                <OWRELROOT>/samples/cplbexam/iostream/iostream/
    <CCCMD> cplbexam/iostream/istream/*                 <OWRELROOT>/samples/cplbexam/iostream/istream/
    <CCCMD> cplbexam/iostream/ostream/*                 <OWRELROOT>/samples/cplbexam/iostream/ostream/
    <CCCMD> cplbexam/string/friend/*                    <OWRELROOT>/samples/cplbexam/string/friend/
    <CCCMD> cplbexam/string/pubfun/*                    <OWRELROOT>/samples/cplbexam/string/pubfun/
    <CCCMD> cplbexam/strstrea/istrstre/*                <OWRELROOT>/samples/cplbexam/strstrea/istrstre/
    <CCCMD> cplbexam/strstrea/ostrstre/*                <OWRELROOT>/samples/cplbexam/strstrea/ostrstre/
    <CCCMD> cplbexam/strstrea/strstre/*                 <OWRELROOT>/samples/cplbexam/strstrea/strstre/

    <CCCMD> cppexamp/*                                  <OWRELROOT>/samples/cppexamp/
    <CCCMD> cppexamp/excarea/*                          <OWRELROOT>/samples/cppexamp/excarea/
    <CCCMD> cppexamp/membfun/*                          <OWRELROOT>/samples/cppexamp/membfun/
    <CCCMD> cppexamp/rtti/*                             <OWRELROOT>/samples/cppexamp/rtti/

    <CCCMD> directx/*                                   <OWRELROOT>/samples/directx/
    <CCCMD> directx/c/d3d/*                             <OWRELROOT>/samples/directx/c/d3d/
    <CCCMD> directx/c/dinput/*                          <OWRELROOT>/samples/directx/c/dinput/
    <CCCMD> directx/c/dshow/*                           <OWRELROOT>/samples/directx/c/dshow/
    <CCCMD> directx/c/dsound/*                          <OWRELROOT>/samples/directx/c/dsound/
    <CCCMD> directx/cpp/d3d/*                           <OWRELROOT>/samples/directx/cpp/d3d/
    <CCCMD> directx/cpp/dinput/*                        <OWRELROOT>/samples/directx/cpp/dinput/
    <CCCMD> directx/cpp/dshow/*                         <OWRELROOT>/samples/directx/cpp/dshow/
    <CCCMD> directx/cpp/dsound/*                        <OWRELROOT>/samples/directx/cpp/dsound/

    <CCCMD> dll/*                                       <OWRELROOT>/samples/dll/

    <CCCMD> fortran/*                                   <OWRELROOT>/src/fortran/
    <CCCMD> fortran/win/*                               <OWRELROOT>/src/fortran/win/
    <CCCMD> fortran/dos/*                               <OWRELROOT>/src/fortran/dos/
    <CCCMD> fortran/os2/*                               <OWRELROOT>/src/fortran/os2/

    <CCCMD> ftnexamp/os2/*                              <OWRELROOT>/samples/fortran/os2/
    <CCCMD> ftnexamp/os2/rexx/*                         <OWRELROOT>/samples/fortran/os2/rexx/
    <CCCMD> ftnexamp/mtdll/*                            <OWRELROOT>/samples/fortran/mtdll/
    <CCCMD> ftnexamp/mthread/mthrdos2.for               <OWRELROOT>/samples/fortran/os2/mthread.for
    <CCCMD> ftnexamp/mthread/makefile.os2               <OWRELROOT>/samples/fortran/os2/makefile
    <CCCMD> ftnexamp/mthread/mthrdnt.for                <OWRELROOT>/samples/fortran/win32/mthread.for
    <CCCMD> ftnexamp/mthread/makefile.nt                <OWRELROOT>/samples/fortran/win32/makefile
    <CCCMD> ftnexamp/win/*                              <OWRELROOT>/samples/fortran/win/
    <CCCMD> ftnexamp/windll/*                           <OWRELROOT>/samples/fortran/win/dll/
    <CCCMD> ftnexamp/dll/*                              <OWRELROOT>/samples/fortran/os2/dll/
    <CCCMD> ftnexamp/dll/*                              <OWRELROOT>/samples/fortran/win32/dll/
    <CCCMD> ftnexamp/dll/os2/makefile                   <OWRELROOT>/samples/fortran/os2/dll/
    <CCCMD> ftnexamp/dll/win32/makefile                 <OWRELROOT>/samples/fortran/win32/dll/

    <CCCMD> goodies/*                                   <OWRELROOT>/samples/goodies/

    <CCCMD> misc/*                                      <OWRELROOT>/src/

    <CCCMD> os2/*                                       <OWRELROOT>/samples/os2/
    <CCCMD> os2/dll/*                                   <OWRELROOT>/samples/os2/dll/
    <CCCMD> os2/pdd/*                                   <OWRELROOT>/samples/os2/pdd/
    <CCCMD> os2/som/*                                   <OWRELROOT>/samples/os2/som/
    <CCCMD> os2/som/animals/*                           <OWRELROOT>/samples/os2/som/animals/
    <CCCMD> os2/som/classes/*                           <OWRELROOT>/samples/os2/som/classes/
    <CCCMD> os2/som/helloc/*                            <OWRELROOT>/samples/os2/som/helloc/
    <CCCMD> os2/som/hellocpp/*                          <OWRELROOT>/samples/os2/som/hellocpp/
    <CCCMD> os2/som/wps/*                               <OWRELROOT>/samples/os2/som/wps/

    <CCCMD> win/*                                       <OWRELROOT>/samples/win/
    <CCCMD> win/alarm/*                                 <OWRELROOT>/samples/win/alarm/
    <CCCMD> win/datactl/*                               <OWRELROOT>/samples/win/datactl/
    <CCCMD> win/edit/*                                  <OWRELROOT>/samples/win/edit/
    <CCCMD> win/generic/*                               <OWRELROOT>/samples/win/generic/
    <CCCMD> win/helpex/*                                <OWRELROOT>/samples/win/helpex/
    <CCCMD> win/iconview/*                              <OWRELROOT>/samples/win/iconview/
    <CCCMD> win/life/*                                  <OWRELROOT>/samples/win/life/
    <CCCMD> win/shootgal/*                              <OWRELROOT>/samples/win/shootgal/
    <CCCMD> win/watzee/*                                <OWRELROOT>/samples/win/watzee/
    <CCCMD> win/alarm/win16/*                           <OWRELROOT>/samples/win/alarm/win16/
    <CCCMD> win/alarm/win32/*                           <OWRELROOT>/samples/win/alarm/win32/
    <CCCMD> win/alarm/win386/*                          <OWRELROOT>/samples/win/alarm/win386/
    <CCCMD> win/alarm/winaxp/*                          <OWRELROOT>/samples/win/alarm/winaxp/
    <CCCMD> win/datactl/win16/*                         <OWRELROOT>/samples/win/datactl/win16/
    <CCCMD> win/datactl/win32/*                         <OWRELROOT>/samples/win/datactl/win32/
    <CCCMD> win/datactl/win386/*                        <OWRELROOT>/samples/win/datactl/win386/
    <CCCMD> win/datactl/winaxp/*                        <OWRELROOT>/samples/win/datactl/winaxp/
    <CCCMD> win/edit/win16/*                            <OWRELROOT>/samples/win/edit/win16/
    <CCCMD> win/edit/win32/*                            <OWRELROOT>/samples/win/edit/win32/
    <CCCMD> win/edit/win386/*                           <OWRELROOT>/samples/win/edit/win386/
    <CCCMD> win/edit/winaxp/*                           <OWRELROOT>/samples/win/edit/winaxp/
    <CCCMD> win/generic/win16/*                         <OWRELROOT>/samples/win/generic/win16/
    <CCCMD> win/generic/win32/*                         <OWRELROOT>/samples/win/generic/win32/
    <CCCMD> win/generic/win386/*                        <OWRELROOT>/samples/win/generic/win386/
    <CCCMD> win/generic/winaxp/*                        <OWRELROOT>/samples/win/generic/winaxp/
    <CCCMD> win/helpex/win16/*                          <OWRELROOT>/samples/win/helpex/win16/
    <CCCMD> win/helpex/win32/*                          <OWRELROOT>/samples/win/helpex/win32/
    <CCCMD> win/helpex/win386/*                         <OWRELROOT>/samples/win/helpex/win386/
    <CCCMD> win/helpex/winaxp/*                         <OWRELROOT>/samples/win/helpex/winaxp/
    <CCCMD> win/iconview/win16/*                        <OWRELROOT>/samples/win/iconview/win16/
    <CCCMD> win/iconview/win32/*                        <OWRELROOT>/samples/win/iconview/win32/
    <CCCMD> win/iconview/win386/*                       <OWRELROOT>/samples/win/iconview/win386/
    <CCCMD> win/iconview/winaxp/*                       <OWRELROOT>/samples/win/iconview/winaxp/
    <CCCMD> win/life/win16/*                            <OWRELROOT>/samples/win/life/win16/
    <CCCMD> win/life/win32/*                            <OWRELROOT>/samples/win/life/win32/
    <CCCMD> win/life/win386/*                           <OWRELROOT>/samples/win/life/win386/
    <CCCMD> win/life/winaxp/*                           <OWRELROOT>/samples/win/life/winaxp/
    <CCCMD> win/shootgal/win16/*                        <OWRELROOT>/samples/win/shootgal/win16/
    <CCCMD> win/shootgal/win32/*                        <OWRELROOT>/samples/win/shootgal/win32/
    <CCCMD> win/shootgal/win386/*                       <OWRELROOT>/samples/win/shootgal/win386/
    <CCCMD> win/shootgal/winaxp/*                       <OWRELROOT>/samples/win/shootgal/winaxp/
    <CCCMD> win/watzee/win16/*                          <OWRELROOT>/samples/win/watzee/win16/
    <CCCMD> win/watzee/win32/*                          <OWRELROOT>/samples/win/watzee/win32/
    <CCCMD> win/watzee/win386/*                         <OWRELROOT>/samples/win/watzee/win386/
    <CCCMD> win/watzee/winaxp/*                         <OWRELROOT>/samples/win/watzee/winaxp/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
