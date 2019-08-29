

section .text

global _start
_start:
call main
mov eax, 1
mov ebx, 0
int 80h

GASCode:

 sum:
push ebp
mov ebp, esp
sub esp, 2
;x is now Variable.
mov eax , [ebp+8]
mov [ sum.x], eax 
;y is now Variable.
mov ebx , [ebp+12]
mov [ sum.y], ebx 
push sum.z
mov ecx , dword [sum.y]
mov edx , dword [sum.x]
add edx , ecx 

pop esi 
mov [esi ], edx 
mov esp, ebp
pop ebp
pop eax
add esp, 8
push dword [sum.z]
jmp eax

 main:
push ebp
mov ebp, esp
sub esp, 0
push a
push b
push a
call sum
pop eax 
pop edi 
mov [edi ], eax 
mov esp, ebp
pop ebp
ret



section .data

a dd 1
b dd 2
c dd 3
 sum.x dd 0
 sum.y dd 0
sum.z dd 0


section .bss

