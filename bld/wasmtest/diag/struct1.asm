.386

.model small

.data

TRAP_ENTRY STRUC
    te_stack_trap       dw      ?
    te_ret_list         dw      ?
    te_ret_offset       dw      ?
    te_context          dw      ?
TRAP_ENTRY ENDS

RET_TRAP STRUC
    rt_call_far         db      ?
    rt_entry            dd      ?
    rt_pad              db      ?
    rt_old_code_handle  dw      ?
    rt_traps            db      size TRAP_ENTRY dup(?)
RET_TRAP ENDS

OVLTAB_ENTRY    STRUC
    ove_flags_anc       dw      ?
    ove_relocs          dw      ?
    ove_start_para      dw      ?
    ove_code_handle     dw      ?
    ove_num_paras       dw      ?
    ove_fname           dw      ?
    ove_disk_addr       dd      ?
OVLTAB_ENTRY    ENDS

OVLTAB_PROLOG   STRUC
    ovp_major           db      ?
    ovp_minor           db      ?
    ovp_start           dd      ?
    ovp_delta           dw      ?
    ovp_ovl_size        dw      ?
OVLTAB_PROLOG   ENDS

OVLTAB          STRUC
    ov_prolog           db  size OVLTAB_PROLOG dup(?)
    ov_entries          db  size OVLTAB_ENTRY dup(?)
OVLTAB          ENDS

  db 10 dup(0)
a db 0

.code

        extrn   __OVLTAB__:OVLTAB

cx2 label byte
cx1:

k1 = 1
k3 equ 1 + [offset   __OVLTAB__]
k2 equ offset (k1 +   2)/3 + k3
k4 equ 1 + [offset cx1]
k5 equ 1 + [offset a]
k6 equ 1 + [offset cx2]

ENTRIES_M_1     equ (offset __OVLTAB__.ov_entries - size OVLTAB_ENTRY)

        mov     BX,RET_TRAP.rt_traps
        test    CS:(ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI]),10H
        test    byte ptr CS:(ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI]),10H
        test    byte ptr CS:ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI],10H
        test    word ptr CS:ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI],1010H
        test    dword ptr CS:ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI],101010H
        mov     CS:(ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI]),10H
        mov     byte ptr CS:(ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI]),10H
        mov     byte ptr CS:ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI],10H
        mov     word ptr CS:ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI],1010H
        mov     dword ptr CS:ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI],101010H
        call    word ptr CS:ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI]
        call    dword ptr CS:ENTRIES_M_1 + OVLTAB_ENTRY.ove_flags_anc + 1[DI]
        mov     AX,10H + RET_TRAP.rt_traps

end
