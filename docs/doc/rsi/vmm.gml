.* (c) Portions Copyright 1990, 1992 by Tenberry Software, Inc.
.*
.chap VMM
.*
.np
.ix 'virtual memory manager'
.ix 'VMM'
The Virtual Memory Manager (VMM) uses a swap file on disk to augment
RAM.
With VMM you can use more memory than your machine actually has.
When RAM is not sufficient, part of your program is swapped out to the
disk file until it is needed again.
The combination of the swap file and available RAM is the
.us virtual memory.
.np
Your program can use VMM if you set the DOS environment variable,
.ev DOS4GVM
.ct , as follows.
To set the
.ev DOS4GVM
environment variable, use the format shown below.
.millust begin
set DOS4GVM= [option[#value]] [option[#value]]
.millust end
.pc
A "#" is used with options that take values since the DOS command shell
will not accept "=".
.np
If you set
.ev DOS4GVM
equal to 1, the default parameters are used for all options.
.exam begin
C>set DOS4GVM=1
.exam end
.*
.section VMM Default Parameters
.*
.np
VMM parameters control the options listed below.
.begnote $setptnt 12
.note MINMEM
.ix 'MINMEM virtual memory option'
The minimum amount of RAM managed by VMM.
The default is 512KB.
.note MAXMEM
.ix 'MAXMEM virtual memory option'
The maximum amount of RAM managed by VMM.
The default is 4MB.
.note SWAPMIN
.ix 'SWAPMIN virtual memory option'
The minimum or initial size of the swap file.
If this option is not used, the size of the swap file is based on
.bi VIRTUALSIZE
(see below).
.note SWAPINC
.ix 'SWAPINC virtual memory option'
The size by which the swap file grows.
.note SWAPNAME
.ix 'SWAPNAME virtual memory option'
The swap file name.
.ix 'DOS4GVM.SWP'
The default name is "DOS4GVM.SWP".
By default the file is in the root directory of the current drive.
Specify the complete path name if you want to keep the swap file
somewhere else.
.note DELETESWAP
.ix 'DELETESWAP virtual memory option'
Whether the swap file is deleted when your program exits.
By default the file is not deleted.
Program startup is quicker if the file is not deleted.
.note VIRTUALSIZE
.ix 'VIRTUALSIZE virtual memory option'
The size of the virtual memory space.
The default is 16MB.
.endnote
.*
.section Changing the Defaults
.*
.np
You can change the defaults in two ways.
.autonote
.note
Specify different parameter values as arguments to the
.ev DOS4GVM
environment variable, as shown in the example below.
.millust begin
set DOS4GVM=deleteswap maxmem#8192
.millust end
.note
Create a configuration file with the filetype extension ".VMC", and use
that as an argument to the
.ev DOS4GVM
environment variable, as shown below.
.millust begin
set DOS4GVM=@NEW4G.VMC
.millust end
.endnote
.*
.beglevel
.*
.section The .VMC File
.*
.np
.ix 'VMC extension'
A ".VMC" file contains VMM parameters and settings as shown in the
example below.
Comments are permitted.
Comments on lines by themselves are preceded by an exclamation point
(!).
Comments that follow option settings are preceded by white space.
Do not insert blank lines: processing stops at the first blank line.
.code begin
!Sample .VMC file
!This file shows the default parameter values.
minmem = 512            At least 512K bytes of RAM is required.
maxmem = 4096           Uses no more than 4MB of RAM
virtualsize = 16384     Swap file plus allocated memory is 16MB
!To delete the swap file automatically when the program exits, add
!deleteswap
!To store the swap file in a directory called SWAPFILE, add
!swapname = c:\swapfile\dos4gvm.swp
.code end
.*
.endlevel
