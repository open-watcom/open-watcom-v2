
DOS/32 Advanced DOS Extender - Source Code - Readme
***************************************************



Recompiling the Source Code
===========================

DOS/32  Advanced  DOS  Extender  and its  tools  have  been created with
Borland  Turbo Assembler v4.0 and Watcom  C/C++ v10.6, and you will need
these  tools  (with the same or  later  version number) to recompile the
source code.

To setup Tasm to compile DOS/32A and its tools copy the files "tasm.cfg"
and "stddef.inc" located in .\SRC\SUTILS\MISC into Tasm's directory (the
directory  containing  "tasm.exe" and "tasm32.exe"  files). You may also
need  to edit the copied file "tasm.cfg" so that the line containing the
include  directory  (-i option) points  to the directory containing file
"stddef.inc".

In addition, certain source files may include "typedefs.h" and "debug.h"
C headers. These headers are provided in the .\H32 directory.

The  source  code for the "build.exe"  file  (used to auto-increment the
build number) is provided in the .\SRC\SUTILS\BUILD directory.

The  "make.bat"  files  are provided with  the  source code allow you to
perform a quick build of the DOS Extender and its tools.

Beware  that  when building a new version  of  DOS/32A, you may run into
problems  with certain tools. In particular, utilities such as SC and SB
may  "spawn()" each other when performing their duties, and if different
versions  of  the DOS Extender are found  in the path, the utilities may
fail  because  DOS/32A  does not  allow  memory sharing between diffrent
versions of the DOS Extender. In other words, if you are getting warning
9006:  "incompatible version of DOS/32A  already running" make sure that
there  are  no different versions of the  DOS  Extender in the PATH. For
more  information  on how DOS/32A and  its utilities search the PATH and
DOS32A  environment  variables see  the documentation (User's Reference,
section  5.0  - DOS/32 Advanced -  Environment  Variable), or the source
code itself.




Notes on DOS/32A
================

DOS/32A  consists  of  two parts: the  Kernel  and the Client, which are
compiled  separately  and  then  linked  together  to  produce the final
executable. The Kernel and the Client are completely independent of each
other   and  communicate  through  a  small  internal  API.  The  Client
implements the DOS Extending duties, such as INT 10h/21h/33h simulation,
initialization, application loading ets, and is basically a higher level
layer  towards the 32-bit application. The Kernel is responsible for the
low-level  stuff  and includes a DPMI host  which  is used by the Client
when no "external" DPMI hosts are present.

In  theory  it  is  possible to  completely  replace  the Kernel without
modifying  the  Client  (and  vice-versa) as  long  as  the internal API
between the two is left intact.

When  certain parts of the DOS  Extender have performed their duties and
are  no  longer  needed, they will be  reused  to save some memory space
(this   applies   to   both  Kernel   and   Client).  For  example  when
initialization  routines  have run, the DOS  Extender will use the space
occupied  by  those routines to store  internal variables later on. This
means  that  when modifying _any_ part of  the  DOS Extender you must be
aware  of  this  and  be  extra careful  (YOU  MUST  KNOW  WHAT  YOU ARE
DOING!!!).  Study  the  layout  of  the  source  code  carefully  before
attempting  to  simply move some routines to  one place or another as it
could  lead  to  unpredictable  results. Use  the  produced  by the Tasm
listing files which outline the complete structure of the produced code,
byte-for-byte, address-for-address.

Some hints on which parts are reused:
-------------------------------------
In Kernel look for labels named:
@area1_db
@area1_dw
@area1_dd

@callback_data

In Client look for labels named:
@area1_db
@area1_dw
@area1_dd

@area2_db
@area2_dw
@area2_dd


The  Client code will also use its Stack space quite heavily to allocate
local variables, for example when loading the 32-bit application.


As  a  conclusion, I would not advice  simply  hacking in stuff when you
wish to modify the DOS Extender's code to add or remove certain features
as  it  may lead to you  breaking DOS/32A completely. Instead allocate a
good  amount of time to study the  code and understand what is going on,
and log every change you make in a text file, as shown in "changes.txt".
If you have access to SourceSafe of CVS, use it!




Notes on SB
===========

The  source code of the SB (SUNSYS Bind Utility) is pretty much straight
forward and self explanatory. There are no tricks or magic.




Notes on SC
===========

SUNSYS  Compress Utility is basically a convertion tool that can read an
LE/LX-style  executable  and  output an  LC-style  executable which is a
propiertary  file format of DOS/32A not  supported by Watcom linker. The
compression of the executable is optional (but is enmabled by default).

An  undocumented  feature of SC is that  it  can append an OEM (Original
Equipment  Manufacturer)  information  to files  it  outputs, and can be
later  viewed by SB or SVER utilities. The information is expected to be
in  ASCII  format  and can be useful  when  you wish to identify what an
executable  is  actually  used  for (think  of  it  as a VS_VERSION_INFO
resource  block  which  can  be  embedded  into  Windows applications to
identify   the  vendor,  description,  copyright  etc).  To  apply  this
information  to  executable(s) simply place  a file named "oemtitle.inf"
containing  whatever you want in ASCII format into the same directory as
the  executable  file  that  is  to be  processed  by  SC.  The  size of
"oemtitle.inf"  must  not exceed 512 bytes.  It is prefectly possible to
modify  SC  in  such a way  that  it  could append larger "oemtitle.inf"
files, and even in binary form.




Notes on SD
===========

The SUNSYS Debugger is written entirely in assembler and is available as
a  standalone application as well as a static library that can be linked
into  application.  SD  is very powerful  if  you  know the x86 assembly
language. It can provide information about the complete state of the DOS
Extender,  and  even  allows you to  debug  its Client/Kernel code. SD's
dissassembler  supports  80486  +  80487  instruction  set,  and  can be
enhanced  to  support the new  instructions  available on new processors
(CMov, MMX, SSE, SSE2, 3DNow and whatnot).

Unfortunately since SD is heavily using DOS/32A's DPMI extensions, it is
not  compatible  with other DPMI servers  and  cannot be run for example
under  Windows. Note that it is possible  to lift up this restriction by
removing all the calls to ADPMI extensions.




Notes on SS
===========

No trickery here either. Everything should be fairly selfexplanatory.




Notes on PCTEST
===============

PCTEST  is  a  funny little devil.  Basically,  it  is very difficult to
precisely measure the speed of mode switching. It does not help that all
the  measuring  is done by using the  PIT (PC Timer Hardware), it really
ought  to  be done with "rdtsc" when  possible. However, when run on the
same  machine with different DOS Extenders several times, one should get
a fairly good picture of the relative performance.



[END OF FILE]
