.386

assume nothing

DGROUP group _DATA

_TEXT   segment use32 word public 'CODE'

assume  ds:DGROUP

        mov     ds,my_ds
        mov     ds,ds:my_ds
        mov     ds,DGROUP:my_ds
        mov     ds,_DATA:my_ds
        mov     ds,_TEXT:my_ds

assume  ds:_DATA
        mov     ds,my_ds
        mov     ds,ds:my_ds
        mov     ds,DGROUP:my_ds
        mov     ds,_DATA:my_ds
        mov     ds,_TEXT:my_ds

assume  ds:_TEXT
        mov     ds,my_ds
        mov     ds,ds:my_ds
        mov     ds,DGROUP:my_ds
        mov     ds,_DATA:my_ds
        mov     ds,_TEXT:my_ds

        mov     ds,cs:my_ds
        mov     ds,es:my_ds
        mov     ds,ss:my_ds
        mov     ds,fs:my_ds
        mov     ds,gs:my_ds
_TEXT   ends

_DATA   segment dword public 'DATA'
my_ds     dw    0
_DATA   ends

        end
