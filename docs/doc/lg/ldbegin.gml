.*
.*
.dirctv BEGIN
.*
.np
The "BEGIN" directive is used to define the start of an overlay area.
The "END" directive is used to define the end of an overlay area.
An overlay area is a piece of memory in which overlays are loaded.
All overlays defined between a "BEGIN" directive and
the corresponding "END" directive are loaded into that overlay area.
.keep
.np
The format of the "BEGIN" directive (short form "B") is as follows.
.mbigbox
    BEGIN
.embigbox
.np
The format of the "END" directive (short form "E") is as follows.
.mbigbox
    END
.embigbox
