   TITLE   mif

; Title   : Microsoft C language interface to the Logitech mouse driver
; Author  : ra
; Date    : 10-30-85
; System  : Microsoft Macro Assembler
; Version : 1.00


; Short Explanation:
;   The Microsoft C compiler can link an object module produced by the
;  macro assembler with its own object modules. When this interface
;  module is assembled it produces an object module which exports mouse
;  driver interface functions for use by a C program.
;   The mouse driver functions are documented in the Logitech Mouse Driver
;  programming interface manual.
;   This driver interface is compatible with the Microsoft Mouse Driver
;  interface.

;  Microsoft is a registered trademark of Microsoft Corporation.


; Naming Convention:
;   The names follow the same convention as the Logitech Modula-2 language
;  interface.

;
; N.B. : This interface assumes the LARGE memory model - ie long (4 byte)
;        addresses for both data and subroutine calls.
;        When the user compiles the application to link with the interface
;        he/she should specify this model.On the Microsoft C compiler this
;        is done using the -Ml option, ' cc -c -Ml main.c '.
;

; ****************************************************************
; *                                                              *
; *                  M  A  C  R  O  S                            *
; *                                                              *
; ****************************************************************
; enter and leave are now instructions

do_enter   MACRO
        push  bp           ; standard C compiler call entry
        mov   bp, sp
        push  di
        push  si
        ENDM

do_leave   MACRO
        pop   si           ; standard call exit
        pop   di
        mov   sp, bp
        pop   bp
        ret
        ENDM


; ****************************************************************
; *                                                              *
; *          A S S E M B L E R    D I R E C T I V E S            *
; *                                                              *
; ****************************************************************

_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS

CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS

_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS

_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS

DGROUP  GROUP   CONST,  _BSS,   _DATA
        ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

PUBLIC  _FlagReset
PUBLIC  _ShowCursor
PUBLIC  _HideCursor
PUBLIC  _GetPosBut
PUBLIC  _SetCursorPos
PUBLIC  _GetButPres
PUBLIC  _GetButRel
PUBLIC  _SetHorizontalLimits
PUBLIC  _SetVerticalLimits
PUBLIC  _SetGraphicCursor
PUBLIC  _ReadMotionCounters
PUBLIC  _SetEventHandler
PUBLIC  _LightPenOn
PUBLIC  _LightPenOff
PUBLIC  _SetMickeysPerPixel
PUBLIC  _ConditionalOff
PUBLIC  _SetSpeedThreshold


_DATA   SEGMENT
_DATA   ENDS

_TEXT      SEGMENT

; ****************************************************************
; *                                                              *
; *         T H E   I N T E R F A C E   R O U T I N E S          *
; *                                                              *
; ****************************************************************


CDataSeg  dw  DGROUP

; ****************************************************************
; *        F U N C T I O N  0  :  F l a g R e s e t              *
; ****************************************************************

PUBLIC  _FlagReset
;
;     FlagReset( mouseStatusPtr, numberOfButtonsPtr )
;     int *mouseStatusPtr, *numberOfButtonsPtr;
;

_FlagReset      PROC FAR
;          Output: AX --> mouse status
;                   0 (FALSE): mouse hardware and software
;                              not installed
;                  -1 (TRUE) : mouse hardware and software
;                              installed
;                  BX --> number of mouse buttons
;
                            do_enter
                            mov   ax, 0        ; function 0
                            int   33h          ; call mouse driver function
                            les   di, 6[bp]    ; address of mouseStatus
                            mov   es:[di], ax  ; mouseStatus = ax
                            les   di, 10[bp]   ; address of numberOfButtons
                            mov   es:[di], bx  ; numberOfButtons = bx
                            do_leave

_FlagReset                  ENDP


; ****************************************************************
; *        F U N C T I O N  1  :
; ****************************************************************

PUBLIC _ShowCursor
;
; ShowCursor();
;

_ShowCursor  PROC FAR
                            do_enter
                            mov   ax, 1        ; function 1
                            int   33h          ; call mouse driver function
                            do_leave

_ShowCursor                 ENDP

; ****************************************************************
; *        F U N C T I O N  2  :
; ****************************************************************

PUBLIC _HideCursor
;
; HideCursor();
;

_HideCursor PROC FAR
                            do_enter
                            mov   ax, 2        ; function 2
                            int   33h          ; call mouse driver function
                            do_leave

_HideCursor                 ENDP


; ****************************************************************
; *        F U N C T I O N  3  :
; ****************************************************************

PUBLIC _GetPosBut
;
; GetPosBut ( buttonStatus, horizontal, vertical )
; struct {
;    unsigned leftButton  : 1;
;    unsigned rightButton : 1;
;        } *buttons;
; int *horizontal, *vertical;
;

