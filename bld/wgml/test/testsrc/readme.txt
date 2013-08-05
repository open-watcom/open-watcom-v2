I. Directory "testlib"
    A. Test Device Description
    B. File Names
    C. Include Files
    D. Test Device Structure Mapping
II. Test Device Output
    A. What Is An "Instance"?
    B. Default Font Linkages
    C. Determining Word Length
III. Directory "testsrc"
    A. Using gendev Directly
    B. Using wmake
IV. The "Memory Exhausted" Error

I. Directory "testsrc"

This directory contains the source for the five test devices found in the
directory "testlib".

    A. Test Device Description

The five test devices are:

TEST01  -- a typical character-mode device, using NEWLINE blocks, and 
           containing both FINISH blocks to see if both are ever used by wgml 4.0
TEST02  -- TEST01 with ABSOLUTEADDRESS instead of NEWLINE and with the HLINE, 
           VLINE and DBOX blocks added; used to verify that, for devices with the
           same metrics, the text is assigned the same position on the page 
           whether ABSOLUTEADDRESS is defined or is not defined and the NEWLINE 
           blocks are used. It also has only the DOCUMENT FINISH block, showing
           that wgml 4.0 will use that block if no END FINISH block is present.
PSTEST  -- TEST02 with the "ps" prefix, used to identify the first set of PS 
           Augmentations found. This has the same character-mode metrics as 
           TEST01 and TEST02
PSSPEC  -- This is, essentially, the PS device using the test blocks instead of 
           the real ones. It was used to identify still more PS Augmentations. 
           It is useful in determining exactly how large each word is, as each 
           word is output separately.
XPSSPEC -- This is PSSSPEC with the names changed to start with "X". This can be
           used to verify that something being done by wgml 4.0 is not, in fact,
           specific to the PS device but rather to all devices defining the
           ABSOLUTEADDRESS block.

    B. File Names

This table gives the defined names and member names for the DEVICE and DRIVER 
blocks. The source file names are the same as the member names.

Device   | Member Name   | Defined Name
TEST01:  |               |
 device  | test01        | test01
 driver  | tdrv01        | testdrv01
TEST02:  |               |
 device  | test02        | test02
 driver  | tdrv02        | testdrv02
PSTEST:  |               |
 device  | pstest        | pstest
 driver  | pstdrv        | pstestdrv
PSSPEC:  |               |
 device  | psspec        | psspec
 driver  | pssdrv        | psspecdrv
XPSSPEC: |               |
 device  | xpsspec       | xpsspec
 driver  | xpssdrv       | xpsspecdrv

The document files produced will have these extensions (unless overridden by 
the OUTput option):

TEST01  -- tx1
TEST02  -- tx2
PSTEST  -- pst
PSSPEC  -- pss
XPSSPEC -- psx

Two sets of ten fonts each are provided, one set for use with PSSPEC and XPSSPEC 
(based on one of the PS device fonts) and the other set for use with TEST01, TEST02 
and PSTEST (based on the font MONO01):

Intended
Font Number  | Member Name   | Defined Name
TEST01, TEST02 and PSTEST:
 0           | tfon00        | tfon00
 1           | tfon01        | tfon01
 2           | tfon02        | tfon02
 3           | tfon03        | tfon03
 4           | tfon04        | tfon04
 5           | tfon05        | tfon05
 6           | tfon06        | tfon06
 7           | tfon07        | tfon07
 8           | tfon08        | tfon08
 9           | tfon09        | tfon09
PSSPEC:
 0           | pssfon00      | psspecfon00
 1           | pssfon01      | psspecfon01
 2           | pssfon02      | psspecfon02
 3           | pssfon03      | psspecfon03
 4           | pssfon04      | psspecfon04
 5           | pssfon05      | psspecfon05
 6           | pssfon06      | psspecfon06
 7           | pssfon07      | psspecfon07
 8           | pssfon08      | psspecfon08
 9           | pssfon09      | psspecfon09

    C. Include Files

For the device files:

