.*
.*
.option SCREENNAME
.*
.np
The "SCREENNAME" option specifies the name of the first screen (the
screen that is automatically created when an NLM is loaded).
The format of the "SCREENNAME" option (short form "SCR") is as
follows.
.mbigbox
    OPTION SCREENNAME 'name'
.embigbox
.synote
.mnote name
specifies the screen name.
.esynote
.np
If the "SCREENNAME" option is not specified, the
.sy description
text specified in the "FORMAT" directive is used as the screen name.
