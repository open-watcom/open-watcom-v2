;
;Some basic EQU's for the assemblers sake
;
w	equ word ptr
b	equ byte ptr
d	equ dword ptr


;
;Some text handling equates
;
Cr	equ	13
Lf	equ	10
Tab	equ	9


;
;Some keyboard handling equates.
;
EscapeK	equ	0+(1*256)+27		;Escape key code.
CursorDownK	equ	0+(80*256)+255	;cursor down key.
CursorPgDnK	equ	0+(81*256)+255
CursorPgUpK	equ	0+(73*256)+255
CursorUpK	equ	0+(72*256)+255	;cursor up key.
CursorLeftK	equ	0+(75*256)+255	;cursor left key.
CursorRightK	equ	0+(77*256)+255	;cursor right key.
TabK	equ	0+(15*256)+255	;tab key.


;
;Some Psp equates
;
Enviroment	equ	2ch


;--------------------------------------------------------------------------
;KEYBOARD KEY DEFINITIONS
;--------------------------------------------------------------------------
k_f1	equ	59
k_f2	equ	60
k_f3	equ	61
k_f4	equ	62
k_f5	equ	63
k_f6	equ	64
k_f7	equ	65
k_f8	equ	66
k_f9	equ	67
k_f10	equ	68
k_escape	equ	1	;27
k_tab	equ	15	;9
k_backspace	equ	14	;7
k_return	equ	28
k_home	equ	71
k_up	equ	72
k_page_up	equ	73
k_left	equ	75
k_center	equ	76
k_right	equ	77
k_end	equ	79
k_down	equ	80
k_page_down	equ	81

K_1	equ	2
K_2	equ	3
k_3	equ	4
k_4	equ	5
k_5	equ	6
k_6	equ	7
k_7	equ	8
k_8	equ	9
k_9	equ	10
k_0	equ	11

k_q	equ	16
k_w	equ	17
k_e	equ	18
k_r	equ	19
k_t	equ	20
k_y	equ	21
k_u	equ	22
k_i	equ	23
k_o	equ	24
k_p	equ	25

k_a	equ	30
k_s	equ	31
k_d	equ	32
k_f	equ	33
k_g	equ	34
k_h	equ	35
k_j	equ	36
k_k	equ	37
k_l	equ	38

k_z	equ	44
k_x	equ	45
k_c	equ	46
k_v	equ	47
k_b	equ	48
k_n	equ	49
k_m	equ	50
