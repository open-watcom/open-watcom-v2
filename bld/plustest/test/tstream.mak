# TSTREAM -- common definitions for C++ Test Streams
#
# Driving makefile sets:
#       * tstrm_opts -- C++ options for this teststream
#
#       * ==> optional option
#       - ==> required option
#
# Options for a file XXX.Y can be set by defining opt_XXX
#
# 93/06/02 -- J.W.Welch         -- defined
#
# 94/02/24 -- J.W.Welch         -- added echos at start
#                               -- used $(h_files)
#                               -- used $cpp_test_dir)

# .optimize

!ifdef h_files
includes_are=$(bld_h);$(bld_h)\win;$(watcom_h);$(%include);$(h_files)
!else
includes_are=$(bld_h);$(bld_h)\win;$(watcom_h);$(%include)
!endif

.before
    @echo environment: $(testenv)
    @echo includes: $(includes_are)
    @set include=$(includes_are)
    @echo compile_options: $(tstrm_dopts) $(tstrm_opts)
    @echo cpp_test_switches: $(cpp_test_switches)
    @echo compiler: $(tstrm_cpp)
    @echo ----------------------------------------------------------


# COMMON DEFINITIONS

tstrm_linker=wlink.exe option undefsok
tstrm_dopts = -zq -e1000


# OPTIONS FOR ENVIRONMENTS

!include $(cpp_test_dir)\testenv.mak

.extensions:
.extensions: .vcl $(tstrm_runext) .lnk .obj .ppo .err .cpp .c

.c: $(c_files)
.cpp: $(cpp_files)
.ppo: testout\
.obj: testout\
.err: testout\
.lnk: testout\
.vcl: testout\
$(tstrm_runext): testout\

.c.err:
        @echo ...compile: $[@ $(opt_$[&)
        @-$(tstrm_cpp) $[@ $(tstrm_dopts) $(tstrm_opts) $(opt_$[&) >$^@

.cpp.err:
        @echo ...compile: $[@ $(opt_$[&)
        @-$(tstrm_cpp) $[@ $(tstrm_dopts) $(tstrm_opts) $(opt_$[&) >$^@

.cpp.obj:
        @echo ...compile: $[@ $(opt_$[&)
        @-$(tstrm_cpp) $[@ $(tstrm_dopts) $(tstrm_opts) $(opt_$[&) -fotestout\$^@ >testout\$^*.err

.c.obj:
        @echo ...compile: $[@ $(opt_$[&)
        @-$(tstrm_cpp) $[@ $(tstrm_dopts) $(tstrm_opts) $(opt_$[&) -fotestout\$^@ >testout\$^*.err

.cpp.ppo:
        @echo ...preproc: $[@ $(opt_$[&)
        @-$(tstrm_cpp) -p $[@ $(tstrm_dopts) $(tstrm_opts) $(opt_$[&) >$^@

.c.ppo:
        @echo ...preproc: $[@ $(opt_$[&)
        @-$(tstrm_cpp) -p $[@ $(tstrm_dopts) $(tstrm_opts) $(opt_$[&) >$^@

.obj$(tstrm_runext):
        @echo ...linking: testout\$^@
        @$(tstrm_linker) name testout\$^@ $(tstrm_lnk) file $[@

.lnk$(tstrm_runext):
        @echo ...linking: testout\$^@
        @$(tstrm_linker) $(tstrm_lnk) @$[@

$(tstrm_runext).vcl:
        @echo ...execute: $[*
        @-$(tstrm_run) $[* >$^@
        @if exist $[*.obj @del $[*.obj
        @if exist $[*.lnk @del $[*.lnk
        @del $[*$(tstrm_runext)
