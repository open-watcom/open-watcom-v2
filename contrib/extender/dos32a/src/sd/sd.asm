;
; Copyright (C) 1996-2002 Supernar Systems, Ltd. All rights reserved.
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

		.486p
		.MODEL flat
; LOCALS
; NOJUMPS

include	stddef.inc
.DATA?
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

__screen	db 0C000h dup(?)	; allocate memory for user screen
__screen2	db 10000h dup(?)
__vgastatebuf	db 4096 dup(?)
__svgastatebuf	db 4096 dup(?)



.DATA
externdef __cputype:byte
externdef __fputype:byte
externdef __buffer:dword
externdef __exec_handle:word
externdef __exec_start:dword
externdef __free_mem:dword
externdef __base_mem:dword
externdef _old_int10off:dword
externdef _old_int10sel:dword
externdef _old_int21off:dword
externdef _old_int21sel:dword
externdef _old_int31off:dword
externdef _old_int31sel:dword
externdef _old_int33off:dword
externdef _old_int33sel:dword
externdef __idstring:byte
externdef __message0:byte
externdef __error0:byte
externdef __error1:byte
externdef __error2:byte
externdef __error3:byte
externdef __error4:byte
externdef __error5:byte
externdef __error6:byte
externdef __lerror1:byte
externdef __lerror2:byte
externdef __lerror3:byte
externdef __lerror4:byte
externdef __lerror5:byte
externdef __lerror6:byte
externdef __lerror7:byte
externdef __lerror8:byte
externdef _app_type:byte
externdef _app_type2:byte
externdef _app_tmp_addr1:dword
externdef _app_tmp_addr2:dword
externdef _app_num_objects:dword
externdef _app_off_objects:dword
externdef _app_off_objpagetab:dword
externdef _app_off_fixpagetab:dword
externdef _app_off_fixrectab:dword
externdef _app_off_datapages:dword
externdef _app_off_pageshift:dword
externdef _app_siz_fixrecstab:dword
externdef _app_siz_lastpage:dword
externdef _app_buf_fixrecstab:dword
externdef _app_eip_object:dword
externdef _app_esp_object:dword
externdef _app_eip:dword
externdef _sel32_cs:word
externdef _app_esp:dword
externdef _sel32_ss:word
externdef _unreloc_esp:dword
externdef _obj_selector:word
externdef _obj_address:dword
externdef _obj_sel_ptr:dword
externdef _acc_rights:word
externdef __saved_esp:dword
externdef library_string:byte
externdef _registers:dword
externdef _eax:dword
externdef _ax:word
externdef _al:byte
externdef _ah:byte
externdef _ecx:dword
externdef _cx:word
externdef _cl:byte
externdef _ch:byte
externdef _edx:dword
externdef _dx:word
externdef _dl:byte
externdef _dh:byte
externdef _ebx:dword
externdef _bx:word
externdef _bl:byte
externdef _bh:byte
externdef _esp:dword
externdef _sp:word
externdef _ebp:dword
externdef _bp:word
externdef _esi:dword
externdef _si:word
externdef _edi:dword
externdef _di:word
externdef _cs:dword
externdef _ds:dword
externdef _es:dword
externdef _ss:dword
externdef _fs:dword
externdef _gs:dword
externdef _eip:dword
externdef _ip:word
externdef _efl:dword
externdef _fl:word
externdef _cr0:dword
externdef _cr2:dword
externdef _cr3:dword
externdef _old_registers:dword
externdef _old_eax:dword
externdef _old_ecx:dword
externdef _old_edx:dword
externdef _old_ebx:dword
externdef _old_esp:dword
externdef _old_ebp:dword
externdef _old_esi:dword
externdef _old_edi:dword
externdef _old_cs:dword
externdef _old_ds:dword
externdef _old_es:dword
externdef _old_ss:dword
externdef _old_fs:dword
externdef _old_gs:dword
externdef _old_eip:dword
externdef _old_efl:dword
externdef _old_cr0:dword
externdef _old_cr2:dword
externdef _old_cr3:dword
externdef _errorcode:dword
externdef _es_acc:word
externdef _cs_acc:word
externdef _ss_acc:word
externdef _ds_acc:word
externdef _fs_acc:word
externdef _gs_acc:word
externdef _es_base:dword
externdef _cs_base:dword
externdef _ss_base:dword
externdef _ds_base:dword
externdef _fs_base:dword
externdef _gs_base:dword
externdef _es_limit:dword
externdef _cs_limit:dword
externdef _ss_limit:dword
externdef _ds_limit:dword
externdef _fs_limit:dword
externdef _gs_limit:dword
externdef _dr0:dword
externdef _dr1:dword
externdef _dr2:dword
externdef _dr3:dword
externdef _dr7:dword
externdef _old_stack:dword
externdef _old_int9sel:dword
externdef _old_int9off:dword
externdef _old_intAsel:dword
externdef _old_intAoff:dword
externdef _old_intBsel:dword
externdef _old_intBoff:dword
externdef _old_intCsel:dword
externdef _old_intCoff:dword
externdef _old_intDsel:dword
externdef _old_intDoff:dword
externdef _old_intEsel:dword
externdef _old_intEoff:dword
externdef _old_picmask:word
externdef old_video_mode:word
externdef old_video_bank:byte
externdef old_video_pos:word
externdef screen_base:dword
externdef xpos:byte
externdef ypos:byte
externdef color:byte
externdef _keycode:byte
externdef _keychar:byte
externdef _keyshift:byte
externdef _keyhit:byte
externdef _keytab:byte
externdef kbxtbl:byte
externdef selbuf:byte
externdef __page_faults:dword
externdef _temp:dword
externdef _fname:byte
externdef _data_byte_tab:dword
externdef msg00:byte
externdef cpu_table:byte
externdef fpu_table:byte
externdef msg10:byte
externdef msg11:byte
externdef msg12:byte
externdef msg13:byte
externdef msg14:byte
externdef msg15:byte
externdef msg16:byte
externdef msg10f:byte
externdef msg11f:byte
externdef msg12f:byte
externdef msg13f:byte
externdef msg14f:byte
externdef msg15f:byte
externdef msg16f:byte
externdef msg17f:byte
externdef msg18f:byte
externdef msg19f:byte
externdef msg1Ef1:byte
externdef msg1Ef2:byte
externdef msg1Ef3:byte
externdef msg1Ef4:byte
externdef msg1Ef5:byte
externdef msg1Ef6:byte
externdef msg1Ef7:byte
externdef msg20:byte
externdef msg21:byte
externdef msg22:byte
externdef msg2es:byte
externdef msg2cs:byte
externdef msg2ss:byte
externdef msg2ds:byte
externdef msg2fs:byte
externdef msg2gs:byte
externdef msg2er:byte
externdef msg2sip:byte
externdef msg2sim:byte
externdef msg2dip:byte
externdef msg2dim:byte
externdef msg30:byte
externdef msg3spp:byte
externdef msg3spm:byte
externdef selerr1:byte
externdef selerr2:byte
externdef msg40:byte
externdef msg086:byte
externdef msg386:byte
externdef msgrdy:byte
externdef msgbusy:byte
externdef msg50:byte
externdef msg55:byte
externdef msg56:byte
externdef msg57:byte
externdef msg58:byte
externdef msg60:byte
externdef msg61:byte
externdef msg62:byte
externdef errortab:dword
externdef err0:byte
externdef err1:byte
externdef errterm:byte
externdef errkbrk:byte
externdef errcrt0:byte
externdef errcrt1:byte
externdef errcrt2:byte
externdef errcrt3:byte
externdef errcrt4:byte
externdef errcrt5:byte
externdef errec:byte
externdef err00:byte
externdef err01:byte
externdef err02:byte
externdef err03:byte
externdef err04:byte
externdef err05:byte
externdef err06:byte
externdef err07:byte
externdef err08:byte
externdef err09:byte
externdef err0A:byte
externdef err0B:byte
externdef err0C:byte
externdef err0D:byte
externdef err0E:byte
externdef err0F:byte
externdef hlpid:byte
externdef hlp00:byte
externdef hlp0A:byte
externdef hlp0B:byte
externdef hlp0C:byte
externdef hlp0D:byte
externdef hlp01:byte
externdef hlp10:byte
externdef hlp11:byte
externdef hlp12:byte
externdef hlp13:byte
externdef hlp14:byte
externdef hlp20:byte
externdef hlp21:byte
externdef hlp22:byte
externdef hlp25:byte
externdef hlp26:byte
externdef hlp30:byte
externdef hlp31:byte
externdef hlp32:byte
externdef hlp33:byte
externdef hlp34:byte
externdef hlp40:byte
externdef hlp41:byte
externdef hlp50:byte
externdef hlp51:byte
externdef hlp52:byte
externdef hlp53:byte
externdef hlp54:byte
externdef hlp55:byte
externdef hlp56:byte
externdef hlp60:byte
externdef hlp61:byte
externdef hlp62:byte
externdef hlp70:byte
externdef hlp71:byte
externdef hlp72:byte
externdef hlp80:byte
externdef hlp81:byte
externdef hlp82:byte
externdef hlp90:byte
externdef hlp91:byte
externdef hlp92:byte
externdef hlpA0:byte
externdef hlpA1:byte
externdef hlpA2:byte
externdef hlpA3:byte
externdef hlpA4:byte
externdef hlpA5:byte
externdef hlpA6:byte
externdef hlpA7:byte
externdef hlpA8:byte
externdef hlpB0:byte
externdef hlpB1:byte
externdef hlpB2:byte
externdef _acc_typeg:byte
externdef _acc_typeb:byte
externdef _pagenum:byte
externdef _gdt_string:byte
externdef _gdt_string2:byte
externdef _idt_string:byte
externdef _idt_string2:byte
externdef _emb_string:byte
externdef _emb_string2:byte
externdef _emb_str1:byte
externdef _emb_str2:byte
externdef _emb_str3:byte
externdef _emb_str4:byte
externdef _hist_str01:byte
externdef _hist_str02:byte
externdef _hist_str03:byte
externdef _hist_str04:byte
externdef _hist_str05:byte
externdef _hist_str06:byte
externdef _hist_str07:byte
externdef _hist_str10:byte
externdef _hist_str11:byte
externdef _hist_str12:byte
externdef _hist_str20:byte
externdef _hist_str30:byte
externdef _hist_str31:byte
externdef _hist_str32:byte
externdef _hist_str33:byte
externdef _hist_str34:byte
externdef __api_off:dword
externdef __api_sel:word
externdef _gdt_ptr:dword
externdef _idt_ptr:dword
externdef _emb_num:dword
externdef _emb_mem_ptr:dword
externdef _emb_mem_top:dword
externdef __history_bufsize:dword
externdef __history_bufbase:dword
externdef __history_bufhand:dword
externdef __history_bufptr:dword
externdef __history_bufnum:dword
externdef __history_current:dword
externdef __history_bufon:byte
externdef __history_intnum:byte
externdef __kernel_codesel:word
externdef __kernel_datasel:word
externdef __kernel_zerosel:word
externdef __kernel_codeseg:word
externdef __client_codesel:word
externdef __irq1_buffer_sel:dword
externdef __irq1_buffer_off:dword
externdef _dumpname:byte
externdef _cpu_xpos:dword
externdef _cpu_ypos:dword
externdef _cpu_xptr:dword
externdef _mem_xpos:dword
externdef _mem_ypos:dword
externdef _stk_xpos:dword
externdef _stk_ypos:dword
externdef _reg_xpos:dword
externdef _reg_ypos:dword
externdef _reg_ldat:dword
externdef _reg_xptr:dword
externdef _reg_xtab:byte
externdef _reg_ytab:byte
externdef _reg_ltab:byte
externdef _reg_rtab:dword
externdef _current_window:dword
externdef _windowtab:dword
externdef _cursortab:dword
externdef _addrbuffer:dword
externdef _fpuregs_on:byte
externdef _fpuregs_set:byte
externdef _assmmode_flag:byte
externdef _showmode_flag:byte
externdef _commmode_flag:byte
externdef _data_override:byte
externdef _data_addr:dword
externdef _code_addr:dword
externdef _stack_addr:dword
externdef _system_block:dword
externdef _vgastate_buf:dword
externdef _svgastate_buf:dword
externdef _unreloc_eip:dword
externdef _show_unreloc:byte
externdef _vgastate_set:byte
externdef _svgastate_set:byte
externdef _system_stack:dword
externdef _opsiz:byte
externdef _adrsiz:byte
externdef _prefix:byte
externdef _refaddr:dword
externdef _refshow:byte
externdef _jump_taken:byte
externdef _prefix_esp:byte
externdef __temp_addr:dword
externdef cmdadr:dword
externdef textadr:dword
externdef commadr:dword
externdef cmdbuf:byte
externdef textbuf:byte
externdef commbuf:byte
externdef hexbuf:byte
externdef hextab:byte
externdef _text_fabs:byte
externdef _text_fadd:byte
externdef _text_faddp:byte
externdef _text_fbld:byte
externdef _text_fbstp:byte
externdef _text_fchs:byte
externdef _text_fnclex:byte
externdef _text_fcom:byte
externdef _text_fcomp:byte
externdef _text_fcompp:byte
externdef _text_fcos:byte
externdef _text_fdecstp:byte
externdef _text_fdiv:byte
externdef _text_fdivp:byte
externdef _text_fdivr:byte
externdef _text_fdivrp:byte
externdef _text_ffree:byte
externdef _text_fiadd:byte
externdef _text_ficom:byte
externdef _text_ficomp:byte
externdef _text_fidiv:byte
externdef _text_fidivr:byte
externdef _text_fild:byte
externdef _text_fimul:byte
externdef _text_fincstp:byte
externdef _text_fninit:byte
externdef _text_fist:byte
externdef _text_fistp:byte
externdef _text_fisub:byte
externdef _text_fisubr:byte
externdef _text_fld:byte
externdef _text_fld1:byte
externdef _text_fldl2e:byte
externdef _text_fldl2t:byte
externdef _text_fldlg2:byte
externdef _text_fldln2:byte
externdef _text_fldpi:byte
externdef _text_fldz:byte
externdef _text_fldcw:byte
externdef _text_fldenv:byte
externdef _text_fmul:byte
externdef _text_fmulp:byte
externdef _text_fnop:byte
externdef _text_fpatan:byte
externdef _text_fprem:byte
externdef _text_fprem1:byte
externdef _text_fptan:byte
externdef _text_frndint:byte
externdef _text_frstor:byte
externdef _text_fnsave:byte
externdef _text_fscale:byte
externdef _text_fsetpm:byte
externdef _text_fsin:byte
externdef _text_fsincos:byte
externdef _text_fsqrt:byte
externdef _text_fst:byte
externdef _text_fstp:byte
externdef _text_fnstcw:byte
externdef _text_fnstenv:byte
externdef _text_fnstsw:byte
externdef _text_fsub:byte
externdef _text_fsubp:byte
externdef _text_fsubr:byte
externdef _text_fsubrp:byte
externdef _text_ftst:byte
externdef _text_fucom:byte
externdef _text_fucomp:byte
externdef _text_fucompp:byte
externdef _text_fwait:byte
externdef _text_fxam:byte
externdef _text_fxch:byte
externdef _text_fxtract:byte
externdef _text_fyl2x:byte
externdef _text_fyl2xp1:byte
externdef _text_f2xm1:byte
externdef _text_fpremi:byte
externdef _text_feni:byte
externdef _text_fdisi:byte
externdef _text_aaa:byte
externdef _text_aad:byte
externdef _text_aam:byte
externdef _text_aas:byte
externdef _text_adc:byte
externdef _text_add:byte
externdef _text_and:byte
externdef _text_arpl:byte
externdef _text_bound:byte
externdef _text_bsf:byte
externdef _text_bsr:byte
externdef _text_bswap:byte
externdef _text_bt:byte
externdef _text_btc:byte
externdef _text_btr:byte
externdef _text_bts:byte
externdef _text_call:byte
externdef _text_cbw:byte
externdef _text_cdq:byte
externdef _text_clc:byte
externdef _text_cld:byte
externdef _text_cli:byte
externdef _text_clts:byte
externdef _text_cmc:byte
externdef _text_cmp:byte
externdef _text_cmps:byte
externdef _text_cmpx:byte
externdef _text_cmpxchg8b:byte
externdef _text_cpuid:byte
externdef _text_cwd:byte
externdef _text_cwde:byte
externdef _text_daa:byte
externdef _text_das:byte
externdef _text_dec:byte
externdef _text_div:byte
externdef _text_emms:byte
externdef _text_enter:byte
externdef _text_hlt:byte
externdef _text_idiv:byte
externdef _text_imul:byte
externdef _text_in:byte
externdef _text_inc:byte
externdef _text_ins:byte
externdef _text_int:byte
externdef _text_int1:byte
externdef _text_int3:byte
externdef _text_into:byte
externdef _text_invd:byte
externdef _text_invlpg:byte
externdef _text_iret:byte
externdef _text_jmp:byte
externdef _text_jcxz:byte
externdef _text_jecxz:byte
externdef _text_lahf:byte
externdef _text_lar:byte
externdef _text_lea:byte
externdef _text_leave:byte
externdef _text_lgdt:byte
externdef _text_lidt:byte
externdef _text_lldt:byte
externdef _text_lmsw:byte
externdef _text_lock:byte
externdef _text_lods:byte
externdef _text_loop:byte
externdef _text_loope:byte
externdef _text_loopne:byte
externdef _text_les:byte
externdef _text_lds:byte
externdef _text_lss:byte
externdef _text_lfs:byte
externdef _text_lgs:byte
externdef _text_lsl:byte
externdef _text_ltr:byte
externdef _text_mov:byte
externdef _text_movs:byte
externdef _text_movsx:byte
externdef _text_movzx:byte
externdef _text_mul:byte
externdef _text_neg:byte
externdef _text_nop:byte
externdef _text_not:byte
externdef _text_or:byte
externdef _text_out:byte
externdef _text_outs:byte
externdef _text_pop:byte
externdef _text_popa:byte
externdef _text_popf:byte
externdef _text_push:byte
externdef _text_pusha:byte
externdef _text_pushf:byte
externdef _text_rcl:byte
externdef _text_rcr:byte
externdef _text_rdmsr:byte
externdef _text_rdtsc:byte
externdef _text_rep:byte
externdef _text_repne:byte
externdef _text_ret:byte
externdef _text_retf:byte
externdef _text_rol:byte
externdef _text_ror:byte
externdef _text_sahf:byte
externdef _text_sar:byte
externdef _text_sbb:byte
externdef _text_scas:byte
externdef _text_setalc:byte
externdef _text_sgdt:byte
externdef _text_shl:byte
externdef _text_shld:byte
externdef _text_shr:byte
externdef _text_shrd:byte
externdef _text_sidt:byte
externdef _text_sldt:byte
externdef _text_smsw:byte
externdef _text_stc:byte
externdef _text_std:byte
externdef _text_sti:byte
externdef _text_stos:byte
externdef _text_str:byte
externdef _text_sub:byte
externdef _text_test:byte
externdef _text_verr:byte
externdef _text_verw:byte
externdef _text_wait:byte
externdef _text_wbinvd:byte
externdef _text_wrmsr:byte
externdef _text_xadd:byte
externdef _text_xchg:byte
externdef _text_xlat:byte
externdef _text_xor:byte
externdef _text_esc:byte
externdef _text_aldx:byte
externdef _text_dxal:byte
externdef _text_axdx:byte
externdef _text_dxax:byte
externdef _text_eaxdx:byte
externdef _text_dxeax:byte
externdef _text_byte:byte
externdef _text_short:byte
externdef _text_far:byte
externdef _text_bptr:byte
externdef _text_wptr:byte
externdef _text_dptr:byte
externdef _text_long:byte
externdef _text_temp:byte
externdef err_addr_f2:byte
externdef err_addr_f:byte
externdef err_addr_d:byte
externdef err_addr_w:byte
externdef err_addr_b:byte
externdef _text_none:byte
externdef _fpureg:byte
externdef _fputab87:byte
externdef _regtab8:byte
externdef _regtab16:byte
externdef _regtab32:byte
externdef _regtab86:byte
externdef _segtab86:byte
externdef _crtab386:byte
externdef _drtab386:byte
externdef _trtab386:byte
externdef _jumptab86:byte
externdef _settab386:byte
externdef _esc0tab0:dword
externdef _esc0tab1:dword
externdef _esc1tab0:dword
externdef _esc1tab1:dword
externdef _esc2tab0:dword
externdef _esc2tab1:dword
externdef _esc3tab0:dword
externdef _esc3tab1:dword
externdef _esc4tab0:dword
externdef _esc4tab1:dword
externdef _esc5tab0:dword
externdef _esc5tab1:dword
externdef _esc6tab0:dword
externdef _esc6tab1:dword
externdef _esc7tab0:dword
externdef _esc7tab1:dword
externdef _grouptab:dword
externdef _group0Ftab:dword
externdef _type0tab:dword
externdef _type1tab:dword
externdef _type2tab:dword
externdef _type3tab:dword
externdef _type4tab:dword
externdef _type5tab:dword
externdef _type6tab:dword
externdef _type7tab:dword
externdef _type8tab:dword
externdef _type9tab:dword
externdef _typeAtab:dword
externdef _typeBtab:dword
externdef _typeCtab:dword
externdef _typeDtab:dword
externdef _typeEtab:dword
externdef _typeFtab:dword
externdef _type0tab0F:dword
externdef _type1tab0F:dword
externdef _type2tab0F:dword
externdef _type3tab0F:dword
externdef _type4tab0F:dword
externdef _type5tab0F:dword
externdef _type6tab0F:dword
externdef _type7tab0F:dword
externdef _type8tab0F:dword
externdef _type9tab0F:dword
externdef _typeAtab0F:dword
externdef _typeBtab0F:dword
externdef _typeCtab0F:dword
externdef _typeDtab0F:dword
externdef _typeEtab0F:dword
externdef _typeFtab0F:dword
externdef _grp1tab:dword
externdef _grp2tab:dword
externdef _grp3tab:dword
externdef _grp4tab:dword
externdef _grp5tab:dword
externdef _grp6tab:dword
externdef _grp7tab:dword
externdef _grp8tab:dword
externdef _r86tab:dword
.CODE
externdef @debug@:byte
externdef __datasel:word
externdef __zerosel:word
externdef __savedstack:dword
externdef fix_tab:dword
externdef _jumptbl:dword

