.*
.*
.chap The &company &e2bcmdu Utility
.*
.*
.np
The &e2bcmd utility strips off the header of a DOS executable file and
applies any necessary fixups. In addition, it is able to display the header
and relocations of an executable file in human readable format.
.np
When DOS executes a program (supplied as an ".exe" file) it first reads the
header of the executable file and ensures there is enough memory to load the
program. If there is, DOS loads the file &mdash. excluding the header
&mdash. to memory. Before jumping to the entry point, DOS has to adjust a
number of certain locations that depend on the load address of the program.
These adjustments consist of the addition of the load address to each entry in
the above mentioned list of relocations. These relocations are part of the
header of an executable file. The load address may vary from invocation to
invocation, this creates the need for the existence of relocations.
.np
As &e2bcmd. strips the executable header, the relocations are lost (among other
things). This would render the resulting output useless, if &e2bcmd. were not
to apply the relocations as part of the conversion process. Just like DOS,
&e2bcmd. therefore needs to know the load address. This is supplied via an
argument to &e2bcmd..
.np
Some programs do not rely on the address they are being loaded at, and
consequently do not contain any relocations. In this case &e2bcmd. merely
copies the contents of the input file (apart from the header) to the
output file.
.np
The phrase "binary part" (also "binary data") is used as a technical term in
the documentation of &e2bcmd.. It denotes the data following the header. The
length of the binary data is determined by the header entries "Size mod 512",
"Number of pages" and "Size of header". It is not directly related to the
actual size of the input file.
.*
.remark
Although &e2bname is capable of producing DOS ".COM" executables, this
functionality is only provided for compatibility with other tools. The
preferred way of generating ".COM" executables is to use
the &lnkname. with directive
.mono "format dos com".
Refer to the &lnkname Guide for details.
.eremark
.np
.*
.*
.section The &e2bname. Utility Command Line
.*
.np
The format of the &e2bname command line is as follows.
Items enclosed in square brackets ("[ ]") are optional.
.*
.mbigbox
&e2bcmdup [options] exe_file [bin_file]
.embigbox
.*
.synote
.mnote options
is a list of options, each preceded by a dash ("&minus."). On non-UNIX platforms,
a slash ("/") may be also used instead of a dash.
Options may be specified in any order. Supported options are:
.*
.begnote
.note h
display the executable file header
.note r
display the relocations of the executable file
.note l=<seg>
specify the load address of the binary file
.note x
enable extended capabilities of &e2bname.
.endnote
.*
.mnote exe_file
is a file specification for a 16-bit DOS executable file used as input. If no
file extension is specified, a file extension of ".exe" is assumed.
Wild card specifiers may not be used.
.mnote bin_file
is an optional file specification for a binary output file. If no file name
is given, the extension of the input file is replaced by "bin" and taken as
the name for the binary output file.
.esynote
.np
.*
.autonote Description:
.*
.note
If are any relocations in the input file, the &sw.l option becomes
mandatory (and is useless otherwise).
.note
If &e2bcmd. is called without the &sw.x option, certain restrictions to the
input file apply (apart from being a valid DOS executable file):
.begbull
.bull
the size of the binary data must be <= 64 KByte
.bull
no stack must be defined, i.e. ss:sp = 0x0000:0x0000
.bull
the code segment must be always zero, i.e. cs = 0x0000
.bull
the initial instruction pointer must be either ip = 0x0000 or ip = 0x0100
.endbull
None of the above restrictions apply if the &sw.x option is supplied.
.note
If cs:ip = 0x0000:0x0100 and the &sw.x option is not specified, no relocations
are allowed in the input file. Furthermore, &e2bcmd. skips another 0x100 bytes
following the header (in addition to the latter).
.np
This behaviour allows the creation of DOS ".COM" executables and is implemented
for backward compatibility. It is however strongly suggested to use the &lnkname.
instead (together with directive
.mono "format dos com"
).
.endnote
.np
.*
The examples below illustrate the use of &e2bname..
.exam begin
&e2bcmd prog.exe
.exam end
Strips off the executable header from
.mono prog.exe
and writes the binary part to
.mono prog.bin.
If there are any relocations in
.mono prog.exe
or if the input file violates any of the restrictions listed above,
the execution of &e2bcmd fails.
.*
.exam begin
&e2bcmd -x prog.exe
.exam end
Same as above but the
.mono "-x"
option
relaxes certain restrictions.
.*
.remark
Even if &e2bcmd. is sucessfully invoked with identical input files as in the
preceding examples (i.e. with vs. without &sw.x) the output files may differ.
This happens when cs:ip = 0x0000:0x0100 causes &e2bcmd. to skip additional
0x100 bytes from the input file, if the user did not specify &sw.x.
.eremark
.*
.exam begin
&e2bcmd &sw.h prog.exe test.bin
.exam end
Displays the header of
.mono prog.exe
, strips it off and copies the binary part to
.mono test.bin.
.*
.exam begin
&e2bcmd &sw.h &sw.r &sw.x &sw.l=0xE000 bios.exe bios.rom
.exam end
Displays the header and the relocations (if any) of
.mono bios.exe
strips the header and applies any fixups to (i.e. relocates)
.mono bios.exe
as if it were to be loaded at 0xE000:0x0000. The result will be written to
.mono bios.rom
.np
The above command line may serve as an example of creating a 128 KByte BIOS
image for the PC-AT architecture.
.*
.*
.section &e2bcmdu. Messages
.*
.np
This is a list of the diagnostic messages &e2bcmd. may display, accompanied
by more verbose descriptions and some possible causes.
.*
.begnote $break
.*
.mnote Error opening %s for reading.
The input executable file could not be opened for reading.
.np
Check that the input file exists and &e2bcmd. has read permissions.
.*
.mnote Error opening %s for writing.
The output binary file could not be opened for writing.
.np
Make sure the media is not write protected, has enough free space to hold the
output file, and &e2bcmd. has write permissions.
.*
.mnote Error allocating file I/O buffer.
There is not enough free memory to allocate a file buffer.
.*
.mnote Error reading while copying data.
An error occured while reading the binary part of the input file.
.np
This is most likely due to a corrupted executable header. Run &e2bcmd. with the
&sw.h option and check the size reported. The size of the input file
must be at least ("Number of pages" - 1) * 512 + "Size mod 512". Omit
decrementing the number of pages if "Size mod 512" happens to equal zero.
.*
.mnote Error writing while copying data.
The output binary file can not be written to.
.np
Make sure the media has enough free space to hold the output file and is not
removed while writing to it.
.*
.mnote Error. %s has no valid executable header.
The signature (the first two bytes of the input file) does not match "MZ".
.np
&e2bcmd. can only use valid DOS executable files as input.
.*
.mnote Error allocating/reading reloc-table.
There is either not enough free memory to allocate a buffer for the relocations
(each relocation takes about 4 bytes) or there was an error while reading
from the input file.
.*
.mnote Error. Option "-l=<seg>" mandatory (there are relocations).
The executable file contains relocations. Therefore, &e2bcmd. needs to know the
segment the binary output file is supposed to reside at.
.np
Either provide a segment as an argument to the &sw.l option or rewrite your
executable file to not contain any relocations.
.*
.mnote Error: Binary part exceeds 64 KBytes.
The binary part of the input file is larger than 64 KBytes.
.np
The restriction applies because the &sw.x option was not specified.
Check if the extended behaviour is suitable or rewrite the program to shorten
the binary part.
.*
.mnote Error: Stack segment defined.
The header defines an initial stack, i.e. ss:sp != 0x0000:0x0000.
.np
The restriction applies because the &sw.x option was not specified.
Check if the extended behaviour is suitable or rewrite the program to not have
a segment of class "stack".
.*
.mnote Error: CS:IP neither 0x0000:0x0000 nor 0x0000:0x0100.
The header defines an initial cs:ip not matching any of the two values.
.np
The restriction applies because the &sw.x option was not specified.
Check if the extended behaviour is suitable or rewrite the program to have a
different entry point (cf. &lnkname.
.mono "option start"
).
.*
.mnote Error: com-file must not have relocations.
Although the binary part is <= 64 KByte in length, there is no stack defined
and the cs:ip is 0x0000:0x0100, i.e. &e2bcmd. assumes you try to generate a
".COM" executable, there are relocations in the input file.
.np
".COM" files are not allowed to contain relocations. Either produce an ".EXE"
file instead or rewrite the program to avoid the need for relocations. In order
to do the latter, look for statements that refer to segments or groups such as
.mono mov ax, _TEXT
or
.mono mov ax, DGROUP.
.*
.endnote
.*