DEV01.INC   -- The PAUSE blocks for all five test devices
DEV02.INC   -- The DEVICEFONT and DEFAULTFONT blocks for TEST01, TEST02, PSTEST
DEV02PS.INC -- The DEVICEFONT and DEFAULTFONT blocks for PSSPEC and XPSSPEC
DEV03.INC   -- The FONTPAUSE blocks for all five test devices
DEV04.INC   -- The BOX block for TEST01, TEST02, PSTEST
DEV04PS.INC -- The BOX block for PSSPEC and XPSSPEC
DEV05.INC   -- The USCORE block for all five test devices
DEV06.INC   -- The PAGESTART and PAGEOFFSET blocks for TEST01, TEST02, PSTEST
DEV06PS.INC -- The PAGESTART and PAGEOFFSET blocks for PSSPEC and XPSSPEC
DEV07.INC   -- INTRANS and OUTRANS blocks for TEST01, TEST02, PSTEST
DEV07PS.INC -- INTRANS and OUTRANS blocks for PSSPEC and XPSSPEC
INTRAN.TBL  -- A file included by every device in the repository and every device
               provided with the wgml 3.3 update

DEV07PS.INC includes DEV07.INC
DEV07.INC includes INTRAN.TBL

For the driver files:

DRV01.INC -- The INIT and DOCUMENT DEFINE blocks
DRV02.INC -- The remaining DRIVER blocks
DRV03.INC -- The PAGEADDRESS block for TEST01, TEST02, PSTEST
DRV04.INC -- ABSOLUTEADDRESS, HLINE, VLINE, and DBOX for all but TEST01

No include files are used with fonts; this allows the individual fonts to be
reconfigured as needed for testing.

    D. Test Device Structure Mapping

Blocks are shown in the order used in the Wiki. Where multiple instances are
allowed, all instances are in the file indicated. The fields with "--" 
indicate that that block is not defined at all for that device.
   
Block           | TEST01    | TEST02    | PSTEST    | PSSPEC    | XPSSPEC
Attributes      | test01    | test02    | pstest    | psspec    | xpsspec
PAUSE           | dev01     | dev01     | dev01     | dev01     | dev01
DEVICEFONT      | dev02     | dev02     | dev02     | dev02ps   | dev02ps
DEFAULTFONT     | dev02     | dev02     | dev02     | dev02ps   | dev02ps
FONTPAUSE       | dev03     | dev03     | dev03     | dev03     | dev03
BOX             | dev04     | dev04     | dev04     | dev04ps   | dev04ps
UNDERSCORE      | dev05     | dev05     | dev05     | dev05     | dev05
PAGESTART       | dev06     | dev06     | dev06     | dev06ps   | dev06ps
PAGEOFFSET      | dev06     | dev06     | dev06     | dev06ps   | dev06ps
INTRANS         | dev07     | dev07     | dev07     | dev07ps   | dev07ps
OUTTRANS        | dev07     | dev07     | dev07     | dev07ps   | dev07ps

Block           | TDRV01    | TDRV02    | PSTDRV    | PSSDRV    | XPSSDRV
Attributes      | tdrv01    | tdrv02    | pstdrv    | pssdrv    | xpssdrv
INIT (both)     | drv01     | drv01     | drv01     | drv01     | drv01
FINISH (doc)    | drv01     | drv01     | drv01     | drv01     | drv01
FINISH (end)    | tdrv01    | --        | --        | --        | --
NEWLINE         | tdrv01    | --        | --        | --        | --
NEWPAGE         | drv02     | drv02     | drv02     | drv02     | drv02
HTAB            | drv02     | drv02     | drv02     | drv02     | drv02
FONTSTYLE       | drv02     | drv02     | drv02     | drv02     | drv02
FONTSWITCH      | drv02     | drv02     | drv02     | drv02     | drv02
PAGEADDRESS     | drv03     | drv03     | drv03     | pssdrv    | xpssdrv
ABSOLUTEADDRESS | --        | drv04     | drv04     | drv04     | drv04
HLINE           | --        | drv04     | drv04     | drv04     | drv04
VLINE           | --        | drv04     | drv04     | drv04     | drv04
DBOX            | --        | drv04     | drv04     | drv04     | drv04

II. Test Device Output

The test devices were developed to test various aspects of wgml 4.0's output,
since the actual devices do not, in general, provide enough information.

Each block announces itself with a very explicit character string which begins 
and ends with an asterisk ("*") and appears on its own line. Thus, the first 
line of the output file will be:

*START INIT VALUE block*