start32:
	push	ds
	push	es
	push	ss
	push	fs
	push	gs
	pushfd
	pushad
	mov	edx,offs __message0
	call	print_string
	call	check_d32a_dpmi
	call	get_d32a_selectors
	popad
	popfd

	pushfd
	pushad
	call	Debug_Init_		; initialize debugger
	call	fill_memory_with_ints

	mov	ebx,__history_bufsize	; setup history buffer
	mov	ax,0FF91h
	int	21h
	mov	__history_bufbase,ebx
	mov	__history_bufhand,esi
	mov	__history_bufnum,0
	mov	__history_bufptr,0

	call	check_command_line
	test	esi,esi
	jz	@err1
	call	open_exec
	call	load_exec_header
	call	update_environment
	mov	bx,ds
	mov	ax,000Ah
	int	31h
	mov	bx,ax
	mov	dx,wptr __buffer+0
	mov	cx,wptr __buffer+2
	mov	ax,0007h
	int	31h
	push	fs
	pop	gs			; GS = zero selector
	mov	fs,bx			; FS = buffer selector
	mov	ax,cs
	lar	dx,ax
	shr	dx,8			; set rights = data
	and	dx,00060h
	xor	dx,0C092h
	mov	_acc_rights,dx

	movzx	edx,wptr __exec_start+0
	movzx	ecx,wptr __exec_start+2
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to file start
	jc	@err4
	mov	edx,__buffer		; DS:EDX = current ptr
	mov	ecx,4			; ECX = bytes to load
	mov	bx,__exec_handle
	mov	ax,3F00h
	int	21h
	jc	@err4
	movzx	edx,wptr __exec_start+0
	movzx	ecx,wptr __exec_start+2
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to file start
	jc	@err4

	push	offs @start32_2
	mov	edx,__buffer
	mov	ax,[edx+0]		; get file signature
	mov	bx,[edx+2]
	test	bx,bx
	jnz	@lerr6
	cmp	ax,'EL'			; 'LE' type
	jz	load_le_app
	cmp	ax,'XL'			; 'LX' type
	jz	load_lx_app
	cmp	ax,'CL'			; 'LC' type
	jz	load_lc_app
	jmp	@lerr6

