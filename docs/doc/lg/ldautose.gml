.*
.*
.dirctv AUTOSECTION
.*
.np
The "AUTOSECTION" directive specifies that each object file that
appears in a subsequent "FILE" directive, up to the next "SECTION" or
"END" directive, will be assigned a different overlay.
The "AUTOSECTION" method of defining overlays is most useful when
using the dynamic overlay manager, selected by specifying the
"DYNAMIC" option.
For more information on the dynamic overlay manager,
see the section entitled :HDREF refid='useover'..
.np
The format of the "AUTOSECTION" directive (short form "AUTOS") is as
follows.
.mbigbox
    AUTOSECTION [INTO ovl_file]
.embigbox
.synote
.mnote INTO
specifies that all overlays are to be placed into a file, namely
.sy ovl_file.
If "INTO" (short form "IN") is not specified, the overlays are placed
in the executable file.
.mnote ovl_file
is the file specification for the name of an overlay file.
If no file extension is specified, a file extension of "ovl" is
assumed.
.esynote
.np
Placing overlays in separate files has a number of advantages.
For example, if your application was linked into one file, it may not
fit on a single diskette, making distribution of your application
difficult.
