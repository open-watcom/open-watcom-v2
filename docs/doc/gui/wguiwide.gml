.chap &wide
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.*
.do end
.*
.np
.ix '&ide'
Software development normally involves managing a number of different
files for each project. A project often includes multiple source
files, include files, and libraries. In addition, there are usually
several programming tools involved: compilers, linkers, preprocessors,
debuggers, editors, and so on. But this is likely not new to you. What
will be new is the &wide (&vip).
.np
The &wide is a software development tool that manages project files
and language tools. Although it is really a tool itself, the common
term to describe it (and possibly the best analogy) is
.us Development Environment.
By using the &wide, you have direct access to each of the files
relating to a project and all of the language tools you require to
build your application.
.np
:HBMP 'ide_s.bmp' c
.*
.section How the Environment Operates
.*
.np
.ix '&ide' 'environment'
The &wide has a graphical interface that combines file management and
tool management. An &vip session deals with a
.us Project.
A project can be generally defined as the collection of files and
necessary information that relates to the development of project
components. However, a project is more than just a graphical
representation of a file list&mdash.it contains all of the underlying
smarts to understand the interrelation of all files associated with
the project and to ensure that these files are up-to-date.
.np
Each project component is referred to as a
.us Target.
An &ide target is made up of the source files, include files,
libraries, and MAKE instructions to create the target file (for
example, ".exe", ".dll", ".lib", or ".hlp"). A project often has more
than one target. Each target can be set to create the associated
target file for a specific destination platform.
.np
Within a target, there are
.us Sources.
A source is an individual file (such as a ".c" or ".cpp" file, but it
can also be a ".bmp", ".rc", or ".lib" file) used to create the target
file. A target usually has more than one source.
.*
.section How the &ide Looks
.*
.np
.ix 'look and feel of the &ide'
In the following illustration, the main window is the project window
for a project entitled
.fi first.wpj.
The sub-window represents the target for this project,
.fi target1.exe.
The source file for the target is listed within the target window. In
this project, there is only a single target with a single source file.
.figure *depth='3.8' *scale=60 *file='ide1' The &company &vip window displays project and target windows.
.np
The language tools are available on the toolbar and the drop-down
menus. For example, on the Sources menu, you can work with the source
that is selected in the target window. Here you can add a new source,
remove or rename an existing source, edit a source, and so on. A
similar set of target actions are available in the Targets menu and
project actions are located in the File menu.
.np
A single &vip session displays a project. If the project consists of a
number of components, such as two executables and one library, these
appear as target sub-windows on the project window. Each target window
displays the files necessary to construct the target and associates
files with a rule describing the construction mechanism based on
filename extension. For example, a filename with the extension ".exe"
may associate with the rule for constructing executables, or a
filename with the extension ".lib" may associate with the rule for
constructing static libraries.
.np
Different projects can refer to the same target. If they do, either
project can manipulate the shared target and any changes to either
project affect the shared target.
.*
.section The Files the &ide Uses
.*
.np
.ix 'projects'
The &wide creates and manages a project, referencing a number of
targets. The &vip uses filename extensions to represent projects and
targets. The extension ".wpj" represents projects and the extension
".tgt" represents targets. The &vip compiles and links the target in
the target directory.
.np
The project file (.wpj) contains the names of all of the target files
related to the project, including housekeeping information about the
project and the current &vip session.
.np
Each target file (.tgt) describes one target and includes the name and
type of the target executable, a reference to a rule describing how to
build the target, and a list of relevant switches the rule uses. In
addition, target files also contain the names of all of the source
files used to construct the target program, references to similar
rules for compiling them, and relevant switches.
.np
In constructing targets, the &ide references a list of rules defined
in a configuration file, called
.fi &vip_cfg..
Each rule defines a command line used to do the construction and a
list of actions you can perform on the target program (such as make,
run, or debug) or on the files in the item list (such as make or
edit). Additionally, rules refer to tools.
.np
Tools define which command line switches you can use in the &wide..
.*
.section Using the &wide
.*
.np
.ix '&ide' 'using the'
This section discusses the following topics:
.begbull $compact
.bull
Starting the &wide
.bull
Quitting the &wide
.bull
The &vip Menu Bar
.bull
The &vip Status Bar
.bull
The &vip Toolbar
.endbull
.*
.beglevel
.*
.section Starting the &wide
.*
.np
.ix '&ide' 'starting the'
To start the &ide, double-click on the &vip icon. This opens the
&company &vip Project window.
.*
.section Quitting the &wide
.*
.np
.ix '&ide' 'quitting the'
To exit the &ide, choose Exit from the File menu of the &company &vip
Project window or double-click on the system menu box on the same
window.
.*
.section The &vip Menu Bar
.*
.np
.ix '&ide' 'menu bar'
.ix 'menu bar' '&ide'
The &vip's menu bar consists of the following eight menus:
.begpoint
.point File
Open new or existing projects, close and save projects, exit the &vip,
and show or hide the toolbar and status bar
.point Actions
Make all targets, run applications, and invoke other GUI Tools
.point Targets
Work with targets, including setting switches and creating, removing,
and renaming targets
.point Sources
Work with sources, including setting switches and creating, removing,
and renaming sources
.point Options
Set switches for creating targets and sources
.point Log
Perform functions relating to the &vip Log window
.point Window
Refresh the contents of the &vip windows and view the targets currently
available on the &vip window
.point Help
Access on-line help information
.endpoint
.*
.section The &vip Status Bar
.*
.np
.ix '&ide' 'status bar'
.ix 'status bar' '&ide'
The Status Bar appears along the bottom of the &company &vip Project
window. This bar displays hint text for the toolbar button and menu
option over which your cursor is positioned. Status messages, such as
Saving, Adding, and Executing Log, appear in this bar so you are aware
of what action the &vip is performing. A check mark beside the Show
Status Bar menu option in the File menu indicates that this feature is
enabled. To disable this feature, choose the option again from the File
menu and the status bar disappears from the window.
.figure *depth='3.8' *scale=60 *file='ide2' The &vip Status Bar can be toggled from the File menu.
.*
.section The &vip Toolbar
.*
.np
.ix '&ide' 'tool bar'
.ix 'tool bar' '&ide'
In the &company &vip Project window, the toolbar appears below the
menu bar when the Show Tool Bar option in the File menu is activated
(default). A check mark beside the menu option indicates that it is
enabled. To disable this feature, choose the option again from the
File menu and the toolbar disappears.
.figure *depth='0.42' *scale=100 *file='ide3' You can access thirteen &vip functions from the toolbar.
.np
The following explains the function each icon performs, as well as the
equivalent function on the menu bar.
.begpoint $break
.point Create
:HBMP 'newproj.bmp' i
.ix 'new project'
Create a new project. This is equivalent to the option New Project in
the File menu.
.point Open
:HBMP 'openproj.bmp' i
.ix 'open project'
Open an existing project. This is equivalent to the option Open
Project in the File menu.
.point Save
:HBMP 'saveproj.bmp' i
.ix 'save project'
Save the current project. This is equivalent to the option Save
Project in the File menu.
.point Edit
:HBMP 'edititem.bmp' i
.ix 'edit text'
Edit the selected source file. This is equivalent to the option Edit
Text in the Sources menu.
.point Make Source
:HBMP 'makeitem.bmp' i
.ix 'make source'
Make the selected source file. This is equivalent to the option Make
in the Sources menu.
.point Make Target
:HBMP 'maketarg.bmp' i
.ix 'make target'
Make the current target. This is equivalent to the option Make in the
Targets menu.
.point Run
:HBMP 'runtarg.bmp' i
.ix 'run target'
Run the current target. This is equivalent to the option Run in the
Targets menu.
.point Debug
:HBMP 'debgtarg.bmp' i
.ix 'debug target'
Debug the current target. This is equivalent to the option Debug in
the Targets menu.
.point Remote Debug
:HBMP 'rdbgtarg.bmp' i
.ix 'remote debug target'
Debug the current target on a remote machine. This is equivalent to
the option Remote Debug in the Targets menu.
.point Browse
:HBMP 'browtarg.bmp' i
.ix 'browse target'
Browse the current target. This is equivalent to the option Browse in
the Targets menu.
Currently, browsing is supported by the &watc compilers only
(&watf does not support browsing).
.point Sample
:HBMP 'samptarg.bmp' i
.ix 'sample target'
Run and sample the current target. This is equivalent to the option
Sample in the Targets menu.
.point Profile
:HBMP 'proftarg.bmp' i
.ix 'profile target'
Profile the current target. This is equivalent to the option Profile
in the Targets menu.
.point Make All
:HBMP 'makeproj.bmp' i
.ix 'make all targets'
Make all targets in the project. This is equivalent to the option Make
All in the Actions menu.
.endpoint
.*
.endlevel
.*
.section Getting Started with a Short Tutorial
.*
.np
.ix '&ide' 'tutorial'
.ix 'tutorial' '&ide'
Perhaps the easiest way to understand how the &wide manages an entire
project's files and tools, is to work through a brief example.
In this walk-through, you create and execute a simple C program using
the &vip..
FORTRAN developers can follow along substituting FORTRAN code and file
names where appropriate.
The session will help you understand the basic concepts of the &vip
and the steps involved in project development.
.np
The illustrations shown in this section are taken from the Windows
environment. The OS/2 environment is very similar with some minor
differences. For example, in the OS/2 environment the IDE uses the
OS/2 Editor rather than &company's Windows Editor. Although this
tutorial builds a Win32 executable, you can choose to build a Win16 or
OS/2 PM executable. The procedures to follow are not that much
different. When the time comes to select the type of target, simply
choose an environment that you can run on your machine so that you can
run your application to make sure it works.
.*
.beglevel
.*
.section A Simple Executable
.*
.ix '&ide' 'defining a project'
.begstep
.step Double-click on the &vip icon
in the &company Tools window.
.result
The &company &vip Project window appears.
.ix 'new project'
.step Choose New Project
from the File menu.
.result
The Enter Project Filename dialog appears. The default project name is
.fi noname.wpj.
.step Enter the filename
.fi \project\first
in the File Name field and click on OK.
.figure *depth='3.11' *scale=84 *file='ide4' Enter the filename in the Enter Project Filename dialog.
.result
This is the name of the project you are creating. If you enter a
directory that doesn't exist (in this case,
.fi \project
.ct ),
the &vip reveals a message box asking if you want to create the
specified directory.
Click on OK to create the specified directory and project file.
.begnote
.note Note:
The &vip looks for existing projects with the new name. If a project
with the specified name already exists, the &vip will prompt you
rather than simply overwriting the existing project.
.endnote
.np
The project name appears in the title bar of the &company &vip Project
window. Note that the &vip added the extension ".wpj" to the project
name. The New Target dialog appears on the screen where you define a
target for the new project.
.step Enter the desired target name
in the Target Name field (we suggest
.mono target1
.ct ).
Next, select the appropriate Target Environment (we suggest
"Win32") and Image Type (we suggest "Character-mode Executable").
Click on OK when completed.
.figure *depth='3.24' *scale=64 *file='ide5' Select target name, target environment and image type
.result
The New Target dialog closes and the &vip creates a target window
entitled
.mono target1.exe
for the new target. Note that the &vip appends the appropriate
extension to the target name for you once you click on OK. You can now
add sources to the target.
.step Choose New Source
from the Sources menu.
.result
The Add File(s) dialog appears where you specify the sources you want
to add to the target. Source names must be filenames with extensions;
pathnames are not required.
.np
For the OS/2 &vip, you can add multiple source files to the target at
one time by inserting a space between each filename you enter. You can
also use the file browser to add files by clicking on the Browse...
button.
.np
For all other systems, you can enter the names of the source files
(with extensions) in the File Name field or you can use the file
browser to select files. The type of files displayed in file list
window can be controlled by modifying the List Files of Type field.
Once you have typed a name in the File Name field or selected a name
from the file list, you can add it to the list of source files to be
included by clicking the Add button. All files in the file list can be
added by clicking the Add All button. Individual names can be removed
from the "add" list by selecting them and clicking the Remove button.
.figure *depth='4.86' *scale=90 *file='ide6' Select source file names in the Add File(s) dialog
.step Enter the following source file names.
.millust begin
source1.c   (or source1.for)
source2.c   (or source2.for)
source3.rc
.millust end
.np
For the OS/2 &vip, enter all the names into the field separating each
name by a space. Click on OK when done.
.np
For all other systems, enter one name at a time into the File Name
field, pressing Enter or clicking on Add after each one has been
entered.
Click on Close when done.
.figure *depth='4.86' *scale=90 *file='ide7' Enter one or several source files in the Add File(s) dialog.
.result
The &vip adds the new source files to the target window, followed by
the characters [n/a]. This indicates that the source files are not
available, meaning that the files do not yet exist. The indicators
are:
.begpoint
.point [n/a]
This code appears if the file does not exist.
.point [r/o]
This code appears if the file is Read Only.
.point [sw]
This code appears if the user has set switches for the specified
source file.
.endpoint
.np
The &ide organizes the source files in the target window under
headers according to the filename extension. You can expand and
collapse the file groups by clicking on the folder icon.
.np
For this tutorial, we only need one source file, so now we will delete
the extra files from the target.
.step Click on the file
.fi source2.c
(or
.fi source2.for
.ct )
to select it then choose Remove Source from the Sources menu.
.result
A message box appears asking if you want to remove the selected source.
Click on the Yes button to continue. The &vip removes the selected source
file from the target window. Now we can remove
.fi source3.rc.
To do this we will perform the function of removing all source files
with a common extension.
.step Click on the (.rc) header
on the target window then choose Remove Source from the Sources menu.
.result
A message box appears on the screen asking if you want to remove all
of the ".rc" files. Click on the Yes button to continue. The message
box closes and the &vip removes the
.fi source3.rc
file from the target window. Now you can edit the remaining source
file.
.step Highlight the line
.fi source1.c
(or
.fi source1.for
.ct )
and choose Edit Text from the Sources menu to edit the selected source
file.
.result
The &edname appears. The name of the selected source
file,
.fi source1.c
(or
.fi source1.for
.ct ),
appears in the window. Here you can enter the source code.
.hint
You can also invoke the &edname by double-clicking on
the source file that you want to edit.
.ehint
.np
In this case, the &edname appears because the selected source file has
a ".c" or ".for" extension.
Performing an edit can open different editors, depending on the
extension of the selected source file.
.step Enter the following lines of source code.
.begnote
.note Important:
For the purpose of this session, we ask you to enter
.us ii
instead of
.us i
in the "print" line.
This will allow you to see how the &ide reacts to errors.
You will then be able to correct the error.
.endnote
.millust begin
#include <stdio.h>

