.begnote $compact $setptnt 12
:DTHD.Compiler options:
:DDHD.Description:
.note 0
.ix 'options' '0'
(16-bit only)
assume 8088/8086 processor
.note 1
.ix 'options' '1'
(16-bit only)
assume 188/186 processor
.note 2
.ix 'options' '2'
(16-bit only)
assume 286 processor
.note 3
.ix 'options' '3'
assume 386 processor
.note 4
.ix 'options' '4'
assume 486 processor
.note 5
.ix 'options' '5'
assume Pentium processor
.note 6
.ix 'options' '6'
assume Pentium Pro processor
.note [NO]ALign
.ix 'options' 'align'
align COMMON segments
.note [NO]AUtomatic
.ix 'options' 'automatic'
all local variables on the stack
.note BD
.ix 'options' 'bd'
(32-bit only)
dynamic link library
.note BM
.ix 'options' 'bm'
(32-bit only)
multithread application
.note [NO]BOunds
.ix 'options' 'bounds'
generate subscript bounds checking code
.note BW
.ix 'options' 'bw'
(32-bit only)
default windowed application
.note [NO]CC
.ix 'options' 'cc'
carriage control recognition requested for output devices such as the
console
.note CHInese
.ix 'options' 'chinese'
Chinese character set
.note [NO]COde
.ix 'options' 'code'
constants in code segment
.note D1
.ix 'options' 'd1'
include line # debugging information
.note D2
.ix 'options' 'd2'
include full debugging information
:CMT. .note [NO]DB
:CMT. .ix 'options' 'db'
:CMT. generate browse information
.note [NO]DEBug
.ix 'options' 'debug'
perform run-time checking
.note DEFine=<macro>
.ix 'options' 'define'
define macro
.note [NO]DEPendency
.ix 'options' 'dependency'
generate file dependencies
.note [NO]DEScriptor
.ix 'options' 'descriptor'
pass character arguments using string descriptor
.note DIsk
.ix 'options' 'disk'
write listing file to disk
.note DT=<size>
.ix 'options' 'dt'
set data threshold
.note [NO]ERrorfile
.ix 'options' 'errorfile'
generate an error file
.note [NO]EXPlicit
.ix 'options' 'explicit'
declare type of all symbols
.note [NO]EXtensions
.ix 'options' 'extensions'
issue extension messages
.note [NO]EZ
.ix 'options' 'ez'
(32-bit only)
Easy OMF-386 object files
.note FO=<obj_default>
.ix 'options' 'fo'
set default object file name
.note [NO]FORmat
.ix 'options' 'format'
relax format type checking
.note FPC
.ix 'options' 'fpc'
generate calls to floating-point library
.note FPD
.ix 'options' 'fpd'
enable generation of Pentium FDIV bug check code
.note FPI
.ix 'options' 'fpi'
generate inline 80x87 instructions with emulation
.note FPI87
.ix 'options' 'fpi87'
generate inline 80x87 instructions
.note FPR
.ix 'options' 'fpr'
floating-point backward compatibility
.note FP2
.ix 'options' 'fp2'
generate inline 80x87 instructions
.note FP3
.ix 'options' 'fp3'
generate inline 80387 instructions
.note FP5
.ix 'options' 'fp5'
optimize floating-point for Pentium
.note FP6
.ix 'options' 'fp6'
optimize floating-point for Pentium Pro
.note [NO]FSfloats
.ix 'options' 'fsfloats'
FS not fixed
.note [NO]GSfloats
.ix 'options' 'gsfloats'
GS not fixed
.note HC
.ix 'options' 'hc'
Codeview debugging information
.note HD
.ix 'options' 'hd'
DWARF debugging information
.note HW
.ix 'options' 'hw'
&company debugging information
.note [NO]INCList
.ix 'options' 'inclist'
write content of INCLUDE files to listing
.note INCPath=[&dr4]path;[&dr4]path...
.ix 'options' 'incpath'
path for INCLUDE files
.note [NO]IPromote
.ix 'options' 'ipromote'
promote INTEGER*1 and INTEGER*2 arguments to INTEGER*4
.note Japanese
.ix 'options' 'japanese'
Japanese character set
.note KOrean
.ix 'options' 'korean'
Korean character set
.note [NO]LFwithff
.ix 'options' 'lfwithff'
LF with FF
.note [NO]LIBinfo
.ix 'options' 'libinfo'
include default library information in object file
.note [NO]LISt
.ix 'options' 'list'
generate a listing file
.note [NO]MAngle
.ix 'options' 'mangle'
mangle COMMON segment names
.note MC
.ix 'options' 'mc'
(32-bit only)
compact memory model
.note MF
.ix 'options' 'mf'
(32-bit only)
flat memory model
.note MH
.ix 'options' 'mh'
(16-bit only)
huge memory model
.note ML
.ix 'options' 'ml'
large memory model
.note MM
.ix 'options' 'mm'
medium memory model
.note MS
.ix 'options' 'ms'
(32-bit only)
small memory model
.note OB
.ix 'options' 'ob'
(32-bit only)
base pointer optimizations
.note OBP
.ix 'options' 'obp'
branch prediction
.note OC
.ix 'options' 'oc'
do not convert "call" followed by "ret" to "jmp"
.note OD
.ix 'options' 'od'
disable optimizations
.note ODO
.ix 'options' 'odo'
DO-variables do not overflow
.note OF
.ix 'options' 'of'
always generate a stack frame
.note OH
.ix 'options' 'oh'
enable repeated optimizations (longer compiles)
.note OI
.ix 'options' 'oi'
generate statement functions in-line
.note OK
.ix 'options' 'ok'
enable control flow prologues and epilogues
.note OL
.ix 'options' 'ol'
perform loop optimizations
.note OL+
.ix 'options' 'ol+'
perform loop optimizations with loop unrolling
.note OM
.ix 'options' 'om'
generate floating-point 80x87 math instructions in-line
.note ON
.ix 'options' 'on'
numeric optimizations
.note OP
.ix 'options' 'op'
precision optimizations
.note OR
.ix 'options' 'or'
instruction scheduling
.note OS
.ix 'options' 'os'
optimize for space
.note OT
.ix 'options' 'ot'
optimize for time
.note OX
.ix 'options' 'ox'
equivalent to OBP, ODO, OI, OK, OL, OM, OR, and OT (16-bit)
or        OB, OBP, ODO, OI, OK, OL, OM, OR, and OT (32-bit)
.note PRint
.ix 'options' 'print'
write listing file to printer
.note [NO]Quiet
.ix 'options' 'quiet'
operate quietly
.note [NO]Reference
.ix 'options' 'reference'
issue unreferenced warning
.note [NO]RESource
.ix 'options' 'resource'
messages in resource file
.note [NO]SAve
.ix 'options' 'save'
SAVE local variables
.note [NO]SC
.ix 'options' 'sc'
(32-bit only)
stack calling convention
.note [NO]SEpcomma
.ix 'options' 'sepcomma'
allow comma separator in formatted input
.note [NO]SG
.ix 'options' 'sg'
(32-bit only)
automatic stack growing
.note [NO]SHort
.ix 'options' 'short'
set default INTEGER/LOGICAL size to 2/1 bytes
.note [NO]SR
.ix 'options' 'sr'
save/restore segment registers
.note [NO]SSfloats
.ix 'options' 'ssfloats'
(16-bit only)
SS is not default data segment
.note [NO]STack
.ix 'options' 'stack'
generate stack checking code
.note [NO]SYntax
.ix 'options' 'syntax'
syntax check only
.note [NO]TErminal
.ix 'options' 'terminal'
messages to terminal
.note [NO]TRace
.ix 'options' 'trace'
generate code for run-time traceback
.note TYpe
.ix 'options' 'type'
write listing file to terminal
.note [NO]WArnings
.ix 'options' 'warnings'
issue warning messages
.note [NO]WILd
.ix 'options' 'wild'
relax wild branch checking
.note [NO]WIndows
.ix 'options' 'windows'
(16-bit only)
compile code for Windows
.note [NO]XFloat
.ix 'options' 'xfloat'
extend floating-point precision
.note [NO]XLine
.ix 'options' 'xline'
extend line length to 132
.endnote
