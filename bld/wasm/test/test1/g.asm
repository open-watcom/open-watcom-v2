.model small
.data
SAVESAVEOFF     equ     -12H
.code
    call        dword ptr es:SAVESAVEOFF[bx]; save saved commands
end
