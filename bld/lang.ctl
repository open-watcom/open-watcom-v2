# --------------------------------------------------------------------

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <DEVDIR>/<LOGFNAME>.<LOGEXT> ]

set FULLBUILD=1

cdsay .

echo Languages Build: <1> <2> <3>
echo Building WATCOM on <BLD_HOST>, using WATCOM from <WATCOM>

[ BLOCK <BUILDMODE> bootstrap ]
#==============================
echo Bootstrapping compiler using GNU tools...
[ INCLUDE <DEVDIR>/langlnx.ctl ]

[ BLOCK . . ]
#
#        First of all build prerequisite utilities used in the build
#
[ INCLUDE <DEVDIR>/builder/lang.ctl ]
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
[ INCLUDE <DEVDIR>/wstub/lang.ctl ]
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
[ INCLUDE <DEVDIR>/hdr/lang.ctl ]
#        Same for OS/2 and Win32 headers/import libs
[ INCLUDE <DEVDIR>/os2api/lang.ctl ]
[ INCLUDE <DEVDIR>/w32api/lang.ctl ]
#        Same for Win16 headers/import libs
[ INCLUDE <DEVDIR>/w16api/lang.ctl ]
#
#        Next step, build libraries used by various projects
#
#        RC builds resource file libraries for everybody
[ INCLUDE <DEVDIR>/wres/lang.ctl ]
#        WPI and commonui needs to be done before SDK
[ INCLUDE <DEVDIR>/wpi/lang.ctl ]
[ INCLUDE <DEVDIR>/commonui/lang.ctl ]
#
[ INCLUDE <DEVDIR>/rc/lang.ctl ]
#        SDK must be done after DISASM?
[ INCLUDE <DEVDIR>/sdk/lang.ctl ]
#        Librariess for different program are next
#        WPI must be done before SDK and UI libs
[ INCLUDE <DEVDIR>/ncurses/lang.ctl ]
[ INCLUDE <DEVDIR>/ui/lang.ctl ]
[ INCLUDE <DEVDIR>/gui/lang.ctl ]
#        AUI needs to be built after UI/ncurses and GUI
[ INCLUDE <DEVDIR>/aui/lang.ctl ]
#        OWL/ORL must be done early so that clients are up-to-date
[ INCLUDE <DEVDIR>/owl/lang.ctl ]
#        AS must be done after OWL but before CFE
[ INCLUDE <DEVDIR>/as/lang.ctl ]
[ INCLUDE <DEVDIR>/orl/lang.ctl ]
#        DWARF must be done early so that DWARF library users are up-to-date
[ INCLUDE <DEVDIR>/dwarf/lang.ctl ]
#
# Hack for build OW 1.9 by OW 1.8
# build new linker which is able to create executable
# format not available in OW 1.8 linker (RDOS target)
[ INCLUDE <DEVDIR>/wl/prereq.ctl ]
#
#        Now build Open Watcom libraries
#
#        emu libraries must be made before C libraries
[ INCLUDE <DEVDIR>/emu/lang.ctl ]
[ INCLUDE <DEVDIR>/emu86/lang.ctl ]
#        C libraries must be made after linker
[ INCLUDE <DEVDIR>/lib_misc/lang.ctl ]
[ INCLUDE <DEVDIR>/clib/lang.ctl ]
[ INCLUDE <DEVDIR>/mathlib/lang.ctl ]
[ INCLUDE <DEVDIR>/cfloat/lang.ctl ]
[ INCLUDE <DEVDIR>/win386/lang.ctl ]
#        src must be made after the C library
[ INCLUDE <DEVDIR>/src/lang.ctl ]
#        graphics library must be made after C library
[ INCLUDE <DEVDIR>/graphlib/lang.ctl ]
[ INCLUDE <DEVDIR>/nwlib/lang.ctl ]
#        RCSDLL must be before VI and VIPER
[ INCLUDE <DEVDIR>/rcsdll/lang.ctl ]
[ INCLUDE <DEVDIR>/causeway/lang.ctl ]
#
#       Now build the compilers
#
#       W32LDR must be done before EXEs that use the loader
[ INCLUDE <DEVDIR>/w32loadr/lang.ctl ]
#
#        Starting with the code generators
#
#        WOMP must be done before F77, now it is not necessary
#[ INCLUDE <DEVDIR>/womp/lang.ctl ]
#        WASM must be done early so that inline assembler users are uptodate
#        (no longer necessary, can be anywhere)
[ INCLUDE <DEVDIR>/wasm/lang.ctl ]
[ INCLUDE <DEVDIR>/cg/lang.ctl ]
#        C front ends must be built after code generator
[ INCLUDE <DEVDIR>/cc/lang.ctl ]
[ INCLUDE <DEVDIR>/wcl/lang.ctl ]
[ INCLUDE <DEVDIR>/plusplus/lang.ctl ]
[ INCLUDE <DEVDIR>/fe_misc/lang.ctl ]
#        FORTRAN 77 compilers must be built after code generators
[ INCLUDE <DEVDIR>/f77/lang.ctl ]
#        FORTRAN 77 libraries
[ INCLUDE <DEVDIR>/f77/f77lib/lang.ctl ]
[ INCLUDE <DEVDIR>/f77/samples/lang.ctl ]
[ INCLUDE <DEVDIR>/wl/lang.ctl ]
#
#        Now we can build the C++ libraries - must be done after C library
#        as well as after the C++ compilers
#
[ INCLUDE <DEVDIR>/cpplib/lang.ctl ]
[ INCLUDE <DEVDIR>/wclass/lang.ctl ]
#        rtdll must be made after all other libraries
#[ INCLUDE <DEVDIR>/rtdll/lang.ctl ]
#        WATTCP library must be build before TRAP
[ INCLUDE <DEVDIR>/wattcp/lang.ctl ]
#
#        Now let's build the utilities and other stuff
#
[ INCLUDE <DEVDIR>/vi/lang.ctl ]
[ INCLUDE <DEVDIR>/wdisasm/lang.ctl ]
[ INCLUDE <DEVDIR>/ndisasm/lang.ctl ]
[ INCLUDE <DEVDIR>/bdiff/lang.ctl ]
[ INCLUDE <DEVDIR>/misc/lang.ctl ]
[ INCLUDE <DEVDIR>/techinfo/lang.ctl ]
[ INCLUDE <DEVDIR>/help/lang.ctl ]
[ INCLUDE <DEVDIR>/cmdedit/lang.ctl ]
[ INCLUDE <DEVDIR>/brinfo/lang.ctl ]
[ INCLUDE <DEVDIR>/trap/lang.ctl ]
[ INCLUDE <DEVDIR>/dip/lang.ctl ]
[ INCLUDE <DEVDIR>/mad/lang.ctl ]
[ INCLUDE <DEVDIR>/wv/lang.ctl ]
[ INCLUDE <DEVDIR>/wsample/lang.ctl ]
[ INCLUDE <DEVDIR>/wprof/lang.ctl ]
[ INCLUDE <DEVDIR>/wmake/lang.ctl ]
[ INCLUDE <DEVDIR>/wtouch/lang.ctl ]
[ INCLUDE <DEVDIR>/wstrip/lang.ctl ]
[ INCLUDE <DEVDIR>/editdll/lang.ctl ]
[ INCLUDE <DEVDIR>/mstools/lang.ctl ]
#[ INCLUDE <DEVDIR>/online/lang.ctl ]
[ INCLUDE <DEVDIR>/posix/lang.ctl ]
[ INCLUDE <DEVDIR>/cpp/lang.ctl ]
[ INCLUDE <DEVDIR>/pgchart/lang.ctl ]
[ INCLUDE <DEVDIR>/dig/lang.ctl ]
[ INCLUDE <DEVDIR>/uiforms/lang.ctl ]
[ INCLUDE <DEVDIR>/watcom/lang.ctl ]
[ INCLUDE <DEVDIR>/pbide/lang.ctl ]
[ INCLUDE <DEVDIR>/trmem/lang.ctl ]
#[ INCLUDE <DEVDIR>/version/lang.ctl ]
[ INCLUDE <DEVDIR>/dmpobj/lang.ctl ]
[ INCLUDE <DEVDIR>/exedump/lang.ctl ]
[ INCLUDE <DEVDIR>/cvpack/lang.ctl ]
[ INCLUDE <DEVDIR>/wic/lang.ctl ]
[ INCLUDE <DEVDIR>/redist/lang.ctl ]
#[ INCLUDE <DEVDIR>/wgml/lang.ctl ]
[ INCLUDE <DEVDIR>/parsedlg/lang.ctl ]
[ INCLUDE <DEVDIR>/helpcomp/lang.ctl ]
[ INCLUDE <DEVDIR>/wipfc/lang.ctl ]
[ INCLUDE <DEVDIR>/uninstal/lang.ctl ]
[ INCLUDE <DEVDIR>/setupgui/lang.ctl ]
# Build IDE and browser last, as they depend on wgml
[ INCLUDE <DEVDIR>/browser/lang.ctl ]
[ INCLUDE <DEVDIR>/viper/lang.ctl ]
[ INCLUDE <DEVDIR>/idebatch/lang.ctl ]
[ INCLUDE <DEVDIR>/viprdemo/lang.ctl ]
[ BLOCK <DOC_BUILD> 1 ]
[ INCLUDE <OWROOT>/docs/lang.ctl ]
[ BLOCK . . ]
#
#        Do CDSAY to see end time
#
cdsay .
#
#        Clean build tools, must be last item (self destructs)
#
[ INCLUDE <DEVDIR>/builder/clean.ctl ]
