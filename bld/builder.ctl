# --------------------------------------------------------------------

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <DEVDIR>/<LOGFNAME>.<LOGEXT> ]

set FULLBUILD=1

cdsay .

echo Build: <1> <2> <3>
echo Building WATCOM on <BLD_HOST>, using WATCOM from <WATCOM>

[ BLOCK <BUILDMODE> bootstrap ]
#==============================
echo Bootstrapping compiler using GNU tools...
[ INCLUDE <DEVDIR>/buildlnx.ctl ]

[ BLOCK . . ]
#
#        First of all build prerequisite utilities used in the build
#
[ INCLUDE <DEVDIR>/builder/builder.ctl ]
[ BLOCK <BLD_HOST> DOS NT OS2 ]
[ INCLUDE <DEVDIR>/awk/prereq.ctl ]
[ BLOCK . . ]
[ INCLUDE <DEVDIR>/cpp/prereq.ctl ]
[ INCLUDE <DEVDIR>/pmake/prereq.ctl ]
[ INCLUDE <DEVDIR>/nwlib/prereq.ctl ]
[ INCLUDE <DEVDIR>/wcl/prereq.ctl ]
[ INCLUDE <DEVDIR>/wasm/prereq.ctl ]
[ INCLUDE <DEVDIR>/causeway/prereq.ctl ]
[ BLOCK .<BUILDMODE>. .. ]
[ INCLUDE <DEVDIR>/w32loadr/prereq.ctl ]
[ BLOCK . . ]
[ INCLUDE <DEVDIR>/yacc/prereq.ctl ]
[ INCLUDE <DEVDIR>/re2c/prereq.ctl ]
# [ INCLUDE <DEVDIR>/wgml/prereq.ctl ]
[ INCLUDE <DEVDIR>/whpcvt/prereq.ctl ]
[ INCLUDE <DEVDIR>/omftools/prereq.ctl ]
[ INCLUDE <DEVDIR>/helpcomp/prereq.ctl ]
[ INCLUDE <DEVDIR>/hcdos/prereq.ctl ]
[ INCLUDE <DEVDIR>/bmp2eps/prereq.ctl ]
[ INCLUDE <DEVDIR>/ssl/prereq.ctl ]
[ INCLUDE <DEVDIR>/wstub/builder.ctl ]
[ INCLUDE <DEVDIR>/wpack/prereq.ctl ]
#        Prebuild POSIX tools for build process
[ INCLUDE <DEVDIR>/posix/prereq.ctl ]
#        Prebuild wres.lib and new WRC with OS/2 support for build process
[ INCLUDE <DEVDIR>/wres/prereq.ctl ]
[ INCLUDE <DEVDIR>/rc/rc/prereq.ctl ]
#        Prebuild parsedlg tool for build process
[ INCLUDE <DEVDIR>/parsedlg/prereq.ctl ]
#
#        Before building anything for real, create up-to-date header files
#
[ INCLUDE <DEVDIR>/hdr/builder.ctl ]
#        Same for OS/2 and Win32 headers/import libs
[ INCLUDE <DEVDIR>/os2api/builder.ctl ]
[ INCLUDE <DEVDIR>/w32api/builder.ctl ]
#        Same for Win16 headers/import libs
[ INCLUDE <DEVDIR>/w16api/builder.ctl ]
#
#        Next step, build libraries used by various projects
#
#        RC builds resource file libraries for everybody
[ INCLUDE <DEVDIR>/wres/builder.ctl ]
#        WPI and commonui needs to be done before SDK
[ INCLUDE <DEVDIR>/wpi/builder.ctl ]
[ INCLUDE <DEVDIR>/commonui/builder.ctl ]
#
[ INCLUDE <DEVDIR>/rc/builder.ctl ]
#        SDK must be done after DISASM?
[ INCLUDE <DEVDIR>/sdk/builder.ctl ]
#        Librariess for different program are next
#        WPI must be done before SDK and UI libs
[ INCLUDE <DEVDIR>/ncurses/builder.ctl ]
[ INCLUDE <DEVDIR>/ui/builder.ctl ]
[ INCLUDE <DEVDIR>/gui/builder.ctl ]
#        AUI needs to be built after UI/ncurses and GUI
[ INCLUDE <DEVDIR>/aui/builder.ctl ]
#        OWL/ORL must be done early so that clients are up-to-date
[ INCLUDE <DEVDIR>/owl/builder.ctl ]
#        AS must be done after OWL but before CFE
[ INCLUDE <DEVDIR>/as/builder.ctl ]
[ INCLUDE <DEVDIR>/orl/builder.ctl ]
#        DWARF must be done early so that DWARF library users are up-to-date
[ INCLUDE <DEVDIR>/dwarf/builder.ctl ]
#
# Hack for build OW 1.9 by OW 1.8
# build new linker which is able to create executable
# format not available in OW 1.8 linker (RDOS target)
[ INCLUDE <DEVDIR>/wl/prereq.ctl ]
#
#        Now build Open Watcom libraries
#
#        emu libraries must be made before C libraries
[ INCLUDE <DEVDIR>/emu/builder.ctl ]
[ INCLUDE <DEVDIR>/emu86/builder.ctl ]
#        C libraries must be made after linker
[ INCLUDE <DEVDIR>/lib_misc/builder.ctl ]
[ INCLUDE <DEVDIR>/clib/builder.ctl ]
[ INCLUDE <DEVDIR>/mathlib/builder.ctl ]
[ INCLUDE <DEVDIR>/cfloat/builder.ctl ]
[ INCLUDE <DEVDIR>/win386/builder.ctl ]
#        src must be made after the C library
[ INCLUDE <DEVDIR>/src/builder.ctl ]
#        graphics library must be made after C library
[ INCLUDE <DEVDIR>/graphlib/builder.ctl ]
[ INCLUDE <DEVDIR>/nwlib/builder.ctl ]
#        RCSDLL must be before VI and VIPER
[ INCLUDE <DEVDIR>/rcsdll/builder.ctl ]
[ INCLUDE <DEVDIR>/causeway/builder.ctl ]
#
#       Now build the compilers
#
#       W32LDR must be done before EXEs that use the loader
[ INCLUDE <DEVDIR>/w32loadr/builder.ctl ]
#
#        Starting with the code generators
#
#        WOMP must be done before F77, now it is not necessary
#[ INCLUDE <DEVDIR>/womp/builder.ctl ]
#        WASM must be done early so that inline assembler users are uptodate
#        (no longer necessary, can be anywhere)
[ INCLUDE <DEVDIR>/wasm/builder.ctl ]
[ INCLUDE <DEVDIR>/cg/builder.ctl ]
#        C front ends must be built after code generator
[ INCLUDE <DEVDIR>/cc/builder.ctl ]
[ INCLUDE <DEVDIR>/wcl/builder.ctl ]
[ INCLUDE <DEVDIR>/plusplus/builder.ctl ]
[ INCLUDE <DEVDIR>/fe_misc/builder.ctl ]
#        FORTRAN 77 compilers must be built after code generators
[ INCLUDE <DEVDIR>/f77/builder.ctl ]
#        FORTRAN 77 libraries
[ INCLUDE <DEVDIR>/f77/f77lib/builder.ctl ]
[ INCLUDE <DEVDIR>/f77/samples/builder.ctl ]
[ INCLUDE <DEVDIR>/wl/builder.ctl ]
#
#        Now we can build the C++ libraries - must be done after C library
#        as well as after the C++ compilers
#
[ INCLUDE <DEVDIR>/cpplib/builder.ctl ]
[ INCLUDE <DEVDIR>/wclass/builder.ctl ]
#        rtdll must be made after all other libraries
#[ INCLUDE <DEVDIR>/rtdll/builder.ctl ]
#        WATTCP library must be build before TRAP
[ INCLUDE <DEVDIR>/wattcp/builder.ctl ]
#
#        Now let's build the utilities and other stuff
#
[ INCLUDE <DEVDIR>/vi/builder.ctl ]
[ INCLUDE <DEVDIR>/wdisasm/builder.ctl ]
[ INCLUDE <DEVDIR>/ndisasm/builder.ctl ]
[ INCLUDE <DEVDIR>/bdiff/builder.ctl ]
[ INCLUDE <DEVDIR>/misc/builder.ctl ]
[ INCLUDE <DEVDIR>/techinfo/builder.ctl ]
[ INCLUDE <DEVDIR>/help/builder.ctl ]
[ INCLUDE <DEVDIR>/cmdedit/builder.ctl ]
[ INCLUDE <DEVDIR>/brinfo/builder.ctl ]
[ INCLUDE <DEVDIR>/trap/builder.ctl ]
[ INCLUDE <DEVDIR>/dip/builder.ctl ]
[ INCLUDE <DEVDIR>/mad/builder.ctl ]
[ INCLUDE <DEVDIR>/wv/builder.ctl ]
[ INCLUDE <DEVDIR>/wsample/builder.ctl ]
[ INCLUDE <DEVDIR>/wprof/builder.ctl ]
[ INCLUDE <DEVDIR>/wmake/builder.ctl ]
[ INCLUDE <DEVDIR>/wtouch/builder.ctl ]
[ INCLUDE <DEVDIR>/wstrip/builder.ctl ]
[ INCLUDE <DEVDIR>/editdll/builder.ctl ]
[ INCLUDE <DEVDIR>/mstools/builder.ctl ]
#[ INCLUDE <DEVDIR>/online/builder.ctl ]
[ INCLUDE <DEVDIR>/posix/builder.ctl ]
[ INCLUDE <DEVDIR>/cpp/builder.ctl ]
[ INCLUDE <DEVDIR>/pgchart/builder.ctl ]
[ INCLUDE <DEVDIR>/dig/builder.ctl ]
[ INCLUDE <DEVDIR>/uiforms/builder.ctl ]
[ INCLUDE <DEVDIR>/watcom/builder.ctl ]
[ INCLUDE <DEVDIR>/pbide/builder.ctl ]
[ INCLUDE <DEVDIR>/trmem/builder.ctl ]
#[ INCLUDE <DEVDIR>/version/builder.ctl ]
[ INCLUDE <DEVDIR>/dmpobj/builder.ctl ]
[ INCLUDE <DEVDIR>/exedump/builder.ctl ]
[ INCLUDE <DEVDIR>/cvpack/builder.ctl ]
[ INCLUDE <DEVDIR>/wic/builder.ctl ]
[ INCLUDE <DEVDIR>/redist/builder.ctl ]
#[ INCLUDE <DEVDIR>/wgml/builder.ctl ]
[ INCLUDE <DEVDIR>/parsedlg/builder.ctl ]
[ INCLUDE <DEVDIR>/helpcomp/builder.ctl ]
[ INCLUDE <DEVDIR>/wipfc/builder.ctl ]
[ INCLUDE <DEVDIR>/uninstal/builder.ctl ]
[ INCLUDE <DEVDIR>/setupgui/builder.ctl ]
# Build IDE and browser last, as they depend on wgml
[ INCLUDE <DEVDIR>/browser/builder.ctl ]
[ INCLUDE <DEVDIR>/viper/builder.ctl ]
[ INCLUDE <DEVDIR>/idebatch/builder.ctl ]
[ INCLUDE <DEVDIR>/viprdemo/builder.ctl ]
[ BLOCK <DOC_BUILD> 1 ]
[ INCLUDE <OWROOT>/docs/builder.ctl ]
[ BLOCK . . ]
#
#        Do CDSAY to see end time
#
cdsay .
#
#        Clean build tools, must be last item (self destructs)
#
[ INCLUDE <DEVDIR>/builder/clean.ctl ]
