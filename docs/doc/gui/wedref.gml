.chap The &edname
.*
:CMT. :helplvl level=3.
.np
The &edname is a text editor designed for the Windows environment. It
contains a toolbar and menu items which you can use to make your
choices. It can use proportional fonts. It also contains drag and drop
toolbars or palettes that allow you to make choices and then simply
drag them to the elements to which you want to apply them.
.figure *depth='2.63' *scale=68 *file='vi0' The &edname
.*
.section Startup Options
.*
.np
When the &editor is started under Windows, a set of default options
are used.
To find out what options are available, modify the "Command Line:" of
.kw Program Item Properties
dialog by adding "-?" after the program name and then start
the &editor..
Alternatively, you can use the Program Manager
.kw Run
dialogue to start the &editor, specifying "-?" after the program name.
.*
.section Using Menus
.*
.np
.ix '&editor menus'
.ix 'menus' '&editor'
The &editor follows standard Windows conventions for its menus.
A brief description of each menu item appears in the status bar
on the bottom right of the screen when you select it.
.np
The File and Edit menus contain items standard to most Windows
applications.
.*
.section Using Drag and Drop Palettes
.*
.np
.ix 'drag and drop dialogs'
.ix 'dialogs' 'drag and drop'
The &editor contains two drag and drop palettes.
You can affect elements of the &editor by dragging and dropping
&colour.s or fonts onto them.
.*
.section Using the Right Mouse Button
.*
.np
.ix 'context-sensitive menus' '&editor'
.ix '&editor context-sensitive menus'
.ix 'right mouse button'
.ix 'mouse button' 'right'
The &editor has two context-sensitive menus&mdash. one for the current
cursor location if less than one line is selected, and one if several
lines are selected. You can access both of
them by clicking on the right mouse button.
.*
.beglevel
.*
.section When Less Than One Line is Selected
.*
.np
When the cursor is in a word or a word is selected you can press the
right mouse button to select the following options:
.begnote $break
.note Windows Help
Choose Windows Help to find information about the currently selected
text.
.note CLib Help
Choose CLib Help to find information from the C Library Reference
manual about the currently selected text.
.note Open
Choose Open to open a new file. The name of the new file is the
selected text.
.ix 'Open' '&editor'
.note Cut
Choose Cut to delete the selected text and place it on the Windows
clipboard.
.note Copy
Choose Copy to copy the selected text to the Windows clipboard and
leave the original.
.note Find
Choose Find to look for the first occurrence of the selected text. The
Find dialog appears.
.note Fgrep
Choose Fgrep to find all files in the current directory which match
the grep default mask and contain the selected text.
.ix 'Fgrep'
.note Tag
Choose Tag to look for a tag whose name is the selected text.
.endnote
.*
.section When More Than One Line is Selected
.*
.np
When several lines are selected, press the right mouse button to
select the following options:
.begnote $break
.note Cut
Choose Cut to delete the selected lines and place them on the Windows
clipboard.
.note Copy
Choose Copy to copy the selected lines to the Windows clipboard and
leave the original lines.
.note >>Shift Right
.ix 'Shift Right'
.ix 'Right' 'Shift'
Choose Shift Right to move the selected lines right by a number of
spaces equal to the shift width setting.
.note <<Shift Left
.ix 'Shift Left'
.ix 'Left' 'Shift'
Choose Shift Left to move the selected lines left by a number of
spaces equal to the shift width setting.
.endnote
.*
.endlevel
.*
.section Starting and Quitting the &editor
.*
.np
.ix '&editor' 'starting and leaving'
.ix 'start' '&editor'
You can start the &editor from within the &company &vip. or through
Windows.
.np
To open the &editor from within the &company &vip., double click on a
source module. To start the &editor from Windows, double click on
the &edname icon.
.np
.ix 'leave' '&editor'
To leave the &editor, choose Exit from the File menu.
.*
.section Opening and Closing Files
.*
.np
.ix '&editor' 'opening files'
.ix 'opening' '&editor files'
.ix '&editor' 'closing files'
.ix 'closing' '&editor files'
There are three items that you can select from the File menu to open
and close files:
.begpoint $break
.point New
Open a new file.
.point Open
Open an existing file.
.point Close
Close an open file.
.endpoint
.*
.beglevel
.*
.section Opening a New File
.*
.ix '&editor' 'open new file'
.ix 'open' '&editor file'
.begstep
.step Start the &editor..
.step Choose New
from the File menu.
.result
An empty text window appears.
.endstep
.*
.section Opening an Existing File
.*
.ix '&editor' 'open existing file'
.ix 'open' '&editor file'
.begstep
.step Start the &editor..
.step Choose Open
from the File menu.
.result
The standard Windows Open dialog appears.
.step Choose the file and click on OK.
.result
You may have to change the current drive and directory to find the
file. The file then opens.
.endstep
.*
.section Closing a File
.*
.np
.ix '&editor' 'closing a file'
.ix 'close' '&editor file'
To close a file, choose Close from the File menu.
.np
You can also close any
open file by choosing File List from the File menu. You can then select
the file from the list of open files and click on the Close button.
.*
.endlevel
.*
.section Saving Files
.*
.np
.ix '&editor' 'save files'
.ix 'save' '&editor files'
There are three items that you can select from the File menu to save
files:
.begpoint $break
.point Save
Save the changes made to the current file.
.point Save As...
Save the current file under a different name. This is the standard
Windows Save As... dialog. When you save the current file with a new
name, only the new file remains open.
.point Save All
Save the changes made to all open files.
.endpoint
.*
.section The &editor Toolbar
.*
.np
.ix '&editor' 'tool bar'
.ix 'tool bar' '&editor'
In the &edname window, the toolbar appears below the menu bar
when the Toggle Toolbar in the Options menu is selected (default). A
check mark beside the menu option indicates that it is enabled. To
disable this feature, choose the option again from the Options menu
and the toolbar disappears.
.figure *depth='2.63' *scale=68 *file='vi1' You can access up to 15 &editor functions from the toolbar.
.np
The following explains the function each icon performs, as well as the
equivalent function on the menu bar.
.begpoint $break
.point New
:HBMP 'new.bmp' i
.ix 'new file'
Open a new file. This is equivalent to New in the File menu.
.point Open
:HBMP 'open.bmp' i
.ix 'open file'
Open a new or existing file. This is equivalent to Open in the File
menu.
.point Save
:HBMP 'save.bmp' i
.ix 'save file'
Save the current file. This is equivalent to Save in the File menu.
.point Cut
:HBMP 'cut.bmp' i
.ix 'cut text'
Delete the selected text to the clipboard.
This is equivalent to Cut in the Edit menu.
.point Copy
:HBMP 'copy.bmp' i
.ix 'copy text'
Copy the selected text to the clipboard.
This is equivalent to Copy in the Edit menu.
.point Paste
:HBMP 'paste.bmp' i
.ix 'paste text'
Insert the clipboard contents at the current cursor position.
This is equivalent to Paste in the Edit menu.
.point Undo
:HBMP 'undo.bmp' i
.ix 'undo change'
Undo the last change to the current file.
This is equivalent to Undo in the Edit menu.
.point Redo
:HBMP 'redo.bmp' i
.ix 're-do change'
Undo the last undo.
This is equivalent to Redo in the Edit menu.
.point Find
:HBMP 'finddlg.bmp' i
.ix 'find text'
Search for the specified text.
This is equivalent to Find in the Edit menu.
.point Find Next
:HBMP 'refind.bmp' i
.ix 'find next'
Repeat the last search.
This is equivalent to Find Next in the Edit menu.
.point Match
:HBMP 'bmatch.bmp' i
.ix 'find match'
.ix 'match text'
Find matching brackets from the current cursor position.
.point File List
:HBMP 'files.bmp' i
.ix 'file list'
.ix 'list files'
Display a list of all files being edited.
This is equivalent to File List in the File menu.
.point Previous File
:HBMP 'prevfile.bmp' i
.ix 'previous file'
Switch to the previous file.
.point Next File
:HBMP 'nextfile.bmp' i
.ix 'next file'
Switch to the next file.
.point IDE
:HBMP 'hammer.bmp' i
.ix 'previous file'
Reactivate the &vip..
This icon is present on the toolbar if the &editor was invoked from
the &vip..
.endpoint
.*
.section Searching Text
.*
.np
.ix 'finding text'
.ix 'searching text'
.ix 'regular expressions'
The &editor has a powerful search function that you can use to locate
strings of text, including regular expressions. For more information
on regular expressions, refer to the
chapter entitled :HDREF refid='regexp'..
.figure *depth='2.53' *scale=80 *file='vi2' The Find Dialog
.np
To search for a string of text:
.begstep
.step Choose Find
from the Edit menu.
.result
The cursor flashes in the Find box.
.step Enter the search string
in the Find box.
.step Set the find options.
.step Click on the Find button.
.result
When the search is successful, you are repositioned to the line
containing the string.
.endstep
.np
You can use Find Next and Find Previous in the Edit menu to repeat a
search for the same string.
.*
.beglevel
.*
.section Setting Search Options
.*
.np
.ix 'find options'
.ix 'search options'
The Find dialog contains a number of default options that you can set
in the
.us General...
dialog of the Options menu. You can change the default settings for
the current session in the Find dialog.
.np
You can set the following options:
.begpoint $break
.point Ignore Case
You can have the search match the case exactly or match the characters
only and ignore the case.
.point Regular Expressions
You can use regular expressions in your "Find" text if you check this
box.
.point Search Backwards
By default, a search is conducted through the lines that follow the
current cursor position.
Select this option if you wish to search backwards from the current
cursor position.
.point Wrap Search
By default, the search ends when it reaches the end of the file.
However, if your search begins in the middle of the file and you want
to search through the whole file, you can set the search to go to the
beginning of the file and continue until it reaches the search
starting point.
.endpoint
.*
.endlevel
.*
.section Replacing Text
.*
.np
.ix 'search and replace'
.ix 'replacing text'
.ix 'regular expressions'
The &editor has a powerful search and replace function that you can
use to replace strings of text, including regular expressions. For
more information on regular expressions, refer to the
chapter entitled :HDREF refid='regexp'..
.figure *depth='3.20' *scale=77 *file='vi3' The Replace Dialog
.np
To search and replace a string of text:
.begstep
.step Choose Replace
from the Edit menu.
.result
The cursor flashes in the Find box.
.step Enter the search string
in the Find box.
.step Press Tab.
.result
The cursor moves to the Replace box.
.step Enter the replace string.
.step Set the replace options.
.step Click on the Replace button.
.result
When the replace is complete, you return to the file.
.np
If the
.us Prompt on Replace
option is turned on, you must confirm each replacement. If it is
off, all occurrences of the string are replaced automatically.
.endstep
.*
.beglevel
.*
.section Setting Search and Replace Options
.*
.np
.ix 'search and replace options'
.ix 'replace options'
The Replace dialog contains a number of default options that you can
set in the
.us General...
dialog of the Options menu. You can change the default settings for
the current session in the Replace dialog.
.np
You can set the following options:
.begpoint $break
.point Ignore Case
You can have the search match the case exactly or match the characters
only and ignore the case.
.point Regular Expressions
You can use regular expressions in your "Find" and "Replace" text if
you check this box.
.point Prompt on Replace
By default, all found strings are replaced with the replace string.
Turn this option on to be prompted for each replacement.
.point Wrap Search
By default, the search ends when it reaches the end of the file.
However, if your search begins in the middle of the file and you want
to search through the whole file, you can set the search to go to the
beginning of the file and continue until it reaches the search
starting point.
.endpoint
.*
.endlevel
.*
.section Changing the Font
.*
.np
.ix '&editor' 'changing the font'
.ix 'fonts' 'changing in &editor'
You can define the font for the message window, status window, and
all syntax elements in the edit buffers.
.np
Within the edit buffer, all fonts must be the same typeface and point
size. Other areas, such as message or status windows, can have
any typeface or point size.
.figure *depth='2.75' *scale=77 *file='vi4' The Font Settings Dialog
.np
You can use the drag and drop feature with the Font dialog.
To use drag and drop:
.ix 'drag and drop'
.begstep
.step Choose Fonts
from the Options menu.
.step Choose the typeface, style, and point size
that you want. A sample of the text appears in the Drag and Drop
box.
.step Click in the drag and drop box and drag the cursor
to the element to which you want to apply the font characteristics.
.result
All elements change to the new font style. All buffers are
affected when you change one.
.endstep
.*
.section Changing &ccolour.s
.*
.np
.ix '&colour.s' 'defining in the &editor'
.ix 'drag and drop'
Use the Colors drag and drop palette to set the &colour of your windows
and code. You can define the &colour of the toolbar, message window,
status window, and all syntax elements in the edit buffers.
.figure *depth='2.72' *scale=77 *file='vi5' The Colors Palette
.np
To change the &colour of an element in your file:
.begstep
.step Choose Colors
from the Options menu.
.step Click on the &colour you want and drag it
to the screen or syntax element to which you want to apply the &colour.
The element is changed to the new &colour.
.result
Use the left mouse button to &colour foreground elements and the right
mouse button to &colour background elements.
.ix 'mouse buttons' 'right and left in the &editor'
.np
Press Control and the right mouse button to affect the &colour of all
backgrounds. Press Control and the left mouse button to affect all
foregrounds.
.endstep
.*
.section Accessing Help
.*
.np
A description of the currently selected option appears in the Status
bar on the bottom right of the screen.
.np
You can also choose items from the Help menu to see more information
about an item.
.*
.section Using Fgrep Capabilities
.*
.np
.ix 'fgrep'
Fgrep stands for
.us File Global Regular Expression and Print.
It is a powerful tool that allows you to search through a number
of files to find all occurrences of a regular expression.
.figure *depth='2.10' *scale=82 *file='vi6' The Fgrep Dialog permits you to search files containing a certain string.
.np
To perform an fgrep:
.begstep
.step Choose Fgrep
from the Edit menu.
.result
A dialog appears.
.step Enter the text you want to search for.
.step Select any options you want to use during the search.
.step Click on OK.
.endstep
.np
You can set the default "fgrep" file extensions in the
.us File Specific...
dialog of the Options menu. For a description of the options, refer to
the section entitled :HDREF refid='filespo'..
.np
The
.us Files Containing
dialog shows the name of all files that contain the string as well as
the beginning of the line that contains the string.
.figure *depth='3.20' *scale=66 *file='vi7' The Files Containing Dialog shows you all files with occurrences of a string.
.np
From the Fgrep dialog you can:
.begpoint $break
.point Edit
Select an individual occurrence and open that file. The file opens in
the background, but the Fgrep dialog remains.
.point Goto
Open the selected file for editing at the first occurrence of the search
pattern in the Fgrep dialog.
.point Get All
Open all found files. The files open, with the last one in the list
the currently open file.
.point Cancel
Leave the Fgrep dialog.
.endpoint
.*
.section Configuring the &editor
.*
.np
.ix '&editor' 'configure'
.ix 'configure' '&editor'
The Options menu items contain all of the control and formatting
options available with the &editor.. You can configure the &editor as
you like. Most options apply to the &editor, but some apply only to
the current file. A description of each of the dialogs follows.
.*
.beglevel
.*
.section Status Bar Contents
.*
.np
.ix 'editor' 'status bar settings'
.ix 'status bar' 'editor'
Use the Status Bar Contents dialog to set the contents of the status
bar.
.figure *depth='3.15' *scale=77 *file='vi8' The Status Bar Contents dialog
.np
The options in the Status Bar Contents dialog are divided into three
areas:
.begbull $compact
.bull
Items
.bull
Alignment
.bull
Commands
.endbull
.*
.beglevel
.*
.section Status Bar Contents : Items
.*
.np
.ix 'editor' 'status bar items'
The items in this area are:
.begbull $compact
.bull
Time of day
.bull
Current date
.bull
Current insertion mode
.bull
Line number
.bull
Column number
.bull
Menu help text
.endbull
.np
Any of these items may be dragged to one of the windows in the status
bar.
When an item is dragged to one of these windows, it replaces the
contents of that window.
For example, you can drag the "Date" item into the status bar window
displaying the current line number and the current date will be
displayed in its place.
.figure *depth='2.63' *scale=68 *file='vi9' The Status Bar consists of several small windows
.*
.section Status Bar Contents : Alignment
.*
.np
.ix 'editor' 'status bar alignment'
The items in this area are:
.begbull $compact
.bull
Left aligned
.bull
Center aligned
.bull
Right aligned
.endbull
.np
When any of these items are dragged to a window in the status bar, it
affects the alignment of the item currently displayed in the window.
For example, you can drag the "Center aligned" item (the middle one)
into the status bar window displaying the current time and the current
time will be centered in the window.
.*
.section Status Bar Contents : Commands
.*
.np
.ix 'editor' 'status bar commands'
The items in this area are:
.begbull $compact
.bull
Split
.bull
Clump
.bull
Defaults
.endbull
.np
When the "split" item is dragged to a window in the status bar, the
window is split into two evenly-sized smaller windows.
You can adjust the size of a window by dragging the bar between two
windows to the left or right.
.np
When the "clump" item is dragged to a window in the status bar, the
window is removed from the status bar.
.np
When the "defaults" item is dragged to any place on the status bar,
the default settings for the status bar are re-established.
.*
.endlevel
.*
.section General Options
.*
.np
.ix 'editor' 'general options'
Use the General Options dialog to set the general features of the &editor
including save, search, and word definitions.
.figure *depth='2.85' *scale=68 *file='vi10' The General Options Dialog
.np
The options in the General Options dialog are divided into six categories:
.begbull $compact
.bull
Features
.bull
VI Emulation
.bull
Searching
.bull
Word Definitions
.bull
Filenames/Paths
.bull
Miscellaneous
.endbull
.*
.beglevel
.*
.section General Options : Features
.*
.np
.ix 'editor features'
The Features section allow you to set options such as undo, autosave,
automatic save of configuration, and save of files upon &vip build.
.begpoint $break
.point Undo
.ix '&editor' 'undo'
.ix 'undo' 'setting in &editor'
Turn this option on to allow an unlimited number of undo's.
.point AutoSave
.ix 'autosave' '&editor'
.ix '&editor' 'autosave'
Enable or disable the autosave option. This option allows you to
determine how often a copy of your file is saved to the Temp
Directory. Enter the time in seconds in the Interval box.
.point Save configuration on exit
.ix '&editor' 'save configuration'
.ix 'configure' '&editor'
Turn this option on if you want the &editor to save the current
configuration when you leave the &editor..
.point Save files on IDE build
.ix '&editor' 'save files'
.ix 'save files' '&editor'
Turn this option on if you want the &editor to be notified whenever
you start an IDE make.
It will cause the &editor to prompt you for each file that was
modified and has not been saved to disk since the last make (build).
.endpoint
.*
.section General Options : VI Emulation
.*
.np
.ix 'VI emulation'
Beneath the &editor.'s mild-mannered exterior lurks the
full power of the &company VI editor, complete with scripting,
commands and bookmarks.
Turning on VI emulation causes the &editor to emulate the behaviour of
the &company VI editor.
.*
.section General Options : Searching
.*
.np
.ix 'search and replace' 'options'
Use the Searching options dialog to set the default
search and replace options.
.begpoint $break
.point Ignore case
.ix '&editor' 'ignore case'
.ix 'ignore case'
.ix 'case' 'ignoring'
Turn this option on if you do not want the Search function to match
case when searching for a string.
.point Wrap at EOF
.ix 'EOF' 'wrap at'
.ix '&editor' 'wrap at EOF'
Turn this option on if you want the search to continue at the beginning
of the file when it reaches the end of the file. It will then
continue to the starting point of the search.
.endpoint
.*
.section General Options : Word Definitions
.*
.np
.ix 'word definitions'
.ix 'words' 'defining'
Use the Word Definition options to define pairs or ranges
that are considered valid
parts of a word. The default for Ctrl-Left/Right
.ix 'Ctrl-Left/Right' 'in &editor'
.ix '&editor' 'Ctrl-Left/Right'
is __09AZaz which defines the underscore
character, all numbers, and all upper and lower case letters.
.ix 'mouse clicks' 'defining words for in &editor'
.ix '&editor' 'mouse clicks'
The default for mouse clicks is ::..\__09AZaz which defines the colon,
period, backslash, and underscore characters as well as all numbers,
and upper and lower case letters.
.*
.section General Options : Filenames/Paths
.*
.np
.ix 'directory' 'default in &editor'
.ix '&editor' 'default directory'
.ix 'default directory' 'in &editor'
Use the Filenames/Paths option to define the default directory to
which the &editor writes temporary files and the &editor's history
file.
.np
The Temp Directory is set, by default, from the
.ev TMPDIR
environment variable if it is defined.
.np
Autosave files are written to the Temp Directory.
.*
.section General Options : Miscellaneous
.*
.np
Use the miscellaneous options to set:
.begpoint $break
.point Beep on error
.ix '&editor' 'beep on error'
.ix 'error' 'beep on'
Turn this option on if you want an audible beep to sound when
the &editor encounters an error.
.point Same-file check on open
.ix 'same-file check'
.ix '&editor' 'same-file check'
Use this option to have the &editor check that a file you are opening
is not already open. You can open multiple copies of the same file,
however, the &editor keeps only the last version of the file saved. To
open multiple copies of the same file, you must specify different
paths.
.point Close rotates forward
.ix '&editor' 'rotate files forward'
Turn this option on if you want the &editor to rotate forward through
the open files when you close the current file.
.endpoint
.*
.endlevel
.*
.section *refid=filespo File Specific Options
.*
.np
.ix '&editor' 'file specific options'
.ix 'file specific options'
The File Specific Options dialog contains settings that control your file
including tabs, tags, line numbers, and default file extensions.
.figure *depth='3.20' *scale=63 *file='vi11' The File Specific Options Dialog
The settings are divided into five categories:
.begbull $compact
.bull
Source
.bull
Language
.bull
Tabs
.bull
Tags
.bull
Miscellaneous
.endbull
.*
.beglevel
.*
.section File Specific Options : Source
.*
.np
.ix '&editor' 'source options'
.ix 'source options'
The source section contains a number of options that allow you to control
your source code.
.begpoint $break
.point Read entire file
Turn this on if you want the &editor to read the entire file when it
opens a new file. Turn this option off if you want the &editor to load
small pieces of the file as needed.
.point Check read-only flag
.ix 'read only flag'
Turn this option on if you want files that you open to be read only. You
cannot change or write the files under the same name.
.point Ignore Ctrl-Z
Turn this option on if you want the &editor to ignore Ctrl-Z as the end
of file marker.
.point CRLF Autodetect
Turn this option on if you want the &editor to detect if the file it is
opening has CR and LF at the end of each line. If it does, it will write
the file in the same format.
.point Write CRLFs
.ix 'CRLF' 'write'
Turn this option on if you want both a CR and an LF at the end of each line.
This is the standard format for DOS files.
.point Use eight-bit characters
.ix 'eight-bit characters'
Turn this option on if you want to edit the IBM extended characters,
such as line drawing. If this option is off, you can edit normal text
only.
.endpoint
.*
.section File Specific Options : Language
.*
.np
.ix '&editor' 'language options'
.ix 'language options'
The &editor allows you to highlight different parts of the syntax for the
language you are using. This allows you to easily find the parts of code
you are looking for because they are different &colour.s and/or fonts.
The &editor highlights lexical elements for C, C++, Fortran, Java
and JavaScript, and HTML code (among others).
.np
The Language section also allows you to turn on C Indent Assist.
The &editor then uses common C formatting conventions such as smart
indenting and outdenting with braces.
.*
.section File Specific Options : Tabs
.*
.np
.ix '&editor' 'tabs'
.ix 'tabs'
Use the tab options to define what the tab key does and the tab amounts.
.begpoint $break
.point Tab amount
.ix 'tab width' '&editor'
.ix '&editor' 'tab width'
Set the tab width if real tabs has not been set.
Tab amount specifies the number of spaces indented by tab key.
.point Shiftwidth
.ix '&editor' 'shiftwidth'
.ix 'shiftwidth'
Set the shiftwidth value. The default is four characters.
.point Real Tabs
.ix 'tabs' 'in &editor'
Turn Real Tabs on to use the ASCII tab character instead of spaces when
you press TAB. Turn this option off to have tabs replaced with the
appropriate number of spaces.
.point Hard Tabs
Set the width of hard tabs. The default is eight characters.
.point Autoindent
.ix 'autoindent'
.ix '&editor' 'autoindent'
Turn on the Autoindent feature. Each new line then indents to the same
level as the previous line.
.endpoint
.*
.section File Specific Options : Tags
.*
.np
.ix '&editor' 'tags'
.ix 'tags' 'configuring in the &editor'
Use the Tags options to control the file that contains the list of
code items in your source files.
.begpoint $break
.point Tag File
Enter the name of the tag file that contains your ctags.
.point Ignore Case
Turn this option on, if you want the tag file to ignore the case of
tags.
.point Prompt on multiple tags
Turn this option on, if you want to be prompted when you tag a function
that has multiple entries in the tags file.
.endpoint
.np
Tag files are created by the CTAGS utility, which is described in the 
&company VI editor documentation.
.*
.section File Specific Options : Miscellaneous
.*
.np
.ix '&editor' 'grep extensions'
.ix 'fgrep extensions'
.ix 'grep extensions'
Use the Miscellaneous options to define the default file extensions
for &editor files and to show matching brackets.
.begpoint $break
.point Grep extensions
Define the default file extension to grep when you choose fgrep from
the right mouse button pop-up menu.
.point Show matching brackets
Turn this option on for the cursor to flash to the opening brace when you
type the closing brace.
.ix 'matching brace'
.ix 'brace' 'matching'
.endpoint
.*
.endlevel
.*
.section Screen/Window Options
.*
.np
.ix '&editor' 'screen/window options'
Use the Screen and Window Options dialog to define how you move around
your screen and what appears on it.
.figure *depth='2.51' *scale=80 *file='vi12' The Screen/Window Options Dialog
.*
.beglevel
.*
.section Screen/Window Options : Editing
.*
.np
The Editing section allows you to to define how the text moves around
the screen.
.begpoint $break
.point Jumpy Scrolling
.ix '&editor' 'scrolling'
.ix 'scrolling' '&editor'
Turn this option on to scroll two lines at a time instead of one.
.point Line-based Selection
Turn this option on
to select a whole line no matter where you are in the line when you select
more than one line. You can still however, select part of one line.
If this option is off, you can select any part of the first and last
line in the block of lines you select.
.point Paging
.ix '&editor' 'page up and down'
.ix 'paging' '&editor'
Enter the number of lines that you want to overlap when you use the Page Up
and Page Down keys.
.endpoint
.*
.section Screen/Window Options : Miscellaneous
.*
.np
The Miscellaneous options allow you to set the following:
.begbull
.bull
Enter your own string to appear at the end of the file.
.ix '&editor' 'EOF string'
.ix 'end-of-file string' '&editor'
For example, you may want the word
.ul
END
to appear at the end of the file.
.bull
Save the &editor's screen position when you exit the file.
.bull
Clear messages after the next key is pressed instead of having them
remain until the next error message occurs.
.endbull
.*
.endlevel
.*
.section Saving the Configuration
.*
.np
.ix '&editor' 'saving configuration'
.ix 'saving configuration'
The Save Configuration menu item will save your current configuration
regardless of the status of the "Save configuration on exit" setting.
The configuration information is saved to a
.fi weditor.ini
file either in the user's home directory (on multi-user systems) or
in the Windows system directory.
.*
.endlevel
.*