@start32_2:	call	remove_name_from_cmd
	call	close_exec
	clr	ebx
	mov	ax,0300h
	int	10h			; get DOS cursor position
	dec	dh
	dec	dh
	mov	ax,0200h
	int	10h
	push	edx
	mov	ecx,160
@start32_l1:	mov	ax,0E20h
	xor	ebx,ebx
	push	ecx
	int	10h
	pop	ecx
	loop	@start32_l1
	pop	edx
	mov	ax,0200h
	int	10h
	xor	eax,eax
	int	33h

	cli
	cld
	mov	ax,wptr _sel32_cs
	mov	wptr _old_cs,ax
	mov	ax,wptr _sel32_ss
	mov	wptr _old_ds,ax
	mov	wptr _old_ss,ax
	mov	eax,_app_esp
	mov	dptr _old_esp,eax

	mov	bx,_sel32_cs		; get application's CS base
	mov	ax,0006h
	int	31h
	shl	ecx,16
	mov	cx,dx
	add	ecx,_app_eip
	mov	dr0,ecx			; place a breakpoint
	mov	eax,00000303h
	mov	dr7,eax
	mov	__history_bufon,1	; turn on history buffer
	popad
	popfd
	pop	gs	; restore ES, FS, GS
	pop	fs
	pop	ss
	pop	es
	pop	ds

	mov	ds,word ptr cs:_sel32_ss	; load new DS
	lss	esp,fword ptr cs:_app_esp	; load new SS:ESP
	jmp	fword ptr cs:_app_eip




