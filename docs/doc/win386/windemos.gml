.chap Converting the SDK Example Programs
.*
.np
&cmpname for Windows comes with a set of patches that will change the
standard SDK examples into 32-bit applications.
.np
How to install patches to create 32-bit versions of the SDK samples:
.autonote
.note
Make sure
.fi PATCH.EXE
(included with &cmpname for Windows) is somewhere in your current
directory.
.note
Edit
.fi CS.BAT
(in the
.fi &pathnamup\SRC\SKDPATCH
directory) so that things are copied into your
.fi \WINDEV\SAMPLES
directory.
.note
Make sure the
.fi \WINDEV\SAMPLES
directory contains a fresh copy of the files from the SDK disks, or
else the patches will fail.
.note
Run
.mono CS <dirname>
to copy over the patches for a specific directory,
or run
.mono ALL
(in the
.fi &pathnamup\SRC\SKDPATCH
directory) to install all of them.
.note
To apply the patches, go into the appropriate directory and type
.mono APPLY
or
.mono APPLY Y.
APPLY causes the directory
.fi WIN386
to be created, along with everything necessary to compile and link the
32-bit application.
Specifying "Y" causes the code to contain conditional compilation
macros, demonstrating the differences.
All code specifically for 32-bit applications is within the
.mono #ifdef WIN386
areas.
.endnote
