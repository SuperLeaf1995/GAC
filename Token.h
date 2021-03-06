#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <string>
#include <vector>
using namespace std;

extern int usedregister;
extern string sx();
extern string getFreeMemReg();
extern void getFreeReg();
extern string regbuffer;
extern string codbuffer;
class Token;
extern vector<Token> Tokens;
extern vector<string> FunctionNames;
extern vector<string> Stack;
extern int getIndex(string name);
extern int getFromList(vector<string> v, string wanted);
extern bool isType;

class Token
{
  public:
    void makeVar()
    {
      ifVar = true;
      ifReal = true;
      Size = 4;
    }

    void makePtr()
    {
        ifPointer = true;
        ifReal = true;
        Size = 4;
    }

    void makeName(string name)
    {
        Name = name;
    }

    void makePrivate(string funcName)
    {
        FunctionLabelName = funcName;
        ifReal = true;
        owner = funcName;
    }

    void makePublic()
    {
        ifGlobal = true;
        ifReal = true;
    }

    void makeFunc(string func)
    {
        ifFunction = true;
        Name = func;
        ifReal = true;
    }

    void makeType(string typeName)
    {
        ifType = true;
        ifReal = true;
    }

    void makeMacro()
    {
        ifMacro = true;
        ifReal = true;
    }

    void makeArray(int size)
    {
        Size = size;
        ifArray = true;
        ifReal = true;
    }

    void makeString(int size)
    {
        ifString = true;
        ifReal = true;
        Size = size;
    }

    void makeEqu()
    {
        ifEqu = true;
        ifReal = true;
        Size = 2;
    }

    void linkToReg(string reg)
    {
        ifHasReg = true;
        Reg = reg;
        ifReal = true;
    }

    void eraseReg()
    {
        ifHasReg = false;
        Reg = "";
        ifReal = true;
    }


    string getFullName(string fetcher = "")
    {
        if (ifType == false && ifInStack)
        {
            if (Tokens.at(getIndex(FunctionNames.back())).This == "" || Tokens.at(getIndex(FunctionNames.back())).reason != fetcher)
            {
                //if the class address is deleted in midle of function.
                string reg = getFreeMemReg();
                if (fetcher.size() > 0 && Stack.size() > 0)
                {
                    int fetcherIndex = getFromList(Stack, fetcher);
                    codbuffer += sx() + "mov " + reg + ", [esp + " + to_string( (Stack.size() - fetcherIndex - 1) * 4) + "]\n";
                }
                else
                {
                    codbuffer += sx() + "mov " + reg + ", [ebp + 8]\n";
                }
                Tokens.at(getIndex(FunctionNames.back())).This = reg;
                Tokens.at(getIndex(FunctionNames.back())).reason = fetcher;
            }
            string result;
            string funcThisReg = Tokens.at(getIndex(FunctionNames.back())).This;
            This = funcThisReg;
            result = This + "+ " + to_string(PlaceInStack);
            return result;
        }
        if (ifGlobal)
        {
            return Name;
        }
        else
        {
            return FunctionLabelName + "." + Name;
        }
    }

    void makeLink(Token newLink)
    {
        Links.push_back(newLink);
        ifReal = true;
        ParameterAmount++;
    }

    void addChild(Token &t)
    {
        t.PlaceInStack = PlaceInStack * 4;
        PlaceInStack++;
        t.ifInStack = true;
        t.typeName = Name;
        t.This = this->This;
        this->Size += t.Size;
        Links.push_back(t);
    }

    string getReg(string fetcher = "")
    {
        if (Reg == "")
        {
            getFreeReg();
            Reg = regbuffer;
            if (ifFunction)
            {
                codbuffer += sx() + "pop " + Reg + "\n";
            }
            else if (ifVar)
            {
                codbuffer += sx() + "mov " + Reg + ", dword [" + getFullName(fetcher) + "]\n";
            }
            else if (ifArray)
            {
                codbuffer += sx() + "lea " + Reg + ", [" + getFullName(fetcher) + "]\n";
            }
            
            return Reg;
        }
        else
        {
            if (ifReturner)
            {
                codbuffer += sx() + "pop " + Reg + "\n";
            }
            return Reg;
        }
    }

    void makeReturnable()
    {
        ifReturner = true;
    }

    bool ifReal = false;
    bool ifNumber = false;
    bool ifPointer = false;
    bool ifGlobal = false;
    bool ifType = false;
    bool ifFunction = false;
    bool ifMacro = false;
    bool ifArray = false;
    bool ifVar = false;
    bool ifString = false;
    bool ifEqu = false;
    bool ifHasReg = false;
    bool ifChild = false;
    bool ifReturner = false;
    bool ifInStack = false;
    bool ifMirrored = false;
    string origin = "";
    string typeName = "";
    string owner = "";
    string FunctionLabelName = " ";
    string Name;
    int Size = 0;
    string Reg;
    string Value;
    string This;
    string reason;
    int PlaceInStack = 0;
    int ParameterAmount = 0;
    vector <Token> Links;
};


#endif