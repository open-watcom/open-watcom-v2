.*
.begnote
.note Qualifiers
.*
The following table describes the qualifiers for the
.mono &*1.
instruction:
.begnote
.notehd1 Instruction
.notehd2 Qualifier Description
.*
.note no qualifier
VAX Rounding Mode: Normal rounding.
.br
VAX Trap Mode: Imprecise, underflow disabled.
.*
.if '&*1' ne 'cvtqf' .do begin
.if '&*1' ne 'cvtqg' .do begin
.if '&*1' ne 'cvtgq' .do begin
.*
.if '&*1' ne 'cmpgeq' .do begin
.if '&*1' ne 'cmpglt' .do begin
.if '&*1' ne 'cmpgle' .do begin
.*
.note &*1.c
.ix '&*1.c'
VAX Rounding Mode: Chopped.
.*
.*
.note &*1.u
.ix '&*1.u'
VAX Trap Mode: Imprecise, underflow enabled.
.note &*1.uc
.ix '&*1.uc'
Combined meanings of &*1.c and &*1.u.
.*
.do end
.do end
.do end
.*
.note &*1.s
.ix '&*1.u'
VAX Trap Mode: Software, underflow disabled.
.*
.if '&*1' ne 'cmpgeq' .do begin
.if '&*1' ne 'cmpglt' .do begin
.if '&*1' ne 'cmpgle' .do begin
.*
.note &*1.sc
.ix '&*1.sc'
Combined meanings of &*1.s and &*1.c.
.note &*1.su
.ix '&*1.su'
VAX Trap Mode: Software, underflow enabled.
.note &*1.suc
.ix '&*1.suc'
Combined meanings of &*1.s and &*1.uc.
.*
.do end
.do end
.do end
.*
.do end
.do end
.do end
.*
.if '&*1' eq 'cvtqf' or '&*1' eq 'cvtqg' or '&*1' eq 'cvtgq' .do begin
.note &*1.c
.ix '&*1.c'
VAX Rounding Mode: Chopped.
.do end
.if '&*1' eq 'cvtgq' .do begin
.note &*1.v
.ix '&*1.v'
VAX Convert-to-Integer Trap Mode: Imprecise, integer overflow enabled.
.note &*1.vc
.ix '&*1.vc'
Combined meanings of &*1.v and &*1.c.
.note &*1.s
.ix '&*1.s'
VAX Trap Mode: Software, underflow enabled.
.br
VAX Convert-to-Integer Trap Mode: Software, integer overflow disabled,
.note &*1.sc
.ix '&*1.sc'
Combined meanings of &*1.s and &*1.c.
.note &*1.sv
.ix '&*1.sv'
VAX Convert-to-Integer Trap Mode: Software, integer overflow enabled,
.note &*1.svc
.ix '&*1.svc'
Combined meanings of &*1.s, &*1.v and &*1.c.
.do end
.*
.endnote
.endnote
