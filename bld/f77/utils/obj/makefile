__none : .symbolic
        @echo use "WMAKE forceout" or "WMAKE checkout" or "WMAKE checked_out"
        @echo or "WMAKE errsrc" or "WMAKE checkin= checkin"
        @%abort

checkin : .symbolic
        set check_out=checkin
        @%make __err

forceout : .symbolic
        set check_out=forceout
        @%make __err

checkout : .symbolic
        set check_out=checkout
        @%make __err

checked_out : .symbolic
        set check_out=@echo
        @%make __err

__err : errsrc.exe .symbolic
        $(%check_out) $(f77_dir)\f77lib\h\errcod.h
        $(%check_out) $(f77_dir)\f77lib\c\errmsg.c
        $(%check_out) $(f77_dir)\f77lib\c\errmsg.rc
        $(%check_out) $(f77_dir)\f77lib\c\errgrp.c
#       $(%check_out) $(f77_dir)\f77lib\c\jerrmsg.rc
        $(%check_out) $(f77_dir)\wfc\h\errcodi.h
        $(%check_out) $(f77_dir)\wfc\h\errgrpi.c
        $(%check_out) $(f77_dir)\wfc\h\errmsgi.c
        $(%check_out) $(f77_dir)\wfc\h\errmsgi.rc
        $(%check_out) $(f77_dir)\wfc\h\errcod3.h
        $(%check_out) $(f77_dir)\wfc\h\errgrp3.c
        $(%check_out) $(f77_dir)\wfc\h\errmsg3.c
        $(%check_out) $(f77_dir)\wfc\h\errmsg3.rc
        $(%check_out) $(f77_dir)\wfc\h\errcodr.h
        $(%check_out) $(f77_dir)\wfc\h\errgrpr.c
        $(%check_out) $(f77_dir)\wfc\h\errmsgr.c
        $(%check_out) $(f77_dir)\wfc\h\errmsgr.rc
        $(%check_out) $(f77_dir)\wfc\h\jerrmsgi.rc
        $(%check_out) $(f77_dir)\wfc\h\jerrmsg3.rc
        $(%check_out) $(f77_dir)\wfc\h\jerrmsgr.rc
        $(%check_out) $(f77_dir)\wfl\h\errcodi.h
        $(%check_out) $(f77_dir)\wfl\h\errmsgi.c
        $(%check_out) $(f77_dir)\wfl\h\errmsgi.rc
        $(%check_out) $(f77_dir)\wfl\h\errgrpi.c
        $(%check_out) $(f77_dir)\wfl\h\jerrmsgi.rc
        $(%check_out) $(f77_dir)\wfl\h\errcod3.h
        $(%check_out) $(f77_dir)\wfl\h\errmsg3.c
        $(%check_out) $(f77_dir)\wfl\h\errmsg3.rc
        $(%check_out) $(f77_dir)\wfl\h\errgrp3.c
        $(%check_out) $(f77_dir)\wfl\h\jerrmsg3.rc
        $(%check_out) $(f77_dir)\wfl\h\errcodr.h
        $(%check_out) $(f77_dir)\wfl\h\errmsgr.c
        $(%check_out) $(f77_dir)\wfl\h\errmsgr.rc
        $(%check_out) $(f77_dir)\wfl\h\errgrpr.c
        $(%check_out) $(f77_dir)\wfl\h\jerrmsgr.rc
