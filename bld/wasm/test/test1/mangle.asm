.model small
public x
extern "C", _foo_:dword
global "C", _bar_:dword
externdef "C", _sam_:byte
comm "C", _kyb_:byte
.data
    x dw ?
    y db 'F'
    z db ?
end