which identifies, not just the START block, and not just the INIT START block
(as opposed to the DOCUMENT START block), but the first block inside the INIT 
START block, which happens to be a VALUE block (as opposed to a FONTVALUE 
block).

In these blocks, the values returned by various device functions then appear.
Thus, the second and third lines of the file will be:

Font Number: 0 Instance: 0
Pages: 0 Location: 0,0

because these values are all "0" at this point. The values shown are from 
device functions %font_number(), %pages() and (forming the Location) 
%x_address(), %y_address(). The "Instance" is discussed next.

    A. What Is An "Instance"?

The concept of "instance" was developed to solve a problem: how to identify
which NEWPAGE blocks were being interpreted for a device page and which for a
document page. Instances solve the problem by providing a general mechanism 
for coordinating screen output with the contents of the output file.

This is done by initializing a symbol to "0" and then incrementing it each 
time a FONTPAUSE block is interpreted. Since the FONTPAUSE block is 
interpreted after switching from the prior font to the current font, each 
successive value of instance marks every event occuring between font switches.
Aptly-placed highlighted phrases can then be used to determine which bit of
text appears on which page, and whether a given NEWPAGE block was done as part
of the same instance as the corresponding DEVICE_PAGE or a DOCUMENT_PAGE PAUSE 
block.

    B. Default Font Linkages

The DEVICEFONT block associates fontname, fontswitch, and fontpause by name; 
the DEFAULTFONT block associates fontname with the font number. FONTPAUSE and 
FONTSWITCH blocks are defined with types "pause00" through "pause09" and 
"switch00" through "switch09" and associated with the fonts so that (except for 
PSSPEC) the last two digits of the fontname, fontpause and fonstwitch in each 
DEVICEFONT are identical. Each DEFAULTFONT uses the fontname that whose last 
digit matches the value of font (that is, the font number) -- and each fontname 
so used occurs only once.

Indeed, the two sets of ten fonts were provided so that they may be used this 
way in testing (only the last two digits of the font name are shown):

*00 -- 05 in DEFAULTFONT blocks
*06 and 07 with the BOX and UNDERSCORE blocks
*08 and 09 with the FONT option

each with its own unique FONTPAUSE and FONTSWITCH block. Currently, UNDERSCORE 
does not specify a font, merely duplicating the default value; however, at one 
time it was tested with a fontname and a font number as well so that the results 
could be explored.

The result is that, unless the default linkage is changed by editing the source
files or by using the FONT command-line option, the last digit of the 
FONTPAUSE or FONTSWITCH name will be identical to the font number.

Six FONTSTYLE blocks are defined, one for each of 'plain', 'bold', 'uline',
'uscore', 'ulbold', and 'usbold'. These names correspond to the values used 
before the FONTSTYLE block was developed. Originally, the action of each was 
modeled on the standard multipass font styles discussed in the Wiki; however, 
in most cases, this is more confusing than it is helpful, and so they are all
now implemented as a single pass doing %textpass(), that is, outputting the text. 

    C. Determining Word Length

Consider the output which surrounds each bit of text:

Pages: 1 Location: 19,2
||is
*FONTSTYLE 'plain' Pass 1 LINEPROC ENDWORD block*
Font Number: 0 Instance: 1
Pages: 1 Location: 23,2

The first thing to point out is that the "|" characters appear because the 
space character is output-translated to "|". This makes the space characters
quite easy to see and to count. It also helps distinguish output which is
output-translated from output which is not.

In this case, of course, the word length can be computed by simply counting 
the number of non-space letters. But notice what happens when the total number
of characters output (4) is added to the location above the text ("19,2"): we
get the location below the text ("23,2"). 

This is the same item from a file produced for test device PSTEST:

Pages: 1 Location: 14,11
(is)|shwd|
*FONTSTYLE 'plain' Pass 1 LINEPROC ENDWORD block*
Font Number: 0 Instance: 1
Pages: 1 Location: 16,11

This shows the format used to output text to device PS: the text itself is 
enclosed in parentheses and it is followed by "sd" (or "shwd"). As the "|" 
characters show, the entire output is output-translated, and includes a space 
following the "sd". 

This also shows that the difference between the horizontal locations (16 - 14, 
or 2) equals the number of characters inside the parentheses, not the number of 
characters output to the file. The reason for this, of course, is that, for the
PS device at least, the output file is used to display just the text in the 
parentheses, and so it is the text in the parentheses which must be positioned.

