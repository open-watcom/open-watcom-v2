# --------------------------------------------------------------------

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <SRCDIR>/<LOGFNAME>.<LOGEXT> ]

set FULLBUILD=1

cdsay .

echo Build: <1> <2> <3>
echo Building WATCOM on <BLD_HOST>, using WATCOM from <WATCOM>

[ BLOCK <BUILDMODE> bootstrap ]
#==============================
echo Bootstrapping compiler using GNU tools...
[ INCLUDE <SRCDIR>/buildlnx.ctl ]

[ BLOCK . . ]
#
#        First of all build prerequisite utilities used in the build
#
[ INCLUDE <SRCDIR>/builder/builder.ctl ]
[ BLOCK <BLD_HOST> DOS NT OS2 ]
[ INCLUDE <SRCDIR>/awk/prereq.ctl ]
[ BLOCK . . ]
[ INCLUDE <SRCDIR>/cpp/prereq.ctl ]
[ INCLUDE <SRCDIR>/pmake/prereq.ctl ]
[ INCLUDE <SRCDIR>/nwlib/prereq.ctl ]
[ INCLUDE <SRCDIR>/wcl/prereq.ctl ]
[ INCLUDE <SRCDIR>/wasm/prereq.ctl ]
[ INCLUDE <SRCDIR>/causeway/prereq.ctl ]
[ BLOCK .<BUILDMODE>. .. ]
[ INCLUDE <SRCDIR>/w32loadr/prereq.ctl ]
[ BLOCK . . ]
[ INCLUDE <SRCDIR>/yacc/prereq.ctl ]
[ INCLUDE <SRCDIR>/re2c/prereq.ctl ]
# [ INCLUDE <SRCDIR>/wgml/prereq.ctl ]
[ INCLUDE <SRCDIR>/whpcvt/prereq.ctl ]
[ INCLUDE <SRCDIR>/omftools/prereq.ctl ]
[ INCLUDE <SRCDIR>/helpcomp/prereq.ctl ]
[ INCLUDE <SRCDIR>/hcdos/prereq.ctl ]
[ INCLUDE <SRCDIR>/bmp2eps/prereq.ctl ]
[ INCLUDE <SRCDIR>/ssl/prereq.ctl ]
[ INCLUDE <SRCDIR>/wstub/builder.ctl ]
[ INCLUDE <SRCDIR>/wpack/prereq.ctl ]
#        Prebuild POSIX tools for build process
[ INCLUDE <SRCDIR>/posix/prereq.ctl ]
#        Prebuild wres.lib and new WRC with OS/2 support for build process
[ INCLUDE <SRCDIR>/wres/prereq.ctl ]
[ INCLUDE <SRCDIR>/rc/rc/prereq.ctl ]
#        Prebuild parsedlg tool for build process
[ INCLUDE <SRCDIR>/parsedlg/prereq.ctl ]
#        Prebuild vi script compiler for build process
[ INCLUDE <SRCDIR>/vi/vicomp/prereq.ctl ]
#
#        Before building anything for real, create up-to-date header files
#
[ INCLUDE <SRCDIR>/hdr/builder.ctl ]
#        Same for OS/2 and Win32 headers/import libs
[ INCLUDE <SRCDIR>/os2api/builder.ctl ]
[ INCLUDE <SRCDIR>/w32api/builder.ctl ]
#        Same for Win16 headers/import libs
[ INCLUDE <SRCDIR>/w16api/builder.ctl ]
#
#        Next step, build libraries used by various projects
#
#        RC builds resource file libraries for everybody
[ INCLUDE <SRCDIR>/wres/builder.ctl ]
#        WPI and commonui needs to be done before SDK
[ INCLUDE <SRCDIR>/wpi/builder.ctl ]
[ INCLUDE <SRCDIR>/commonui/builder.ctl ]
#
[ INCLUDE <SRCDIR>/rc/builder.ctl ]
#        SDK must be done after DISASM?
[ INCLUDE <SRCDIR>/sdk/builder.ctl ]
#        Librariess for different program are next
#        WPI must be done before SDK and UI libs
[ INCLUDE <SRCDIR>/ncurses/builder.ctl ]
[ INCLUDE <SRCDIR>/ui/builder.ctl ]
[ INCLUDE <SRCDIR>/gui/builder.ctl ]
#        AUI needs to be built after UI/ncurses and GUI
[ INCLUDE <SRCDIR>/aui/builder.ctl ]
#        OWL/ORL must be done early so that clients are up-to-date
[ INCLUDE <SRCDIR>/owl/builder.ctl ]
#        AS must be done after OWL but before CFE
[ INCLUDE <SRCDIR>/as/builder.ctl ]
[ INCLUDE <SRCDIR>/orl/builder.ctl ]
#        DWARF must be done early so that DWARF library users are up-to-date
[ INCLUDE <SRCDIR>/dwarf/builder.ctl ]
#
# Hack for build OW 1.9 by OW 1.8
# build new linker which is able to create executable
# format not available in OW 1.8 linker (RDOS target)
[ INCLUDE <SRCDIR>/wl/prereq.ctl ]
#
#        Now build Open Watcom libraries
#
#        FPU emu libraries must be made before C libraries
[ INCLUDE <SRCDIR>/fpuemu/builder.ctl ]
#        C libraries must be made after linker
[ INCLUDE <SRCDIR>/lib_misc/builder.ctl ]
[ INCLUDE <SRCDIR>/clib/builder.ctl ]
[ INCLUDE <SRCDIR>/mathlib/builder.ctl ]
[ INCLUDE <SRCDIR>/cfloat/builder.ctl ]
[ INCLUDE <SRCDIR>/win386/builder.ctl ]
#        graphics library must be made after C library
[ INCLUDE <SRCDIR>/graphlib/builder.ctl ]
[ INCLUDE <SRCDIR>/nwlib/builder.ctl ]
#        RCSDLL must be before VI and VIPER
[ INCLUDE <SRCDIR>/rcsdll/builder.ctl ]
[ INCLUDE <SRCDIR>/causeway/builder.ctl ]
#
#       Now build the compilers
#
#       W32LDR must be done before EXEs that use the loader
[ INCLUDE <SRCDIR>/w32loadr/builder.ctl ]
#
#        Starting with the code generators
#
#        WOMP must be done before F77, now it is not necessary
#[ INCLUDE <SRCDIR>/womp/builder.ctl ]
#        WASM must be done early so that inline assembler users are uptodate
#        (no longer necessary, can be anywhere)
[ INCLUDE <SRCDIR>/wasm/builder.ctl ]
[ INCLUDE <SRCDIR>/cg/builder.ctl ]
#        C front ends must be built after code generator
[ INCLUDE <SRCDIR>/cc/builder.ctl ]
[ INCLUDE <SRCDIR>/wcl/builder.ctl ]
[ INCLUDE <SRCDIR>/plusplus/builder.ctl ]
[ INCLUDE <SRCDIR>/fe_misc/builder.ctl ]
#        FORTRAN 77 compilers must be built after code generators
[ INCLUDE <SRCDIR>/f77/builder.ctl ]
#        FORTRAN 77 libraries
[ INCLUDE <SRCDIR>/f77/f77lib/builder.ctl ]
[ INCLUDE <SRCDIR>/wl/builder.ctl ]
#
#        Now we can build the C++ libraries - must be done after C library
#        as well as after the C++ compilers
#
[ INCLUDE <SRCDIR>/cpplib/builder.ctl ]
[ INCLUDE <SRCDIR>/wclass/builder.ctl ]
#        rtdll must be made after all other libraries
#[ INCLUDE <SRCDIR>/rtdll/builder.ctl ]
#        WATTCP library must be build before TRAP
[ INCLUDE <SRCDIR>/wattcp/builder.ctl ]
#
#        Now let's build the utilities and other stuff
#
[ INCLUDE <SRCDIR>/vi/builder.ctl ]
[ INCLUDE <SRCDIR>/wdisasm/builder.ctl ]
[ INCLUDE <SRCDIR>/ndisasm/builder.ctl ]
[ INCLUDE <SRCDIR>/bdiff/builder.ctl ]
[ INCLUDE <SRCDIR>/misc/builder.ctl ]
[ INCLUDE <SRCDIR>/techinfo/builder.ctl ]
[ INCLUDE <SRCDIR>/help/builder.ctl ]
[ INCLUDE <SRCDIR>/cmdedit/builder.ctl ]
[ INCLUDE <SRCDIR>/brinfo/builder.ctl ]
[ INCLUDE <SRCDIR>/trap/builder.ctl ]
[ INCLUDE <SRCDIR>/dip/builder.ctl ]
[ INCLUDE <SRCDIR>/mad/builder.ctl ]
[ INCLUDE <SRCDIR>/wv/builder.ctl ]
[ INCLUDE <SRCDIR>/wsample/builder.ctl ]
[ INCLUDE <SRCDIR>/wprof/builder.ctl ]
[ INCLUDE <SRCDIR>/wmake/builder.ctl ]
[ INCLUDE <SRCDIR>/wtouch/builder.ctl ]
[ INCLUDE <SRCDIR>/wstrip/builder.ctl ]
[ INCLUDE <SRCDIR>/editdll/builder.ctl ]
[ INCLUDE <SRCDIR>/mstools/builder.ctl ]
#[ INCLUDE <SRCDIR>/online/builder.ctl ]
[ INCLUDE <SRCDIR>/posix/builder.ctl ]
[ INCLUDE <SRCDIR>/cpp/builder.ctl ]
[ INCLUDE <SRCDIR>/dig/builder.ctl ]
[ INCLUDE <SRCDIR>/uiforms/builder.ctl ]
[ INCLUDE <SRCDIR>/watcom/builder.ctl ]
[ INCLUDE <SRCDIR>/pbide/builder.ctl ]
[ INCLUDE <SRCDIR>/trmem/builder.ctl ]
#[ INCLUDE <SRCDIR>/version/builder.ctl ]
[ INCLUDE <SRCDIR>/dmpobj/builder.ctl ]
[ INCLUDE <SRCDIR>/exedump/builder.ctl ]
[ INCLUDE <SRCDIR>/cvpack/builder.ctl ]
[ INCLUDE <SRCDIR>/wic/builder.ctl ]
[ INCLUDE <SRCDIR>/redist/builder.ctl ]
#[ INCLUDE <SRCDIR>/wgml/builder.ctl ]
[ INCLUDE <SRCDIR>/parsedlg/builder.ctl ]
[ INCLUDE <SRCDIR>/helpcomp/builder.ctl ]
[ INCLUDE <SRCDIR>/wipfc/builder.ctl ]
[ INCLUDE <SRCDIR>/uninstal/builder.ctl ]
[ INCLUDE <SRCDIR>/setupgui/builder.ctl ]
# Build IDE and browser last, as they depend on wgml
[ INCLUDE <SRCDIR>/browser/builder.ctl ]
[ INCLUDE <SRCDIR>/viper/builder.ctl ]
[ INCLUDE <SRCDIR>/idebatch/builder.ctl ]
[ INCLUDE <SRCDIR>/viprdemo/builder.ctl ]
# prepare source code samples
[ INCLUDE <SRCDIR>/src/builder.ctl ]
#
[ BLOCK <OWDOCBUILD> 1 ]
[ INCLUDE <OWROOT>/docs/builder.ctl ]
[ BLOCK . . ]
#
#        Do CDSAY to see end time
#
cdsay .
#
#        Clean build tools, must be last item (self destructs)
#
[ INCLUDE <SRCDIR>/builder/clean.ctl ]