;=============================================================================
check_d32a_dpmi:
	push	es	; check for DOS/32 Advanced ADPMI
	push	ds
	mov	esi,offs __idstring
	mov	ax,0A00h
	int	31h
	pop	ds
	mov	__api_off,edi
	mov	__api_sel,es
	pop	es
	jc	@err0
	mov	ax,es
	cmp	ax,__api_sel
	jz	@err0
	test	bl,02h
	jz	@err2
	cmp	cl,04h
	jb	@err6
	ret

get_d32a_selectors:
	mov	al,06h			; get ADPMI selectors
	call	fword ptr __api_off
	mov	__kernel_codesel,bx
	mov	__kernel_datasel,cx
	mov	__kernel_zerosel,dx
	mov	__kernel_codeseg,si
	mov	__client_codesel,di
	ret

fill_memory_with_ints:
	push	es
	push	ds
	pop	es
	mov	ax,0FF90h		; get free memory information
	int	21h
	mov	__free_mem,eax
	mov	ebx,eax			; allocate memory
	mov	ax,0FF91h
	int	21h
	jc	@fill_memory_with_ints_done
	mov	__base_mem,ebx
	mov	eax,0CCCCCCCCh		; set memory to INT 3
	mov	edi,__base_mem
	mov	ecx,__free_mem
	mov	dl,cl
	shr	ecx,2
	rep	stosd
	mov	cl,dl
	and	cl,3
	rep	stosb
	mov	ax,0FF92h		; free memory
	int	21h
@fill_memory_with_ints_done:	pop	es
	ret



;=============================================================================
@err0:	mov	edx,offs __error0
	call	print_string
	jmp	_abort
@err1:	mov	edx,offs __error1
	call	print_string
	jmp	_abort
@err2:	mov	edx,offs __error2
	call	print_string
	jmp	_abort
@err3:	mov	edx,offs __error3
	call	print_string
	jmp	_abort
@err4:	mov	edx,offs __error4
	call	print_string
	jmp	_abort
@err5:	mov	edx,offs __error5
	call	print_string
	jmp	_abort
@err6:	mov	edx,offs __error6
	call	print_string
	jmp	_abort





;=============================================================================
check_command_line:
	mov	edi,80h
	movzx	ecx,byte ptr es:[edi]	; get length of command line
	jecxz	@check_command_line_err			; if zero, error
	inc	edi			; offset to start of command line
	mov	al,20h
	repe	scasb			; look for non-space character
	jz	@check_command_line_err			; if not found, error
	dec	edi
	inc	ecx
	mov	ebx,edi
@check_command_line_1:	mov	al,es:[edi]
	cmp	al,09h			; look for TAB character
	jz	@check_command_line_2
	cmp	al,0Dh			; look for CR character
	jz	@check_command_line_2
	cmp	al,20h			; look for ' ' character
	jz	@check_command_line_2
	inc	edi
	loop	@check_command_line_1
@check_command_line_2:	mov	ecx,edi
	mov	esi,ebx			; SI = pointer to file name
	mov	edi,ebx			; DI = pointer to file name
	sub	ecx,ebx
@check_command_line_done:	ret
@check_command_line_err:	xor	esi,esi
	ret
remove_name_from_cmd:
	call	check_command_line
	jz	@remove_name_from_cmd_done
	mov	al,20h
	rep	stosb
@remove_name_from_cmd_done:	ret