Now consider the same bit of text from a file produced for test device PSSPEC:

Pages: 1 Location: 1791,10466
(is)|sd|
*FONTSTYLE 'plain' Pass 1 LINEPROC ENDWORD block*
Font Number: 0 Instance: 1
Pages: 1 Location: 1884,10466

The difference between between the horizontal locations (1884 - 1971, or 113)
is the length of the characters in parentheses. Since the metrics of PSSPEC 
match those of device PS, that is how wide "is" will be with the PS device 
(for the same font and metrics) as well. 

However, in some cases space characters are included in the parentheses as 
well:

Pages: 1 Location: 1412,10299
(|values.)|shwd|
*FONTSTYLE 'plain' Pass 1 LINEPROC ENDWORD block*
Font Number: 0 Instance: 1
Pages: 1 Location: 1839,10299

Here, the difference between the horizontal locations (1839 - 1412, or 427)
includes the width of a space character. Unless the FONT option is used to 
change the metrics, the devldchk research program can be invoked

devldchk psspec

to list the metrics of each DEFAULTFONT block. As currently defined, each font
has a "Space char width" of 35. Thus, the string "values." without the 
preceding space is 392.

The FONT option is not processed by devldchk and can affect the width of the
space character. If PSSPEC is being used to determine word length, the FONT 
option should not be used. The DEFAULTFONT blocks can be modified provided 
devldchk is then used to provide the correct value for the space character.

All of the PSSPEC-related fonts start with the same metrics; in particular, 
with the same WIDTH table. If detailed word width comparisons are used, 
modifying the WIDTH table of PSSFON01 and then comparing the same text in the
default font (that is, PSSFON00) and an HP1/eHP1 (that is, PSSFON01) phrase
would allow very precise control.

When justification is on, output for PSSPEC and XPSSPEC does not always produce 
the correct value for word width when the above computation is done. If the wgml
4.0 word width is an issue, then a line such as

.ty &'width(<text>,U)

will provide it. Note that, if <text> includes a comma, then it must be delimited.

III. Directory "testsrc"

Directory testsrc is actually intended not so much to be used as to illustrate
how the actual syslib used by the Open Watcom build process could be added to
the build system. Of course, actually doing this would require the recovery 
of the missing source files for some of the items in that directory, and a 
decision as to whether or not to move the source to a syssrc directory (which
appears to be standard wgml practice), and whether the WHELP device should be
moved to this library or left where it is, so this is only a demo of one part
of the puzzle.

    A. Using gendev Directly

To use gendev 4.1 directly, the source files must be accessible. The environment 
variable GMLINC is intended to be used for this purpose. The rest of this
discussion presumes that gendev 4.1 can find the source files; if it cannot, 
then it will not create the binary device library under any conditions.

The environment variable GMLLIB is checked by gendev 4.1. If there is none, 
then gendev 4.1 will simply create the binary device library in the directory 
in which it was invoked. If environment variable GMLLIB does exist but does not 
include the directory in which gendev 4.1 was invoked, this warning message 
results:

SN--082: Current disk location and library path do not match

However, gendev 4.1 will still create the bindary device library in the 
directory it is run in.

    B. Using wmake

The makefile provided has the usual facilities:

1. Invoking "wmake" by itself causes each binary file encoding a DEVICE, DRIVER,
or FONT block to be checked and recreated if the source is newer.
2. Invoking "wmake <name>" causes <name>.cop to be recreated, usually only if 
the source is newer. "wmake genall", in contrast, will always run gendev 4.1 
with the genall.pcd file.
3. Invoking "wmake clean" deletes all of the COP files.

After a "wmake clean", a simple "wmake" will cause each COP file to be 
recreated individually; "wmake genall" will cause all of them to be recreated
in one invocation of gendev 4.1. 

A brief note on genall.pcd: testing with an earlier version of the test 
devices showed that, if a device providing fewer DEFAULTFONT blocks is listed
in genall.pcd after one providing more DEFAULTFONT blocks, then gendev will 
create the larger number of DEFAULTFONT blocks for the device providing the 
smaller number, but the extras will have junk in them. wgml 4.0 will not, in
this case, use font 0 if one of the extras is used, but instead will attempt
to use the data provided by gendev 4.1, with disasterous results.

