# wmake lang=f77 targ=os2v2
# wmake lang=f77 targ=nt
# wmake lang=c   targ=os2
# wmake lang=c   targ=os2v2
# wmake lang=c   targ=nt
# wmake lang=c   targ=ntaxp
# wmake lang=c   targ=qnx
# wmake lang=cpp targ=os2
# wmake lang=cpp targ=os2v2
# wmake lang=cpp targ=nt
#
# options rtdll=1 -> use -br and runtime DLLs for C/C++
#

!ifeq lang f77
zuopt = /ssfloats
!else
zuopt = /zu
!endif

!ifeq targ nt
mt_opts         = /bm /d1
!else ifeq targ ntaxp
mt_opts         = /bm /d1
!else ifeq targ os2
mt_opts         = /bm /d1 $(zuopt)
!else ifeq targ os2v2
mt_opts         = /bm /d1
!else ifeq targ qnx
mt_opts         = /d1
!endif

!ifeq targ nt
dll_opts        = /bd /d1
!else ifeq targ ntaxp
dll_opts        = /bd /d1
!else ifeq targ os2
dll_opts        = /bd /d1 $(zuopt)
!else ifeq targ os2v2
dll_opts        = /bd /d1
!endif

!ifeq lang f77
ext     = for
opts    = /q /xline
cmp     = wfc386
!ifeq targ nt
opts    += /def=nt
!endif
!endif

!ifeq lang c
ext     = c
opts    = /ox /zq
!ifeq targ nt
cmp     = wcc386
opts    += /bt=NT /i=$(%watcom)\h\nt
!else ifeq targ ntaxp
cmp     = wccaxp
opts    += /bt=NT /i=$(%watcom)\h\nt
!else ifeq targ os2
cmp     = wcc
opts    += /ml /bt=OS2 /i=$(%watcom)\h\os21x
!else ifeq targ os2v2
cmp     = wcc386
opts    += /bt=OS2 /i=$(%watcom)\h\os2
!else ifeq targ qnx
cmp     = wcc386
opts    += /bt=qnx /i=$(%watcom)\qh
!endif
!endif

!ifeq lang cpp
ext     = c
opts    = /ox /zq
form    = lib
!ifeq targ nt
cmp     = wpp386
opts    += /bt=NT /i=$(%watcom)\h\nt
!else ifeq targ ntaxp
cmp     = wppaxp
opts    += /bt=NT /i=$(%watcom)\h\nt
!else ifeq targ os2
opts    += /ml /bt=OS2 /i=$(%watcom)\h\os21x
cmp     = wpp
!else ifeq targ os2v2
cmp     = wpp386
opts    += /bt=OS2 /i=$(%watcom)\h\os2
!else ifeq targ qnx
cmp     = wpp386
opts    += /bt=qnx /i=$(%watcom)\qh
!endif
!endif

!ifneq lang f77
!ifeq form lib
dll_opts+= /dDLLEXPORT=__export
!else
dll_opts+= /dDLLEXPORT=
!endif
!ifeq rtdll 1
opts += /br
case = option caseexact
!endif

!endif


!ifeq targ qnx
targets = mthread.qnx
!else
targets = dllsamp.dll mthread.exe
!endif

all : .symbolic $(targets)
        @%null

!ifeq targ os2

mthread.obj : mthread.$(ext)
        $(cmp) $(opts) $(mt_opts) mthread

