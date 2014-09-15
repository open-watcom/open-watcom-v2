;
; Copyright (C) 1996-2006 by Narech K. All rights reserved.
;
; Redistribution  and  use  in source and  binary  forms, with or without
; modification,  are permitted provided that the following conditions are
; met:
;
; 1.  Redistributions  of  source code  must  retain  the above copyright
; notice, this list of conditions and the following disclaimer.
;
; 2.  Redistributions  in binary form  must reproduce the above copyright
; notice,  this  list of conditions and  the  following disclaimer in the
; documentation and/or other materials provided with the distribution.
;
; 3. The end-user documentation included with the redistribution, if any,
; must include the following acknowledgment:
;
; "This product uses DOS/32 Advanced DOS Extender technology."
;
; Alternately,  this acknowledgment may appear in the software itself, if
; and wherever such third-party acknowledgments normally appear.
;
; 4.  Products derived from this software  may not be called "DOS/32A" or
; "DOS/32 Advanced".
;
; THIS  SOFTWARE AND DOCUMENTATION IS PROVIDED  "AS IS" AND ANY EXPRESSED
; OR  IMPLIED  WARRANTIES,  INCLUDING, BUT  NOT  LIMITED  TO, THE IMPLIED
; WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED.  IN  NO  EVENT SHALL THE  AUTHORS  OR  COPYRIGHT HOLDERS BE
; LIABLE  FOR  ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF
; SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF  USE,  DATA,  OR  PROFITS; OR
; BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
; WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
; OTHERWISE)  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
; ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;


; table		   	string  /len/subroutine
;-----------------------------------------------
		Align 2
dfn_tab		dw	df1_str, 5, cfg_env_quiet
		dw	df2_str, 5, cfg_env_print
		dw	df3_str, 5, cfg_env_sound
		dw	df4_str, 6, cfg_env_extmem
		dw	df5_str, 6, cfg_env_dosbuf
		dw	df6_str, 7, cfg_env_test
		dw	df8_str, 7, cfg_env_restore
		dw	df9_str, 5, cfg_env_nullp
		dw	dfA_str, 7, cfg_env_verbose
		dw	dfB_str, 6, cfg_env_nowarn
		dw	dfC_str, 3, cfg_env_noc
		dw	-1


; table		errcode, errstatus, errtab
;------------------------------------------
		Align 2
g_errtab	db	00h,	0
		dw	errtab_00xx
		db	10h,	0
		dw	errtab_10xx
		db	20h,	0
		dw	errtab_20xx
		db	30h,	0
		dw	errtab_30xx
		db	40h,	0
		dw	errtab_40xx
		db	60h,	2
		dw	errtab_60xx
		db	80h,	0
		dw	errtab_80xx
		db	90h,	1
		dw	errtab_90xx
		dw	-1

h_errtab	dw	errmsg1, errmsg2, errmsg3

errtab_00xx	dw	d_err0,	d_err1,	d_err2,	d_err3, d_err4
		dw	d_err5,	d_err6,	d_err7
errtab_10xx	dw	n_msg,	m_err1,	m_err2
errtab_20xx	dw	e_err0,	e_err1,	e_err2
errtab_30xx	dw	n_msg,	a_err1, a_err2, a_err3,	a_err4, a_err2, a_err4
errtab_40xx	dw	n_msg,	l_err1,	l_err2,	l_err3, l_err4,	l_err5,	l_err6,	l_err7
errtab_60xx	dw	r_msg80,n_msg,	r_msg81,r_msg82,r_msg83,r_msg84
errtab_80xx	dw	n_msg,	x_err1,	x_err2, x_err3
errtab_90xx	dw	n_msg,	w_msg1,	w_msg2,	w_msg3,	w_msg4, w_msg5, w_msg6

i_msg		dw	i_msg00, i_msg01, i_msg02, i_msg03
		dw	i_msg04, i_msg05, i_msg06, i_msg07
		dw	i_msg08, i_msg09, i_msg0A, i_msg0B
		dw	i_msg0C, i_msg0D, i_msg0E, n_msg






;=============================================================================
		Align 4
_misc_byte	db 3		; DOS/32A misc. bits			; +00
_misc_byte2	db 7							; +01
_version	dw 0		; version				; +02
_cpu_type	db 0		; CPU type:	80x86			; +04
_sys_type	db 0		; system type:	raw/XMS/VCPI/DPMI	; +05
_sys_misc	dw 0		; system misc. bits			; +06
_buf_size	dw 0		; protected mode buffer size		; +08