;-----------------------------------------------------------------------------
update_environment:
	push	ds
	push	es
	sub	esp,64
	mov	ebp,esp
	mov	ah,19h
	int	21h
	mov	dl,al
	add	al,'A'
	mov	byte ptr [ebp+0],al	; set drive
	mov	word ptr [ebp+1],'\:'
	inc	dl
	mov	ah,47h
	lea	esi,[ebp+3]
	int	21h			; read directory
	push	ds
	pop	es
	xor	al,al
	mov	edi,esi
	mov	ecx,64
	repne	scasb
	cmp	byte ptr [edi-2],'\'
	jnz	@update_environment_0
	dec	edi
@update_environment_0:	mov	byte ptr [edi-1],'\'
	mov	es,[esp+64]
	push	edi
	call	check_command_line
	pop	edi
	push	ds
	pop	es
	mov	ds,[esp+64]
@update_environment_1:	lodsb
	stosb
	test	al,al
	loopne	@update_environment_1
	xor	al,al
	stosb
	push	es
	pop	ds
	mov	ecx,-1
	mov	edi,ebp
	repne	scasb
	not	ecx
	mov	ebx,ecx
	mov	es,[esp+64]
	mov	ax,es:[002Ch]		; get environment selector
	test	ax,ax			; check if selector is NULL
	jz	@update_environment_err			; if yes, jump to error
	lar	cx,ax
	jnz	@update_environment_err
	mov	es,ax
	xor	al,al
	xor	edi,edi
	mov	ecx,7FFFh
@update_environment_2:	repne	scasb
	scasb
	jnz	@update_environment_2
	add	edi,2
	mov	ecx,-1
	mov	edx,edi
	repne	scasb
	not	ecx
	cmp	ebx,ecx
	ja	create_new_environment
	mov	ecx,ebx
	mov	edi,edx
	mov	esi,ebp
	rep	movsb
@update_environment_done:	add	esp,64
	pop	es
	pop	ds
	ret
@update_environment_err:	add	esp,64
	pop	es
	pop	ds
	stc
	ret

create_new_environment:
	push	ebx
	push	ebp
	mov	ecx,-1
	xor	edi,edi
@create_new_environment_1:	repne	scasb
	dec	ecx
	scasb
	jnz	@create_new_environment_1
	not	ecx
	add	ecx,2
	push	ecx
	add	ecx,ebx
	shr	ecx,4
	inc	ecx
	mov	ebx,ecx
	mov	ax,0100h
	int	31h
	jc	_abort
	pop	ecx
	push	es
	pop	ds
	mov	es,dx
	xor	esi,esi
	xor	edi,edi
	rep	movsb
	pop	esi
	pop	ecx
	push	ss
	pop	ds
	rep	movsb
	add	esp,64
	pop	es
	pop	ds
	mov	es:[002Ch],dx
	ret


;-----------------------------------------------------------------------------
open_exec:
	push	ds
	push	es
	push	ss
	push	es
	pop	ds
	pop	es
	mov	edi,cs:_system_block
	rep	movsb
	mov	byte ptr es:[edi],0
	push	ss
	pop	ds
	mov	edx,_system_block
	mov	ax,3DC0h
	int	21h
	jc	@open_exec_1			; if error opening, try ".EXE"
	pop	es
	pop	ds
	mov	__exec_handle,ax
	jc	@err3
	ret

@open_exec_1:	mov	ebx,_system_block
@open_exec_2:	cmp	byte ptr [ebx],2Eh
	stc
	jz	@open_exec_err
	inc	ebx
	cmp	ebx,edi
	jb	@open_exec_2
	mov	eax,4558452Eh
	cmp	eax,[edi-4]
	stc
	jz	@open_exec_err
	mov	eax,6578652Eh
	cmp	eax,[edi-4]
	stc
	jz	@open_exec_err
	mov	[edi],eax
	mov	byte ptr [edi+4],0
	mov	ax,3DC0h
	int	21h
@open_exec_err:	pop	es
	pop	ds
	mov	__exec_handle,ax
	jc	@err3
	ret


load_exec_header:
	mov	ax,3F00h
	mov	bx,__exec_handle
	mov	ecx,64			; ECX = 64 bytes to load
	mov	edx,__buffer		; DS:EDX = ptr
	int	21h
	jc	@err4
	push	edx
	xor	ecx,ecx
	xor	edx,edx
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to file start
	pop	edx
	jc	@err4

	xor	ebp,ebp			; reset ptr in app
	cmp	word ptr [edx],'ZM'	; is exec 'MZ' file type
	jnz	search_for_le		; if not, search for known exec type

	mov	eax,[edx+18h]		; MZ reloc-tab must be at offset 0040h
	cmp	ax,40h
	jnz	search_for_mz

	mov	eax,[edx+3Ch]		; get start of 32-bit code
	test	ax,ax			; check if exec is bound
	jz	search_for_mz		; if not, search
	mov	__exec_start,eax
	mov	_app_off_datapages,0
	ret


search_for_mz:
	xor	esi,esi

@search_for_mz_0:	movzx	eax,wptr [edx+04h]	; get pages in file
	shl	eax,9			; *512
	movzx	ebx,wptr [edx+02h]	; get bytes on last page
	add	eax,ebx
	cmp	wptr [edx],'ZM'
	jz	@search_for_mz_1
	cmp	wptr [edx],'WB'
	jz	@search_for_mz_2
	jmp	@search_for_mz_3
@search_for_mz_1:	sub	eax,0200h
@search_for_mz_2:	mov	esi,ebp
	add	ebp,eax

	mov	ecx,ebp
	mov	edx,ebp
	shr	ecx,16
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to next exec in file
	jc	@err4
	mov	ax,3F00h
	mov	bx,__exec_handle
	mov	ecx,64
	mov	edx,__buffer
	int	21h
	jc	@err4
	test	eax,eax
	jz	@err5
	jmp	@search_for_mz_0

@search_for_mz_3:	cmp	wptr [edx],'EL'
	jz	@search_for_mz_4
	cmp	wptr [edx],'XL'
	jz	@search_for_mz_4
	mov	ecx,ebp
	mov	edx,ebp
	shr	ecx,16
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to next exec in file
	jc	@err4
	call	search_for_le
@search_for_mz_4:	mov	__exec_start,ebp
	mov	_app_off_datapages,esi
	ret


search_for_le:
@search_for_le_1:	mov	edx,__buffer		; DS:EDX = current ptr
	mov	ecx,1000h		; ECX = bytes to load
	mov	ax,3F00h
	mov	bx,__exec_handle
	int	21h
	jc	@err4
	test	eax,eax			; check if no bytes read
	jz	@err5			; if true, no app in file
	shr	ecx,1
@search_for_le_2:	mov	ax,[edx+0]
	mov	bx,[edx+2]
	test	bx,bx
	jnz	@search_for_le_4
	cmp	ax,'EL'			; 'LE' type
	jz	@search_for_le_3
	cmp	ax,'XL'			; 'LX' type
	jz	@search_for_le_3
@search_for_le_4:	cmp	ax,'CL'			; 'LC' type (Linear Compressed)
	jz	@search_for_le_3
	add	edx,2
	add	ebp,2			; increment pointer in file
	loop	@search_for_le_2
	jmp	@search_for_le_1
@search_for_le_3:	mov	_app_off_datapages,0
	ret

close_exec:
	mov	ah,0Dh			; flush disk buffers
	int	21h
	mov	bx,__exec_handle
	mov	ah,3Eh
	int	21h
	ret


