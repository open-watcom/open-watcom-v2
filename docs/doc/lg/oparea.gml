.*
.*
.option AREA
.*
.np
The "AREA" option can be used to set the size of the memory pool in
which overlay sections are loaded by the dynamic overlay manager.
The format of the "AREA" option (short form "AR") is as follows.
.mbigbox
    OPTION AREA=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
The default size of the memory pool for a given application is
selected by the &lnkname and is equal to twice the size of the largest
overlay.
.np
It is also possible to add to the memory pool at run-time.
If you wish to add to the memory pool at run-time,
see the section entitled :HDREF refid='incover'..