; _sys_misc map:
;----------------
;	bit  0:	0=no stub32c present, 1=program started from stub32c
;	bit  8:	0=no CTRL-C, 1=CTRL-C detected (run-time)


_pic_mask	dw 0		; PIC mask
_err_code	dw 0
_acc_rights	dw 0
_mus_size	dd 0
_mus_backoff	dw 0
_mus_backseg	dw 0
_mus_off	dd 0
_mus_sel	dd 0
_mus_data	db 0,0

_seg_ds		dw 0		; real mode segment storage
_seg_es		dw 0
_seg_ss		dw 0
_seg_env	dw 0
_seg_buf	dw 0
_seg_dta	dw 0
_seg_mus	dw 0
_seg_id32	dw 0
_seg_kernel	dw 0

_sel_cs		dw 0		; protected mode selector storage
_sel_ds		dw 0
_sel_es		dw 0
_sel_esp	dd 0
_sel_ss		dw 0
_sel_zero	dw 0		; ZERO_32	Data selector
_sel_env	dw 0		; default environment selector
_process_id	dw 0

_sel32_cs	dw 0,0		; CODE_32	Code selector
_sel32_ss	dw 0,0		; DATA_32	Data selector

_membase	dw 0		; memory base after stk+int+buf
_lowmembuf	dw 0		; size of DOS INT 21h buffer
_lobufzero	dd 0
_lobufbase	dd 0
_lobufsize	dd 0
_seg_ds_base	dd 0		; 32bit base of DS (CS) segment
_dta_sel	dw 0
_dta_off	dd 0
_app_dta_sel	dw 0
_app_dta_off	dd 0

;-----------------------------------------------------------------------------
_int10_ip	dd 0		; default INT 10h handler
_int10_cs	dd 0
_int21_ip	dd 0		; default INT 21h handler
_int21_cs	dd 0
_int23_ip	dd 0		; default INT 23h handler
_int23_cs	dd 0
_int33_ip	dd 0		; default INT 33h handler
_int33_cs	dd 0
_exc_tab	dd 16 * 2 dup(0)


;-----------------------------------------------------------------------------
_exec_handle		equ @area1_dw+00h		;dw 0
_exec_start		equ @area1_dd+04h		;dd 0
_app_tmp_addr1		equ @area1_dd+08h		;dd 0
_app_tmp_addr2		equ @area1_dd+0Ch		;dd 0

_app_num_objects	equ @area1_dd+10h		;dd 0
_app_off_objects	equ @area1_dd+14h		;dd 0

_app_off_objpagetab	equ @area1_dd+18h		;dd 0
_app_off_fixpagetab	equ @area1_dd+1Ch		;dd 0
_app_off_fixrectab	equ @area1_dd+20h		;dd 0
_app_off_datapages	equ @area1_dd+24h		;dd 0
_app_off_pageshift	equ @area1_dd+28h		;dd 0

_app_eip_object		equ @area1_dd+2Ch		;dd 0
_app_esp_object		equ @area1_dd+30h		;dd 0
_app_eip		equ @area1_dd+34h		;dd 0
_app_esp		equ @area1_dd+38h		;dd 0
_unreloc_eip		equ @area1_dd+3Ch		;dd 0
_unreloc_esp		equ @area1_dd+40h		;dd 0
_app_siz_fixrecstab	equ @area1_dd+44h		;dd 0
_app_buf_fixrecstab	equ @area1_dd+48h		;dd 0
_app_siz_lastpage	equ @area1_dd+4Ch		;dd 0

_int_number		equ @area1_db+50h		;db 0,0
_int_temp		equ @area1_dw+52h		;dw 0
_int_errcode		equ @area1_dd+54h		;dd 0
_int_esp		equ @area1_dd+58h		;dd 0
_int_ss			equ @area1_dd+5Ch		;dw 0
_int_erravail		equ @area1_db+5Eh		;db 0

_app_type		equ @area1_db+60h		;db 0
_app_load		equ @area1_db+61h		;db 0

_mus_esp		equ @area1_dd+64h		;dd 0
_mus_ss			equ @area1_dw+68h		;dw 0

