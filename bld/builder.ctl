# Master Open Watcom Builder Control file
# =======================================

[ INCLUDE <OWROOT>/build/master.ctl ]
#[ LOG <LOGFNAME>.<LOGEXT> ]
[ LOG <OWSRCDIR>/<LOGFNAME>.<LOGEXT> ]

echo Build host: <BLD_HOST>

cdsay .

[ BLOCK <1> boot bootclean ]
#===========================
# Build Open Watcom tools using the host platform's native compiler.
#
# NB: The ordering of the following inclusions is significant!
# At the beginning, we only have wmake and builder/pmake
# If necessary, build POSIX tools (awk, sed, cp, ...)
[ IFDEF <BLD_HOST> OS2 NT DOS ]
[ INCLUDE <OWSRCDIR>/posix/builder.ctl ]
[ INCLUDE <OWSRCDIR>/awk/builder.ctl ]
[ ENDIF ]
# Start with the ORL and librarian - boot build has no other dependencies
[ INCLUDE <OWSRCDIR>/cpp/builder.ctl ]
[ INCLUDE <OWSRCDIR>/orl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/nwlib/builder.ctl ]
# Build our version of yacc
[ INCLUDE <OWSRCDIR>/yacc/builder.ctl ]
# Build wsplice and genverrc
[ INCLUDE <OWSRCDIR>/builder/builder.ctl ]
# Next build wres and the resource compiler
[ INCLUDE <OWSRCDIR>/wres/builder.ctl ]
[ INCLUDE <OWSRCDIR>/rc/rc/builder.ctl ]
# Build wstrip
[ INCLUDE <OWSRCDIR>/wstrip/builder.ctl ]
# Continue with the linker
[ INCLUDE <OWSRCDIR>/dwarf/dw/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wl/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/wl/exe2bin/builder.ctl ]
# Now we also have everything to build wasm and owcc
[ INCLUDE <OWSRCDIR>/wasm/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wcl/386/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wcl/i86/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wcl/owcc/builder.ctl ]
# Build the OWL for code generator
[ INCLUDE <OWSRCDIR>/owl/builder.ctl ]
# Build the 16-bit and 32-bit x86 compilers
[ INCLUDE <OWSRCDIR>/cfloat/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cg/intel/386/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cg/intel/i86/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cc/386/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cc/i86/builder.ctl ]
[ INCLUDE <OWSRCDIR>/plusplus/386/builder.ctl ]
[ INCLUDE <OWSRCDIR>/plusplus/i86/builder.ctl ]
# Build RISC assemblers
[ INCLUDE <OWSRCDIR>/re2c/builder.ctl ]
[ INCLUDE <OWSRCDIR>/as/builder.ctl ]
# Now miscellaneous utilities required during build
[ INCLUDE <OWSRCDIR>/ssl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/causeway/cwc/builder.ctl ]
[ INCLUDE <OWSRCDIR>/parsedlg/builder.ctl ]
[ INCLUDE <OWSRCDIR>/vi/vicomp/builder.ctl ]
[ INCLUDE <OWSRCDIR>/omftools/builder.ctl ]
[ INCLUDE <OWSRCDIR>/whpcvt/builder.ctl ]

## The following tools are not required during build
#[ INCLUDE <OWSRCDIR>/ndisasm/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/exedump/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/dmpobj/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/orl/test/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/dwarf/util/builder.ctl ]
## Debugger/profiler libraries
#[ INCLUDE <OWSRCDIR>/dip/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/mad/builder.ctl ]

#[ IFDEF <BLD_HOST> UNIX ]
##========================
## Build UNIX specific utilities using native tools.
## 
## Build our vi editor and the libs it requires
#[ INCLUDE <OWSRCDIR>/ncurses/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/ui/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/vi/builder.ctl ]
##[ INCLUDE <OWSRCDIR>/hlpview/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/help/builder.ctl ]
## Build gui/aui lib and the profiler
#[ INCLUDE <OWSRCDIR>/gui/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/aui/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/wprof/builder.ctl ]
#[ ENDIF ]

[ BLOCK <1> build rel clean ]
#============================
# Build all of Open Watcom using freshly built tools.
#
# NB: Again, the order is significant.
# At the beginning, assume to have compilers/assemblers/librarian/linker
# running on the host platform, but not necessarily anything else.
# Start with language and API headers
[ INCLUDE <OWSRCDIR>/hdr/builder.ctl ]
[ INCLUDE <OWSRCDIR>/os2api/builder.ctl ]
[ INCLUDE <OWSRCDIR>/w16api/builder.ctl ]
[ INCLUDE <OWSRCDIR>/w32api/builder.ctl ]
# Continue with runtime libraries.
[ INCLUDE <OWSRCDIR>/fpuemu/builder.ctl ]
[ INCLUDE <OWSRCDIR>/clib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/mathlib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cpplib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/f77/f77lib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/graphlib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/rtdll/builder.ctl ]
[ INCLUDE <OWSRCDIR>/win386/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wattcp/builder.ctl ]
# Continue with Causeway DOS extender.
[ INCLUDE <OWSRCDIR>/causeway/builder.ctl ]
# Continue with DOS extenders stubs.
[ INCLUDE <OWSRCDIR>/w32loadr/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wstuba/builder.ctl ]
# Now we have enough to start cross building everything else