void main()
{
    int i;
    for (i=0; i<10; i++)
    {
        printf( "Value is %d\n", ii );
    }
    printf("Press Enter...\n");
    getchar();
}
.millust end
.np
FORTRAN programmers can enter the following lines of code.
.millust begin
      program main
      implicit none
      integer i

      do i = 0, 9
        print *, 'Value is ', ii
      enddo

      print *, 'Press Enter...'
      read *

      end
.millust end
.np
You can now save your work and close the Editor in one of the
following two ways: close down the entire editor or close down only
the source file window. For this exercise, close only the source file
window as follows.
.step Choose Close
from the File menu to close down the current source file window.
.result
In both cases, if changes have been made to the file and not saved, a
message prompts you to save the changes before exiting. The advantage
of closing only the Editor window is that the Editor continues to run.
If you need to access it again, you will not need to wait for it to go
through the start-up procedures again.
.begnote
.note Important:
You must save the file. The &company compilers do not access the
source lines in the Editor's buffers.
.endnote
.step Minimize the Editor window.
.orstep Click on the &company &vip Project window
to bring it to the foreground.
.step Choose Refresh
from the Window menu.
.result
This action forces the &vip to update the display with the state of
all files.
.ix 'auto refresh'
It is not necessary to use this function if you have selected Auto
Refresh in the File menu.
If Auto Refresh is disabled then it may be necessary to alert the &vip
of the presence of the file on disk.
.ix 'save project'
.step Choose Save Project
from the File menu to save your project up to this point.
.result
You are now ready to make and run the target. Making a target is the
act of constructing a target file. In this case, it involves compiling
the source file
.fi source1.c
(or
.fi source1.for
.ct )
using the appropriate &company compiler and linking the target
executable using the &lnkname..
.step Highlight the line
.fi source1.c
and choose C Compiler Switches... from the Source Options item located
under the Sources menu to set compile options.
.np
.us FORTRAN programmers:
Highlight the line
.fi source1.for
and choose Fortran Compiler Switches... from the Source Options item located
under the Sources menu to set compile options.
.result
The C Compiler Switches dialog appears.
If you are using FORTRAN, the Fortran Compiler Switches dialog appears.
.step Click on the ">>" button
until the
.mono 7. CodeGen Strategy Switches
dialog appears.
Click on the "Multithreaded application [-bm]" checkbox button to select
this option.
.np
.us FORTRAN programmers:
Click on the ">>" button until the
.mono 9. Application Type Switches
dialog appears.
Click on the "Multithreaded application [-bm]" checkbox to select
this option.
.result
A check-mark appears in the checkbox.
.np
Note: Selecting this option is for demonstration purposes only, it is not
required for this simple application.
.np
Click OK to close the dialog.
.step Choose Make
from the Targets menu.
.result
The &vip constructs a makefile based on its knowledge of the target
and construction rules and then invokes the &makname utility to create
.fi target1.exe.
The &vip Log window appears at the bottom of the &company &vip Project
window.
Use this window to monitor the procedure.
Since you entered
.us ii
instead of
.us i,
the MAKE fails.
.figure *depth='3.79' *scale=60 *file='ide8' Use the &vip Log window to monitor the make procedure.
.step Highlight the error message
in the &vip Log window and choose Help on Message from the Log menu.
.result
Help information regarding the error message appears on the screen.
.figure *depth='1.35' *scale=60 *file='ide9' Help for error messages is available
.np
Close the Help window and edit the source file as follows.
.step Highlight the error message
in the &vip Log window and choose Edit File from the Log menu.
.orstep Double-click on the error message
in the &vip Log window.
.result
The &edname opens and the cursor appears at the position
of the error. Edit and correct the error and then save the file again.
Minimize the Editor window and bring the &vip window to the foreground.
.step Choose Make
from the Targets menu again.
.result
This time, the procedure creates the executable file
.fi target1.exe.
.step Choose Run
from the Targets menu.
.result
The &vip executes the program
.fi target1.exe.
.figure *depth='3.14' *scale=63 *file='ide10' Run from the Targets menu runs the target.
The program you wrote is a character mode application and its output
appears in a separate console window.
You are now ready to debug your target program. You probably do not need
to debug this simple program with the &dbgname but go through the steps
so that you know what to do for more complex programs.
.np
The &vip has several debugging information options which it uses as
defaults when it is started. The first thing to do is check to make
sure that your application compiled with the default debugging
information activated.
.step Choose Target Options
from the Targets menu to activate the Target Options menu.
.result
The option Use Development Switches should appear in grey text with a
check mark next to it. Development Switches automatically include
debugging information in the target. If you were to select Use Release
Switches, the &vip would select options useful for a production
version of your program. Selecting this option turns the text grey and
places a check mark beside it. The &vip uses the development switches
by default.
.begnote
.note Note:
For the purpose of this session, you should make the target with the
development switches on. If this is not the option selected, select it
and remake the target before debugging.
.endnote
.step Choose Debug
from the Targets menu.
.result
The &vip starts the &dbgname..
.step Choose Step Over
from the Run menu of the &dbgname. to step through the code.
Choose
.bd Go
from the Run menu of the &dbgname. to continue execution to the end of
the code.
For more information on using the &dbgname., refer to the
.book &dbgname. User's Guide.
.us The task has completed running
appears when the program has completed execution.
.step Choose Exit
from the File menu to terminate the debugging session.
.result
The &dbgname. closes and you return to the &company &vip Project window.
You can now exit the &vip..
.step Choose Exit
from the File menu to exit the &vip..
.result
The &vip prompts you to save the current project if you have not
saved it since the last change.
.endstep
.*
.section Review of What Happened
.*
.np
.ix '&ide' 'tutorial review'
In this session, you created a project called
.fi first.wpj.
This project had one target called
.fi target1.exe,
a 32-bit Windows executable (unless you specified another target
type). Using the &edname, you entered and edited the source text
in a file called
.fi source1.c
or
.fi source1.for
(if you tried the FORTRAN example).
You compiled and linked it into an executable program using the
&makname utility, a &company compiler, and the &lnkname..
You ran it directly and under the control of the &dbgname..
.np
When you saved the project, the &wide created the following permanent
files:
.begpoint
.point first.wpj
Describes the screen layout and refers to a file called
.fi target1.tgt
.point target1.tgt
Describes the target executable
.fi target1.exe
and all switches required to link it. It also describes
.fi source1.c
(or
.fi source1.for
.ct )
and the switches required to compile it.
.endpoint
.*
.endlevel
.*
.section The &company &vip Project Window
.*
.np
.ix '&ide' 'project window'
.ix 'project window' '&ide'
When you first start the &wide, an empty &company &vip Project window
opens. This window represents the project that is currently open in
the &vip.. The name of the current project appears in the title bar of
this window.
.np
The first thing you need to do is open a project for the &company &vip
Project window. This can be a new project you create or an existing
project you open.
.begnote
.note Important:
You can have only one project open at a time in the &vip.. If you
attempt to open or create a project while one already exists in the
&vip window, a message box appears informing you that the &vip is
closing the current project and asks if you want to save any changes
made to that project. Choose Yes, No, or Cancel in this box to
continue.
.endnote
.*
.beglevel
.*
.section Opening a Project
.*
.ix '&ide' 'opening a project'
.ix 'opening a project'
.begstep To create a new project:
.ix 'new project'
.step Choose New Project
from the File menu.
.result
The Enter Project Filename dialog appears on the screen where you
enter the name of the project you are creating.
.step Enter the desired name
in the File Name field and choose the desired directory.
.result
All projects must have the extension ".wpj".
.figure *depth='3.11' *scale=84 *file='ide11' Use the Enter Project Filename dialog to create a new project.
.step Click on OK
when completed.
.result
The Enter Project Filename dialog closes and the new project name
appears in the title bar of the &company &vip Project window. A New
Target dialog opens on the screen where you add a target to the new
project. Refer to the section Adding a Target for more information.
.endstep
.begstep To open an existing project:
.ix 'open project'
.step Choose Open Project
from the File menu.
.result
The Open dialog appears on the screen where you select the project you
want to open.
.figure *depth='3.11' *scale=84 *file='ide12' Use the Open dialog to open an existing project.
.step Choose the desired project.
.result
All projects have the extension ".wpj".
.step Click on OK
when the desired project appears in the File Name field.
.result
The Open dialog closes and the name of the selected project appears in
the title bar of the &company &vip Project window. If targets exist for
the selected project, windows representing the targets appear in the
main window.
.endstep
.*
.section Closing a Project
.*
.ix '&ide' 'closing a project'
.ix 'closing a project'
.ix 'close project'
.begstep To close the current project:
.step Choose Close Project
from the File menu.
.result
The &vip removes all target windows for the current project from the
&company &vip Project window. The project name disappears from the title
bar and the window is empty.
.endstep
.*
.section Saving a Project
.*
.np
.ix '&ide' 'saving a project'
.ix 'saving a project'
There are two ways of saving your current project. You can perform a
simple save where the project is updated with any changes made since
the last save or you can save the project under a different project
name using the Save Project As function.
.*
.beglevel
.*
.section Updating the Project
.*
.ix '&ide' 'updating a project'
.ix 'updating a project'
.ix 'save project'
.begstep To perform a simple save on your project:
.step Choose Save Project
from the File menu.
.result
The &vip updates the project specified in the title bar. The status bar
along the bottom of the window indicates that the save function is
being executed.
.endstep
.*
.section Saving the Project Under a New Name
.*
.ix '&ide' 'saving a project under a new name'
.ix 'saving a project under a new name'
.ix 'save project as'
.begstep To save the current project using a different name:
.step Choose Save Project As
from the File menu.
.result
The Save As dialog appears on the screen where you specify the file
name by which you want to save the current project.
.step Enter the desired project name
in the File Name field.
.result
This is the new name by which you want to save the current project.
This can be the name of a new project or an existing project.
.begnote
.note Important:
Entering the name of an existing project overwrites the contents of
that project with the contents of the current project.
.endnote
.step Choose the desired directory
for the project using the Directories list on the Save As dialog.
.step Click on OK
when completed.
.result
The Save As dialog closes and the new project name appears in the
title bar of the &company &vip Project window.
.endstep
.*
.endlevel
.*
.endlevel
.*
.section Working with Targets
.*
.np
.ix '&ide' 'targets'
.ix 'targets'
Once you have created a project, it is a simple matter to add
additional targets. You can also change the name of a target, remove
targets, change the target environment, and change the image type.
This section discusses the following topics:
.begbull $compact
.bull
Adding a Target
.bull
Minimizing a Target
.bull
Removing a Target
.bull
Renaming a Target
.bull
Marking a Target for Remake
.bull
Making Targets
.bull
Running a Target
.endbull
.*
.beglevel
.*
.section Adding a Target
.*
.np
.ix '&ide' 'adding a target'
.ix 'targets' 'adding'
You add targets to the current project using the New Target dialog.
When you create a new project, this dialog appears automatically. You
have two options when adding targets to a project: you can either
enter the name of a new target or you can choose an existing target,
previously created in another project. This section describes both
methods for adding a target.
.begstep To add a new target to the current project:
.step Choose New Target
from the Targets menu.
.result
The New Target dialog appears on the screen where you enter the
details of the target you are adding.
.figure *depth='3.24' *scale=64 *file='ide13' Enter the new target details in the New Target dialog.
.step Enter the target name
in the Target Name field.
.result
Enter the entire pathname to ensure the directory is created where you
want.
Depending on the nature of the target, it may or may not be able to
share the same directory as other pre-existing targets.
Generally, if the target shares one or more of the same source files as
other targets in the project, then it must be placed in its own
subdirectory.
The object files that are produced for the target may have different
attributes than a pre-existing target's object files (e.g., 16-bit vs
32-bit).
This is why the target should be placed in a separate subdirectory.
.step Select the desired Target Environment and Image Type
as desired.
.step Click on OK
when the Target Name, Target Environment and Image Type fields are
correct.
.result
If you specify a new directory in the target pathname, a message box
appears asking if you want to create the specified directory. Choose
Yes or No to continue. A window representing the new target appears on
the &company &vip Project window. The target name appears in the title
bar.
.np
The first line in this window describes the target. It specifies the
current mode, the target type. The current mode is represented by
either the letter D or R which appears in parentheses in front of the
target type. The letter D indicates that the target is placed in
Development mode. The letter R indicates that the target is placed in
Release mode. The rest of the window displays the source files for the
target.
.endstep
.figure *depth='3.78' *scale=60 *file='ide14' A project may have multiple targets.
.begstep To add an existing target to a current project:
.step Choose New Target
from the Targets menu.
.result
The New Target dialog appears on the screen.
.step Click on the Browse button.
.result
The Enter Target Filename dialog appears on the screen. This dialog
allows you to browse the directories and files and select the ".tgt"
file for the target you want to add.
.figure *depth='3.11' *scale=84 *file='ide15' Choose an existing target from the Pick Target Filename dialog for the current project.
.step Click on the desired target
to select it.
.result
The selected target appears in the File Name field.
.step Click on OK
to process the target selection.
.result
The Enter Target Filename dialog closes.
.step Click on OK
to close the New Target dialog.
.result
A window representing the new target appears on the &company &vip
Project window.
The target name appears in the title bar.
.np
The first line in this window describes the target. It specifies the
current mode and the target type. The current mode is represented by
either the letter D or R which appears in parentheses in front of the
target type. The letter D indicates that the target is placed in
development mode. The letter R indicates that the target is placed in
Release mode. The rest of the window displays the source files for the
target.
.endstep
.*
.section Minimizing a Target
.*
.np
.ix '&ide' 'minimizing a target'
.ix 'minimizing a target'
If you are working with a project that has multiple targets, the
&company &vip Project window can become cluttered. You can tidy your
window by minimizing the target windows that you are not currently
using.
.begstep To minimize a target window:
.step Click on the system box
in the upper left-hand corner of the target window you want to close.
.result
A menu appears listing the functions you can perform on the selected window.
.step Choose Minimize
from the menu.
.result
The selected target window closes and an icon appears along the bottom
of the screen representing that window. The name of the target appears
on or under the icon.
.step Double-click on the icon
to open the target window again.
.endstep
.figure *depth='3.78' *scale=60 *file='ide16' Minimize targets to reduce clutter in the project window.
.*
.section Removing a Target
.*
.np
.ix '&ide' 'removing a target'
.ix 'removing a target'
The procedure for removing a target from a project is simple. If your
project has multiple targets, it is important to ensure that the
target you want to remove is the one selected. Removing a target does
not permanently delete the target, it simply removes it from the
current project. If the target is attached to multiple projects, those
projects will not be affected by the removal of the selected target
from the current project.
.begstep To remove a target from the current project:
.step Click once on the target window
you want to remove from the project.
.result
The title bar of the selected target window is highlighted.
.step Choose Remove Target
from the Targets menu.
.result
A message box appears on the screen asking if you really want to
remove the selected target.
.step Choose OK
to continue.
.result
The message box closes and the selected target window disappears from
the &company &vip Project window.
.endstep
.*
.section Renaming a Target
.*
.np
.ix '&ide' 'renaming a target'
.ix 'renaming a target'
When renaming a target, you can either give it a new name or you can
give it the name of an existing target. Choosing an existing target
name overwrites the contents of the existing target with the contents
of the current target.
.begstep To rename a target:
.step Click once on the target window
you want to rename.
.result
The title bar of the selected target window is highlighted.
.step Choose Rename Target
from the Targets menu.
.result
The Rename Target dialog appears.
The Target Name, Target Environment, and Image Type of the selected
target appear in the appropriate fields.
.figure *depth='3.24' *scale=64 *file='ide17' Change a target's name using the Rename Target dialog.
.step Enter the new target name
in the Target Name field.
.orstep Click on the Browse button
and select an existing target name; click on OK when completed.
.result
The selected or entered target name appears in the Target Name field.
.step Verify that the Target Environment and Image Type are correct.
.step Click on OK
on the Rename Target window.
.result
The Rename Target dialog closes. The title bar of the selected target
window now displays the new target name.
.endstep
.*
.section Marking a Target for Remake
.*
.np
.ix '&ide' 'marking a target'
.ix 'marking a target'
Marking a target for remake ensures that the &vip will perform the link
the next time you make that target or make all targets. This function
is necessary if you are not using the Autodepend function in the Setup
Target submenu under the Targets menu. By marking a target for remake,
you control when and which targets need to be remade.
.begstep To mark a target for remake:
.step Click once on the target window
you want to mark.
.result
The title bar of the selected target window is highlighted.
.step Choose Mark Target for Remake
from the Targets menu.
.result
A message box appears asking if you want to also mark all of the
source files of the selected target.
.step Choose Yes or No
to continue.
.result
Choosing Yes marks all of the source files for the selected target as
well as the target itself. This means that the next time you do a
make, the source files for the selected target will be compiled.
Choosing No marks only the selected target for remake. In both cases,
the message box closes and you return to the &company &vip Project
window.
.endstep
.*
.section Making Targets
.*
.np
.ix '&ide' 'making a target'
.ix 'making a target'
The Make function in the Targets menu creates an executable for the
selected target. When you perform this function, the &vip compiles the
source files for the selected target into object files. The object
files are then linked together to create the target file.
.np
There are two make functions available to you for targets: Make and
Make All. The Make All function in the Actions menu tells the &vip to
compile and link all of the targets for the current project. This can
be a lengthy process, depending upon the number of targets in your
project, so you may choose to only make targets to which changes have
been made. This enables you to see if they can be made successfully.
.np
Performing the Make function opens the &vip Log window which allows you
to monitor the progress of the make.
.begstep To make a single target:
.step Click once on the target window
you want to make to select it.
.result
The title bar of the selected target window is highlighted.
.step Choose Make
from the Targets menu.
.result
The &vip Log window opens along the bottom of the &company &vip Project
window. In the log window you can monitor the make procedure.
.us Execution Complete
appears in the &vip Log window when the make is completed on the
selected target. If the make is successful an executable file for the
selected target can be found in the directory associated with the
target window.
.figure *depth='3.78' *scale=60 *file='ide18' Use the &vip Log window to monitor the make procedure.
.endstep
.begstep To make all targets for the current project:
.step Choose Make All
from the Actions Menu.
.result
The &vip Log window opens along the bottom of the &company &vip Project
window. In the log window you can monitor the make procedure.
.us Execution Complete
appears in the &vip Log window when the make is completed on the
targets. If the make is successful an executable file for each target
in the project can be found in the directory associated with the
target window.
.endstep
.*
.beglevel
.*
.section Viewing the Make Command for a Target
.*
.np
.ix '&ide' 'viewing make commands'
.ix 'viewing make commands'
With the &vip you can use the Show Make Command function to display the
makefile commands for the selected target. These are the commands that
will be used to compile the selected target when you choose the
function Make from the Targets menu or Make All from the Actions menu.
.begstep To view the make commands for targets:
.step Click once on the target window
whose makefile commands you want to view.
.result
The title bar of the selected target window is highlighted.
.step Choose Target Options
from the Targets menu.
.result
A submenu appears on the screen from the Target Options menu.
.step Choose Show Make Command
from the Target Options submenu.
.result
The Target Command dialog appears on the screen. This dialog
displays the makefile commands for the selected target.
.figure *depth='2.45' *scale=81 *file='ide19' The Target Command dialog displays the makefile commands for the selected target.
.step Click on OK
to close the dialog.
.endstep
.*
.endlevel
.*
.section Running a Target
.*
.np
.ix '&ide' 'running a target'
.ix 'running a target'
Once you have compiled and linked your target using the Make function,
you can run the target to execute the program.
.begstep To run a target:
.step Click once on the target window
you want to run.
.result
The title bar of the selected target window is highlighted.
.step Choose Run
from the Targets menu.
.result
The &vip executes the target program.
.figure *depth='3.14' *scale=63 *file='ide20' Run from the Targets menu runs the target.
.endstep
.*
.beglevel
.*
.section Other Run Functions
.*
.np
.ix '&ide' 'running programs'
.ix 'running programs'
In the &wide you can run programs other than the targets in your
current project window. Use the run functions in the Actions menu to
specify the program you want to run. The Run item runs the specified
program under your host environment. The Run Batch item runs the
specified program in the &vip Log window.
.begstep To run other programs in the &vip:
.step Choose Run
from the Actions menu.
.orstep Choose Run Batch
from the Actions menu.
.result
A Run or Run Batch dialog appears on the screen. These dialogs are
identical.
.figure *depth='1.32' *scale=100 *file='ide21' Use the Run (or Run Batch) dialog to run other programs in the &vip..
.step Enter the name of the program
you want to run in the Command field.
.step Click on OK
to run the specified program.
.result
The dialog closes and the specified program runs, either under the
host environment or in the &vip Log window.
.endstep
.*
.endlevel
.*
.section Setting Application Parameters
.*
.np
.ix '&ide' 'setting options'
.ix 'setting options'
The Targets and Sources menus and their submenus are divided into
sections. Horizontal lines separate each section. The menu items in
the bottom sections of the Targets and Sources menus are actions that
you can perform on the selected target or source file in the current
project. If there are switches that can be set for these actions,
there are corresponding menu item entries in the bottom sections of
the submenus. Choose Target Options and Source Options from their
respective menus to display the submenus.
.*
.endlevel
.*
.section Working with Source Files
.*
.np
.ix '&ide' 'source files'
.ix 'source files'
Once your project has a target you need to add source files to the
target. The procedure for adding a source file to a target is simple.
Once you have source files that make up your target, you can also
change the name of a source file and remove source files. This section
discusses the following topics:
.begbull $compact
.bull
Adding Source Files
.bull
Removing Source Files
.bull
Renaming a Source File
.bull
Marking a Source File for Remake
.bull
Making Source Files
.bull
Viewing Included Files
.endbull
.*
.beglevel
.*
.section Adding Source Files
.*
.np
.ix '&ide' 'adding source files'
.ix 'source files' 'adding'
You add source files to the selected target (the target whose title
bar is currently highlighted) using the Add File(s) dialog. The
current project must have a target in order to use the New Source
function. You have two options when adding source files to a target:
you can either enter the name of a new source file or you can choose
an existing source file. When adding existing source files you can use
the file browser, type sources one at a time into the Add File(s)
dialog, or use wild cards in the Add File(s) dialog. This section
describes all methods for adding source files.
.np
The extension you use with the source file name determines the
compiler that the &vip uses to create the target. For example, the ".c"
extension, invokes the &company C compiler to make the target. You can
specify several different compilers:
.begnote $compact
.note xxx.asm
.ix 'extensions' '.asm'
The ".asm" extension invokes the &asmname..
.note xxx.c
.ix 'extensions' '.c'
The ".c" extension invokes the &company C compiler.
.note xxx.cpp
.ix 'extensions' '.cpp'
The ".cpp" extension invokes the &company C++ compiler.
.note xxx.for
.ix 'extensions' '.for'
The ".for" extension invokes the &company FORTRAN 77 compiler.
.note xxx.hlp
.ix 'extensions' '.hlp'
The ".hlp" extension invokes either the Win16, OS/2, or
Win32 compiler.
.note xxx.rc
.ix 'extensions' '.rc'
The ".rc" extension invokes the &company resource compiler.
.note xxx.res
.ix 'extensions' '.res'
The ".res" extension invokes the &company Resource Editor.
.note xxx.sqc
.ix 'extensions' '.sqc'
The ".sqc" extension invokes the embedded SQL preprocessor and the
&company C compiler.
.note xxx.sqp
.ix 'extensions' '.sqp'
The ".sqp" extension invokes the embedded SQL preprocessor and the
&company C++ compiler.
.endnote
.*
.begstep To add new source files to the selected target:
.step Choose New Source
from the Sources menu.
.result
The Add File(s) dialog appears where you specify the name(s) of the
source file(s) you are adding. Source names must be filenames with
extensions; pathnames are not required.
.np
For the OS/2 &vip, you can add multiple source files to the target at
one time by inserting a space between each filename you enter into
the filenames field.
.np
For all other systems, you can enter the names of the source files
(with extensions) in the File Name field, one at a time. Once you have
typed a name in the File Name field, you can add it to the list of
source files to be included by clicking the Add button. Individual
names can be removed from the "add" list by selecting them and
clicking the Remove button.
.figure *depth='4.86' *scale=90 *file='ide22' Add one or several source files to a target using the Add File(s) dialog.
.step Enter the source file name(s).
.np
For the OS/2 &vip, enter all the names into the field separating each
name by a space. Click on OK when done.
.np
For all other systems, enter one name at a time into the File Name
field, pressing Enter or clicking on Add after each one has been
entered.
Click on Close when done.
.np
The Add File(s) dialog closes and the selected source file(s) appear
in the selected target window on the &company &vip Project window.
.np
The source files are grouped by their file extensions and appear in
alphabetical order. You can expand and collapse the list of source
files by clicking on the folder icon to the left of the header.
.endstep
.*
.begstep To add existing source files to the selected target using the file browser:
.step Choose New Source
from the Sources menu.
.result
The Add File(s) dialog appears on the screen.
.np
For the OS/2 &vip, click on the Browse button and the Pick Filename
dialog appears on the screen. This dialog allows you to browse the
directories and files and select the source file(s) you want to add to
the current target. Click on the desired source file to select it.
The selected source file(s) appear highlighted in the Pick Filename
dialog.
When done browsing files, click on OPEN to add the selected files. The
Pick Filename dialog closes. When done selecting files, click on OK.
.np
For all other systems, you can use the file browser to select files.
The type of files displayed in file list window can be controlled by
modifying the List Files of Type field. Once you have selected a name
from the file list, you can add it to the list of source files to be
included by clicking the Add button. All files in the file list can be
added by clicking the Add All button. Individual names can be removed
from the "add" list by selecting them and clicking the Remove button.
When done, click on Close to add the selected files.
.np
The Add File(s) dialog closes and the selected source file(s) appear
in the selected target window on the &company &vip Project window.
.np
The source files are grouped by their file extensions and appear in
alphabetical order. You can expand and collapse the list of source
files by clicking on the folder icon to the left of the header.
.endstep
.*
.begstep To add existing source files to the selected target using wild cards:
.step Choose New Source
from the Sources menu.
.result
The Add File(s) dialog appears on the screen.
.np
For the OS/2 &vip, enter the source file name pattern in the
filename(s) field using wild cards. For example, enter
.fi sou*.c
to add all source files whose name begins "sou" and have a ".c"
extension to the current target. When done, click on OK to add the
selected files.
.hint
Even when entering wild cards, you can add multiple source file groups
by separating each entry with one space (e.g., sou*.c sou*.cpp).
.ehint
.np
For all other systems, enter the source file name pattern in the File
Name field using wild cards. For example, enter
.fi sou*.c
to select all source files whose name begins "sou" and have a ".c"
extension. Click on the Add All button to add all the names that match
the wild card pattern. Individual names can be removed from the "add"
list by selecting them and clicking the Remove button.
When done, click on Close to add the selected files.
.np
The Add File(s) dialog closes and the selected source file(s) appear
in the selected target window on the &company &vip Project window.
.np
The source files are grouped by their file extensions and appear in
alphabetical order. You can expand and collapse the list of source
files by clicking on the folder icon to the left of the header.
.endstep
.*
.section Removing a Source File
.*
.np
.ix '&ide' 'removing source files'
.ix 'source files' 'removing'
The procedure for removing a source file from a project is simple. You
can remove one source at a time or you can remove all source files
with a common file extension from the target. Removing a source file
does not permanently delete the it. The &vip simply removes it from the
current target. If the removed source file is common to multiple
targets, the other targets will not be affected by the removal of the
selected source file.
.begstep To remove one source file from a target:
.step Click once on the source file
you want to remove from the target.
.result
The selected source file appears highlighted in the target window.
.step Choose Remove Source
from the Sources menu.
.result
A message box appears asking if you are sure you want to remove the
selected source file.
.step Click on the Yes button.
.result
The message box closes and the &vip removes the selected source file
from the target window.
.endstep
.begstep To remove a group of source files from a target:
.step Click once on the header
to the right of the folder icon of the source files group you want to
remove from the target.
.result
The selected header appears highlighted in the target window.
.figure *depth='3.78' *scale=60 *file='ide23' Click on the header to remove all source files with a common file extension.
.step Choose Remove Source
from the Sources menu.
.result
A message window appears asking if you want to remove all source files
with the selected file extension.
.step Choose OK
to continue.
.result
The message box closes and the &vip removes the selected group of
source files from the target.
.endstep
.*
.section Renaming a Source File
.*
.np
.ix '&ide' 'renaming source files'
.ix 'source files' 'renaming'
When renaming a source file, you can either give it a new name or you
can give it the name of an existing source file. Choosing an existing
source file name overwrites the contents of that source file with the
contents of the current source file.
.begstep To rename a source file:
.step Click once on the source file
you want to rename.
.result
The selected source file appears highlighted in the target window.
.step Choose Rename Source
from the Sources menu.
.result
The Rename File dialog appears where you enter the new name of the
selected source file.
.np
.figure *depth='3.78' *scale=60 *file='ide24' Use the Rename File dialog to rename the selected source file.
.step Enter the new source file name
in the field.
.orstep Click on the Browse button
and select an existing source file name; click on OK when completed.
.result
The selected or entered source file name appears in the
Rename File dialog.
.step Click on OK
on the Rename File dialog.
.result
The Rename File dialog closes. The target window displays the updated
source file.
.endstep
.*
.section Marking a Source File for Remake
.*
.np
.ix '&ide' 'marking source files for remake'
.ix 'source files' 'marking for remake'
The Mark Source for Remake function in the Sources menu enables you to
mark a source file to ensure that the &vip will recompile it the next
time you make the target. This function is useful because although
changes may not have been made to the selected source file, changes
may have been made to a source file on which the selected source file
depends. This function is necessary if you are not using the
Autodepend function in the Target Options submenu under the Targets
menu. Marking source files for remake gives you control over when and
which source files need to be remade.
.np
With the Mark function you can mark an individual source file or a
group of source files with a common file extension.
.begstep To mark a source file or a group of source files for remake:
.step Click once on the source file
you want to mark.
.orstep Click once on the header
of the group of source files you want to mark.
.result
The selection is highlighted in the target window.
.step Choose Mark Source for Remake
from the Sources menu.
.result
If you selected a group of source files to mark, a message box appears
asking if you want to also mark all of the source files of the
specified extension.
.step Choose Yes
to continue.
.result
The message box disappears and the &vip marks all of the source files
in the selected group to be recompiled.
.endstep
.*
.section Making Source Files
.*
.np
.ix '&ide' 'making source files'
.ix 'source files' 'making'
The Make function in the Sources menu enables you to compile a
selected source file into an object file. You can also perform the
Make function on a group of source files with a common file extension.
.np
Performing the Make function opens the &vip Log window which allows you
to monitor the progress of the make.
.begstep To make source files:
.step Click once on the source file
you want to make.
.orstep Click once on the header
of the group of source files you want to make.
.result
The selected source files are highlighted in the target window.
.step Choose Make
from the Sources menu.
.result
The &vip Log window opens along the bottom of the &company &vip Project
window. In the log window you can monitor the make procedure.
.us Execution Complete
appears in the &vip Log window when the make is completed on the
selected source files.
.endstep
.begstep To make all targets for the current project:
.step Choose Make All
from the Actions Menu.
.result
The &vip Log window opens along the bottom of the &company &vip Project
window. In the log window you can monitor the make procedure.
.us Execution Complete
appears in the &vip Log window when the make is completed on the
targets. If the make is successful an executable file for each target
in the project can be found in the directory associated with the
target window.
.endstep
.*
.beglevel
.*
.section Viewing the Make Command for a Source File
.*
.np
.ix '&ide' 'viewing make commands'
.ix 'viewing make commands'
With the &vip you can use the Show Make Command function to display the
makefile commands for the selected source file. These are the commands
that will be used to compile the selected source file when you choose
the function Make from the Sources menu.
.begstep To view the make commands for source files:
.step Click once on the source file
whose makefile commands you want to view.
.orstep Click once on the header
of the group of source files whose makefile commands you want to view.
.result
The selected source file or group header is highlighted.
.step Choose Source Options
from the Sources menu.
.result
A submenu appears on the screen from the Source Options menu.
.step Choose Show Make Command
from the Source Options submenu.
.result
The Source Command dialog appears on the screen. This dialog
displays the makefile commands for the selected source file or source
file header.
.figure *depth='1.2' *scale=73 *file='ide25' The &vip Source Command dialog displays the makefile commands for the selected source file(s).
.step Click on OK
to close the dialog.
.endstep
.*
.endlevel
.*
.section Viewing Included Files
.*
.np
.ix '&ide' 'viewing included files list'
.ix 'viewing included files list'
The Included Files function in the Sources menu enables you to select
a source file and view a list of all of the files included by that
source file. This is a list of all files on which the selected source
file depends. From this list you can select included files and edit
them.
.np
Because the selected source file depends on the files in this list, it
is necessary to remake it if changes are made to any of the included
files. If you are using the Autodepend function, the &vip automatically
remakes the source file if any of the included files are modified.
.begstep To view the list of included files:
.step Click on the source file
whose list of included files you want to view.
.result
The selected source file is highlighted in the target window.
.step Choose Included Files
from the Sources menu.
.result
The Files Included dialog appears on the screen. This dialog lists all
source files on which the selected source files depends.
.endstep
.begstep To edit a source file from the File Included dialog:
.step Click once on the source file
in the included files list that you want to edit.
.result
The selected source file is highlighted in the File Included dialog.
.step Click on OK.
.orstep Double-click
on the selected source file.
.result
The Files Included dialog closes and the &vip invokes the appropriate
editor for the selected included file. The editor appears and you can
edit the file. Close the editor when you are finished making the
desired changes to return to the &company &vip Project window.
.endstep
.*
.endlevel
.*
.section Using the &vip Log Window
.*
.np
.ix '&ide' 'using the log window'
.ix 'using the log window'
.ix 'log window' 'using the'
When you compile a target or source file, the &vip Log window appears
along the bottom of the screen. Once this window is open it remains on
your screen until you close the project. However, to maximize your
project window space you can minimize the &vip Log window when it is
not being used.
.np
The Log menu contains three functions that you use in relation to the
&vip Log window. This section discusses the following functions:
.begbull $compact
.bull
Stopping the Make Function
.bull
Getting Help
.bull
Editing the Source File
.bull
Saving the Log
.endbull
.*
.beglevel
.*
.section Stopping the Make Function
.*
.np
.ix '&ide' 'stopping the make'
.ix 'stopping the make'
Choosing to make a source file, target, or all targets can be a
lengthy process. The Stop function in the Log menu gives you the
opportunity to cancel the make function once it has started. This
is a useful function in case you decide that you do not want to
continue with the make.
.begstep To stop the make function:
.step Choose Stop
from the Log menu.
.result
A message box appears asking if it is okay to interrupt the
executing process.
.step Click on OK
to continue.
.result
The message box closes and the &vip stops the make function. The &vip
Log window displays the message
.us Stop Invoked
followed by the message
.us Execution Complete.
.endstep
.*
.section Getting Help
.*
.np
.ix '&ide' 'getting help'
.ix 'getting help'
The Help on Message function in the Log menu gives you access to
on-line help information for the selected error message in the &vip
Log window.
.begstep To reveal help for an error message:
.step Click once on the error message
in the &vip Log window on which you want information.
.result
The selected error message is highlighted in the &vip Log window.
.step Choose Help on Message
from the Log menu.
.result
This opens the Help window. Information on the selected error message
appears in this window.
.step Choose Close
from the system menu on the Help window to cancel the Help facility.
.result
The Help window closes and you return to the &vip Log window.
.endstep
.*
.section Editing the Source File
.*
.np
.ix '&ide' 'editing source'
.ix 'editing source'
.ix 'source files' 'editing'
The Edit File function in the Log menu allows you to select an error
message in the &vip Log window and edit the corresponding source file.
This function invokes the appropriate editor for the source file and
positions your cursor in the source code where the error occurred (if
the error message contained line and column information).
.begstep To edit a source file:
.step Click once on the error message
in the &vip Log window that you want to edit.
.result
The selected error message is highlighted in the &vip Log window.
.step Choose Edit File
from the Log menu.
.orstep Double-click
on the selected error message.
.result
Both of these actions open the appropriate editor. If the &edname
appears and the error message contained column and line details, your
cursor will be positioned at the error in the source code.
.step Make the required changes
to the source file and exit the Editor.
.endstep
.*
.section Saving the Log
.*
.np
.ix '&ide' 'saving the log'
.ix 'saving the log'
The Save Log As function in the Log menu allows you to save the contents
of the Log window into a file.
.begstep To save the log:
.step Choose Save Log As
from the Log menu.
.result
The Save Log As dialog appears where you specify the filename by which you
want to save the contents of the Log window.
.step Enter the desired log name
in the File Name field.
.step Specify the directory
to which you want to save the log.
.step Click on the OK button.
.result
The &vip closes the Save Log As dialog and saves the current log as
specified
.endstep
.*
.endlevel
.*
.section The &vip Switches
.*
.np
.ix '&ide' 'setting switches'
.ix 'setting switches'
.ix 'switches' 'setting'
There are a number of different switches you can set in the &wide..
Switches enables you to change the way the various tools, such as
compilers and linkers, perform their jobs. The switches discussed in
this section are:
.begbull $compact
.bull
Release and Development Switches
.bull
Target and Source File Switches
.bull
The Autodepend Switch
.endbull
.np
This section discusses the different switches available in the &vip and
how you can use them.
.*
.beglevel
.*
.section Using Release and Development Switches
.*
.np
.ix '&ide' 'release switches'
.ix 'release switches'
.ix 'switches' 'release'
.ix '&ide' 'development switches'
.ix 'development switches'
.ix 'switches' 'development'
There are two sets of switches the &vip can use during the make
process: Development and Release switches. You are able to specify
different switches in each set for each target in your project. The
letters D and R in the target window to the left of the target type indicate
which set of switches is being used. You choose the switches you want
the &vip to use from the Target Options submenu under the Targets menu.
The selected set of switches has a check mark beside it and appears in
grey text.
.begstep To change the selected set of switches for a target:
.step Click once on the target
whose switches you want to change.
.result
The title bar of the selected target is highlighted.
.step Choose Target Options
from the Targets menu.
.result
A submenu appears from the Target Options menu.
.step Choose Use Development Switches or Use Release Switches
(whichever is not currently selected) from the submenu, as desired.
.result
A message box appears asking if you want to mark the selected target
and its source files for remake.
.step Choose Yes or No,
as desired, to continue.
.result
The message box closes and you return to the &company &vip window. The
next time you perform a make, the &vip will use the selected set of
switches.
.endstep
.*
.section Setting Target and Source File Switches
.*
.np
.ix '&ide' 'setting switches'
.ix 'switches' 'setting'
You can set a variety of command-line compiler and linker options in
the &wide.. This section shows you how to use the Switches dialogs
to choose your options.
.np
You can set the &ide to work in one of two switch modes: Use
Development Switches or Use Release Switches. Either of these two
options uses a default set of switch settings, unless you specify
otherwise. You can set switches for an individual target, specific
source file type, or individual source file.
.*
.beglevel
.*
.section Setting Target Switches
.*
.ix '&ide' 'setting target switches'
.ix 'targets' 'setting switches'
.ix 'switches' 'setting target'
.begstep To set switches for a target:
.step Click once on the target window
for which you want to set switches.
.result
The title bar of the selected window is highlighted.
.step Choose Windows Linking Switches
from the Options menu.
.orstep Choose Target Options
from the Targets menu and then choose Windows Linking Switches from
the Target Options submenu.
.result
The Windows Linking Switches dialog appears on the screen. The
switches you access from this dialog affect only the currently
selected target. There are four Switches dialogs, each one allowing
you to set the following types of switches:
.begbull $compact
.bull
Basic
.bull
Import, Export and Library
.bull
Advanced
.bull
Special
.bull
Resource
.endbull
.np
You can use the forward (>>) and backward (<<) buttons or the
drop-down list to choose the set of switches you want to set.
.figure *depth='3.09' *scale=55 *file='ide26' Use the Windows Linking Switches dialog to set command-line compiler and linker options.
.np
The first dialog allows you to choose the debugging information you
want to see. Remember, if the option Use Development Switches is
enabled in the Target Options submenu of the Targets menu, the &ide
automatically sets switches to include all debugging information. You can
change this setting by selecting Use Release Switches from the Target Options
submenu under the Targets menu or choosing specific switches in the
Switches dialogs.
.np
Select the
.us CopyRel
button in the Switches dialog to copy the set of Release switches to
the current dialog. You can then make further changes, as required. This
button is available only if the Development switches are selected in the Setup
Target submenu under the Targets menu.
.np
Select the
.us CopyDev
button in the Switches dialog to copy the set of Development switches
to the current dialog. You can then make further changes, as required.
This button is available only if the Release switches are selected in the
Target Options submenu under the Targets menu.
.np
Select the Reset button to reset the switches to the default Release
or Development switches, as selected in the Target Options submenu under
the Targets menu.
.step Click on OK
when you are finished setting switches for the selected target.
.result
The Switches dialog closes and you return to the &company &vip Project window.
.endstep
.*
.section Setting Source File Switches
.*
.np
.ix '&ide' 'setting source file switches'
.ix 'source files' 'setting switches'
.ix 'switches' 'setting source file'
The Switches function in the Source Options submenu of the Sources menu
enables you to set switches for the compiler that is associated with
the selected source file or source file group.
.begstep To set switches for a source file group:
.step Click once on the target window
that contains the relevant source files.
.step Choose the appropriate item
from the Options menu.
.result
For example, to set switches for all ".c" files in the selected target,
choose C Compiler Switches from the Options menu.
.np
The Compiler Switches dialog appears on the screen. The switches you
access from this dialog affect only the currently selected source file
or source file group. There are eleven Switches dialogs for &company
C/C++, each one allowing you to set the following types of switches:
.begbull $compact
.bull
File Option Switches
.bull
Diagnostics Switches
.bull
Source Switches
.bull
Miscellaneous Switches
.bull
Optimization Switches
.bull
Debugging Switches
.bull
CodeGen Strategy Switches
.bull
Register Usage Switches
.bull
CodeGen Option Switches
.bull
Memory Model and Processor Switches
.bull
C or C++ Compiler Switches
.endbull
.np
There are ten Switches dialogs for &company FORTRAN 77.
.begbull $compact
.bull
File Option Switches
.bull
Compile-time Diagnostic Switches
.bull
Run-time Diagnostic Switches
.bull
Debugging Switches
.bull
Code Generation Strategy Switches
.bull
Code Generation Option Switches
.bull
Register Usage Switches
.bull
Optimization Switches
.bull
Application Type Switches
.bull
Miscellaneous Switches
.endbull
.np
You can use the forward (>>) and backward (<<) buttons or the
drop-down list to choose the set of switches you want to set.
.figure *depth='3.01' *scale=55 *file='ide27' Use the C Compiler Switches dialog to set switches for the compiler associated with the selected source file(s).
.np
The first of several compiler switch dialogs appears. Of note is the
sixth screen, Debugging Switches, which allows you to choose the
debugging information you want to see. Remember, if the option Use
Development Switches is enabled in the Target Options submenu of the
Targets menu, the &ide automatically sets switches to include all debugging
information. You can change this setting by selecting Use Release
Switches from the Target Options submenu under the Targets menu or
choosing specific switches in the Switches dialogs.
.np
Select the
.us CopyRel
button in the Switches dialog to copy the set of Release switches to
the current dialog. You can then make further changes, as required. This
button is available only if the Development switches are selected in the
Target Options submenu under the Targets menu.
.np
Select the
.us CopyDev
button in the Switches dialog to copy the set of Development switches
to the current dialog. You can then make further changes, as required.
This button is available only if the Release switches are selected in the
Target Options submenu under the Targets menu.
.np
Select the Reset button to reset the switches to the default Release
or Development switches, as selected in the Target Options submenu under
the Targets menu.
.step Click on OK
when you are finished setting switches for the selected source file or
group of source files.
.result
The Switches dialog closes and you return to the &company &vip
Project window.
.endstep
.*
.begstep To set switches for a specific source file:
.step Click once on the source file
for which you want to set switches.
.result
The selected source file is highlighted.
.step Choose Source Options
from the Sources menu.
.result
A submenu for the Source Options menu appears on the screen.
.step Choose the appropriate item
from the Source Options submenu.
.result
For example, if the selected source file has a ".c" extension,
choose C Compiler Switches from the Options menu.
.np
The Compiler Switches dialog appears on the screen. The switches you
access from this dialog affect only the currently selected source
file. There are eleven Switches dialogs for &company C/C++, each one
allowing you to set the following types of switches:
.begbull $compact
.bull
File Option Switches
.bull
Diagnostics Switches
.bull
Source Switches
.bull
Miscellaneous Switches
.bull
Optimization Switches
.bull
Debugging Switches
.bull
CodeGen Strategy Switches
.bull
Register Usage Switches
.bull
CodeGen Option Switches
.bull
Memory Model and Processor Switches
.bull
C or C++ Compiler Switches
.endbull
.np
There are ten Switches dialogs for &company FORTRAN 77.
.begbull $compact
.bull
File Option Switches
.bull
Compile-time Diagnostic Switches
.bull
Run-time Diagnostic Switches
.bull
Debugging Switches
.bull
Code Generation Strategy Switches
.bull
Code Generation Option Switches
.bull
Register Usage Switches
.bull
Optimization Switches
.bull
Application Type Switches
.bull
Miscellaneous Switches
.endbull
.np
You can use the forward (>>) and backward (<<) buttons or the
drop-down list to choose the set of switches you want to set.
.figure *depth='3.01' *scale=55 *file='ide28' Use the C Compiler Switches dialog to set switches for the compiler associated with the selected source file(s).
.np
The first of several compiler switch dialogs appears. Of note is the
sixth screen, Debugging Switches, which allows you to choose the
debugging information you want to see. Remember, if the option Use
Development Switches is enabled in the Target Options submenu of the
Targets menu, the &ide automatically sets switches to include all debugging
information. You can change this setting by selecting Use Release
Switches from the Target Options submenu under the Targets menu or
choosing specific switches in the Switches dialogs.
.np
Select the
.us CopyRel
button in the Switches dialog to copy the set of Release switches to
the current dialog. You can then make further changes, as required. This
button is available only if the Development switches are selected in the
Target Options submenu under the Targets menu.
.np
Select the
.us CopyDev
button in the Switches dialog to copy the set of Development switches
to the current dialog. You can then make further changes, as required.
This button is available only if the Release switches are selected in the
Target Options submenu under the Targets menu.
.np
Select the Reset button to reset the switches to the default Release
or Development switches, as selected in the Target Options submenu under
the Targets menu.
.step Click on OK
when you are finished setting switches for the selected source file or
group of source files.
.result
The Switches dialog closes and you return to the &company &vip
Project window.
.endstep
.*
.endlevel
.*
.section Using the Autodepend Switch
.*
.np
.ix '&ide' 'autodepend switch'
.ix 'switches' 'autodepend'
The Use Autodepend switch in the Target Options submenu of the Targets
menu keeps track of updated source files. Once a source is modified,
the &vip remembers that it needs to be recompiled the next time you do
a make. By using the Use Autodepend switch, the &vip also keeps track
of changes made to source files on which other source files depend.
.np
For example, let's say that your current project has a source file,
.fi source.cpp,
that depends on a header file,
.fi source.hpp.
If the Use Autodepend feature is not enabled and changes are made to
.fi source.hpp
after
.fi source.cpp
in your current project has been compiled, the &vip does not recognize
that your current source file is now out of date. This means that the
next time you do a make, the &vip will not compile
.fi source.cpp.
The Use Autodepend feature tells the &vip to compare the date and time
stamps on all of the dependent source files in addition to the source
files in the current project and their corresponding object files, so
the &vip recognizes when your current source file needs to be
recompiled.
.begstep To set the Use Autodepend switch:
.step Choose Target Options
from the Targets menu.
.result
A submenu appears from the Target Options menu. A check mark beside the
menu option indicates that it is enabled.
.step Select the Use Autodepend option
from the Target Options submenu.
.result
If the option was enabled when you selected it, you have disabled the
Use Autodepend feature. If the option had no check mark when you
selected it, you have enabled the Use Autodepend feature.
.endstep
.*
.endlevel
.*
.section Configuring the &ide
.*
.np
.ix '&ide' 'configuration'
.ix 'configuring the &vip'
.ix '&ide' 'customization'
.ix 'customizing the &vip'
The &ide can be reconfigured to some extent.
You can customize the following features.
.begbull
.bull
Show or hide the Toolbar
.bull
Show or hide the Status Bar
.bull
Enable or disable automatic updates of file status in the target windows
.bull
Select your own text editor
.bull
Select a source revision control system
.endbull
.np
The next few sections describe these features.
.*
.beglevel
.*
.section Show Toolbar
.*
.np
.ix 'show tool bar'
The &ide Toolbar that appears under the Menu Bar can be removed from
or added to the &vip window.
.begstep To add or remove the toolbar:
.step Choose File
from the menu bar.
.result
A checkmark beside Show Tool Bar indicates that the toolbar is present
on the screen.
.step Select Show Tool Bar.
.result
The Show Tool Bar state will be toggled.
If the toolbar was on the screen, it will disappear.
If the toolbar was not on the screen, it will reappear.
.endstep
.*
.section Show Status Bar
.*
.np
.ix 'show status bar'
The &ide Status Bar that appears at the bottom of the &vip window can
be removed from or added to the screen.
.begstep To add or remove the status bar:
.step Choose File
from the menu bar.
.result
A checkmark beside Show Status Bar indicates that the status bar is
present on the screen.
.step Select Show Status Bar.
.result
The Show Status Bar state will be toggled.
If the status bar was on the screen, it will disappear.
If the status bar was not on the screen, it will reappear.
.endstep
.*
.section Auto Refresh
.*
.np
.ix 'auto refresh'
The &vip will automatically update the target windows with the state
of all files when this option is enabled.
.begstep To enable or disable auto refresh:
.step Choose File
from the menu bar.
.result
A checkmark beside Auto Refresh indicates that auto refresh is
enabled.
.step Select Auto Refresh.
.result
The Auto Refresh state will be toggled.
When Auto Refresh is disabled, it may be necessary to select Refresh
from the Window menu whenever the target window status becomes out of
date.
.endstep
.*
.section Selecting Your Own Text Editor
.*
.np
.ix 'set text editor'
You can use your own favourite text editor from within the &ide..
.begstep To select your own text editor:
.step Choose Set Text Editor
from the File menu.
.result
The Set Text Editor dialog appears.
You can enter the name of the text editor in the first field.
You must also indicate whether the text editor is an executable
file or a Dynamic Link Library (DLL).
If the editor is an executable file (rather than a DLL), then you can
enter an argument line in the second field.
The argument line will be supplied to the editor whenever it is
started by the &vip..
The argument line can include any of three different macros which will
be filled in by the &vip..
The macros are:
.begnote
.note %f
The name of the file to be edited.
.note %r
The row in the file at which to position the cursor.
If the editor is invoked from a diagnostic message which contains a
line number then the row value is extracted from the message;
otherwise the row value is 1.
.note %c
The column in the file at which to position the cursor.
If the editor is invoked from a diagnostic message which contains a
column number then the column value is extracted from the message;
otherwise the column value is 1.
.endnote
.np
For example, if the editor argument line that you specified was:
.millust begin
file='%f' row='%r' col='%c'
.millust end
.pc
and you double click on an error message in the Log window that names
the file
.id foobar.c
with an error at line 215 and column 31, then the argument line
that is passed to your editor is:
.millust begin
file='foobar.c' row='215' col='31'
.millust end
.np
This flexibility allows you to specify the name of the file to edit
and the row and/or column at which to position the text cursor.
If no row or column is available, then the &vip will supply the value
of 1 as a default.
.step Select OK
when you wish to confirm the selection of a new editor.
.orstep Select Cancel
when you wish to cancel the selection of a new editor.
.orstep Select Default
when you wish to restore the default editor selection
and then select OK or Cancel.
.endstep
.*
.section Selecting a Source Revision Control System
.*
.np
.ix 'set source control'
.ix 'RCS'
You can use a source code Revision Control System (RCS) in conjunction
with the &ide..
.begstep To select a revision control system
.step Choose Set Source Control
from the File menu.
.result
A number of options are presented:
.begnote
.note MKS RCS
.ix 'MKS RCS'
Mortice Kern Systems Revision Control System
.note MKS SI
.ix 'MKS SI'
Mortice Kern Systems Source Integrity
.note PVCS
.ix 'PVCS'
Intersolv Project Version Control System
.note Other
.ix 'generic RCS'
Any other flavour of revision control system (default)
.note None
No revision control system
.note Object Cycle
.ix 'Object Cycle'
Powersoft's Object Cycle
.note Perforce
.ix 'Perforce'
Perforce
.endnote
.endstep
.np
.ix 'checkout source'
.ix 'checkin source'
When you select a source revision control system, the Checkout Source
and Checkin Source items in the Sources menu are enabled.
The Checkout Source and Checkin Source items spawn batch/command files
or dialogs corresponding to your selection of revision control system.
The batch/command files are located in the &pathnamup\BINW and
&pathnamup\BINP directories.
These files can be modified to suit your environment.
.begnote
.note MKS RCS
.begpoint
.point MKS_CO
for checking out/locking files with MKS RCS
.point MKS_CI
for checking in/unlocking files with MKS RCS
.endpoint
.note MKS SI
.begpoint
.point Checkout Dialog
a check out dialog will appear on the screen when using MKS SI
.point Checkin Dialog
a check in dialog will appear on the screen when using MKS SI
.endpoint
.note PVCS
.begpoint
.point PVCS_CO
for checking out/locking files with Intersolv PVCS
.point PVCS_CI
for checking in/unlocking files with Intersolv PVCS
.endpoint
.note Other
.begpoint
.point GEN_CO
a file which you can edit to provide check out (and lock)
functionality to any generic revision control system
.point GEN_CI
a file which you can edit to provide check in (and unlock)
functionality to any generic revision control system
.endpoint
.note None
When selected, Checkout Source and Checkin Source are disabled.
.endnote
.np
When one of the batch files is spawned, a number of arguments are
passed to the batch file.
.np
For checking out files, the following arguments are passed:
.autopoint
.point
"pause" or "nopause" to enable optional screen pause after completion
of action
.point
full source file path specification
.point
full project file path specification (.wpj file)
.point
full target file path specification (.tgt file)
.endpoint
.np
For checking in files, the following arguments are passed:
.autopoint
.point
full source file path specification
.point
full message file path specification (this file contains the textual
message that you enter when checking in a file)
.point
full project file path specification (.wpj file)
.point
full target file path specification (.tgt file)
.endpoint
.*
.endlevel
.*
.section Executing Special Commands
.*
.np
.ix '&ide' 'before make'
.ix 'before'
.ix 'before make'
.ix '&ide' 'after make'
.ix 'after'
.ix 'after make'
Sometimes you may find it necessary to execute commands before and/or
after performing a make. The &wide provides Before and After commands
in the File and Targets menus. These commands enable you to perform
actions immediately before or after making either the selected target
or all targets for the current project.
.begstep To set Before or After directives:
.step Click once on the target
for which you want to set a before or after directive.
.result
The title bar of the selected target is highlighted.
.step Choose Target Options
from the Targets menu.
.result
A submenu appears from the Target Options menu.
.step Choose Execute Before or Execute After
from the Target Options submenu.
.result
A Target Before or Target After dialog appears on your screen.
.begnote
.note Note:
If you want to set before or after directives to be executed when you
make all targets, select the before and after commands from the File
menu.
.endnote
.step Enter the command
you want to execute before or after the selected target is made.
.figure *depth='2.7' *scale=76 *file='ide29' Enter execute commands in the Target Before or Target After dialogs.
.step Click on OK
to continue.
.result
The &vip stores the information you entered and closes the dialog. When
you next perform a make on the selected target, the &vip will execute
the before or after directive you entered.
.endstep
.*
.section Adding Resources to a Target
.*
.np
.ix '&ide' 'adding resources'
.ix 'resources' 'adding'
You may want to include resources in your target executable. There are
two ways of doing this in the &vip: indirectly and directly. The act of
adding resources to a target can only be done for targets that support
resources, such as Windows or OS/2 PM.
.begstep To add a resource to a target using the indirect method:
.step Create a new source file
with the extension ".rc".
.step Click once on the new source file
to select it.
.step Choose Edit Text
from the Sources menu to invoke the &edname..
.result
The editor opens on your screen with an empty window. You can now add
the resource file text.
.step Enter the resource statements
in the editor.
.step Save your work and close the editor
when you are finished.
.result
You return to the &company &vip Project window. You can now compile the
new source file.
.step Click once on the new source file
to select it.
.result
The selected source file is highlighted in the target window.
.step Choose Make
from the Sources menu to compile the new source file.
.result
The &vip compiles the selected source file. To do this, the &vip
invokes the Resource Compiler which creates a ".res" file. The ".res"
file will be automatically included in the target when it is linked.
.endstep
.np
When you create the resource file directly, the source file contains
the same information but it is stored in a different format.
.begstep To add a resource to a target using the direct method:
.step Create a new source file
with the extension ".res".
.step Click once on the new source file
to select it.
.step Choose Edit Resources
from the Sources menu to invoke the &company Resource Editor.
.result
The editor opens on your screen with an empty window. You can now
create the resources.
.step Create the resources.
.step Save your work and close the editor
when you are finished.
.result
You return to the &company &vip Project window. You can now compile the
new source file.
.step Click once on the new source file
to select it.
.result
The selected source file is highlighted in the target window.
.step Choose Make
from the Sources menu to compile the new source file.
.result
The &vip compiles the selected source file. The &vip does not need to
invoke the Resource Compiler to create a ".res" file because you
created the ".res" file directly using the Resource Editor. The ".res"
file will be automatically included in the target when it is linked.
.endstep
.*
.section Including Libraries in a Target
.*
.np
.ix '&ide' 'adding libraries'
.ix 'libraries' 'adding'
You use the same procedure to include static and DLL libraries in your
target as you use to add any source file to a target. The &vip includes
the libraries when the target is linked.
.begstep To create static and DLL libraries:
.step Create a new target
entering the library name in the Target Name field and choosing the
appropriate image type.
.result
Clicking on OK on the New Target dialog closes the dialog and creates
a new target with the specified name in the title bar.
.step Add the desired source files,
new or existing, to the library you have created.
.begnote
.note Note:
If you want one of your other targets to use that library, you must
enter the library as a source file for that target. When you enter the
library as a source file you must give it the extension ".lib",
regardless of whether the library is a static or DLL library. For
example, if your library is entitled Target A and you want your
existing Target B to use that library, add Target A as a source file
on Target B with the extension ".lib".
.endnote
.endstep
.*
.section Targets and Image Types Supported by the &vip
.*
.np
.ix 'targets'
The &vip supports the creation of a wide variety of targets.
The following sections summarize the choices available.
.*
.beglevel
.*
.section Win32 Targets
.*
.ix 'targets' 'Win32'
.ix 'Win32 targets'
.begnote
.note Character-mode Executable [.exe]
.br
The target is a non-GUI Win32 application.
.note Windowed Executable [.exe]
.br
The target is a GUI Win32 application.
.note Dynamic Link Library [.dll]
.br
The target is a Win32 DLL.
.note Library [.lib]
.br
The target is a Win32 library.
.note Help File [.hlp]
.br
The target is a help file.
.endnote
.*
.section Win16 Targets
.*
.ix 'targets' 'Win16'
.ix 'Win16 targets'
.begnote
.note Executable [.exe]
.br
The target is a 16-bit Windows application.
.note Dynamic Link Library [.dll]
.br
The target is a 16-bit Windows DLL.
.note Library [.lib]
.br
The target is a 16-bit Windows library.
.note Help File [.hlp]
.br
The target is a help file.
.endnote
.*
.section Win386 (Watcom Extender) Targets
.*
.ix 'targets' 'Win386'
.ix 'Win386 targets'
.begnote
.note Executable [.exe]
.br
The target is a 32-bit &company Windows extender application.
.note Dynamic Link Library [.dll]
.br
The target is a 32-bit &company Windows extender DLL.
.note Library [.lib]
.br
The target is a 32-bit &company Windows extender library.
.note Help File [.hlp]
.br
The target is a help file.
.endnote
.*
.section DOS - 16-bit Targets
.*
.ix 'targets' 'DOS 16-bit'
.ix 'DOS 16-bit targets'
.begnote
.note Executable [.exe]
.br
The target is a 16-bit DOS .exe file.
.note .COM Executable [.com]
.br
The target is a 16-bit DOS .com file.
.note Library [.lib]
.br
The target is a 16-bit DOS .lib file.
.endnote
.*
.section DOS - 32-bit Targets
.*
.ix 'targets' 'DOS 32-bit'
.ix 'DOS 32-bit targets'
.begnote
.note CauseWay Executable [.exe]
.br
The target is a 32-bit CauseWay .exe file.
.note CauseWay DLL (register based) [.dll]
.br
The target is a 32-bit CauseWay dll using register-based calling
conventions.
.note CauseWay DLL (stack based) [.dll]
.br
The target is a 32-bit CauseWay dll using stack-based calling
conventions.
.note DOS4GW Executable [.exe]
.br
The target is a 32-bit DOS/4GW .exe file.
.note PMODW/W Executable [.exe]
.br
The target is a 32-bit PMODE/W .exe file.
.note DOS32/A Executable [.exe]
.br
The target is a 32-bit DOS32/A .exe file.
.note Pharlap Executable [.exp]
.br
The target is a 32-bit PharLap "RUN386" .exe file.
.note Pharlap TNT Executable [.exe]
.br
The target is a 32-bit PharLap TNT .exe file.
.note Flashtek Executable [.exe]
.br
The target is a 32-bit FlashTek .exe file using register-based calling
conventions.
.note Flashtek -- Stack based [.exe]
.br
The target is a 32-bit FlashTek .exe file using stack-based calling
conventions.
.note Library [.lib]
.br
The target is a 32-bit DOS .lib file.
.endnote
.*
.section OS/2 - 16-bit Targets
.*
.ix 'targets' 'OS/2 16-bit'
.ix 'OS/2 16-bit targets'
.begnote
.note Executable [.exe]
.br
The target is a 16-bit OS/2 console application.
.note PM Executable [.exe]
.br
The target is a 16-bit OS/2 Presentation Manager application.
.note Fullscreen Executable [.exe]
.br
The target is a 16-bit OS/2 Fullscreen application.
.note Dynamic Link Library [.dll]
.br
The target is a 16-bit OS/2 DLL.
.note Library [.lib]
.br
The target is a 16-bit OS/2 library file.
.note Help File [.hlp]
.br
The target is an OS/2 help file (HLP format).
.note INF File [.inf]
.br
The target is an OS/2 help file (INF format).
.endnote
.*
.section OS/2 - 32-bit Targets
.*
.ix 'targets' 'OS/2 32-bit'
.ix 'OS/2 32-bit targets'
.begnote
.note Executable [.exe]
.br
The target is a 32-bit OS/2 console application.
.note PM Executable [.exe]
.br
The target is a 32-bit OS/2 Presentation Manager application.
.note Fullscreen Executable [.exe]
.br
The target is a 32-bit OS/2 Fullscreen application.
.note Virtual Device [.exe]
.br
The target is a 32-bit OS/2 Virtual Device Driver application.
.note Dynamic Link Library [.dll]
.br
The target is a 32-bit OS/2 DLL.
.note Library [.lib]
.br
The target is a 32-bit OS/2 library file.
.note Help File [.hlp]
.br
The target is an OS/2 help file (HLP format).
.note INF File [.inf]
.br
The target is an OS/2 help file (INF format).
.endnote
.*
.section Linux Targets
.*
.ix 'targets' 'Linux'
.ix 'Linux targets'
.begnote
.note Executable [.exe]
.br
The target is a Linux application.
.note Shared Library [.dll]
.br
Shared libraries are not yet supported.
.note Library [.lib]
.br
The target is a Linux static library file.
.endnote
.*
.endlevel
.*
.section Invoking Other Applications from the &vip
.*
.np
.ix '&ide' 'invoking tools'
.ix 'invoking tools'
When using the &wide, there are several other applications you may
want to use. To save you the frustration of having to constantly exit
the &vip and start up a new application, we have made it possible to
invoke other applications from within the &ide.. Following are the
applications you can invoke from the &vip:
.begbull $compact
.bull
&br
.bull
&dbgname
.bull
&smpname
.bull
&prfname
.bull
&disname
.bull
&edname (text editor)
.bull
&company Resource Editor
.bull
&company Zoom
.bull
&company Heap Walker
.bull
&spy
.bull
&dde
.endbull
.np
You invoke these applications by making a selection from the toolbar
or various menus in the &vip.. The Actions menu contains some of the
applications. You can find others in the Targets and Sources menus.
The menu options that appear vary, depending on the contents of the
current project. The applications are available in the menu items only
when appropriate. For example, if the currently selected target has no
source files, no applications appear in the Sources menu.
.hint
.ix 'mouse' 'right button'
.ix 'right button'
An alternate way to view the contents of the Targets and Sources menu
is to click the right mouse button while your cursor is positioned in
the project window. If your cursor is positioned over a target, the
Targets menu appears. If your cursor is positioned over a source file,
the Sources menu appears.
.ehint
.np
Refer to the appropriate sections in this manual for detailed
information on using the various applications.
.*
.section Invoking DOS Applications from the &vip under OS/2
.*
.np
.ix 'DOS settings'
.ix 'WIN-OS/2 settings'
There are occasions where you may wish to change the default DOS or
WIN-OS/2 settings when running a DOS/Windows application from the
&vip..
The mechanism which we provide is through the use of a special
.ev IDE_DOS_SETTINGS
environment variable.
For example, to set the DPMI memory limit to 16 and to enable DOS
BREAK, you can set the environment variable as follows:
.exam begin
set ide_dos_settings=dpmi_memory_limit#16;dos_break#1
.exam end
.pc
Due to the command processor limitations, the "#" character is
used in place of the "=".
The above command is equivalent to configuring a DOS session with DOS
settings as follows:
.millust begin
Variable            Value
DPMI_MEMORY_LIMIT   16
DOS_BREAK           ON
.millust end
.*
.section Using the Information of the &vip from outside
.*
.np
.ix '&vip' 'make'
.ix '&id2mkname' 'Utility'
The batch utility &id2mkname generates .mk and .mk1 makefiles
for use with &makname from the &vip .wpj and .tgt files.
See &id2mkname in &company Tools User's Guide.