print_string:
	pushad
	clr	eax
	clr	ecx
	mov	ebx,edx
@print_string_1:	cmp	al,[ebx]
	jz	@print_string_2
	inc	ebx
	inc	ecx
	jmp	@print_string_1
@print_string_2:	mov	eax,4000h
	mov	ebx,0002h
	int	21h
	popad
	ret









;=============================================================================
	Align 4
Debug_:
@debug@	db	0C3h		; RET
	xchg	eax,[esp]
	mov	ds:_temp,eax
	xchg	eax,[esp]
	lea	esp,[esp+4]	; remove caller from stack, don't change flags
	pushfd			; simulate an INT (push eflags)
	cli
	push	cs
	push	_temp		; push original caller
	jmp	_int03		; execute INT 3 handler






;=============================================================================
	Align 4
Debug_Init_:				; needed to initialize debugger's INTs
	pushfd
	pushad
	push	cs
	push	ds
	push	es
	push	ss
	push	fs
	push	gs
	cli
	cld

	mov	ax,001Fh		; disable mouse driver
	int	33h
	cli
	cld

	push	ds
	pop	es

	mov	ds:__datasel,ds		; set up selectors
	mov	ds:__zerosel,fs		; DOS/32A defines DS=ES=CS=0
	mov	ds:__savedstack,esp
	add	ds:__savedstack,56

	mov	ax,0400h
	int	31h
	mov	__cputype,cl
	mov	ax,0E00h
	int	31h
	shr	al,4
	mov	__fputype,al

	mov	edi,offs _registers
	lea	esi,[esp+52]
	mov	ecx,14
@Debug_Init_l1:	mov	eax,[esi]		; copy general & segment regs into buf
	sub	esi,4
	mov	[edi],eax
	add	edi,4
	loop	@Debug_Init_l1
	mov	eax,cr0
	mov	_cr0,eax
	mov	eax,cr2
	mov	_cr2,eax
	mov	eax,cr3
	mov	_cr3,eax
	mov	eax,[esp+60]
	mov	_eip,eax
	mov	eax,[esp+56]
	mov	_efl,eax
	mov	esi,offs _registers	; copy new regs into old regs
	mov	edi,offs _old_registers
	mov	ecx,19
	rep	movsd
	call	clearkeytab
	xor	ecx,ecx
	xor	edx,edx

;	mov	al,08h
;	mov	ecx,seg crt16
;	mov	edx,offs 0
;	call	fword ptr __api_off

	mov	ax,0204h		; get INT vectors
	mov	bl,10h
	int	31h
	mov	_old_int10sel,ecx
	mov	_old_int10off,edx
	mov	bl,21h
	int	31h
	mov	_old_int21sel,ecx
	mov	_old_int21off,edx
	mov	bl,31h
	int	31h
	mov	_old_int31sel,ecx
	mov	_old_int31off,edx
	mov	bl,33h
	int	31h
	mov	_old_int33sel,ecx
	mov	_old_int33off,edx

	mov	ax,0205h
	mov	bl,21h
	mov	cx,cs
	mov	edx,offs _int21		; INT 21h
	int	31h
	mov	ax,0205h
	mov	bl,31h
	mov	cx,cs
	mov	edx,offs _int31		; INT 31h
	int	31h
	mov	ax,0205h
	mov	bl,10h
	mov	cx,cs
	mov	edx,offs _int10		; INT 10h
	int	31h
	mov	ax,0205h
	mov	bl,33h
	mov	cx,cs
	mov	edx,offs _int33		; INT 33h
	int	31h


	mov	ax,0204h		; get IRQ vectors
	mov	bl,09h
	int	31h
	mov	_old_int9sel,ecx
	mov	_old_int9off,edx
	mov	bl,0Ah
	int	31h
	mov	_old_intAsel,ecx
	mov	_old_intAoff,edx
	mov	ax,0204h
	mov	bl,0Bh
	int	31h
	mov	_old_intBsel,ecx
	mov	_old_intBoff,edx
	mov	ax,0204h
	mov	bl,0Ch
	int	31h
	mov	_old_intCsel,ecx
	mov	_old_intCoff,edx
	mov	ax,0204h
	mov	bl,0Dh
	int	31h
	mov	_old_intDsel,ecx
	mov	_old_intDoff,edx
	mov	ax,0204h
	mov	bl,0Eh
	int	31h
	mov	_old_intEsel,ecx
	mov	_old_intEoff,edx

;	mov	ax,0205h
;	mov	cx,cs
;	mov	bl,9
;	mov	edx,offs _int09
;	int	31h

	mov	ax,0203h		; install INT handlers
	mov	cx,cs
	mov	bl,0
	mov	edx,offs _int00		; divide by zero
	int	31h
	mov	bl,1
	mov	edx,offs _int01		; INT 01h (trap)
	int	31h
	mov	bl,3
	mov	edx,offs _int03		; INT 03h (breakpnt)
	int	31h
	mov	bl,4
	mov	edx,offs _int04		; INTO
	int	31h
	mov	bl,5
	mov	edx,offs _int05		; bound
	int	31h
	mov	bl,6
	mov	edx,offs _int06		; invalid op
	int	31h
	mov	bl,0Ah
	mov	edx,offs _int0A
	int	31h
	mov	bl,0Bh
	mov	edx,offs _int0B
	int	31h
	mov	bl,0Ch
	mov	edx,offs _int0C
	int	31h
	mov	bl,0Dh
	mov	edx,offs _int0D
	int	31h
	mov	bl,0Eh
	mov	edx,offs _int0E
	int	31h

@Debug_Init_dpmi:	clr	eax			; initialize DEBUG REGISTERS
	mov	dr0,eax		; clear DR0-DR7: linear addresses
	mov	dr1,eax
	mov	dr2,eax
	mov	dr3,eax
	mov	dr6,eax		; clear DR6: bits T,S,D and breakpoint matches
	mov	dr7,eax		; setup DR7: disable all breakpoints
	mov	_dr0,eax
	mov	_dr1,eax
	mov	_dr2,eax
	mov	_dr3,eax
	mov	_dr7,eax
	xor	wptr _dr7,0300h
	mov	old_video_mode,ax
	mov	_data_addr,eax
	mov	_stack_addr,eax
	mov	_current_window,eax
	mov	eax,_eip		; setup variables
	mov	_code_addr,eax
	mov	_data_override,3
	mov	_cpu_xpos,1
	mov	_cpu_ypos,1
	mov	_cpu_xptr,0
	mov	_mem_xpos,14
	mov	_mem_ypos,39
	mov	_stk_xpos,62
	mov	_stk_ypos,30
	mov	_reg_xpos,1
	mov	_reg_ypos,32
	mov	_reg_ldat,12
	mov	_reg_xptr,0
	mov	__buffer,offset __screen2
	mov	_system_block,offset __screen
	mov	_vgastate_buf,offset __vgastatebuf
	mov	_svgastate_buf,offset __svgastatebuf
	mov	_vgastate_set,0			; set no VGA save/restore
	mov	_svgastate_set,0		; set no SVGA save/restore

	mov	ax,1C00h
	mov	cx,0007h
	int	10h
	cmp	al,1Ch
	jnz	@Debug_Init_verr
	cmp	ebx,4096/64
	ja	@Debug_Init_verr
	mov	_vgastate_set,1

	mov	ax,4F04h
	mov	cx,000Fh
	mov	dx,0000h
	int	10h
	cmp	ax,004Fh
	jnz	@Debug_Init_verr
	cmp	ebx,4096/64
	ja	@Debug_Init_verr
	mov	_svgastate_set,1

