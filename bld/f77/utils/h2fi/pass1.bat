@echo off
if [.%1] == [.] goto help
if [.%2] == [.] goto help
echo ==== beginning pass 1 ====
echo function init_types() { >tmp1
touch tmp2
echo } >tmp3
set counter=
rm -f tmp_out
:loop
    if [%counter%] == [11111] goto end_loop
    echo ==== starting iteration ====
    cat tmp1 tmp2 tmp3 >tmp.awk
    call run1.bat %1basedef.h tmp.awk tmp_out
    call run1.bat %1basemac.h tmp.awk tmp_out
    call run1.bat %1basemid.h tmp.awk tmp_out
    call run1.bat %1bdcalls.h tmp.awk tmp_out
    call run1.bat %1bse.h tmp.awk tmp_out
    call run1.bat %1bsedev.h tmp.awk tmp_out
    call run1.bat %1bsedos.h tmp.awk tmp_out
    call run1.bat %1bseerr.h tmp.awk tmp_out
    call run1.bat %1bsememf.h tmp.awk tmp_out
    call run1.bat %1bseord.h tmp.awk tmp_out
    call run1.bat %1bsesub.h tmp.awk tmp_out
    call run1.bat %1bsetib.h tmp.awk tmp_out
    call run1.bat %1bsexcpt.h tmp.awk tmp_out
    call run1.bat %1cnrtree.h tmp.awk tmp_out
    call run1.bat %1hmtailor.h tmp.awk tmp_out
    call run1.bat %1mi.h tmp.awk tmp_out
    call run1.bat %1os2.h tmp.awk tmp_out
    call run1.bat %1os2def.h tmp.awk tmp_out
    call run1.bat %1os2nls.h tmp.awk tmp_out
    call run1.bat %1os2std.h tmp.awk tmp_out
    call run1.bat %1pm.h tmp.awk tmp_out
    call run1.bat %1pmavio.h tmp.awk tmp_out
    call run1.bat %1pmbitmap.h tmp.awk tmp_out
    call run1.bat %1pmddim.h tmp.awk tmp_out
    call run1.bat %1pmdev.h tmp.awk tmp_out
    call run1.bat %1pmerr.h tmp.awk tmp_out
    call run1.bat %1pmfont.h tmp.awk tmp_out
    call run1.bat %1pmgpi.h tmp.awk tmp_out
    call run1.bat %1pmhelp.h tmp.awk tmp_out
    call run1.bat %1pmmle.h tmp.awk tmp_out
    call run1.bat %1pmord.h tmp.awk tmp_out
    call run1.bat %1pmpic.h tmp.awk tmp_out
    call run1.bat %1pmsei.h tmp.awk tmp_out
    call run1.bat %1pmshl.h tmp.awk tmp_out
    call run1.bat %1pmspl.h tmp.awk tmp_out
    call run1.bat %1pmstddlg.h tmp.awk tmp_out
    call run1.bat %1pmtypes.h tmp.awk tmp_out
    call run1.bat %1pmwin.h tmp.awk tmp_out
    call run1.bat %1pmwp.h tmp.awk tmp_out
    call run1.bat %1rexxsaa.h tmp.awk tmp_out
    cat tmp2 tmp_out >tmp_out2
    rm -f tmp_out
    mv tmp_out2 tmp2
    set counter=%counter%1
    goto loop
:end_loop
mv tmp.awk inityp.awk
rm -f tmp1 tmp2 tmp3
goto done
:help
echo usage: pass1 path_to_h path_to_fi
echo for example: pass1 h\ fi\
:done
