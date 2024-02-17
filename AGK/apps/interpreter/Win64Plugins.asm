
.code
PUBLIC win64_plugin_call
win64_plugin_call PROC

; RCX = registerCount
; RDX = pRegisterParams
; R8 = pRegisterParamTypes
; R9 = stackCount
; RSP+40 = pStackParams
; RSP+48 = returnType
; RSP+56 = pFunc

mov [rsp+8], r12
mov [rsp+16], r13
mov [rsp+24], r14
mov [rsp+32], r15

mov r11, rdx
mov r12, r8
mov r13, r9
mov r14, [rsp+56]
mov r15d, DWORD PTR [rsp+48]

; RAX = registerCount (modified)
; RBX = unused
; RCX = reserved
; RDX = reserved
; R8  = reserved
; R9  = reserved
; R10 = temp
; R11 = pRegisterParams
; R12 = pRegisterParamTypes
; R13 = stackCount
; R14 = pFunc
; R15 = returnType

; param 1
mov rax, rcx
cmp rax, 0
je paramend
mov r10d, DWORD PTR [r12]
cmp r10d, 0
jne floatparam1
mov rcx, [r11]
jmp nextparam
floatparam1:
movss xmm0, DWORD PTR [r11]

nextparam:
; param 2
sub rax, 1
cmp rax, 0
je paramend
mov r10d, DWORD PTR [r12+4]
cmp r10d, 0
jne floatparam2
mov rdx, [r11+8]
jmp nextparam2
floatparam2:
movss xmm1, DWORD PTR [r11+8]

nextparam2:
; param 3
sub rax, 1
cmp rax, 0
je paramend
mov r10d, DWORD PTR [r12+8]
cmp r10d, 0
jne floatparam3
mov r8, [r11+16]
jmp nextparam3
floatparam3:
movss xmm2, DWORD PTR [r11+16]

nextparam3:
; param 4
sub rax, 1
cmp rax, 0
je paramend
mov r10d, DWORD PTR [r12+12]
cmp r10d, 0
jne floatparam4
mov r9, [r11+24]
jmp nextparam4
floatparam4:
movss xmm3, DWORD PTR [r11+24]

nextparam4:

cmp r13, 0
je paramend

mov rax, r13
mov r10, [rsp+40]
shl r13, 3

; RAX = stackCount (modified)
; RBX = unused
; RCX = reserved
; RDX = reserved
; R8  = reserved
; R9  = reserved
; R10 = pStackParams
; R11 = unused
; R12 = temp
; R13 = stackCount
; R14 = pFunc
; R15 = returnType

; check for even number of stack params
mov r12, rax
and r12, 1
cmp r12, 0
jne nextloop

; if even then maintain 16-byte aligned stack pointer after return address is pushed
; must be done before parameters are pushed to the stack
sub rsp, 8
add r13, 8

; stack params
nextloop:
sub rax, 1
cmp rax, 0
jl paramendstack ; end loop

push [r10+rax*8]
jmp nextloop

; if there were no stack parameters then we must push 8 bytes to maintain 16-byte alignment after the return address is pushed
paramend:
sub rsp, 8
add r13, 8

paramendstack:
sub rsp, 32
; call function
call r14

; clean stack
add rsp, 32
add rsp, r13

; handle return type
cmp r15, 2
jne normalreturn
movd rax, xmm0

normalreturn:

; restore registers
mov r12, [rsp+8]
mov r13, [rsp+16]
mov r14, [rsp+24]
mov r15, [rsp+32]

ret
win64_plugin_call ENDP

END