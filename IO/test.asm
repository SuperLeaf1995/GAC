char: 
main: 
mov AL , byte 1

add AL , byte [EBP  + 8]
mov [EBP  - 0], byte AL 
mov AL , byte [EBP  - 0]

ret 
