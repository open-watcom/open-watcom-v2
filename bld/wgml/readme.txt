
            The WGML/Script Project
            =======================

This project has a page on the Wiki, which should be consulted for further 
information. This file will contain notes on integration with the build 
system.

At present, this project has only been partially integrated with the 
DOS386/NT386/OS2386 build system. When "builder wgml" (only) is invoked, the 
programs for all four targets will result. Both "builder clean" and "builder 
wgml" invoked from ow\bld\wgml will affect both wgml and wgml\research.

To extend this to "builder" and "builder rel2", the sections

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> wgml ]
#========================
    pmake -d wgml <2> <3> <4> <5> <6> <7> <8> <9> -h

should be replaced with the section

[ BLOCK <1> build rel2 wgml ]
#=======================
    pmake -d wgml <2> <3> <4> <5> <6> <7> <8> <9> -h

which will then build all four of the versions in all three 
cases. If it is not desirable to build the research programs as part of the 
Open Watcom build process, then a different solution will have to be found.

Nothing will appear in the appropriate bld\build subdirectory until the file 
bld\lang.ctl has been opened and the line [ INCLUDE <DEVDIR>/wgml/prereq.ctl ] 
uncommented. This should not be done until the wgml from this project is ready 
to be used in the build process. Since there is no "prereq.ctl" in the research 
sub-project, this should only cause compilation of wgml and gendev.

Building these programs on Linux has not been implemented at all. Presumably,
this would involve modifying the Linux sections of bld\lang.ctl, and perhaps
other files as well. Since I don't do Linux, I cannot be certain what might be
needed.