; machine state
ms_reg_cr0		equ @area1_dd+ 80h		;dd 0
ms_reg_cr2		equ @area1_dd+ 84h		;dd 0
ms_reg_cr3		equ @area1_dd+ 88h		;dd 0
ms_reg_dr6		equ @area1_dd+ 8Ch		;dd 0
ms_eflags		equ @area1_dd+ 90h		;dd 0
ms_ec			equ @area1_dd+ 94h		;dd 0
ms_frame_esp		equ @area1_dd+0A0h		;dd 0	; exception handler's stack frame: ESP
ms_frame_ss		equ @area1_dw+0A4h		;dw 0	; exception handler's stack frame: SS
ms_reg_ds		equ @area1_dw+0A8h		;dw 0
ms_int			equ @area1_dw+0ACh		;dw 0	; INT #

_app_buf_allocsel	equ @area2_dw+00h		;dw 0
_app_buf_allocbase	equ @area2_dd+APP_MAXOBJECTS*2	;dd APP_MAXOBJECTS*2 dup(00h)


;-----------------------------------------------------------------------------
n_msg	db 0
cpr_msg	db 'DOS/32A -- DOS Extender '
If EXEC_TYPE eq 2
	db 'BETA '
EndIf
	db 'version '
ver_msg	db 'x.x.x',cr
	db 'Copyright (C) 1996-2006 by Narech K.',cr
cpr_end	label byte

errmsg1	db 'DOS/32A fatal (%w): ',0
errmsg2	db 'DOS/32A warning (%w): ',0
errmsg3	db 'DOS/32A run-time (%w): ',0

;-----------------------------------------------------------------------------
dos_str	db 'DOS32A='
df1_str	db 'QUIET'		; print =OFF, sound =OFF		--
df2_str	db 'PRINT'		; print (off=only errors reported)	on/off
df3_str	db 'SOUND'		; sound on error			on/off
df4_str	db 'EXTMEM'		; max Extended memory to alloc		(KB)
df5_str	db 'DOSBUF'		; lowmembuf, DOS INT 21h buffer		(KB)
df6_str	db 'DPMITST'		; test DPMI/VCPI or VCPI/DPMI		on/off
df8_str	db 'RESTORE'		; restore int vector table on exit	on/off
df9_str	db 'NULLP'		; install Null-Pointer protection	on/off
dfA_str	db 'VERBOSE'		; use verbose mode			on/off
dfB_str	db 'NOWARN'		; disable particular warning		(NUM)
dfC_str db 'NOC'		; disable copyright banner		--

	Align 4
@area1_db label byte
@area1_dw label word
@area1_dd label dword


; INIT errors		00xx
;=============================================================================
d_err0	db 'this program requires DOS 4.0 or higher'			,0
d_err1	db '80386 processor or better required to run protected mode'	,0
d_err2	db 'system software does not follow VCPI/DPMI specifications'	,0
d_err3	db 'present DPMI host does not support 32bit applications'	,0
d_err4	db 'incompatible VCPI PIC mappings'				,0
d_err5	db 'could not enter 32bit protected mode'			,0
d_err6	db 'could not allocate system selectors'			,0
d_err7	db 'could not enable A20 line'					,0


; MEMORY errors		10xx
;=============================================================================
m_err1	db 'not enough DOS memory, additional %dKB needed'		,0
m_err2	db 'DOS reported insufficient memory'				,0


; EXEC errors		20xx
;=============================================================================
e_err0	db 'invalid environment'					,0
e_err1	db 'could not open exec file "%s"'				,0
e_err2	db 'error in exec file "%s"'					,0


; APPLICATION errors	30xx
;=============================================================================
a_err1	db 'could not open application file "%s"'			,0
a_err2	db 'error in application file "%s"'				,0
a_err3	db 'file "%s" does not contain any valid exec format'		,0
a_err4	db 'exec format not supported in file "%s"'			,0


; LOADER errors		40xx
;=============================================================================
l_err1	db 'too many objects in application exec "%s"'			,0
l_err2	db 'not enough DOS memory to load application exec "%s"'	,0
l_err3	db 'not enough extended memory to load application exec "%s"'	,0
l_err4	db 'not enough extended memory to load fixups for exec "%s"'	,0
l_err5	db 'unrecognized fixup data in application exec "%s"'		,0
l_err6	db '16bit fixup overflow in application exec "%s"'		,0
l_err7	db 'not enough DOS Transfer Buffer space to load LC-exec "%s"'	,0


; MISC. errors		80xx
;=============================================================================
x_err1	db 'syntax is DOS32A <execname.xxx>'				,0
x_err2	db 'DOS reported an error (#%wh)'				,0
x_err3	db 'DPMI host reported an error (#%wh)'				,0