mthread.exe : mthread.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system os2
        @%append $(%lnk) option map
        @%append $(%lnk) file $[@
!ifeq lang c
        @%append $(%lnk) import dll_entry_1_ 'dllsamp'
        @%append $(%lnk) import dll_entry_2_ 'dllsamp'
        @%append $(%lnk) import dll_print_   'dllsamp'
        @%append $(%lnk) import dll_begin_thread_    'dllsamp'
        @%append $(%lnk) import dll_entry_terminate_ 'dllsamp'
!endif
!ifeq lang cpp
        @%append $(%lnk) library dllsamp.lib
!endif
        wlink name $^& @$(%lnk)

dllsamp.obj : dllsamp.$(ext)
        $(cmp) $(opts) $(dll_opts) dllsamp

dllsamp.dll : dllsamp.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system os2 dll initinstance
        @%append $(%lnk) option manyautodata, map
!ifeq form lib
        @%append $(%lnk) option implib
!endif
        @%append $(%lnk) file $[@
!ifeq lang c
        @%append $(%lnk) export dll_entry_1_
        @%append $(%lnk) export dll_entry_2_
        @%append $(%lnk) export dll_print_
        @%append $(%lnk) export dll_begin_thread_
        @%append $(%lnk) export dll_entry_terminate_
!endif
!ifeq lang cpp
# relies on form lib
!endif
        wlink name $^& @$(%lnk)

!endif

!ifeq targ os2v2

mthread.obj : mthread.$(ext)
        $(cmp) $(opts) $(mt_opts) mthread

mthread.exe : mthread.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system os2v2
        @%append $(%lnk) option map $(case)
        @%append $(%lnk) file $[@
!ifeq lang c
        @%append $(%lnk) import dll_entry_1_ 'dllsamp'
        @%append $(%lnk) import dll_entry_2_ 'dllsamp'
        @%append $(%lnk) import dll_print_   'dllsamp'
        @%append $(%lnk) import dll_begin_thread_    'dllsamp'
!endif
!ifeq lang cpp
        @%append $(%lnk) library dllsamp.lib
!endif
!ifeq lang f77
        @%append $(%lnk) import DLL_ENTRY_1 'dllsamp'
        @%append $(%lnk) import DLL_ENTRY_2 'dllsamp'
        @%append $(%lnk) import DLL_PRINT   'dllsamp'
        @%append $(%lnk) import DLL_BEGIN_THREAD   'dllsamp'
!endif
        wlink name $^& @$(%lnk)

dllsamp.obj : dllsamp.$(ext)
        $(cmp) $(opts) $(dll_opts) dllsamp

dllsamp.dll : dllsamp.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system os2v2 dll initinstance terminstance
        @%append $(%lnk) option manyautodata, map $(case)
!ifeq form lib
        @%append $(%lnk) option implib
!endif
        @%append $(%lnk) file $[@
!ifeq lang c
        @%append $(%lnk) export dll_entry_1_
        @%append $(%lnk) export dll_entry_2_
        @%append $(%lnk) export dll_print_
        @%append $(%lnk) export dll_begin_thread_
!endif
!ifeq lang cpp
# relies on form lib
!endif
!ifeq lang f77
        @%append $(%lnk) export DLL_ENTRY_1
        @%append $(%lnk) export DLL_ENTRY_2
        @%append $(%lnk) export DLL_PRINT
        @%append $(%lnk) export DLL_BEGIN_THREAD
!endif
        wlink name $^& @$(%lnk)

!endif

!ifeq targ qnx

mthread.obj : mthread.$(ext)
        $(cmp) $(opts) $(mt_opts) mthread

mthread.qnx : mthread.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system qnx386
        @%append $(%lnk) option map
        @%append $(%lnk) file $[@
        wlink name $^& @$(%lnk)

!endif

!ifeq targ nt

mthread.obj : mthread.$(ext)
        $(cmp) $(opts) $(mt_opts) mthread

mthread.exe : mthread.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system nt
        @%append $(%lnk) option map
        @%append $(%lnk) file $[@
!ifeq lang c
        @%append $(%lnk) import dll_entry_1_ 'DLLSAMP.DLL'
        @%append $(%lnk) import dll_entry_2_ 'DLLSAMP.DLL'
        @%append $(%lnk) import dll_print_   'DLLSAMP.DLL'
        @%append $(%lnk) import dll_begin_thread_   'DLLSAMP.DLL'
!endif
!ifeq lang cpp
        @%append $(%lnk) library dllsamp.lib
!endif
!ifeq lang f77
        @%append $(%lnk) import DLL_ENTRY_1 'DLLSAMP.DLL'
        @%append $(%lnk) import DLL_ENTRY_2 'DLLSAMP.DLL'
        @%append $(%lnk) import DLL_PRINT   'DLLSAMP.DLL'
        @%append $(%lnk) import DLL_BEGIN_THREAD 'DLLSAMP.DLL'
!endif
        wlink name $^& @$(%lnk)

dllsamp.obj : dllsamp.$(ext)
        $(cmp) $(opts) $(dll_opts) dllsamp

dllsamp.dll : dllsamp.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system nt_dll initinstance terminstance
        @%append $(%lnk) option modname=DLLSAMP, map
!ifeq form lib
        @%append $(%lnk) option implib
!endif
        @%append $(%lnk) file $[@
!ifeq lang c
        @%append $(%lnk) export dll_entry_1_
        @%append $(%lnk) export dll_entry_2_
        @%append $(%lnk) export dll_print_
        @%append $(%lnk) export dll_begin_thread_
!endif
!ifeq lang cpp
# relies on form lib
!endif
!ifeq lang f77
        @%append $(%lnk) export DLL_ENTRY_1
        @%append $(%lnk) export DLL_ENTRY_2
        @%append $(%lnk) export DLL_PRINT
        @%append $(%lnk) export DLL_BEGIN_THREAD
!endif
        wlink name $^& @$(%lnk)

!endif

!ifeq targ ntaxp

mthread.obj : mthread.$(ext)
        $(cmp) $(opts) $(mt_opts) mthread

mthread.exe : mthread.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system ntaxp
        @%append $(%lnk) option map
        @%append $(%lnk) file $[@
!ifeq lang c
        @%append $(%lnk) import dll_entry_1 'DLLSAMP.DLL'
        @%append $(%lnk) import dll_entry_2 'DLLSAMP.DLL'
        @%append $(%lnk) import dll_print   'DLLSAMP.DLL'
        @%append $(%lnk) import dll_begin_thread   'DLLSAMP.DLL'
!endif
!ifeq lang cpp
        @%append $(%lnk) library dllsamp.lib
!endif
!ifeq lang f77
        @%append $(%lnk) import DLL_ENTRY_1 'DLLSAMP.DLL'
        @%append $(%lnk) import DLL_ENTRY_2 'DLLSAMP.DLL'
        @%append $(%lnk) import DLL_PRINT   'DLLSAMP.DLL'
        @%append $(%lnk) import DLL_BEGIN_THREAD 'DLLSAMP.DLL'
!endif
        wlink name $^& @$(%lnk)

dllsamp.obj : dllsamp.$(ext)
        $(cmp) $(opts) $(dll_opts) dllsamp

dllsamp.dll : dllsamp.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system ntaxp_dll initinstance terminstance
        @%append $(%lnk) option modname=DLLSAMP, map
!ifeq form lib
        @%append $(%lnk) option implib
!endif
        @%append $(%lnk) file $[@
!ifeq lang c
        @%append $(%lnk) export dll_entry_1
        @%append $(%lnk) export dll_entry_2
        @%append $(%lnk) export dll_print
        @%append $(%lnk) export dll_begin_thread
!endif
!ifeq lang cpp
# relies on form lib
!endif
!ifeq lang f77
        @%append $(%lnk) export DLL_ENTRY_1
        @%append $(%lnk) export DLL_ENTRY_2
        @%append $(%lnk) export DLL_PRINT
        @%append $(%lnk) export DLL_BEGIN_THREAD
!endif
        wlink name $^& @$(%lnk)

!endif

clean : .symbolic
        @-rm *.lnk *.exe *.dll *.map *.obj *.lib *.err 1>nul 2>nul
