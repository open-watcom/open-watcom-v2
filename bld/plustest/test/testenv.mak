# TESTENV.MAK -- set C++ testing environment
#
# The testing environment is set by:
#
# (1) When testenv=name is specified on the TSTRM command
#
# (2) Otherwise, the def_env enviroment variable is used if defined
#
# (3) Otherwise, the following are used:
#       def_cpp         compiler to be used
#       def_run         run386 or nothing
#       def_runext      .exp or .exe
#       def_lnk         linker options
#
# 93/06/03 -- J.W.Welch         -- defined

!ifndef testenv
!       ifdef %def_env
!           define testenv $(%def_env)
!       endif
!endif

!ifdef testenv
!       ifeq testenv d386
!           define tstrm_cpp=run386 $(plusplus_dir)\dos386\wcpp386d.exp
!           define tstrm_opts -ew
!           define tstrm_run run386
!           define tstrm_runext .exp
!           define tstrm_lnk system pharlap option quiet debug all
!       else ifeq testenv d386X
!           define tstrm_cpp run386 $(plusplus_dir)\dos386\wcpp386d.exp
!           define tstrm_opts -zm -xs -ew
!           define tstrm_run run386
!           define tstrm_runext .exp
!           define tstrm_lnk option eliminate system pharlap option quiet debug all
!       else ifeq testenv d3S
!           define tstrm_cpp run386 $(plusplus_dir)\dosi86\wcpp386d.exp
!           define tstrm_opts -zm -xs -ew -3s
!           define tstrm_run run386
!           define tstrm_runext .exp
!           define tstrm_lnk option eliminate system pharlap option quiet debug all
!       else ifeq testenv di86C
!           define tstrm_cpp run386 $(plusplus_dir)\dosi86\wcppi86d.exp
!           define tstrm_opts -mc -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate system dos option quiet debug all
!       else ifeq testenv di86S
!           define tstrm_cpp run386 $(plusplus_dir)\dosi86\wcppi86d.exp
!           define tstrm_opts -ms -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate system dos option quiet debug all
!       else ifeq testenv 386R
!           define tstrm_cpp $(lang_root)\bin\wpp386.exe
!           define tstrm_opts -ew
!           define tstrm_run run386
!           define tstrm_runext .exp
!           define tstrm_lnk option eliminate system pharlap option quiet debug all
!       else ifeq testenv 286S
!           define tstrm_cpp $(lang_root)\bin\wpp.exe
!           define tstrm_opts -ms -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate system dos option quiet debug all
!       else ifeq testenv 286C
!           define tstrm_cpp $(lang_root)\bin\wpp.exe
!           define tstrm_opts -mc -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate system dos option quiet debug all
!       else ifeq testenv os2d386 
!           define tstrm_cpp $(plusplus_dir)\os2386\wcpp386d.exe
!           define tstrm_opts -ew $(cpp_test_switches)
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv os2di86 
!           define tstrm_cpp $(plusplus_dir)\os2i86\wcppi86d.exe
!           define tstrm_opts -ew $(cpp_test_switches)
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv os2386 
!           define tstrm_cpp $(lang_root)\binp\wpp386.exe
!           define tstrm_opts -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv os2di86 
!           define tstrm_cpp $(plusplus_dir)\os2i86\wcppi86d.exe
!           define tstrm_opts -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv os2di86c
!           define tstrm_cpp $(plusplus_dir)\os2i86\wcppi86d.exe
!           define tstrm_opts -ew -mc
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv os2i86 
!           define tstrm_cpp $(lang_root)\binp\wpp.exe
!           define tstrm_opts -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv os2i86c 
!           define tstrm_cpp $(lang_root)\binp\wpp.exe
!           define tstrm_opts -ew -mc
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv os2d3s
!           define tstrm_cpp $(plusplus_dir)\os2386\wcpp386d.exe
!           define tstrm_opts -ew $(cpp_test_switches)
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv ntd386 
!           define tstrm_cpp $(plusplus_dir)\nt386\wcpp386d.exe
!           define tstrm_opts -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv nt386 
!           define tstrm_cpp $(lang_root)\binnt\wpp386.exe
!           define tstrm_opts -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv ntdi86 
!           define tstrm_cpp $(plusplus_dir)\nti86\wcppi86d.exe
!           define tstrm_opts -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv ntdi86c
!           define tstrm_cpp $(plusplus_dir)\nti86\wcppi86d.exe
!           define tstrm_opts -ew -mc
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else ifeq testenv nti86 
!           define tstrm_cpp $(lang_root)\binnt\wpp.exe
!           define tstrm_opts -ew
!           define tstrm_run
!           define tstrm_runext .exe
!           define tstrm_lnk option eliminate option quiet debug all
!       else
!           error *** invalid environment: $(testenv)
!       endif
!       define def_linker wlink.exe
!else
!       ifdef %def_cpp
!               define tstrm_cpp $(%def_cpp)
!               define tstrm_run $(%def_run)
!               define tstrm_runext $(%def_run_ext)
!               define tstrm_lnk $(%def_lnk)
!       else
!               error *** None of testenv, def_env, or def_cpp are defined!!!
!       endif
!endif


# List of defined enviroments:

environments: .symbolic
    @echo *** Valid environments are:
    @echo ***     d386  -- development C++ for 386
    @echo ***     d386x -- development C++ for 386, -xs-zm, linker stripping
    @echo ***     d3s   -- development C++ for 386, -3s-xs-zm, linker stripping
    @echo ***     di86c -- development C++ for i86, -mc
    @echo ***     386r  -- 386 reference compiler linker stripping
    @echo ***     286s  -- 286 reference compiler (-ms) linker stripping
    @echo ***     286c  -- 286 reference compiler (-mc) linker stripping
    @echo ***   os2386  -- production C++ for 386 under OS/2
    @echo ***   os2i86  -- production C++ for i86 under OS/2 -ms
    @echo ***   os2i86c -- production C++ for i86 under OS/2 -mc
    @echo ***  os2d386  -- development C++ for 386 under OS/2
    @echo ***  os2di86  -- development C++ for i86 under OS/2
    @echo ***  os2d3s   -- development C++ for 386, -3s-xs-zm, linker stripping
    @echo ***   nt386   -- production C++ for 386 under NT
    @echo ***   nti86   -- production C++ for i86 under NT
    @echo ***  ntd386   -- development C++ for 386 under NT
    @echo ***  ntdi86   -- development C++ for i86 under NT
    @echo ***  ntdi86c  -- development C++ for i86 under NT, -mc
    @echo ***