# Start with the libs used by various tools
[ INCLUDE <OWSRCDIR>/wres/builder.ctl ]
[ INCLUDE <OWSRCDIR>/orl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/owl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/dwarf/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cfloat/builder.ctl ]
# Continue with the assemblers/librarian/linker/make
[ INCLUDE <OWSRCDIR>/wasm/builder.ctl ]
[ INCLUDE <OWSRCDIR>/as/builder.ctl ]
[ INCLUDE <OWSRCDIR>/nwlib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wmake/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wtouch/builder.ctl ]
# On to the compilers
[ INCLUDE <OWSRCDIR>/cg/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cc/builder.ctl ]
[ INCLUDE <OWSRCDIR>/plusplus/builder.ctl ]
[ INCLUDE <OWSRCDIR>/f77/wfc/builder.ctl ]
# Resource tools, first Resource compiler
[ INCLUDE <OWSRCDIR>/rc/builder.ctl ]
# Continue with SDK tools
[ INCLUDE <OWSRCDIR>/wpi/builder.ctl ]
[ INCLUDE <OWSRCDIR>/commonui/builder.ctl ]
[ INCLUDE <OWSRCDIR>/sdk/builder.ctl ]
# Now miscellaneous command line tools
[ INCLUDE <OWSRCDIR>/ndisasm/builder.ctl ]
[ INCLUDE <OWSRCDIR>/exedump/builder.ctl ]
[ INCLUDE <OWSRCDIR>/dmpobj/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wcl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/f77/wfl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wstrip/builder.ctl ]
# User interface libs
[ INCLUDE <OWSRCDIR>/ncurses/builder.ctl ]
[ INCLUDE <OWSRCDIR>/ui/builder.ctl ]
[ INCLUDE <OWSRCDIR>/gui/builder.ctl ]
[ INCLUDE <OWSRCDIR>/aui/builder.ctl ]
# The vi(w) editor
[ INCLUDE <OWSRCDIR>/rcsdll/builder.ctl ]
[ INCLUDE <OWSRCDIR>/vi/builder.ctl ]
# Build the debugger and sampler
[ INCLUDE <OWSRCDIR>/dip/builder.ctl ]
[ INCLUDE <OWSRCDIR>/mad/builder.ctl ]
[ INCLUDE <OWSRCDIR>/rsilib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wv/builder.ctl ]
[ INCLUDE <OWSRCDIR>/trap/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wsample/builder.ctl ]
# GUI tools libraries
[ INCLUDE <OWSRCDIR>/wclass/builder.ctl ]
# Other GUI tools
[ INCLUDE <OWSRCDIR>/wprof/builder.ctl ]
[ INCLUDE <OWSRCDIR>/browser/builder.ctl ]
# The IDE tools
[ INCLUDE <OWSRCDIR>/editdll/builder.ctl ]
[ INCLUDE <OWSRCDIR>/idebatch/builder.ctl ]
[ INCLUDE <OWSRCDIR>/ide/builder.ctl ]
# The miscelaneous tools
[ INCLUDE <OWSRCDIR>/cmdedit/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cvpack/builder.ctl ]
# OW clones for MS tools
[ INCLUDE <OWSRCDIR>/mstools/builder.ctl ]
# Miscelaneous files
[ INCLUDE <OWSRCDIR>/misc/builder.ctl ]
# Source code samples
[ INCLUDE <OWSRCDIR>/src/builder.ctl ]
# IDE samples
[ INCLUDE <OWSRCDIR>/idedemo/builder.ctl ]
# Build help viewer
#[ INCLUDE <OWSRCDIR>/hlpview/builder.ctl ]
[ INCLUDE <OWSRCDIR>/help/builder.ctl ]
# Build help compilers
[ INCLUDE <OWSRCDIR>/hcdos/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/hcwin/builder.ctl ]
[ INCLUDE <OWSRCDIR>/helpcomp/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wipfc/builder.ctl ]
[ INCLUDE <OWSRCDIR>/bmp2eps/builder.ctl ]
# Copy the redistributable components
[ INCLUDE <OWSRCDIR>/redist/builder.ctl ]
# Build installer tools
[ INCLUDE <OWSRCDIR>/setupgui/builder.ctl ]

# Build documentation
[ IFDEF <OW_DOC_BUILD> 1 ]
[ INCLUDE <OWROOT>/docs/builder.ctl ]
[ ENDIF ]

[ BLOCK <1> xx ]

#
#        Do CDSAY to see end time
#
cdsay .
#
#        Clean build tools, must be last item (self destructs)
#
[ INCLUDE <OWSRCDIR>/builder/clean.ctl ]

[ BLOCK <1> test testclean ]
#===========================
[ INCLUDE <OWSRCDIR>/wasmtest/builder.ctl ]
[ INCLUDE <OWSRCDIR>/ctest/builder.ctl ]
[ INCLUDE <OWSRCDIR>/f77/regress/builder.ctl ]
[ INCLUDE <OWSRCDIR>/plustest/builder.ctl ]

[ BLOCK . . ]
#============
cdsay .
