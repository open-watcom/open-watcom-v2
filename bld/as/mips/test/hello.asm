; Trivial hello world program for Linux

.data                       ; section declaration - variables only

msg:
        .string "Hello, world!\n"
;       len = . - msg       ; length of string

.text                       ; section declaration - code
.globl _start

_start:
                            ; write string to stdout

        li      $v0,4004    ; syscall number (sys_write)
        li      $a0,1       ; first argument: file descriptor (stdout)
        la      $a1,msg     ; second argument: pointer to message to write
        li      $a2,14;len  ; third argument: message length
        syscall             ; call kernel

                            ; and exit

        li      $v0,4001    ; syscall number (sys_exit)
        li      $a0,1       ; first argument: exit code
        syscall             ; call kernel