!ifndef checkin
        errsrc eo r
        cp errcod.h $(f77_dir)\f77lib\h\errcod.h
        del errcod.h
        cp errmsg.c $(f77_dir)\f77lib\c\errmsg.c
        del errmsg.c
        cp errmsg.rc $(f77_dir)\f77lib\c\errmsg.rc
        del errmsg.rc
        cp errgrp.c $(f77_dir)\f77lib\c\errgrp.c
        del errgrp.c
        del errmsg.msg
        errsrc eoic
        cp errcod.h $(f77_dir)\wfc\h\errcodi.h
        del errcod.h
        cp errmsg.c $(f77_dir)\wfc\h\errmsgi.c
        del errmsg.c
        cp errmsg.rc $(f77_dir)\wfc\h\errmsgi.rc
        del errmsg.rc
        cp errgrp.c $(f77_dir)\wfc\h\errgrpi.c
        del errgrp.c
        del errmsg.msg
        errsrc eo3c
        cp errcod.h $(f77_dir)\wfc\h\errcod3.h
        del errcod.h
        cp errmsg.c $(f77_dir)\wfc\h\errmsg3.c
        del errmsg.c
        cp errmsg.rc $(f77_dir)\wfc\h\errmsg3.rc
        del errmsg.rc
        cp errgrp.c $(f77_dir)\wfc\h\errgrp3.c
        del errgrp.c
        del errmsg.msg
        errsrc eorc
        cp errcod.h $(f77_dir)\wfc\h\errcodr.h
        del errcod.h
        cp errmsg.c $(f77_dir)\wfc\h\errmsgr.c
        del errmsg.c
        cp errmsg.rc $(f77_dir)\wfc\h\errmsgr.rc
        del errmsg.rc
        cp errgrp.c $(f77_dir)\wfc\h\errgrpr.c
        del errgrp.c
        del errmsg.msg
        errsrc elic
        cp errcod.h $(f77_dir)\wfl\h\errcodi.h
        del errcod.h
        cp errmsg.c $(f77_dir)\wfl\h\errmsgi.c
        del errmsg.c
        cp errmsg.rc $(f77_dir)\wfl\h\errmsgi.rc
        del errmsg.rc
        cp errgrp.c $(f77_dir)\wfl\h\errgrpi.c
        del errgrp.c
        del errmsg.msg
        errsrc el3c
        cp errcod.h $(f77_dir)\wfl\h\errcod3.h
        del errcod.h
        cp errmsg.c $(f77_dir)\wfl\h\errmsg3.c
        del errmsg.c
        cp errmsg.rc $(f77_dir)\wfl\h\errmsg3.rc
        del errmsg.rc
        cp errgrp.c $(f77_dir)\wfl\h\errgrp3.c
        del errgrp.c
        del errmsg.msg
        errsrc elrc
        cp errcod.h $(f77_dir)\wfl\h\errcodr.h
        del errcod.h
        cp errmsg.c $(f77_dir)\wfl\h\errmsgr.c
        del errmsg.c
        cp errmsg.rc $(f77_dir)\wfl\h\errmsgr.rc
        del errmsg.rc
        cp errgrp.c $(f77_dir)\wfl\h\errgrpr.c
        del errgrp.c
        del errmsg.msg
#       errsrc jo r
#       cp errmsg.rc $(f77_dir)\f77lib\c\jerrmsg.rc
#       del errmsg.rc
#       del errcod.h
#       del errmsg.c
#       del errgrp.c
#       del errmsg.msg
        errsrc joic
        cp errmsg.rc $(f77_dir)\wfc\h\jerrmsgi.rc
        del errmsg.rc
        del errcod.h
        del errmsg.c
        del errgrp.c
        del errmsg.msg
        errsrc jo3c
        cp errmsg.rc $(f77_dir)\wfc\h\jerrmsg3.rc
        del errmsg.rc
        del errcod.h
        del errmsg.c
        del errgrp.c
        del errmsg.msg
        errsrc jorc
        cp errmsg.rc $(f77_dir)\wfc\h\jerrmsgr.rc
        del errmsg.rc
        del errcod.h
        del errmsg.c
        del errgrp.c
        del errmsg.msg
        errsrc jlic
        cp errmsg.rc $(f77_dir)\wfl\h\jerrmsgi.rc
        del errmsg.rc
        del errcod.h
        del errmsg.c
        del errgrp.c
        del errmsg.msg
        errsrc jl3c
        cp errmsg.rc $(f77_dir)\wfl\h\jerrmsg3.rc
        del errmsg.rc
        del errcod.h
        del errmsg.c
        del errgrp.c
        del errmsg.msg
        errsrc jlrc
        cp errmsg.rc $(f77_dir)\wfl\h\jerrmsgr.rc
        del errmsg.rc
        del errcod.h
        del errmsg.c
        del errgrp.c
        del errmsg.msg
!endif

errsrc : errsrc.exe .symbolic
        @%null

errsrc.exe : errsrc.obj
        wlink d a f errsrc

errsrc.obj : ../c/errsrc.c
        wcc386 $[@ -w3 -d1 -I"$(lang_root)/h" -I"$(c_dir)h"

makedoc : errdoc.exe .symbolic
        errdoc

errdoc.exe : errdoc.obj
        wlink d a f errdoc

errdoc.obj : ../c/errdoc.for
        wfc386 $[@ -d1

clean: .symbolic
        rm errmsg.* errsrc.* errgrp.* errdoc.* errcod.* *.ipf *.gml
