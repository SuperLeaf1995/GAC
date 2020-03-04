#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <string>
#include <vector>

using namespace std;

//Identifiers
#define _Number_ (1<<0)
#define _External_ (1<<1)
#define _Type_ (1<<2)
#define _Function_ (1<<3)
#define _Array_ (1<<4)
#define _Condition_ (1<<5)
#define _Operator_ (1<<6)
#define _Returning_ (1<<7)
#define _Call_ (1<<8)
#define _Parameter_ (1<<9)
#define _Parenthesis_ (1<<10)
#define _Constructor_ (1<<11)
#define _Register_ (1<<12)

//ADVANCED
#define _Inheritting_ (1<<13)

#define Task_For_Returning (1<<14)
#define Task_For_Type_Address_Basing (1<<15)
#define Task_For_Type_Address (1<<16)
#define Task_For_Moving_Parameter (1<<17)
#define Task_For_Offsetting (1<<18)
#define Task_For_Remainder (1<<19)
#define Task_For_General_Purpose (1<<20)
#define Task_For_Floating_Math (1<<20)

class Token
{
  public:
    int Flags = 0;
    int Size = 0;
    int Static = 0;
    int Initial_Value = 0;
    int Changable_Value = 0;
    int StackOffset = 0;
    int ParameterCount = 0;
    int ID = 0;
    bool Semanticked = false;
    //Token* Parent = nullptr;
    Token* Left_Side_Token = nullptr;
    Token* Right_Side_Token = nullptr;
    Token* Left_Non_Operative_Token = nullptr;
    Token* Right_Non_Operative_Token = nullptr;
    vector<Token*> Childs;
    string Name = "";
    string Type = "";
    string PreFix_Type = "";
    string UID = "";
    Token(){}
    Token &operator=(const Token& name);
    Token(string name, int size, Token* child) {
        Name = name;
        Size = size;
        Childs.push_back(child);
        Flags |= _Register_;
    }
    Token(string name, int size, Token* child1, Token* child2) {
        Name = name;
        Size = size;
        Childs.push_back(child1);
        Childs.push_back(child2);
        Flags |= _Register_;
    }
    Token(string name, int size) {
        Name = name;
        Size = size;
        Flags |= _Register_;
    }

    bool is(int flag);
    bool Any(int flags);
    string Get_Additive_Operator();
};
#endif