; WARNINGS		90xx
;=============================================================================
w_msg1	db 'no extended memory has been allocated'			,0
w_msg2	db 'PICs have been relocated to INT %bh, INT %bh'		,0
w_msg3	db 'real mode interrupt vector has been modified: INT %bh'	,0
w_msg4	db 'mouse initialization failed'				,0
w_msg5	db 'object #%d contains no data or code'			,0
w_msg6	db 'incompatible version of DOS/32A already running'		,0


; RUN-TIME errors	60xx
;=============================================================================
r_msg80	db 'unknown error code (#%bh)'					,0
r_msg81	db 'out of real-mode virtual stacks'				,0
r_msg82	db 'out of protected-mode virtual stacks'			,0
r_msg83	db 'extended memory blocks have been corrupted (#%l)'		,0
r_msg84	db 'DOS/4G API calls not supported'				,0



; Verbose messages
;=============================================================================
v_msg01	db 'Processor: %d, System: %s, Memory: DOS=%dKB, DPMI=%d%s',cre
v_msg02	db 'NONE',0, 'XMS',0,0, 'VCPI',0, 'DPMI',0
v_msg03	db 'LE',0,'LX',0,'LC',0,'PE',0
v_msg04	db 'KB',0
v_msg05	db 'MB',0
v_msg10	db 'Loading program "%s", %s-style',cre
v_msg11	db 'Object #%d loaded at %l, V/Psize: %l/%l, Flags=%w, Sel=%w',cre
v_msg12	db 'Startup CS:EIP=%w:%l, SS:ESP=%w:%l, %s EIP=%d:%l',cr
	db 'Memory left: DOS=%dKB, DPMI=%d%s. PSP_Sel=%w, Env_Sel=%w, Env_Seg=%w',cre


;=============================================================================
excmsgE	db 'exception',0
excmsgI	db 'unexpected interrupt',0
excmsg1	db '%s %bh',cr
	db 'Identity: %s at %w:%l',cre
excmsg2	db '%s crash address %d:%l',0
excmsg4	db ', error code pushed on stack %l',0

excmsgA	db 'Linear',0
excmsgB	db 'Unrelocated',0
excmsgC	db 'Module name: "%s", ProcessId=%w',cre

nulmsg0	db 'Null-pointer protection at %w:%l',cre

dbgmsg1	db 'EFLAGS = %l [%l.%l]  ',0
dbgmsg2	db 'CS:[EIP] = %b %b %b %b %b %b %b %b %b %b',0
dbgmsg4	db 'EAX = %l    ESI = %l       DR6 = %l    SS:[ESP+00] = %l',cre
dbgmsg5	db 'EBX = %l    EDI = %l       CR0 = %l    SS:[ESP+04] = %l',cre
dbgmsg6 db 'ECX = %l    EBP = %l       CR2 = %l    SS:[ESP+08] = %l',cre
dbgmsg7	db 'EDX = %l    ESP = %l       CR3 = %l    SS:[ESP+0C] = %l',cre
dbgmsgA	db 'Invalid selector',0
dbgmsgB	db 'NULL selector',0

selmsg1	db 'CS: = %w  ',0
selmsg2	db 'DS: = %w  ',0
selmsg3	db 'ES: = %w  ',0
selmsg4	db 'SS: = %w  ',0
selmsg5	db 'FS: = %w  ',0
selmsg6	db 'GS: = %w  ',0
selmsg0	db 'Base=%l Limit=%l Gr=%s Seg=%s/%dbit Type=%d Acc=%w',cre
selmsg9	db 'BYTE',0,'PAGE',0
selmsg8	db 'DATA',0,'CODE',0
selmsgX	db 'KERNEL=',0
selmsgY	db 'CLIENT=',0
selmsgZ	db 'APP/32=',0
selmsgW	db 'APP/??=',0

i_msg00	db 'integer division by zero'					,0
i_msg01	db 'hardware breakpoint'					,0
i_msg02	db 'NMI'							,0
i_msg03	db 'software breakpoint'					,0
i_msg04	db 'overflow check fault'					,0
i_msg05	db 'bounds check fault'						,0
i_msg06	db 'invalid opcode fault'					,0
i_msg07	db 'coprocessor not available'					,0
i_msg08	db 'double fault'						,0
i_msg09	db 'coprocessor segment overrun'				,0
i_msg0A	db 'invalid TSS fault'						,0
i_msg0B	db 'segment not present fault'					,0
i_msg0C	db 'stack fault'						,0
i_msg0D	db 'general protection fault'					,0
i_msg0E	db 'page fault'							,0