@Debug_Init_verr:	mov	byte ptr ds:@debug@,90h	; replace RET with NOP

	pop	gs	; return to caller
	pop	fs
	pop	ss
	pop	es
	pop	ds
	add	esp,4
	sti
	popad
	popfd
	ret




;-----------------------------------------------------------------------------
_exit:	cli
	cld
	mov	al,36h			; reprogram PIT to DOS freq
	out	43h,al
	mov	al,00h
	out	40h,al
	out	40h,al
	clr	eax
	mov	dr6,eax
	mov	dr7,eax			; reset hardware breakpoints
	mov	ds,cs:__datasel
	mov	es,cs:__datasel
	mov	ss,cs:__datasel
	mov	esp,cs:__savedstack
	and	old_video_mode,7F7Fh
	cmp	old_video_mode,0003h
	jnz	@exit_l1
	call	restore_video		; raw exit to DOS
	call	restore_state
	xor	eax,eax
	int	33h
	mov	ax,4C00h
	int	21h
@exit_l1:	mov	ax,0003h
	int	10h
	call	restore_state
	xor	eax,eax
	int	33h
	mov	ax,4C00h
	int	21h
	jmp	$

;-----------------------------------------------------------------------------
_abort:	cli
	cld
	mov	ax,4CFFh
	int	21h
	jmp	$



;-----------------------------------------------------------------------------
	Align 4
_int21:	cld
	push	ds
	push	es
	pushad

	cmp	cs:__history_bufon,0
	jz	@int21_l1
	mov	dx,cs
	mov	ax,[esp+2Ch]
	cmp	ax,dx
	jz	@int21_l1
	cmp	ax,cs:__kernel_codesel
	jz	@int21_l1
	cmp	ax,cs:__client_codesel
	jz	@int21_l1

	mov	ds,cs:__datasel
	mov	edx,__history_bufptr
	imul	edx,40h
	cmp	edx,__history_bufsize
	jb	@int21_1
	xor	edx,edx
	mov	__history_bufptr,edx
@int21_1:	add	edx,__history_bufbase
	mov	eax,__history_bufnum
	inc	__history_bufnum
	inc	__history_bufptr
	mov	[edx+00h],ax		; 00h - NUM
	mov	ax,0021h
	mov	[edx+02h],ax		; 02h - INT
	mov	eax,[esp+20h+8+8]
	mov	[edx+04h],eax		; 04h - EFL
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[edx+08h],eax		; 08h - EIP
	mov	eax,[esp+1Ch]
	mov	[edx+0Ch],eax		; 0Ch - EAX
	mov	eax,[esp+10h]
	mov	[edx+10h],eax		; 10h - EBX
	mov	eax,[esp+18h]
	mov	[edx+14h],eax		; 14h - ECX
	mov	eax,[esp+14h]
	mov	[edx+18h],eax		; 18h - EDX
	mov	eax,[esp+04h]
	mov	[edx+1Ch],eax		; 1Ch - ESI
	mov	eax,[esp+00h]
	mov	[edx+20h],eax		; 20h - EDI
	mov	eax,[esp+08h]
	mov	[edx+24h],eax		; 24h - EBP
	mov	eax,esp
	add	eax,20h+14h
	mov	[edx+28h],eax		; 28h - ESP
	mov	ax,[esp+20h+8+4]
	mov	[edx+2Ch],ax		; 2Ch - CS
	mov	ax,[esp+24h]
	mov	[edx+30h],ax		; 30h - DS
	mov	[edx+32h],es		; 32h - ES
	mov	[edx+34h],ss		; 34h - SS
	mov	[edx+36h],fs		; 36h - FS
	mov	[edx+38h],gs		; 38h - GS

	mov	ds,[esp+24h]
@int21_l1:	popad
	pushad
	cmp	ah,4Ch
	jz	@__4Ch
	popad
	pop	es
	pop	ds
	jmp	fword ptr cs:_old_int21off


@__4Ch:	mov	ax,cs
	cmp	ax,[esp+2Ch]
	jnz	_terminated
	mov	ax,0205h
	mov	bl,21h
	mov	ecx,cs:_old_int21sel
	mov	edx,cs:_old_int21off
	int	31h
	mov	bl,31h
	mov	ecx,cs:_old_int31sel
	mov	edx,cs:_old_int31off
	int	31h
	mov	bl,10h
	mov	ecx,cs:_old_int10sel
	mov	edx,cs:_old_int10off
	int	31h
	mov	bl,33h
	mov	ecx,cs:_old_int33sel
	mov	edx,cs:_old_int33off
	int	31h
	popad
	pop	es
	pop	ds
	jmp	fword ptr cs:_old_int21off

_terminated:
	mov	ds,cs:__datasel
	mov	__history_bufon,0
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[esp+20h+8+0],eax
	popad
	pop	es
	pop	ds

	push	es
	push	ss
	push	ds
	push	fs
	push	gs
	pushad
	call	save_state
	mov	al,4Ch
	jmp	_exception


;-----------------------------------------------------------------------------
	Align 4
_int31:	cld
	push	ds
	push	es
	pushad

	cmp	cs:__history_bufon,0
	jz	@int31_l1
	mov	dx,cs
	mov	ax,[esp+2Ch]
	cmp	ax,dx
	jz	@int31_l1
	cmp	ax,cs:__kernel_codesel
	jz	@int31_l1
	cmp	ax,cs:__client_codesel
	jz	@int31_l1

	mov	ds,cs:__datasel
	mov	edx,__history_bufptr
	imul	edx,40h
	cmp	edx,__history_bufsize
	jb	@int31_1
	xor	edx,edx
	mov	__history_bufptr,edx
@int31_1:	add	edx,__history_bufbase
	mov	eax,__history_bufnum
	inc	__history_bufnum
	inc	__history_bufptr
	mov	[edx+00h],ax		; 00h - NUM
	mov	ax,0031h
	mov	[edx+02h],ax		; 02h - INT
	mov	eax,[esp+20h+8+8]
	mov	[edx+04h],eax		; 04h - EFL
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[edx+08h],eax		; 08h - EIP
	mov	eax,[esp+1Ch]
	mov	[edx+0Ch],eax		; 0Ch - EAX
	mov	eax,[esp+10h]
	mov	[edx+10h],eax		; 10h - EBX
	mov	eax,[esp+18h]
	mov	[edx+14h],eax		; 14h - ECX
	mov	eax,[esp+14h]
	mov	[edx+18h],eax		; 18h - EDX
	mov	eax,[esp+04h]
	mov	[edx+1Ch],eax		; 1Ch - ESI
	mov	eax,[esp+00h]
	mov	[edx+20h],eax		; 20h - EDI
	mov	eax,[esp+08h]
	mov	[edx+24h],eax		; 24h - EBP
	mov	eax,esp
	add	eax,20h+14h
	mov	[edx+28h],eax		; 28h - ESP
	mov	ax,[esp+20h+8+4]
	mov	[edx+2Ch],ax		; 2Ch - CS
	mov	ax,[esp+24h]
	mov	[edx+30h],ax		; 30h - DS
	mov	[edx+32h],es		; 32h - ES
	mov	[edx+34h],ss		; 34h - SS
	mov	[edx+36h],fs		; 36h - FS
	mov	[edx+38h],gs		; 38h - GS

	mov	ds,[esp+24h]