_GetPosBut PROC FAR
                            do_enter
                            mov   ax, 3        ; function 3
                            int   33h          ; call mouse driver function
                            les   di, 6[bp]    ; address of buttonStatus
                            mov   es:[di], bx  ; buttonStatus = bx
                            les   di, 10[bp]   ; address of horizontal
                            mov   es:[di], cx  ; horizontal = cx
                            les   di, 14[bp]   ; address of vertical
                            mov   es:[di], dx  ; vertical = dx
                            do_leave
_GetPosBut ENDP


; ****************************************************************
; *        F U N C T I O N  4  :
; ****************************************************************

PUBLIC _SetCursorPos
;
; SetCursorPos (horizontal, vertical)
; int horizontal, vertical;
;

_SetCursorPos PROC FAR
                            do_enter
                            mov   cx, 6[bp]    ; cx = horizontal
                            mov   dx, 8[bp]    ; dx = vertical
                            mov   ax, 4        ; function 4
                            int   33h          ; call mouse driver function
                            do_leave
_SetCursorPos ENDP


; ****************************************************************
; *        F U N C T I O N  5  :
; ****************************************************************

PUBLIC _GetButPres
;
; GetButPres (button, buttonStatus, buttonPressCount, horizontal, vertical)
; int button, *buttonStatus, *buttonPressCount, *horizontal, *vertical;
;

_GetButPres PROC FAR
                            do_enter
                            mov   bx, 6[bp]    ; bx = button
                            mov   ax, 5        ; function 5
                            int   33h          ; call mouse driver function
                            les   di, 8[bp]   ; address of buttonStatus
                            mov   es:[di], ax  ; buttonStatus = ax
                            les   di, 12[bp]   ; address of buttonPressCount
                            mov   es:[di], bx  ; buttonPressCount = bx
                            les   di, 16[bp]   ; address of horizontal
                            mov   es:[di], cx  ; horizontal = cx
                            les   di, 20[bp]   ; address of vertical
                            mov   es:[di], dx  ; vertical = dx
                            do_leave
_GetButPres ENDP


; ****************************************************************
; *        F U N C T I O N  6  :
; ****************************************************************

PUBLIC _GetButRel
;
; GetButRel (button, buttonStatus, buttonReleaseCount, horizontal, vertical)
; int button, *buttonStatus, *buttonReleaseCount, *horizontal, *vertical;


_GetButRel PROC FAR
                            do_enter
                            mov   bx, 6[bp]    ; bx = button
                            mov   ax, 6        ; function 6
                            int   33h          ; call mouse driver function
                            les   di, 8[bp]    ; address of buttonStatus
                            mov   es:[di], ax  ; buttonStatus = ax
                            les   di, 12[bp]   ; address of buttonReleaseCount
                            mov   es:[di], bx  ; buttonReleaseCount = bx
                            les   di, 16[bp]   ; address of horizontal
                            mov   es:[di], cx  ; horizontal = cx
                            les   di, 20[bp]   ; address of vertical
                            mov   es:[di], dx  ; vertical = dx
                            do_leave
_GetButRel ENDP


; ****************************************************************
; *        F U N C T I O N  7  :
; ****************************************************************

PUBLIC _SetHorizontalLimits
;
; SetHorizontalLimits (minPos, maxPos)
; int minPos, maxPos;
;

_SetHorizontalLimits PROC FAR
                            do_enter
                            mov   cx, 6[bp]    ; cx = minPos
                            mov   dx, 8[bp]    ; dx = minPos
                            mov   ax, 7        ; function 7
                            int   33h          ; call mouse driver function
                            do_leave
_SetHorizontalLimits ENDP


; ****************************************************************
; *        F U N C T I O N  8  :
; ****************************************************************

PUBLIC _SetVerticalLimits
;
; SetHorizontalLimits (minPos, maxPos)
; int minPos, maxPos;
;

_SetVerticalLimits PROC FAR
                            do_enter
                            mov   cx, 6[bp]    ; cx = minPos
                            mov   dx, 8[bp]    ; dx = minPos
                            mov   ax, 8        ; function 8
                            int   33h          ; call mouse driver function
                            do_leave
_SetVerticalLimits ENDP


; ****************************************************************
; *        F U N C T I O N  9  :
; ****************************************************************

PUBLIC _SetGraphicCursor
;
; typedef struct {
;    unsigned int screenMask[16],
;                 cursorMask[16];
;    int          hotX,
;                 hotY;
;                } GraphicCursor;
;
; SetGraphicCursor (cursor)
; GraphicCursor *cursor;
;

_SetGraphicCursor PROC FAR
                            do_enter
                            les   di, 6[bp]     ; address of screenMask
                            mov   dx, di        ; es:dx = address of screenMask
                            mov   bx, es:64[di] ; bx = hotX
                            mov   cx, es:66[di] ; cx = hotY
                            mov   ax, 9         ; function 9
                            int   33h           ; call mouse driver function
                            do_leave
_SetGraphicCursor ENDP



; ****************************************************************
; *        F U N C T I O N  10  :
; ****************************************************************

PUBLIC _SetTextCursor
;
; SetTextCursor (selectedCursor, screenMaskORscanStart, cursorMaskORscanStop)
; unsigned int selectedCursor,
;              screenMaskORscanStart,
;              cursorMaskORscanStop;
;

