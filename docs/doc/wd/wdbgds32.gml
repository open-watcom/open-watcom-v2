.section *refid=vid32 Debugging 32-bit DOS Extender Applications
.*
.np
.ix 'DOS extenders' 'debugging'
.ix '32-bit application debugging'
.ix 'debugging' '32-bit DOS applications'
The &dbgname supports debugging of 32-bit applications developed with
&watc32, &watf32, and assembly language.
.ix 'DOS extenders' 'debugging'
A DOS extender must be used to run the application.
The following DOS extenders are supported.
.begnote $break
.*
.note CauseWay
.ix 'DOS extenders' 'CauseWay'
.ix 'CauseWay'
a public domain DOS extender which is included in the &watc32 and &watf32
packages.
.*
.note DOS/4GW
.ix 'DOS extenders' 'DOS/4GW'
.ix 'DOS/4GW'
.ix 'DOS/4GW' 'version'
.ix 'Tenberry Software, Inc.'
a DOS extender from Tenberry Software, Inc.
DOS/4GW is a subset of Tenberry Software's DOS/4G product.
DOS/4GW is customized for use with &watc32 and &watf32 and is
included in these packages.
.*
.note 386|DOS-Extender
.ix 'DOS extenders' '386|DOS-Extender'
.ix '386|DOS-Extender'
.ix '386|DOS-Extender' 'version'
.ix 'Phar Lap Software, Inc.'
(version 2.2d or later) a DOS extender from Phar Lap Software, Inc.
.endnote
.*
.beglevel
.*
.*
.section Debugging CauseWay 32-bit DOS Extender Applications
.*
.np
.ix 'CauseWay'
.ix 'CWSTUB.EXE'
.ix 'CW.TRP'
.ix 'trap file' 'CW.TRP'
When using the CauseWay DOS extender, the "CWSTUB.EXE" file must be
located in one of the directories listed in the DOS
.ev PATH
environment variable.
The "CWSTUB.EXE" file will usually be stored in the "BINW" directory
of the &company compiler package.
You must also use the
.sy /TRap=CW
option.
The "CW.TRP" file will usually be stored in the "BINW" directory of
the &company compiler package.
You should ensure that this "BINW" directory is included in the DOS
.ev PATH
environment variable.
Otherwise, you must specify the full path name for the trap file.
.np
The help file "CWHELP.EXE" must also be located in one of the
directories listed in the DOS
.ev PATH
environment variable.
It will usually be stored in the "BINW" directory of the &company
compiler package.
.exam begin
C>&dbgcmd /trap=cw hello
  or
C>set &dbgcmd=/trap#cw
C>&dbgcmd hello
.exam end
.*
.*
.section Debugging DOS/4G(W) 32-bit DOS Extender Applications
.*
.np
.ix 'Tenberry Software, Inc.' 'DOS4G.EXE'
.ix 'Tenberry Software, Inc.' 'DOS4GW.EXE'
.ix 'DOS4G.EXE'
.ix 'DOS4GW.EXE'
.ix 'RSI.TRP'
.ix 'trap file' 'RSI.TRP'
When using the Tenberry Software DOS extender, the "DOS4GW.EXE" or
"DOS4G.EXE" file must be located in one of the directories listed in
the DOS
.ev PATH
environment variable.
The "DOS4GW.EXE" file will usually be stored in the "BINW" directory
of the &company compiler package.
You must also use the
.sy /TRap=RSI
option.
The "RSI.TRP" file will usually be stored in the "BINW" directory of
the &company compiler package.
You should ensure that this "BINW" directory is included in the DOS
.ev PATH
environment variable.
Otherwise, you must specify the full path name for the trap file.
.np
The help file "RSIHELP.EXP" must also be located in one of the
directories listed in the DOS
.ev PATH
environment variable.
It will usually be stored in the "BINW" directory of the &company
compiler package.
.exam begin
C>&dbgcmd /trap=rsi hello
  or
C>set &dbgcmd=/trap#rsi
C>&dbgcmd hello
.exam end
.*
.*
.section Debugging Phar Lap 32-bit DOS Extender applications
.*
.np
.ix 'Phar Lap Software, Inc.' 'RUN386.EXE'
.ix 'RUN386.EXE'
.ix 'TNT.EXE'
.ix 'DBGLIB.REX'
.ix 'PLS.TRP'
.ix 'trap file' 'PLS.TRP'
.ix 'PLSHELP.EXP'
.ix 'PEDHELP.EXP'
When using the Phar Lap Software, Inc. DOS extender,
the "RUN386.EXE"
(or "TNT.EXE"),
"DBGLIB.REX"
"PLSHELP.EXP",
and "PEDHELP.EXP"
files must be located in one of the directories listed in the DOS
.ev PATH
environment variable.
You must also use the
.sy /TRap=PLS
option.
The "PLS.TRP", "PLSHELP.EXP" and "PEDHELP.EXP" files will usually be
stored in the "BINW" directory of the &company compiler package.
You should ensure that this "BINW" directory is included in the DOS
.ev PATH
environment variable.
Otherwise, you must specify the full path name for the trap file.
.np
.ix 'Phar Lap Software, Inc.' 'RUN386.EXE'
.ix 'RUN386.EXE'
.ix 'Phar Lap Software, Inc.' 'TNT.EXE'
.ix 'TNT.EXE'
Parameters are passed to the "RUN386" or "TNT" DOS extender using the
/TRAP option.
The entire parameter must be placed within braces.
The following example illustrates how to debug a Phar Lap application
passing the -maxreal switch to RUN386.EXE or TNT.EXE.
.exam begin
C>&dbgcmd /trap=pls;{-maxreal 512} hello
  or
C>set &dbgcmd=/trap#pls;{-maxreal 512}
C>&dbgcmd hello
.exam end
