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
IEEE Rounding Mode: Normal rounding.
.br
IEEE Trap Mode: Imprecise, integer overflow disabled, inexact disabled.
.*
.if '&*1' ne 'cmpteq' .do begin
.if '&*1' ne 'cmptlt' .do begin
.if '&*1' ne 'cmptle' .do begin
.if '&*1' ne 'cmptun' .do begin
.*
.note &*1.c
.ix '&*1.c'
IEEE Rounding Mode: Chopped.
.*
.if '&*1' ne 'cvttq' .do begin
.*
.note &*1.m
IEEE Rounding Mode: Minus infinity.
.note &*1.d
.ix '&*1.d'
IEEE Rounding Mode: Plus infinity (ensure that the "DYN" field of the
FPCR is 11).
.if '&*1' ne 'cvtqs' .do begin
.if '&*1' ne 'cvtqt' .do begin
.note &*1.u
.ix '&*1.u'
IEEE Trap Mode: Imprecise, underflow enabled, inexact disabled.
.note &*1.uc
.ix '&*1.uc'
IEEE Trap Mode: Underflow enabled.
.br
IEEE Rounding Mode: Chopped.
.note &*1.um
.ix '&*1.um'
Combined meanings of &*1.u and &*1.m.
.note &*1.ud
.ix '&*1.ud'
Combined meanings of &*1.u and &*1.d.
.note &*1.su
.ix '&*1.su'
IEEE Trap Mode: Software, underflow enabled, inexact disabled.
.note &*1.suc
.ix '&*1.suc'
Combined meanings of &*1.s, &*1.u, and &*1.c.
.note &*1.sum
.ix '&*1.sum'
Combined meanings of &*1.s, &*1.u, and &*1.m.
.note &*1.sud
.ix '&*1.sud'
Combined meanings of &*1.s, &*1.u, and &*1.d.
.do end
.do end
.note &*1.sui
.ix '&*1.sui'
IEEE Trap Mode: Software, underflow enabled, inexact enabled.
.note &*1.suic
.ix '&*1.suic'
Combined meanings of &*1.sui and &*1.c.
.note &*1.suim
.ix '&*1.suim'
Combined meanings of &*1.sui and &*1.m.
.if '&*1' eq 'cvtqs' or '&*1' eq 'cvtqt' .do begin
.note &*1.suid
.ix '&*1.suid'
Combined meanings of &*1.sui and &*1.d.
.do end
.*
.do end
.*
.do end
.do end
.do end
.do end
.*
.if '&*1' eq 'cmpteq' .do begin
.note &*1.su
.ix '&*1.su'
IEEE Trap Mode: Software, underflow enabled, inexact disabled.
.do end
.if '&*1' eq 'cmptlt' .do begin
.note &*1.su
.ix '&*1.su'
IEEE Trap Mode: Software, underflow enabled, inexact disabled.
.do end
.if '&*1' eq 'cmptle' .do begin
.note &*1.su
.ix '&*1.su'
IEEE Trap Mode: Software, underflow enabled, inexact disabled.
.do end
.if '&*1' eq 'cmptun' .do begin
.note &*1.su
.ix '&*1.su'
IEEE Trap Mode: Software, underflow enabled, inexact disabled.
.do end
.if '&*1' eq 'cvttq' .do begin
.note &*1.v
.ix '&*1.v'
IEEE Convert-to-Integer Trap Mode: Imprecise, integer overflow
enabled, inexact disabled.
.note &*1.vc
.ix '&*1.vc'
Combined meanings of &*1.v and &*1.c.
.note &*1.sv
.ix '&*1.sv'
IEEE Convert-to-Integer Trap Mode: Software, integer overflow enabled,
inexact disabled.
.note &*1.svc
.ix '&*1.svc'
Combined meanings of &*1.s, &*1.v, and &*1.c.
.note &*1.svi
.ix '&*1.svi'
IEEE Convert-to-Integer Trap Mode: Software, integer overflow enabled,
inexact enabled.
.note &*1.svic
.ix '&*1.svic'
Combined meanings of &*1.svi and &*1.c.
.note &*1.d
.ix '&*1.d'
IEEE Rounding Mode: Plus infinity (ensure that the "DYN" field of the
FPCR is 11).
.note &*1.vd
.ix '&*1.vd'
Combined meanings of &*1.v and &*1.d.
.note &*1.svd
.ix '&*1.svd'
Combined meanings of &*1.sv and &*1.d.
.note &*1.svid
.ix '&*1.svid'
Combined meanings of &*1.svi and &*1.d.
.do end
.*
.endnote
.endnote
