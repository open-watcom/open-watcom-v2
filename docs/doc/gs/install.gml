.chap Installation
.*
.np
The package contains the following components:
.begbull $compact
.bull
.us &product CD-ROM
.bull
.us This manual
.endbull
.*
.section Hardware and Software Requirements
.*
.np
.ix 'hardware requirements'
&product requires the following minimum configuration:
.begbull
.bull
IBM PC compatible
.bull
An 80386 or higher processor
.bull
&minram of memory
.bull
Hard disk with enough space available to install the components you require.
.bull
A CD-ROM disk drive
.endbull
.np
.ix 'software requirements'
In addition to the above requirements, you need one of the following
operating systems:
.begbull $compact
.bull
DOS version 5.0 or higher
.bull
Microsoft Windows version 3.1 running in enhanced mode
.bull
Microsoft Windows 95 or higher
.bull
Microsoft Windows NT version 3.1 or higher
.bull
IBM OS/2 2.1 or higher
.endbull
.np
.*
.section The README File
.*
.np
Before you install &product, you should read the contents of the
"README" file which is stored in the root directory of the CD-ROM.
It contains valuable, up-to-date information concerning this product.
.*
.section Installing &product
.*
.np
.ix 'installing &product'
.ix 'installation' 'previous version'
The installation program in this version has been completely
redesigned with several new "smart" features.
If you have installed a previous version of &product then you should
install &product &ver..&rev into the same path (except for the reason
described in the following paragraph).
It will examine a previous installation to determine what features
were previously installed.
It will use this information to establish default settings for the
installation that you are about to attempt.
Of course, you can add or remove features as you progress through the
installation steps.
.np
If you are installing only one of the &watc or &watf products and you
have an older version of the other product, we do NOT recommend that
you install the new product into the same directory as the old
product.
The &watc and &watf products are compatible at the same version
number.
However, the &watc and &watf products are usually NOT compatible
across different version numbers.
If this is the case, care must be exercised when switching between
use of the two products.
Environment variables such as
.ev PATH
and
.ev WATCOM
must be modified and/or corrected.
System files such as
.fi CONFIG.SYS
and
.fi SYSTEM.INI
must be modified and/or corrected.
.np
If you are installing both &watc &ver..&rev and &watf &ver..&rev, we
recommend that you install both products under the same directory.
This will eliminate duplication of files and, as a result, reduce
the total required disk space.
The two products share the use of certain environment variables
which point to the installation directory.
If separate installation directories are used, problems will arise.
.np
When you install &watc and &watf in the same directory, you should not
deselect any options when running the second installation;
otherwise the second product's install may remove files that were
installed (and are required) by the first product's install.
This isn't an issue if you only have one of &watc or &watf..
The problem is that &watc and &watf don't know about the installation
options you have selected for each other's product.
.np
If you wish to create a backup of your previous version, please do so
before installing &product &ver..&rev..
.np
If you decide to install &product &ver..&rev into a different
directory than the previously installed version, you will have to
manually edit system files (e.g., CONFIG.SYS, AUTOEXEC.BAT,
SYSTEM.INI) after the installation process is complete to remove the
old version from various environment variables (e.g., PATH, DEVICE=).
This is necessary since the path to the new version will appear after
the path to the old version.
To avoid this extra work, we recommend installing the new version into
the same path as the old version.
.np
As an example, here are a few of the environment variables and "RUN"
directives that are modified/added to the OS/2
.fi CONFIG.SYS
file.
You should make sure that all references to the older version of the
software are removed.
.exam begin
LIBPATH=...;D:\WATCOM\BINP\DLL;...
SET PATH=...;D:\WATCOM\BINP;D:\WATCOM\BINW;...
SET HELP=...;D:\WATCOM\BINP\HELP;...
SET BOOKSHELF=...;D:\WATCOM\BINP\HELP;...
SET INCLUDE=...;D:\WATCOM\H\OS2;D:\WATCOM\H;
SET WATCOM=D:\WATCOM
SET EDPATH=D:\WATCOM\EDDAT
RUN=D:\WATCOM\BINP\NMPBIND.EXE
.exam end
.np
.ix 'installation' 'multiple operating systems'
You may wish to run &product under more than one operating system on
the same personal computer.
For every operating system that you use, simply start up the operating
system and run the corresponding install procedure.
.np
If you run the Windows 3.x installation procedure, you do not need
to run the DOS installation procedure also.
.np
If you plan to use Win-OS/2 as a development platform under OS/2, you
must run the Windows 3.1 install program (selecting Windows 3.1 host
support).
.np
Place the CD-ROM disk in your CD-ROM drive.
Select one of the following procedures depending on the host operating
system that you are currently running.
Below, substitute the CD-ROM drive specification for "x:".
.begnote $setptnt 12
.note DOS
.ix 'installing &product' 'DOS'
Enter the following command:
.ix 'SETUP'
.millust begin
x:\setup
.millust end
.note Windows 3.x
.ix 'installing &product' 'Windows 3.x'
Start Windows 3.x and choose Run from the File menu of the Program
Manager.
Enter the following command:
.ix 'SETUP'
.millust begin
x:\setup
.millust end
.note Windows 95/98/Me
.ix 'installing &product' 'Windows 95/98/Me'
Choose Run from the Start menu and enter the following command:
.ix 'SETUP'
.millust begin
x:\setup
.millust end
.note Windows NT/2000/XP
.ix 'installing &product' 'Windows NT/2000/XP'
Log on to an account that is a member of the "Administrator" group so
that you have sufficient rights to modify the system environment.
Choose Run from the File menu of the Program Manager.
Enter the following command:
.ix 'SETUP'
.millust begin
x:\setup
.millust end
.note OS/2
.ix 'installing &product' 'OS/2'
Start an OS/2 session and enter the following command:
.ix 'INSTALL'
.millust begin
x:\install
.millust end
.endnote
.*
.section Incremental Installation
.*
.np
.ix 'installation' 'incremental'
You may wish to install &product, and subsequently install features
that you omitted in the first install.
You can also remove features that you no longer wish to have
installed.
You can achieve this as follows:
.autopoint
.point
Start the installation program.
.point
Select any new features that you wish to install.
.point
Deselect any features that you wish to remove.
.point
Re-run the installation program for each host operating system that
you use.
.endpoint
.*
.section *refid=sysfil System Configuration File Modifications
.*
.np
.ix 'installation' 'modifications to files'
The install program makes changes to your operating system startup
files to allow &product to run.
We strongly recommend that you allow the install program to modify
your system configuration files for you, but you may do it by hand.
The changes required may be found in any of the following files which
have been placed in the root of the installation directory:
.begnote $setptnt 15
.note CONFIG.NEW
Changes required for
.fi CONFIG.SYS
(DOS, Windows, Windows 95/98/Me, OS/2)
.note AUTOEXEC.NEW
Changes required for
.fi AUTOEXEC.BAT
(DOS, Windows, Windows 95/98/Me, OS/2)
.note CHANGES.ENV
Changes required for the Windows NT/2000/XP environment
.endnote
.*
.section Installation Notes for Windows 3.x
.*
.autonote
.note
.ix 'Windows' 'Control Panel'
.ix 'Control Panel'
.ix 'Windows' '386 Enhanced'
.ix '386 Enhanced'
.ix 'batch server'
When you use the &ide under Windows 3.x, it is important that the
&vip.'s batch server program be able to run in the background.
Therefore, make sure that the "Exclusive in Foreground" checkbox is
NOT checked in the "Scheduling" options of "386 Enhanced" in the
"Control Panel".
.note
.ix 'SYSTEM.INI'
.ix 'Windows' 'SYSTEM.INI'
.ix 'OverlappedIO'
.ix 'Windows' 'OverlappedIO'
.ix 'batch server'
When you use the &ide under Windows 3.x,
the line
.millust begin
OverlappedIO=ON
.millust end
.pc
in your "SYSTEM.INI" file can cause problems.
This controls (disables) the queuing of DiskIO and makes some changes
between DOS box timings to allow some processes to finish.
.note
.ix 'SYSTEM.INI'
.ix 'Windows' 'SYSTEM.INI'
.ix 'NoEMMDriver'
.ix 'Windows' 'NoEMMDriver'
.ix 'batch server'
When you use the &ide under Windows 3.x, it is important that the line
.millust begin
NoEMMDriver=ON
.millust end
.pc
not appear in your "SYSTEM.INI" file.
It will prevent a link from succeeding in the &vip...
.note
.ix 'SYSTEM.INI'
.ix 'Windows' 'SYSTEM.INI'
.ix 'InDOSPolling'
.ix 'Windows' 'InDOSPolling'
.ix 'batch server'
When you use the &ide under Windows 3.x on the NEC PC-9800 series, it
is important that the line
.millust begin
InDOSPolling=TRUE
.millust end
.pc
not appear in your "SYSTEM.INI" file.
It will prevent a make from succeeding in the &vip..
.note
.ix 'anti-virus'
Central Point Software's anti-virus programs (VDEFEND, VSAFE, VWATCH)
conflict with the &ide under Windows 3.x.
.note
.ix 'Program Information File'
.ix 'BATCHBOX.PIF'
.ix '&vip'
.ix 'PIF'
.ix 'COMMAND.COM'
.ix '4DOS'
The Program Information File "BATCHBOX.PIF" is used by the &ide
(&vip.) to start up a background batch server
for compiling, linking, etc.
The PIF references "COMMAND.COM".
If you are using a substitute for "COMMAND.COM" such as "4DOS.COM"
then you must modify the PIF accordingly using a PIF editor.
.endnote
.*
.section Installation Notes for OS/2
.*
.autonote
.note
.ix 'EPM'
.ix 'Enhanced System Editor'
.ix '&vip'
The &ide (&vip.) uses the IBM OS/2 Enhanced System Editor (EPM) for
editing text files. You must ensure that EPM is installed in your OS/2
system if you are planning to use the &vip.. You can selectively install
the Enhanced Editor by running the OS/2 Setup and Installation program
(Selective Install) and choosing "Enhanced Editor" from the "Tools and
Games" detail page.
.note
.ix 'UNDELETE'
On some systems with limited memory that use the UNDELETE feature of
OS/2, compile times may be slow because OS/2 is saving copies of
compiler temporary files.
You may start the BATSERV process using the OS/2
.fi STARTUP.CMD
file with
.ev DELDIR
turned off as illustrated below.
.millust begin
SET OLD_DEL_DIR=%DELDIR%
SET DELDIR=
DETACH C:\WATCOM\BINP\BATSERV.EXE
SET DELDIR=%OLD_DEL_DIR%
SET OLD_DEL_DIR=
.millust end
.note
.ix 'NMPBIND'
.ix 'debugger'
.ix 'CONFIG.SYS'
If you plan to use the Named Pipe Remote Debugging support of the
&dbgname then the
.fi NMPSERV.EXE.
program must be running. It may be started during OS/2 initialization via
a "RUN=" statement in your
.fi CONFIG.SYS
file or manually as needed through the DETACH command.
.endnote

