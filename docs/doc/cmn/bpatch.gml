.chap The &patchname
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'patches'
.ix 'bugs'
The &patchname is a utility program which may be used to apply patches
or bug fixes to &company's compilers and its associated tools.
As problems are reported and fixed, patches are created and made
available on &company's BBS, &company's FTP site, or CompuServe for
users to download and apply to their copy of the tools.
.*
.section Applying a Patch
.*
.np
The format of the &patchcmdup command line is:
.ix '&patchcmdup' 'command line format'
.ix 'command line format' '&patchcmdup'
.mbigbox
&patchcmdup [options] patch_file
.embigbox
.np
The square brackets [ ] denote items which are optional.
.synote
.mnote options
is a list of valid &patchname options, each preceded by a dash
("&minus.").
Options may be specified in any order.
The possible options are:
.begnote
.note -p
Do not prompt for confirmation
.note -b
Do not create a .BAK file
.note -q
Print current patch level of file
.endnote
.mnote patch_file
is the file specification for a patch file provided by &company..
.esynote
.np
Suppose a patch file called "&lnkcmd..a" is supplied by &company to
fix a bug in the file "&lnkcmdup.&exc".
The patch may be applied by typing the command:
.millust begin
&patchcmd &lnkcmd..a
.millust end
.pc
The &patchname locates the file
.if '&target' eq 'QNX' .do begin
.fi //0/bin/&lnkcmd.&exc
.do end
.el .do begin
.fi C:&pathnamup\BINW\&lnkcmdup.&exc
.do end
using the
.ev PATH
environment variable.
The actual name of the executable file is extracted from the file
.fi &lnkcmd..a.
It then verifies that the file to be patched is the correct one by
comparing the size of the file to be patched to the expected size.
If the file sizes match, the program responds with:
.millust begin
.if '&target' eq 'QNX' .do begin
Ok to modify '//0/bin/&lnkcmd.&exc'? [y|n]
.do end
.el .do begin
Ok to modify 'C:&pathnamup\BINW\&lnkcmdup.&exc'? [y|n]
.do end
.millust end
.pc
If you respond with "yes", &patchcmdup will modify the indicated file.
If you respond with "no", &patchcmdup aborts.
Once the patch has been applied the resulting file is verified.
First the file size is checked to make sure it matches the expected
file size.
If the file size matches, a check-sum is computed and compared to the
expected check-sum.
.autonote Notes:
.note
If an error message is issued during the patch process, the file that
you specified to be patched will remain unchanged.
.note
If a sequence of patch files exist, such as "&lnkcmd..a",
"&lnkcmd..b" and "&lnkcmd..c", the patches must be applied in
order.
That is, "&lnkcmd..a" must be applied first followed by
"&lnkcmd..b" and finally "&lnkcmd..c".
.endnote
.*
.section Diagnostic Messages
.*
.np
.ix '&patchcmdup' 'diagnostics'
.ix 'diagnostics' '&patchcmdup'
If the patch cannot be successfully applied, one of the following
error messages will be displayed.
.begnote $break
.note Usage: &patchcmdup. {-p} {-q} {-b} <file>
.bi -p = Do not prompt for confirmation
.br
.bi -b = Do not create a .BAK file
.br
.bi -q = Print current patch level of file
.br
The command line was entered with no arguments.
:cmt. .if '&target' ne 'QNX' .do begin
:cmt. .note Not enough memory - Use real mode version
:cmt. This message is issued when you are using the protected-mode version
:cmt. of &patchcmdup..
:cmt. The protected-mode version attempts to read the file you are trying to
:cmt. patch into memory.
:cmt. If there is not enough memory to do so, the real-mode version should
:cmt. be used.
:cmt. It will perform the patch in pieces.
:cmt. The real-mode version does not perform the patch as fast as the
:cmt. protected-mode version but will succeed where the protected-mode
:cmt. version fails.
:cmt. .do end
.*
.note File '%s' has not been patched
This message is issued when the "-q" option is used and the file has
not been patched.
.*
.note File '%s' has been patched to level '%s'
This message is issued when the "-q" option is used and the file has
been patched to the indicated level.
.*
.note File '%s' has already been patched to level '%s' - skipping
This message is issued when the file has already been patched to the
same level or higher.
.*
.note Command line may only contain one file name
More than one file name is specified on the command line.
.if '&target' ne 'QNX' .do begin
Make sure that "/" is not used as an option delimiter.
.do end
.*
.note Command line must specify a file name
No file name has been specified on the command line.
.*
.note '%s' is not a &company patch file
The patch file is not of the required format.
The required header information is not present.
.*
.note '%s' is not a valid &company patch file
The patch file is not of the required format.
The required header information is present but the remaining contents
of the file have been corrupted.
.*
.note '%s' is the wrong size (%lu1). Should be (%lu2)
The size of the file to be patched (%lu1) is not the same as the
expected size (%lu2).
.*
.note Cannot find '%s'
Cannot find the executable to be patched.
.*
.note Cannot open '%s'
An error occurred while trying to open the patch file, the file to be
patched or the resulting file.
.*
.note Cannot read '%s'
An input error occurred while reading the old version of the file
being patched.
.*
.note Cannot rename '%s' to '%s'
The file to be patched could not be renamed to the backup file name or
the resulting file could not be renamed to the name of the file that
was patched.
.*
.note Cannot write to '%s'
An output error occurred while writing to the new version of the file
to be patched.
.*
.note I/O error processing file '%s'
An error occurred while seeking in the specified file.
.*
.note No memory for %s
An attempt to allocate memory dynamically failed.
.*
.note Patch program aborted!
This message is issued if you answered no to the "OK to modify" prompt.
.*
.note Resulting file has wrong checksum (%lu) - Should be (%lu2)
The check-sum of the resulting file (%lu) does not match the expected
check-sum (%lu2).
This message is issued if you have patched the wrong version.
.*
.note Resulting file has wrong size (%lu1) - Should be (%lu2)
The size of the resulting file (%lu1) does not match the expected size
(%lu2).
This message is issued if you have patched the wrong version.
.endnote
