dll_opts        = -bd -d1
ext             = for
cmp             = wfc386
linker          = wlink
opts            = -q -xline -expl

all : .symbolic ezrxfunc.dll
    @%null

funcload.obj : funcload.for
        $(cmp) $(opts) $(dll_opts) $^&

func1.obj : func1.for
        $(cmp) $(opts) $(dll_opts) $^&

func2.obj : func2.for
        $(cmp) $(opts) $(dll_opts) $^&

rxsutils.obj : rxsutils.for
        $(cmp) $(opts) $(dll_opts) $^&

EZRXFUNC.DLL : funcload.obj func1.obj func2.obj rxsutils.obj
        @set lnk=$^&.lnk
        @%create $(%lnk)
        @%append $(%lnk) debug all
        @%append $(%lnk) system os2v2 dll initinstance terminstance
        @%append $(%lnk) option manyautodata, map, case
        @%append $(%lnk) file funcload, func1, func2, rxsutils
        @%append $(%lnk) library rexx
        @%append $(%lnk) export EZLoadFuncs resident
        @%append $(%lnk) export EZDropFuncs resident
        @%append $(%lnk) export EZFunc1 resident
        @%append $(%lnk) export EZFunc2 resident
        $(linker) name $^& @$(%lnk)

clean : .symbolic
        @-del *.lnk *.dll *.map *.obj 1>nul 2>nul
