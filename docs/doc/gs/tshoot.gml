.*
.chap Trouble-Shooting
.*
.np
This section is intended to provide you with help on some of the
common (and not so common) problems that users have encountered when
trying to run the software.
.begnote
.note Symptom:
The message "Cannot connect to batch spawn server" is displayed.
.note Resolution:
This message occurs when the &company IDE is trying to connect to the
batch server.
The batch server is a program that is employed by the &company IDE
to run "makes" in the background.
There are several reasons why you may receive this message.
.begbull
.bull
.us Installing Under the Host System and Selecting "Modify
.us System Files" During Install
.np
For the IDE to run properly under a particular host operating system,
the install program must be run on that system.
.np
It is very important to check the "Modify System Files" when
running the install on the host platform.  Otherwise,
changes required in the host system's environment will not
be made.  If this is the problem, you should re-install
under the host environment and select this option.
.bull
.us System stability
.np
If for some reason the operating system has become unstable,
the IDE may have trouble connecting to the batch server.
This can happen if you have run a badly behaved application
that caused an exception, for example a page fault.  If this
is the problem, you should shutdown the operating system and
reboot.
.bull
.us Running Windows 3.1 in Enhanced Mode
.np
The IDE requires at least version 3.1 of Windows to be running, in
enhanced mode. Windows 3.0 is not supported.
.endbull
.endnote
.np
.us Running Third-Party Shell Programs
.np
If you run 4DOS.COM or some other shell besides COMMAND.COM, you must
change the "BATCHBOX.PIF" file accordingly. You can do this using
the standard Windows PIF editor.
.np
.us Running Virus Checkers
.np
If you are running Central Point's PCTOOLS, VWATCH, or some other
virus checker, it may be conflicting with our batch server. To isolate
the problem, uninstall the virus checker and see if the IDE works
again.
.np
.us 386 Enhanced "Exclusive in Foreground" Option
.np
If you go to the Windows Control Panel, "Scheduling Options" of "386
Enhanced", and the "Exclusive in Foreground" checkbox is checked,
uncheck it.
This causes our batch server to starve under Windows.
.np
.us Changes to the SYSTEM.INI File
.np
The WDEBUG.386 driver is required to be installed in the
[386Enh] section of SYSTEM.INI.  This should have been done
automatically by the WATCOM install program when the
software was installed under the host environment and the
"modify system files" option was selected.
.np
It has been reported that the line
.millust begin
OverlappedIO=on
.millust end
.pc
not appear in the SYSTEM.INI file.
.np
It appears the user's problem was related to conflicts with other
devices installed in the SYSTEM.INI file. On its own, the above line
does not appear to affect the execution of the IDE.
.np
.ix 'SYSTEM.INI'
.ix 'Windows' 'SYSTEM.INI'
.ix 'NoEMMDriver'
.ix 'Windows' 'NoEMMDriver'
.ix 'batch server'
It has been reported that the line
.millust begin
NoEMMDriver=ON
.millust end
.pc
not appear in the SYSTEM.INI file.
It will prevent a link from succeeding in the IDE.
.*
.section Win-OS/2 and OS/2 Specific
.*
.np
.us Installing for a Win-OS/2 Host
.np
For Win-OS/2, the install must be run under OS/2 and "Modify
System Files" must be selected.  This installs the batch
server under OS/2, which we require under Win-OS/2.  Then
the Windows install program should be run under Win-OS/2 so
that the program groups will be created.  "Modify System
Files" should also be checked for the Win-OS/2 install.
.np
.us Named Pipe Conflicts
.np
Under OS/2, the batch server is a named pipe.  Win-OS/2 uses
the OS/2 named pipe for the batch server, hence the OS/2 IDE
will conflict with the Win-OS/2 IDE if they are both running.
If this is the problem, you should run only one of the OS/2
and Win-OS/2 IDE, not both at the same time.
.*
.endlevel
