:segment C_FOR_PB & FOR_NT
IncludeFile idepbnt.cfg
:elsesegment C_FOR_PB
IncludeFile idepb.cfg
:elsesegment
:segment FOR_WIN
IncludeFile idewin.cfg
IncludeFile idew386.cfg
:elsesegment FOR_NT & FOR_AXP
IncludeFile ideaxp.cfg
IncludeFile idew32.cfg
:elsesegment FOR_NT
IncludeFile idew32.cfg
IncludeFile ideaxp.cfg
:elsesegment FOR_OS2
IncludeFile ideos232.cfg
IncludeFile ideos2.cfg
:elsesegment FOR_LINUX
IncludeFile idelnx.cfg
:endsegment
IncludeFile idedos.cfg
IncludeFile idedos32.cfg
IncludeFile iderdos.cfg
:segment !FOR_WIN
IncludeFile idewin.cfg
IncludeFile idew386.cfg
:endsegment
:segment !FOR_NT
IncludeFile idew32.cfg
IncludeFile ideaxp.cfg
:endsegment
:segment !FOR_OS2
IncludeFile ideos232.cfg
IncludeFile ideos2.cfg
:endsegment
IncludeFile idenlm.cfg
:segment !FOR_LINUX
IncludeFile idelnx.cfg
:endsegment
:endsegment

Project
:segment C_FOR_PB
  Editor "weditviw"  DLL
  Browse wbrg "wbrw"
  Help ide.hlp
:elsesegment FOR_OS2
  Editor "epmlink" DLL
  Browse wbrg wbrw
  Help ide.hlp
:elsesegment FOR_WIN
  Editor "$(%watcom)\\binw\\weditviw"  DLL
  Browse wbrg "$(%watcom)\\binw\\wbrw"
  Action &Zoom...
    Command "$(%watcom)\\binw\\wzoom"
    Hint "Invoke WZOOM utility"
  Action "&Heap Walker..."
    Command "$(%watcom)\\binw\\wheapwlk"
    Hint "Invoke WHEAPWLK utility"
  Action &Spy...
    Command "$(%watcom)\\binw\\wspy"
    Hint "Invoke WSPY utility"
  Action "&DDE Spy..."
    Command "$(%watcom)\\binw\\wddespy"
    Hint "Invoke WDDESPY utility"
  Help ide.hlp
:elsesegment FOR_NT
  Editor "weditviw" DLL
  Browse wbrg wbrw
  Action &Zoom...
    Command "wzoom"
    Hint "Invoke WZOOM utility"
  Action "&Heap Walker..."
    Command "wheapwlk"
    Hint "Invoke WHEAPWLK utility"
  Action &Spy...
    Command "wspy"
    Hint "Invoke WSPY utility"
  Action "&DDE Spy..."
    Command "wddespy"
    Hint "Invoke WDDESPY utility"
  Help ide.hlp
  HtmlHelp ide.chm
:endsegment

MsgLog
:segment FOR_OS2
  Help .c, wccerrs.hlp, 20000
  Help .cpp, wpperrs.hlp, 20000
  Help .for, wfcerrs.hlp, 20000
:elsesegment FOR_NT & FOR_AXP
  Help .c, wccerrs.hlp, 0
  Help .cpp, wpperrs.hlp, 1
  Help .for, wfcerrs.hlp, 0
:elsesegment C_FOR_PB | FOR_NT | FOR_WIN
  Help .c, wccerrs.hlp, 1
  Help .cpp, wpperrs.hlp, 1
  Help .for, wfcerrs.hlp, 0
  HtmlHelp .c, wccerrs.chm, 1
  HtmlHelp .cpp, wpperrs.chm, 1
  HtmlHelp .for, wfcerrs.chm, 0
:endsegment
