.386p
                NAME    blip
                EXTRN   __Extender :BYTE
DGROUP          GROUP   _DATA
_DATA           SEGMENT BYTE PUBLIC USE32 'DATA'
L2              DW      00H
_DATA           ENDS

_TEXT           SEGMENT BYTE PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
;                PUBLIC  Blip_
                cmp     L2,0000H
_TEXT           ENDS

                END