_SetTextCursor PROC FAR
                            do_enter
                            mov   bx, 6[bp]    ; bx = selectedCursor
                            mov   cx, 8[bp]    ; cx = screenMaskORscanStart
                            mov   dx, 10[bp]   ; dx = cursorMaskORscanStop
                            mov   ax, 10       ; function 10
                            int   33h          ; call mouse driver function
                            do_leave
_SetTextCursor ENDP


; ****************************************************************
; *        F U N C T I O N  11  :
; ****************************************************************

PUBLIC _ReadMotionCounters
;
; ReadMotionCounters ( horizontal, vertical )
; int *horizontal, *vertical;
;

_ReadMotionCounters PROC FAR
                            do_enter
                            mov   ax, 11       ; function 11
                            int   33h          ; call mouse driver function
                            les   di, 6[bp]    ; address of horizontal
                            mov   es:[di], cx  ; horizontal = cx
                            les   di, 10[bp]   ; address of vertical
                            mov   es:[di], dx  ; vertical = dx
                            do_leave
_ReadMotionCounters ENDP


; ****************************************************************
; *        F U N C T I O N  12  :
; ****************************************************************

PUBLIC _SetEventHandler
;
; SetEventHandler (mask, handler)
; int mask;
; int *handler();
;

hand_offset   dw ?
hand_segment  dw ?
saveAX        dw ?

PrivateHandler PROC FAR

process_event:              cli               ; prevent a further interrupt
                                              ; which would change registers
                                              ; ax, bx, cx, dx

                            push  ds          ; save mouse driver ds
                            mov   ds, cs:CDataSeg ; set up C data seg

                            push  dx          ; set up params for c function
                            push  cx
                            push  bx
                            push  ax

                            sti               ; enable interrupts

                            ;call far [dword ptr cs:hand_segment] ; masm4
                            call dword ptr cs:hand_offset ; wasm - 93/07/09 MEP
                            add  sp, 8

                            pop  ds           ; restore mouse driver ds
                            ret


PrivateHandler ENDP

_SetEventHandler PROC FAR
                            do_enter
                            push  es
                            mov   cx, 6[bp]    ; cx = mask
                            les   dx, 8[bp]    ; handler
                            mov   hand_segment, es
                            mov   hand_offset , dx
                            push  cs
                            pop   es
                            lea   dx, cs:PrivateHandler
                            mov   ax, 12       ; function 12
                            int   33h          ; call mouse driver function
                            pop   es
                            do_leave
_SetEventHandler ENDP



; ****************************************************************
; *        F U N C T I O N  13  :
; ****************************************************************

PUBLIC _LightPenOn
;
; LightPenOn ();
;

_LightPenOn PROC FAR
                            do_enter
                            mov   ax, 13       ; function 13
                            int   33h          ; call mouse driver function
                            do_leave
_LightPenOn ENDP


; ****************************************************************
; *        F U N C T I O N  14  :
; ****************************************************************

PUBLIC _LightPenOff
;
; LightPenOff ();
;

_LightPenOff PROC FAR
                            do_enter
                            mov   ax, 14        ; function 14
                            int   33h          ; call mouse driver function
                            do_leave
_LightPenOff ENDP


; ****************************************************************
; *        F U N C T I O N  15  :
; ****************************************************************

PUBLIC _SetMickeysPerPixel
;
; SetMickeysPerPixel (horPix, verPix)
; unsigned int horPix, verPix;
;

_SetMickeysPerPixel PROC FAR
                            do_enter
                            mov   cx, 6[bp]    ; cx = horPix
                            mov   dx, 8[bp]    ; dx = verPix
                            mov   ax, 15       ; function 15
                            int   33h          ; call mouse driver function
                            do_leave
_SetMickeysPerPixel ENDP


; ****************************************************************
; *        F U N C T I O N  16  :
; ****************************************************************

PUBLIC _ConditionalOff
;
; ConditionalOff (left, top, right, bottom)
; int left, top, right, bottom;
;

_ConditionalOff PROC FAR
                            do_enter
                            mov   cx,  6[bp]   ; cx = left
                            mov   dx,  8[bp]   ; dx = top
                            mov   si,  10[bp]  ; si = right
                            mov   di,  12[bp]  ; di = bottom
                            mov   ax, 16       ; function 16
                            int   33h          ; call mouse driver function
                            do_leave
_ConditionalOff ENDP


; ****************************************************************
; *        F U N C T I O N  19  :
; ****************************************************************

PUBLIC _SetSpeedThreshold
;
; SetSpeedThreshold (threshold)
; unsigned int threshold;
;

_SetSpeedThreshold PROC FAR
                            do_enter
                            mov   dx, 6[bp]     ; dx = threshold
                            mov   ax, 19        ; function 19
                            int   33h           ; call mouse driver function
                            do_leave
_SetSpeedThreshold ENDP


_TEXT   ENDS
END
