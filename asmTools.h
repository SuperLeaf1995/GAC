#ifndef _ASMTOOLS_H_
#define _ASMTOOLS_H_

#include <vector>
using namespace std;

#include "Token.h"

string init_size(vector <Token> &Tokens)
{
    string b = 
    "\nfunction_size:\n"
    "  pop ebx\n"
    "  pop edx\n"
    "  xor eax, eax\n"
    "  jmp check\n"
    "  top:\n"
    "  inc edx\n"
    "  inc eax\n"
    "  check:\n"
    "  cmp byte [edx], 0\n"
    "  jnz top\n"
    "  push eax\n"
    "  push ebx\n"
    "ret\n\n"
    ;
    Token sizer;
    sizer.makeFunc("size");
    sizer.makePublic();
    sizer.ParameterAmount = 1;
    sizer.makeReturnable();
    Tokens.push_back(sizer);
    return b;
}

string init_malloc(vector <Token> &Tokens)
{
    string b =  
    "function_malloc:\n"
    "  push dword 0\n"
    "  push dword -1\n"
    "  push dword 0x22\n" //0x22 == heap
    "  push dword 0x03\n"
    "  push dword [esp+20]\n" // size
    "  push dword 0\n"
    "  mov eax, 0x5a\n"
    "  mov ebx, esp\n"
    "  int 0x80\n"
    "  add esp, 24\n"
    "  pop ebx\n"
    "  push eax\n"
    "jmp ebx\n\n"
    ;
    Token alloc;
    alloc.makeFunc("malloc");
    alloc.makePublic();
    alloc.ParameterAmount = 1;
    alloc.makeReturnable();
    Tokens.push_back(alloc);
    return b;
}

string init_char(vector <Token> &Tokens)
{
    string b = 
    "function_char:\n"
    "  pop edi\n"
    "  pop eax\n"
    "  printRAX:\n"
    "    lea ecx, [char.s]\n"
    "    mov ebx, 10\n"
    "    mov [ecx], ebx\n"
    "    inc ecx\n"
    "    mov [char.i], ecx\n"
    "  charLoop:\n"
    "    mov edx, 0\n"
    "    mov ebx, 10\n"
    "    div ebx\n"
    "    push eax\n"
    "    add edx, 48\n"
    "    mov ecx, [char.i]\n"
    "    mov [ecx], dl\n"
    "    inc ecx\n"
    "    mov [char.i], ecx\n"
    "    pop eax\n"
    "    cmp eax, 0\n"
    "    jne charLoop\n"
    "  push char.s\n"
    "jmp edi\n\n"
    ;
    Token toChar;
    toChar.makeFunc("char");
    toChar.makePublic();
    toChar.ParameterAmount = 1;
    toChar.makeReturnable();
    Tokens.push_back(toChar);

    Token chari;
    chari.makeVar();
    chari.makeName("char.i");
    chari.makePrivate("char");
    varbuffer1 += "char.i dd 0\n";
    Tokens.push_back(chari);

    Token chars;
    chars.makeString(4);
    chars.makeName("char.s");
    chars.makePrivate("char");
    varbuffer1 += "char.s dd 0\n";
    Tokens.push_back(chars);

    return b;
}

string init_num(vector <Token> &Tokens)
{
    string b = 
    "function_num:\n"
    "  pop edx\n"
    "  pop eax\n"
    "  mov ecx, 48\n"
    "  sub eax, ecx\n"
    "  push eax\n"
    "  jmp edx\n\n"
    ;
    Token toNum;
    toNum.makeFunc("num");
    toNum.makePublic();
    toNum.ParameterAmount = 1;
    toNum.makeReturnable();
    Tokens.push_back(toNum);
    return b;
}

string init_reverse(vector <Token> &Tokens)
{
    string b = 
    "function_reverse:\n"
    "  push ebp\n"
    "  mov ebp, esp\n"
    "  mov esi, [ebp+8]\n"
    "  push esi\n"
    "  call function_size\n"
    "  pop ecx\n"
    "  mov eax, esi\n"
    "  add eax, ecx\n"
    "  mov edi, eax\n"
    "  dec edi       ; edi points to end of string\n"
    "  shr ecx, 1    ; ecx is count (length/2)\n"
    "  jz reverse.done\n"
    "  reverseLoop:\n"
    "  mov al, [esi] ; load characters\n"
    "  mov bl, [edi]\n"
    "  mov [esi], bl ; and swap\n"
    "  mov [edi], al\n"
    "  inc esi       ; adjust pointers\n"
    "  dec edi\n"
    "  dec ecx       ; and loop\n"
    "  jnz reverseLoop\n"
    "  reverse.done:\n"
    "  mov esp, ebp\n"
    "  pop ebp\n"
    "  ret\n\n"
    ;
    Token Reverse;
    Reverse.makeFunc("reverse");
    Reverse.makePublic();
    Reverse.ParameterAmount = 1;
    Tokens.push_back(Reverse);

    Token reverses;
    reverses.makeString(4);
    reverses.makeName("reverse.s");
    reverses.makePrivate("reverse");
    varbuffer1 += "reverse.s dd 0\n";
    Tokens.push_back(reverses);
    return b;
}


string init_gout(vector <Token> &Tokens)
{
    string b = 
    "function_gout:\n"
    " ;making a function stack frame\n"
    " push ebp\n"
    " mov ebp, esp\n"
    " sub esp, 4\n"
    " ;[ebp +8 ]  ;gout.name\n"
    " push dword [ebp + 8]\n"
    " call function_size\n"
    " pop edx\n"
    " mov eax, 4\n"
    " mov ebx, 1\n"
    " mov ecx, [ebp + 8]\n"
    " int 80h\n"
    " ;making a stack frame end\n"
    " mov esp, ebp\n"
    " pop ebp\n"
    "ret\n"
    ;
    check("size");
    Token gout;
    gout.makeFunc("gout");
    gout.makePublic();
    gout.ParameterAmount = 1;
    Tokens.push_back(gout);

    return b;
}



#endif