The point is that, if wgml 4.0 behaves really oddly after editing a test 
device, one thing to check is how many fonts gendev produced for it. The 
devldchk research program can be used for this purpose; if it shows more
DEFAULTFONT blocks than defined by the device, changing the order in which the
device files are processed in genall.pcd should solve the problem.

IV. The "Memory Exhausted" Error

These test devices are intended to be modified as needed to test various 
conditions. In some cases, this can result in wgml 4.0 reporting this error:

SY--001: Memory exhausted!!!!!!!!!!!

Now, there are memory limits: each individual block of device functions, when
compiled by gendev 4.1, must occupy less than 64KB, and all of the device 
functions in the DEVICE block together must occupy less than 64KB. However, 
these limits are quite generous, and this error is almost never caused by 
exceeding them.

Instead, this has to do with the phenomenon discussed in the Wiki in the 
Multiple Codeblocks section; as I write this, the URL is

http://www.openwatcom.org/index.php/Device_Functions#Multiple_CodeBlocks

but, even if the Wiki is reorganized, the section name, since it is quite apt,
should stay the same and so be findable by searching.

To briefly summarize the situation: in three situations, gendev 4.1 skips a 
byte in forming the output stream. wgml 4.0 corrects two of these skips. The
third causes the error.

This error can be diagnosed: when I was exploring the binary format, I 
"enhanced" the cfparse research program to diagnose and report these problems.

Invoking cfparse this way:

cfparse tdrv01.cop | more

will produce a display of FONTSTYLE block codeblocks (compiled sets of 
functions) intermixed with notices of this sort:

Fontstyle: ulbold
Parsing :FONTSTYLE block: CodeBlock 1 has a shifted designator
Values: 0C 04

The key here is "designator": only shifted designators cause the problem. The
output has four useful pieces of information:
1) A FONTSTYLE block is involved.
2) The name ("type") of the FONSTSTYLE block is "ulbold".
3) This is the second CodeBlock (the count is zero-based, just as it
is in a C array).
4) The second codeblock is type "04" -- but wgml 4.0 will read it as "0C".

It is by misreading the designator that wgml 4.0 concludes it is out of 
memory -- or takes the action that causes it to run out of memory, it is hard
to tell which.

The type code can be decoded using the Designators section located here:

http://www.openwatcom.org/index.php/Meta_Data#Designators_2

This lists the designators for more than just CodeBlocks. A CodeBlock 0x04 
encodes an ENDVALUE block which is not in a LINEPROC block. Searching on 
"codeblock" should produce a link to this section (among many others) 
even if it is moved.

Since the designator is the first byte of the CodeBlock, the solution is to
find the immediately preceding CodeBlock (in this case, CodeBlock 0) and 
modify it. For a FONSTYLE, examining the first two lines of the table of 
CodeBlocks following the above information:

designator = 5, cb05_flag = 0, lp_flag = 0, pass = 0

designator = 4, cb05_flag = 0, lp_flag = 0, pass = 0

and decoding designator "5" shows that this is the STARTVALUE block of the 
FONTSTYLE block (the one that is not in any LINEPROC). 

This particular STARTVALUE block begins with the line

%image("*FONTSTYLE 'ulbold' STARTVALUE block*")

changing this to 

%image("*FONTSTYLE 'ulbold' STARTVALUE block* ")

that is, putting an extra character in it, will shift the all of the other 
CodeBlocks by one character, which will remove the problem once gendev 4.1 is 
used to recreate the binary file. It may, of course, create another problem, 
so the use of cfparse must continue until no such messages appear. This should 
allow wgml 4.0 to use the device.

When modifying the include files and/or solving this problem the use of

gendev genall

is recommended because most of these errors will affect more than one of the
test devices. Once the problems are solved, each device should still be checked
individually since some of them have blocks containing device functions that 
other devices lack.

Depending on the situation, it may be necessary to add a space character to
a different line, so some flexibility is needed. These space characters will 
be emitted, and so may affect the output, but at least the device will work!

NOTE: cfparse will also report "shifted pass" and "shifted count"; these not 
only can be ignored (because wgml 4.0 will correct them) but should be ignored, 
since correcting them may simply produce the "shifted designator" problem, 
which wgml 4.0 cannot correct.
