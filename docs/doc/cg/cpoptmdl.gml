:cmt. .*
:cmt. :OPT refid='SWm' name='m'.?
:cmt. .sy ?
:cmt. is one of
:cmt. .if '&machine' eq '8086' .do begin
:cmt. .sy s, m, c, l
:cmt. or
:cmt. .sy h.
:cmt. .do end
:cmt. .if '&machine' eq '80386' .do begin
:cmt. .sy f, s, m, c
:cmt. or
:cmt. .sy l.
:cmt. .do end
:cmt. A particular memory model is selected when generating object code.
:cmt. (Note: In several places in the text, a pair of underscore
:cmt. characters appears as __ which resembles a single, elongated
:cmt. underscore.)
:cmt. .*
:cmt. :OPTLIST.
.*
.if &e'&$SWmf eq 1 .do begin
:OPT refid='SWmf' name='mf'.
.ix 'options' 'mf'
(32-bit only)
The "flat" memory model (code and data up to 4 gigabytes) is selected.
.if '&target' ne 'QNX' .do begin
By default, the 32-bit compiler will select this memory model unless
the target system is Netware in which case "small" is selected.
.do end
The following macros will be predefined.
.illust begin
.if &version ge 107 .do begin
.ix 'macros' 'M_386FM'
.kwm M_386FM
.ix 'macros' '_M_386FM'
.kwm _M_386FM
.do end
.ix 'macros' '__FLAT__'
.kwm __FLAT__
.illust end
.*
:OPT refid='SWms' name='ms'.&optdag.
.ix 'options' 'ms'
The "small" memory model (small code, small data) is selected.
.if '&target' eq 'QNX' .do begin
By default, both 16-bit and 32-bit compilers will select this memory
model.
.do end
.el .do begin
By default, the 16-bit compiler will select this memory model.
When the target system is Netware, the 32-bit compiler will select
this memory model.
.do end
The following macros will be predefined.
.illust begin
.ix 'macros' 'M_I86SM'
.kwm M_I86SM
.if &version ge 107 .do begin
.kwm _M_I86SM
.kwm M_386SM
.kwm _M_386SM
.do end
.ix 'macros' '__SMALL__'
.kwm __SMALL__
.illust end
.*
:OPT refid='SWmm' name='mm'.&optdag.
.ix 'options' 'mm'
The "medium" memory model (big code, small data) is selected.
The following macros will be predefined.
.ix 'macros' 'M_I86MM'
.ix 'macros' '__MEDIUM__'
.illust begin
.kwm M_I86MM
.kwm __MEDIUM__
.illust end
.*
:OPT refid='SWmc' name='mc'.&optdag.
.ix 'options' 'mc'
The "compact" memory model (small code, big data) is selected.
The following macros will be predefined.
.ix 'macros' 'M_I86CM'
.ix 'macros' '__COMPACT__'
.illust begin
.kwm M_I86CM
.kwm _M_I86CM
.kwm __COMPACT__
.illust end
.*
:OPT refid='SWml' name='ml'.&optdag.
.ix 'options' 'ml'
The "large" memory model (big code, big data) is selected.
The following macros will be predefined.
.ix 'macros' 'M_I86LM'
.ix 'macros' '__LARGE__'
.illust begin
.kwm M_I86LM
.kwm __LARGE__
.illust end
.*
:OPT refid='SWmh' name='mh'.
.ix 'options' 'mh'
(16-bit only)
The "huge" memory model (big code, huge data) is selected.
The following macros will be predefined.
.ix 'macros' 'M_I86HM'
.ix 'macros' '__HUGE__'
.illust begin
.kwm M_I86HM
.kwm __HUGE__
.illust end
.*
:cmt. :eOPTLIST.
.np
Memory models are described in the
chapters entitled :HDREF refid='mdl86'. and :HDREF refid='mdl386'..
Other architectural aspects of the Intel 86 family such as pointer
size are discussed in the sections entitled :HDREF refid='cal86s'. in
the chapter entitled :HDREF refid='cal86' page=no.
or :HDREF refid='cal386s'. in the
chapter entitled :HDREF refid='cal386' page=no..
.do end