@int31_l1:	popad
	pushad

	popad
	pop	es
	pop	ds
	jmp	fword ptr cs:_old_int31off



;-----------------------------------------------------------------------------
	Align 4
_int10:	cld
	push	ds
	push	es
	pushad

	cmp	cs:__history_bufon,0
	jz	@int10_l1
	mov	dx,cs
	mov	ax,[esp+2Ch]
	cmp	ax,dx
	jz	@int10_l1
	cmp	ax,cs:__kernel_codesel
	jz	@int10_l1
	cmp	ax,cs:__client_codesel
	jz	@int10_l1

	mov	ds,cs:__datasel
	mov	edx,__history_bufptr
	imul	edx,40h
	cmp	edx,__history_bufsize
	jb	@int10_1
	xor	edx,edx
	mov	__history_bufptr,edx
@int10_1:	add	edx,__history_bufbase
	mov	eax,__history_bufnum
	inc	__history_bufnum
	inc	__history_bufptr
	mov	[edx+00h],ax		; 00h - NUM
	mov	ax,0010h
	mov	[edx+02h],ax		; 02h - INT
	mov	eax,[esp+20h+8+8]
	mov	[edx+04h],eax		; 04h - EFL
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[edx+08h],eax		; 08h - EIP
	mov	eax,[esp+1Ch]
	mov	[edx+0Ch],eax		; 0Ch - EAX
	mov	eax,[esp+10h]
	mov	[edx+10h],eax		; 10h - EBX
	mov	eax,[esp+18h]
	mov	[edx+14h],eax		; 14h - ECX
	mov	eax,[esp+14h]
	mov	[edx+18h],eax		; 18h - EDX
	mov	eax,[esp+04h]
	mov	[edx+1Ch],eax		; 1Ch - ESI
	mov	eax,[esp+00h]
	mov	[edx+20h],eax		; 20h - EDI
	mov	eax,[esp+08h]
	mov	[edx+24h],eax		; 24h - EBP
	mov	eax,esp
	add	eax,20h+14h
	mov	[edx+28h],eax		; 28h - ESP
	mov	ax,[esp+20h+8+4]
	mov	[edx+2Ch],ax		; 2Ch - CS
	mov	ax,[esp+24h]
	mov	[edx+30h],ax		; 30h - DS
	mov	[edx+32h],es		; 32h - ES
	mov	[edx+34h],ss		; 34h - SS
	mov	[edx+36h],fs		; 36h - FS
	mov	[edx+38h],gs		; 38h - GS

	mov	ds,[esp+24h]
@int10_l1:	popad
	pushad

	popad
	pop	es
	pop	ds
	jmp	fword ptr cs:_old_int10off





;-----------------------------------------------------------------------------
	Align 4
_int33:	cld
	push	ds
	push	es
	pushad

	cmp	cs:__history_bufon,0
	jz	@int33_l1
	mov	dx,cs
	mov	ax,[esp+2Ch]
	cmp	ax,dx
	jz	@int33_l1
	cmp	ax,cs:__kernel_codesel
	jz	@int33_l1
	cmp	ax,cs:__client_codesel
	jz	@int33_l1

	mov	ds,cs:__datasel
	mov	edx,__history_bufptr
	imul	edx,40h
	cmp	edx,__history_bufsize
	jb	@int33_1
	xor	edx,edx
	mov	__history_bufptr,edx
@int33_1:	add	edx,__history_bufbase
	mov	eax,__history_bufnum
	inc	__history_bufnum
	inc	__history_bufptr
	mov	[edx+00h],ax		; 00h - NUM
	mov	ax,0033h
	mov	[edx+02h],ax		; 02h - INT
	mov	eax,[esp+20h+8+8]
	mov	[edx+04h],eax		; 04h - EFL
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[edx+08h],eax		; 08h - EIP
	mov	eax,[esp+1Ch]
	mov	[edx+0Ch],eax		; 0Ch - EAX
	mov	eax,[esp+10h]
	mov	[edx+10h],eax		; 10h - EBX
	mov	eax,[esp+18h]
	mov	[edx+14h],eax		; 14h - ECX
	mov	eax,[esp+14h]
	mov	[edx+18h],eax		; 18h - EDX
	mov	eax,[esp+04h]
	mov	[edx+1Ch],eax		; 1Ch - ESI
	mov	eax,[esp+00h]
	mov	[edx+20h],eax		; 20h - EDI
	mov	eax,[esp+08h]
	mov	[edx+24h],eax		; 24h - EBP
	mov	eax,esp
	add	eax,20h+14h
	mov	[edx+28h],eax		; 28h - ESP
	mov	ax,[esp+20h+8+4]
	mov	[edx+2Ch],ax		; 2Ch - CS
	mov	ax,[esp+24h]
	mov	[edx+30h],ax		; 30h - DS
	mov	[edx+32h],es		; 32h - ES
	mov	[edx+34h],ss		; 34h - SS
	mov	[edx+36h],fs		; 36h - FS
	mov	[edx+38h],gs		; 38h - GS

	mov	ds,[esp+24h]
@int33_l1:	popad
	pushad

	popad
	pop	es
	pop	ds
	jmp	fword ptr cs:_old_int33off



_critical_error_handler:
	push	es
	push	ss
	push	ds
	push	fs
	push	gs
	pushad
	call	save_state
	mov	eax,cs:_eax
	push	ds
	push	es
	push	ecx
	push	esi
	push	edi
	mov	ds,cs:__datasel
	mov	es,cs:__datasel
	mov	esi,offs _old_registers
	mov	edi,offs _registers
	mov	ecx,19
	rep	movsd
	pop	edi
	pop	esi
	pop	ecx
	pop	es
	pop	ds
	jmp	_exception

	Align 4
__datasel	dw 0
__zerosel	dw 0
__savedstack	dd 0





.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
__cputype	db 0
__fputype	db 0
__buffer	dd 0
__exec_handle	dw 0
__exec_start	dd 0
__free_mem	dd 0
__base_mem	dd 0
		evendata
_old_int10off	dd 0
_old_int10sel	dd 0
_old_int21off	dd 0
_old_int21sel	dd 0
_old_int31off	dd 0
_old_int31sel	dd 0
_old_int33off	dd 0
_old_int33sel	dd 0

__idstring	db 'SUNSYS DOS/32A',0
__message0	db 'SD/32A -- Protected Mode Debugger  '
	 db 'Version 7.2',cr
	 db 'Copyright (C) Supernar Systems, Ltd. 1996-2002',cre
__error0 db 'SD/32A fatal: DOS/32 Advanced ADPMI is required!',cr
	 db cr
db 'Note that this program can not be run under Windows or OS/2 systems',cr
db 'as they have an internal DPMI server which runs the protected mode.',cr
db 'If you are running SUNSYS Debugger under one of these systems, or under',cr
db 'any other, external (relative to DOS/32A) DPMI host, please exit to the',cr
db 'real-mode DOS prompt and restart the program from there.',cre

__error1 db 'SD/32A fatal: syntax is SD <execname.xxx>',cre
__error2 db 'SD/32A fatal: internal exception handling must be enabled',cre
__error3 db 'SD/32A fatal: could not open application file exec',cre
__error4 db 'SD/32A fatal: error in application file exec',cre
__error5 db 'SD/32A fatal: application file exec format not supported',cre
__error6 db 'SD/32A fatal: 80486 processor or better required',cre



.STACK
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
		db 4096 dup(?)

include	sloader.asm
include	sdebug.asm

end	start32
