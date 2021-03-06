#ifndef _PARSER_H_
#define _PARSER_H_
#include <string>
#include <vector>
#include <map>
#include <ctype.h>
#include <algorithm>
#include "Token.h"
using namespace std;
vector <Token> Tokens;
string parameters;
string varbuffer;
string bssbuffer;
string codbuffer;
string texbuffer;
string includes2;
int usedregister = 0;
int freeMemReg = 0;
string regbuffer;
int inLayer = 0;
int layerId = 0;
int returnLayer = 0;
int savedIfToElse = 0;
int savedElseToEnd = 0;
int funcID = 0;
vector<string> FunctionNames;  //
vector<string> ifToElse;
vector<string> elseToEnd;
vector<string> jumpToEnd;
bool skippedRet = false;
vector<string> whiles;
int WhileID = 0;
vector<string> whileParams;
vector<string> TypeNames;
vector<string> Stack;
vector<string> ThisParameters;

vector<string> settedFuncs;
map <string, string(*)(vector<Token> &)> setters;

bool hasFunctionStackFrame = false;
int framesAmount = 0;
bool isElse = false;
int Syntax = 0;
bool ifReturnValue = false;
bool isIf = false;
bool isType = false;
bool fastMath = false;

string returningDestName;
string paraAmount;

extern int getError(char, string&, string, int, string&);
extern int getWord(char, string&, string, int);
extern int getReversedIndex(char, string, int);
extern int getReversedWord(char, string&, string, int);
extern string readFile(string name);


void check(string name)
{
    bool ifNotsetted = false;
    for (int i = 0; i < settedFuncs.size(); i++)
    {
        if (settedFuncs.at(i) == name)
        {
            ifNotsetted = true;
            break;
        }
    }
    if (ifNotsetted == false && setters.find(name) != setters.end())
    {
        codbuffer1 = setters.find(name)->second(Tokens) + codbuffer1;
        settedFuncs.push_back(name);
    }

}


string sx()
{
    string spaces = "";
    for (int i = 0; i < Syntax; i++)
    {
        spaces += " ";
    }
    return spaces;
}

int getFromList(vector<string> v, string wanted)
{
    for (int i = 0; i < v.size() ; i++)
    {
        if (v.at(i) == wanted)
        {
            return i;
        }
    }
}

int getIndex(string name)
{
    int secondPriority = 0;
    bool ifChild = false;
    if (name.find('.') != -1)
    {
        string type;
        string child;
        int offset = getWord('.', type, name, 0);
        offset = getWord(' ', child, name, offset);
        name = child;
        isType = true;
        ifChild = true;
    }
    for (int i = 0; i < Tokens.size(); i++)
    {
        if (Tokens.at(i).Name == name)
        {
            if (Tokens.at(i).ifGlobal == true)
            {
                secondPriority = i;
            }
            else if (Tokens.at(i).FunctionLabelName != " " && FunctionNames.back() != " " && (Tokens.at(i).owner == FunctionNames.back() || (Tokens.at(getIndex(Tokens.at(i).owner)).ifType && isType)))
            {
                if (ifChild)
                {
                    isType = false;
                }
                return i;
            }
        }
    }
    if (ifChild)
    {
        isType = false;
    }
    return secondPriority;
    cout << name + "doesnt exist!\n";
}

string autoValue(string normalSize, string multiplyer, int layer)
{
    if (multiplyer.size() > 0)
    {
        return normalSize + "*" + multiplyer + "*" + to_string(layer);
    }
    else
    {
        return normalSize;
    }
    
}

void disconnectReg(string reg)
{
    for (int i = 0; i < Tokens.size(); i++)
    {
        if (Tokens.at(i).Reg == reg)
        {
            Tokens.at(i).Reg = "";
        }
        if (Tokens.at(i).This == reg)
        {
            Tokens.at(i).This = "";
        }
    }
}

void getFreeReg()
{
    if (usedregister == 3)
    {
        regbuffer = "edx ";
        usedregister = 0;
        disconnectReg("edx ");
    }
    else if (usedregister == 2)
    {
        regbuffer = "ecx ";
        usedregister = 3;
        disconnectReg("ecx ");
    }
    else if (usedregister == 1)
    {
        regbuffer = "ebx ";
        usedregister = 2;
        disconnectReg("ebx ");
    }
    else if (usedregister == 0)
    {
        regbuffer = "eax ";
        usedregister = 1;
        disconnectReg("eax ");
    }
}

string getFreeMemReg()
{
    if (freeMemReg == 0)
    {
        freeMemReg++;
        disconnectReg("esi ");
        return "esi ";
    }
    else
    {
        freeMemReg--;
        disconnectReg("edi ");
        return "edi ";
    }
}

string autoName(string name, bool isString = false)
{   
    if (Tokens.size() == 0)
    {
        cout << "no Variables exist\n";
        return "";
    }
    else
    {
        return Tokens.at(getIndex(name)).getFullName();
    }
    
}

string addOffset(int &index, string name)
{
    string skip;
    string offset;
    int offsetI;
    index = getWord(' ', skip, parameters, index);
    index = getWord(' ', offset, parameters, index);
    string test;
    offsetI = getWord(' ', test, parameters, index);
    string Reg = "";
    string loopReg = "";
    if (Tokens.at(getIndex(offset)).ifArray)
    {
        loopReg = addOffset(index, offset);
    }
    if (isdigit(offset.at(0)) || offset.at(0) == '-' && offset != "->")
    {
        getFreeReg();
        Reg = regbuffer;
        codbuffer += sx() + "mov " + Reg + ", dword [" + Tokens.at(getIndex(name)).getReg() + " + " + offset + "]\n";
    }
    else if (Tokens.at(getIndex(offset)).ifVar || Tokens.at(getIndex(test)).ifVar)
    {
        if (offset == "->")
        {
            index = offsetI;
            //its this time. :D
            getFreeReg();
            Reg = regbuffer;
            codbuffer += sx() + "mov " + Reg + ", dword [" + Tokens.at(getIndex(name)).getReg() + " + " + Tokens.at(getIndex(test)).getReg() + "]\n";
        }
        else
        {
            //no this time this time :D.
            getFreeReg();
            Reg = regbuffer;
            codbuffer += sx() + "mov " + Reg + ", dword [" + Tokens.at(getIndex(name)).getReg() + " + " + Tokens.at(getIndex(offset)).getReg() + "]\n";
        }
        
    }
    return Reg;
}

void makeVar(int &index)
{
    string name;
    string setting;
    string value;
    index = getWord(' ', name, parameters, index);  // name
    index = getWord(' ', setting, parameters, index);  // = or :
    index = getWord(' ', value, parameters, index);  // value or size
    Token Variable;
    Token Size;
    Variable.makeName(name);
    // check if it is a local var.
    if (FunctionNames.back() == " ")
    {
        //if it is global var.
        Variable.makePublic();
    }
    else
    {
        //if it is private.
        Variable.makePrivate(FunctionNames.back());
        Variable.owner = FunctionNames.back();
    }

    if (setting == ":")
    {
        Variable.makeArray(stoi(value));
        varbuffer += Variable.getFullName() + " times " + value + " dd 0\n";
    }
    else
    {
        Variable.makeVar();
        if (Variable.ifGlobal)
        {
            varbuffer += Variable.getFullName() + " dd " + value + "\n";
        }
        else
        {
            varbuffer += Variable.getFullName() + " dd " + value + "\n";
            getFreeReg();
            codbuffer += sx() + ";Set the value to local var\n";
            codbuffer += sx() + "mov dword [" + Variable.getFullName() + "], " + value + "\n";
        }
        
    }
    if (TypeNames.size() > 0)
    {
        int type = getIndex(TypeNames.back());
        Tokens.at(type).addChild(Variable);
    }
    Tokens.push_back(Variable);
}

void prepareFunction(int &index, string func)
{
    codbuffer += sx() + ";Functions Parameters\n";

    int funcIndex = getIndex(func);
    
    string parameter;
    vector<string> Params;
    for (int i = 0; i < Tokens.at(funcIndex).ParameterAmount; i++)
    {
        string para2;
        string error;
        int offset = getError(',', para2, parameters, index, error);
        if (para2.size() < 1 || para2.find(')') != -1)
        {
            para2 = "";
            offset = getError(')', para2, parameters, index, error);
            if (para2 == "")
            {
                //if no parameters
                index = offset;
                break;
            }
            else if (error == ")")
            {
                index = offset;
            }
        }
        if (error == ",")
        {
            index = offset + 1;
        }
        Params.push_back(para2);
    }
    for (int i = 0; 0 < Params.size(); i++)
    {
        parameter = Params.back();
        Params.pop_back();
        int paraIndex = getIndex(parameter);
        if (parameter.at(0) == '&')
        {
            parameter.erase(parameter.begin());
            int parIndex = getIndex(parameter);
            codbuffer += sx() + "push " + Tokens.at(parIndex).getFullName() + "\n";
        }
        else if (Tokens.at(paraIndex).ifVar)
        {
            codbuffer += sx() + "push dword [" + Tokens.at(paraIndex).getFullName() + "]\n";
        }
        else if (Tokens.at(paraIndex).ifString)
        {
            codbuffer += sx() + "push " + Tokens.at(paraIndex).getFullName() + "\n";
        }
        else if (Tokens.at(paraIndex).ifArray)
        {
            string skip;
            string bPart;
            int offset = getWord(' ', skip, parameters, index);
            if (skip != ":")
            {
                //just give the array as an pointer.
                int parIndex = getIndex(parameter);
                codbuffer += sx() + "push " + Tokens.at(parIndex).getFullName() + "\n";
            }
            else
            {
                index = offset;
                index = getWord(' ', bPart, parameters, index);
                if (isdigit(bPart.at(0)) || bPart.at(0) == '-')
                {
                    //lea esi, a[123 * 4]
                    //push dword [esi]
                    string reg1 = getFreeMemReg();
                    codbuffer += sx() + "lea " + reg1 + ", " + Tokens.at(paraIndex).getFullName() + "[" + bPart + " * 4]\n";
                    codbuffer += sx() + "push dword [" + reg1 + "]\n";
                }
                else
                {
                    //mov edi, dword [index]
                    //lea esi, a[edi * 4]
                    //push dword [esi]
                    string reg1 = getFreeMemReg();
                    string reg2 = getFreeMemReg();
                    int bIndex = getIndex(bPart);
                    codbuffer += sx() + "mov " + reg1 + ", dword [" + Tokens.at(bIndex).getFullName() + "]\n";
                    codbuffer += sx() + "lea " + reg2 + ", " + Tokens.at(paraIndex).getFullName() + "[" + reg1 + "* 4]\n";
                    codbuffer += sx() + "push dword [" + reg2 + "]\n";
                }
            }
            
        }
        
    }
    if (Tokens.at(funcIndex).ifFunction)
    {
        if (ThisParameters.size() > 0)
        {
            codbuffer += "\n" + sx() + ";Giving the function Type address.\n";
            codbuffer += sx() + "push dword [" + ThisParameters.back() + "]\n";
            ThisParameters.pop_back();
        }
        codbuffer += sx() + "\n" + sx() + ";Call the function\n";
        codbuffer += sx() + "call function_" + Tokens.at(funcIndex).getFullName() + "\n";
        if (Tokens.at(funcIndex).ifReturner == false)
        {
            codbuffer += sx() + ";deleteing the parameters from stack\n";
            codbuffer += sx() + "add esp, " + to_string(Tokens.at(funcIndex).ParameterAmount * 4) + "\n";
        }
        codbuffer += "\n";
    }
    else if (Tokens.at(funcIndex).ifMacro)
    { 
        codbuffer += Tokens.at(funcIndex).getFullName() + "\n";
    }
    else
    {
        cout << sx() + "uknown Function type :c\n";
    }
    
}

string arrayInitialization(int &index, int destIndex)
{
    // get the ":" and the "index".
    string skip;
    string arrayIndex;
    index = getWord(' ', skip, parameters, index);
    index = getWord(' ', arrayIndex, parameters, index);
    if (isdigit(arrayIndex.at(0)) || arrayIndex.at(0) == '-')
    {
        //lea esi, a[123 * 4]
        //push dword [esi]
        string reg1 = getFreeMemReg();
        codbuffer += sx() + "lea " + reg1 + ", " + Tokens.at(destIndex).getFullName() + "[" + arrayIndex + " * 4]\n";
        return reg1;
    }
    else
    {
        //mov edi, dword [banana]
        //lea esi, a[edi * 4]
        //push dword [esi]
        string reg1 = getFreeMemReg();
        int indexIndex = getIndex(arrayIndex);
        codbuffer += sx() + "mov " + reg1 + ", dword [" + Tokens.at(indexIndex).getFullName() + "]\n";
        codbuffer += sx() + "lea " + reg1 + ", " + Tokens.at(destIndex).getFullName() + "[" + reg1 + "* 4]\n";
        return reg1;
    }
}

void makeInitialDestiantion(int &index, string dest)
{
    int destIndex;
    if (dest.size() < 1)
    {
        return;
    }
    codbuffer += sx() + ";The inital destination\n";
    if (dest.find('.') != -1)
    {
        codbuffer += sx() + ";put to stack the class address\n";
        string type;
        string child;
        int offset = getWord('.', type, dest, 0);
        string Reg = getFreeMemReg();
        offset = getWord(' ', child, dest, offset);
        codbuffer += sx() + "lea " + Reg + ", [" + Tokens.at(getIndex(type)).getFullName() + "]\n";
        codbuffer += sx() + "push dword [" + Reg + "+ " + to_string(Tokens.at(getIndex(child)).PlaceInStack) + "]\n";
        Stack.push_back(child);
        return; 
    }
    else
    {
        destIndex = getIndex(dest);
    }
    if (dest.at(0) == '&')
    {
        dest.erase(dest.begin());
        int destIndex2 = getIndex(dest);
        codbuffer += sx() + "push dword [" + Tokens.at(destIndex2).getFullName() + "]\n";
    }
    else if (Tokens.at(destIndex).ifInStack && Tokens.at(destIndex).ifType == false && isType)
    {
        string reg = getFreeMemReg();
        codbuffer += sx() + "lea " + reg + ", [" + Tokens.at(destIndex).getFullName() + "]\n";
        codbuffer += sx() + "push " + reg + "\n";
    }
    else if (Tokens.at(destIndex).ifVar)
    {
        codbuffer += sx() + "push " + Tokens.at(destIndex).getFullName() + "\n";
    }
    else if (Tokens.at(destIndex).ifArray)
    {
        codbuffer += sx() + "push " + arrayInitialization(index, destIndex) + "\n";
    }
    else if (Tokens.at(destIndex).ifFunction)
    {
        // get the parameter's for the function.
        prepareFunction(index, dest);
    }
    else
    {
        cout << "bad destination: " + dest + "\n";
    }
    codbuffer += sx() + "\n";
    Stack.push_back(dest);
}

void getInitalDestination(int &index, string destReg, bool sameParaAsDest)
{
    string memReg = getFreeMemReg();
    codbuffer += "\n" + sx() + ";Get the destination to: " + memReg + "\n";
    codbuffer += sx() + "pop " + memReg + "\n";
    codbuffer += sx() + "mov [" + memReg + "], " + destReg + "\n\n";
    if (sameParaAsDest == false)
    {
        disconnectReg(destReg);
    }
    Stack.pop_back();
}

void callFunction(string function, int &index)
{
    prepareFunction(index, function);
}

void doReturn()
{
    bool waselse = false;
    bool wasif = false;
    bool secondphase = false;
    if (ifReturnValue)
    {
        ifReturnValue = false;
        secondphase = true;
        return;
    }
    else
    {
        codbuffer += "\n";
        codbuffer += sx() + ";making a stack frame end\n";
        codbuffer += sx() + "mov esp, ebp\n" + sx() + "pop ebp\n";
    }
    Syntax--;
    if (isElse)
    {
        codbuffer += elseToEnd.back();
        elseToEnd.pop_back();
        inLayer--;
        isElse = false;
        waselse = true;
    }
    else if (isIf)
    {
        isIf = false;
        wasif = true;
    }
    else if (whiles.size() > 0)
    {
        string a = whileParams.back();
        string Atype;
        string Btype;
        int offset = getWord('.', Atype, a, 0);
        whileParams.pop_back();
        string b = whileParams.back();
        offset = getWord('.', Btype, b, 0);
        whileParams.pop_back();
        codbuffer += "\n" + sx() + ";cheking the while.\n";
        
        codbuffer += sx() + "add dword [" + Tokens.at(getIndex(a)).getFullName() + "], 1\n";
        codbuffer += sx() + "cmp " + Tokens.at(getIndex(a)).getReg() + ", dword [" + Tokens.at(getIndex(b)).getFullName() + "]\n";
        codbuffer += sx() + "jl " + whiles.back() + "\n\n";
        whiles.pop_back();
    }
    else if (framesAmount < 3 && isType || framesAmount == 1 && secondphase == false && waselse == false && wasif == false)
    {
        codbuffer += sx() +  "ret\n\n";
        Syntax = 0;
        disconnectReg("eax ");
        disconnectReg("ebx ");
        disconnectReg("ecx ");
        disconnectReg("edx ");
        disconnectReg("esi ");
        disconnectReg("edi ");
        if (isType && framesAmount == 1)
        {
            varbuffer += TypeNames.back() + "_end:\n";
            TypeNames.pop_back();
            isType = false;
            FunctionNames.pop_back();
        }
        else
        {
            varbuffer += "endVariables_" + FunctionNames.back() + ":\n\n";
        }
        funcID++;
        FunctionNames.pop_back();
    }
    else
    {
        codbuffer += "\n";
    }
}

void doMath(int &index, string a, string math, string destination)
{
    codbuffer += sx() + ";Math do: " + math + "\n";
    string b;
    index = getWord(' ', b, parameters, index);
    //a +/*- b
    if (a == "->")
    {
        a = "";
        index = getWord(' ', a, parameters, index);
    }
    if (b == "->")
    {
        b = "";
        index = getWord(' ', b, parameters, index);
    }
    else if (b.find('.') != -1)
    {

    }
    string Aowner;
    string Bowner;
    int offset1 = getWord('.', Aowner, a, 0);
    offset1 = getWord('.', Bowner, b, 0);
    int aI = getIndex(a);
    int bI = getIndex(b);
    string opCode;
    if (math == "+")
    {
        opCode = "add ";
    }
    else if (math == "-")
    {
        opCode = "sub ";
    }
    else if (math == "/")
    {
        opCode = "idiv ";
    }
    else if (math == "%")
    {
        opCode = "idiv ";
    }
    else
    {
        opCode = "imul ";
    }
    if (Tokens.at(bI).ifFunction)
    {
        prepareFunction(index, b);
    }
    if (opCode != "idiv ")
    {
        codbuffer += sx() + opCode + Tokens.at(aI).getReg(Aowner) + ", " + Tokens.at(bI).getReg(Bowner) + "\n";
    }
    else if (math == "%")
    {
        disconnectReg("eax ");
        disconnectReg("edx ");
        codbuffer += sx() + "xor edx, edx\n";
        codbuffer += sx() + "mov eax, dword [" + Tokens.at(aI).getFullName(Aowner) + "]\n";
        codbuffer += sx() + opCode + "dword [" + Tokens.at(bI).getFullName(Bowner) + "]\n";
        if (b == destination)
        {
            getInitalDestination(index, "edx ", true);
        }
        else
        {
            getInitalDestination(index, "edx ", false);
        }
        fastMath = true;
    }
    else
    {
        disconnectReg("eax ");
        disconnectReg("edx ");
        codbuffer += sx() + "xor edx, edx\n";
        codbuffer += sx() + "mov eax, dword [" + Tokens.at(aI).getFullName(Aowner) + "]\n";
        codbuffer += sx() + opCode + "dword [" + Tokens.at(bI).getFullName(Bowner) + "]\n";
        if (b == destination)
        {
            getInitalDestination(index, "eax ", true);
        }
        else
        {
            getInitalDestination(index, "eax ", false);
        }
        fastMath = true;
    }
    
    
    //check if there is more math to do.
    math = "";
    int offset = getWord(' ', math, parameters, index);
    if (math == "\n" || math == " " || math == ")")
    {
        return;
    }
    else if (math == "+" || math == "-" || math == "/" || math == "*" || math == "%")
    {
        //this means that math exist on this same line of code :D.
        //so lets make it.
        index = offset;
        doMath(index, a, math, destination);
    }
}

bool ifreturnAddress(int &index, string name)
{
    if (name.size() > 0 && name.at(0) == '&')
    {
        string name2;
        int offset = getWord('(', name2, name, 0);
        name = name2;
        name.erase(name.begin());
        int funcIndex = getIndex(name);
        //return just func location as ptr.
        getFreeReg();
        string reg = regbuffer;
        codbuffer += sx() + "lea " + reg + ", [function_" + Tokens.at(funcIndex).getFullName() + "]\n";
        getInitalDestination(index, reg, false);
        return true;
    }
    else
    {
        return false;
    }
}

void useVar(int &index, string destination)
{
    codbuffer += "\n"; //giving some readability
    //save the destination to stack.

    makeInitialDestiantion(index, destination);
    //skip the = mark.
    string skip;
    index = getWord(' ', skip, parameters, index);
    //start the math check.
    string bPart;
    index = getWord(' ', bPart, parameters, index);
    string destTest;
    bool ifAddressReturningFunction = false;


    if (bPart.find('(') != -1)
    {
        int offset = getWord('(', destTest, bPart, 0);
        check(destTest);
        if (int i = getIndex(destTest) != 0)
        {
            int recruit = getReversedIndex('(', bPart, bPart.size());
            bPart = destTest;
            index -= recruit;
        }
    }

    if (bPart == "->")
    {
        bPart = "";
        index = getWord(' ', bPart, parameters, index);
    }

    int bIndex = getIndex(bPart);


    string math;
    int offset = getWord(' ', math, parameters, index);
    if (Tokens.at(bIndex).ifFunction != true)
    {
        if (math == "+" || math == "-" || math == "/" || math == "*" || math == "%")
        {
            //this means that math exist on this same line of code :D.
            //so lets make it.
            index = offset;
            doMath(index, bPart, math, destination);
        }
    }

    // check if B part is a function
    ifAddressReturningFunction = ifreturnAddress(index, bPart);
    if (ifAddressReturningFunction)
    {
        return;
    }

    if (Tokens.at(bIndex).ifFunction)
    {
        prepareFunction(index, bPart);
        string secondareMath;
        offset = getWord(' ', secondareMath, parameters, index);
        if (secondareMath == "+" || secondareMath == "-" || secondareMath == "/" || secondareMath == "*" || secondareMath == "%")
        {
            //this means that math exist on this same line of code :D.
            //so lets make it.
            index = offset;
            doMath(index, bPart, secondareMath, destination);
        }
    }

    //if B part is a array
    if (math == ":")
    {
        if (bPart == destination)
        {
            getInitalDestination(index, addOffset(index, bPart), true);
        }
        else
        {
            getInitalDestination(index, addOffset(index, bPart), false);
        }
        
        return;
    }

    //load the inital destination from stack and give it the inital sum.
        if (fastMath)
        {
            fastMath = false;
        }
        else if (ifAddressReturningFunction)
        {
            return;
        }
        else if (bPart == destination)
        {
            getInitalDestination(index, Tokens.at(bIndex).getReg(), true);
        }
        else
        {
            getInitalDestination(index, Tokens.at(bIndex).getReg(), false);
        }
}

void makeFunc(int &index)
{
    string para1;
    index = getWord('(', para1, parameters, index);
    if (TypeNames.size() > 0 && TypeNames.back() == para1)
    {
        codbuffer += "type_" + para1 + ":\n";
    }
    else
    {
        codbuffer += sx() + "function_" + para1 + ":\n";
        varbuffer += "\nstartVariables_" + para1 + ":\n";
    }
    
    Syntax++;
    Token func;
    func.makeFunc(para1);
    func.makePublic();

    
    returnLayer++;
    vector<string> paraOrder;
    string reg1 = regbuffer;
    int i = 0;
    while (true)
    {
        string para2;
        string error;
        int offset = getError(',', para2, parameters, index, error);
        if (para2.size() < 1 || para2.find(')') != -1)
        {
            para2 = "";
            offset = getError(')', para2, parameters, index, error);
            if (para2 == "")
            {
                //if no parameters
                func.ParameterAmount = 0;
                index = offset;
                break;
            }
        }
        else
        {
            offset++; // skip the " " after the ,
        }
        Token Parameter;
        Parameter.makeName(para2);
        Parameter.makePrivate(para1);
        if (para2.length() > 0) 
        {
            if (para2.find('&')!= string::npos)
            {
                Parameter.makePtr();
            }
            else
            {
                Parameter.makeVar();
            }
        }
        if (error == "\n")
        {
            if (para2 != "(")
            {
                func.makeLink(Parameter);
                index = offset;
            }
            break;
        }
        else if (error == ")")
        {
            index = offset;
            func.makeLink(Parameter);
            break;
        }
        else
        {
            func.makeLink(Parameter);
            index = offset;
        }
        i++;
    }
    codbuffer += sx() + ";making a function stack frame\n";
    codbuffer += sx() + "push ebp\n" + sx() + "mov ebp, esp\n\n";
    if (func.ParameterAmount > 0)
    {
        codbuffer += sx() + "sub esp, " + to_string(func.ParameterAmount * 4) + "\n";
    }
    hasFunctionStackFrame = true;
    FunctionNames.push_back(para1);

    for (int i = 0; i < func.ParameterAmount; i++)
    {
        getFreeReg();
        string reg2 = regbuffer;
        string para3 = func.Links.at(i).Name;
        if (func.Links.at(i).ifPointer)
        {
            para3.erase(para3.begin());
        }


        Token child;
        child.makeName(para3);
        child.ifChild = true;
        child.owner = para1;
        child.Reg = reg2;
        codbuffer += sx() + "\n";
        if (para3 == "this")
        {
            codbuffer += sx() + ";" + para3 + " is CLASS address.\n";
            string addreg = getFreeMemReg();
            func.This = addreg;
            codbuffer += sx() + "mov " + addreg + ", [ebp + 8]\n";
        }
        else if (func.Links.at(i).ifPointer)
        {
            string memreg = getFreeMemReg();
            codbuffer += sx() + ";" + para3 + " is now an Pointer.\n";
            codbuffer += sx() + "mov " + memreg + ", [ebp + " + to_string(4 * i + 8) + "]\n";
            codbuffer += sx() + "lea " + memreg + ", [" + memreg + "]\n";
            child.makePtr();
        }
        else
        {
            codbuffer += sx() + ";" + para3 + " is now an Variable.\n";
            codbuffer += sx() + "mov " + reg2 + ", [ebp +" + to_string(4 * i + 8) + "]\n";
            child.makeVar();
        }
        if (para3 == "this")
        {

        }
        else
        {
            varbuffer += para1 + "." + para3 + " dd 0\n";

            child.makePrivate(para1);
            codbuffer += sx() + "mov [" + child.getFullName() + "], " + reg2 + "\n";
            Tokens.push_back(child);
        }
    }
    paraAmount = to_string(func.ParameterAmount * 4);

    Tokens.push_back(func);
}

string getJump(string condition)
{
    if (condition == "==")
    {
        return "jne ";
    }
    if (condition == "!=")
    {
        return "je ";
    }
    if (condition == ">")
    {
        return "jle ";
    }
    if (condition == "<")
    {
        return "jge ";
    }
    if (condition == "!>" || condition == ">!")
    {
        return "jg ";
    }
    if (condition == "!<" || condition == "<!")
    {
        return "jl ";
    }
    if (condition == "=>" || condition == ">=")
    {
        return "jl ";
    }
    if (condition == "=<" || condition == "<=")
    {
        return "jg ";
    }
    else
    {
        return "jmp ";
    }
}

void doComparing(int &i)
{
    isIf = true;
    string a;  //a
    string condition;  // ==
    string b;  //b
    string acomp;
    string bcomp;
    i = getWord(' ', a, parameters, i);
    int aI = getIndex(a);
    if (Tokens.at(aI).ifArray)
    {
        getFreeReg();
        string reg1 = regbuffer;
        codbuffer += sx() + "mov " + reg1 + ", dword [" + arrayInitialization(i, aI) + "]\n";
        acomp = reg1;
    }
    else
    {
        string type;
        int offset = getWord('.', type, a, 0);
        acomp = Tokens.at(aI).getReg();
    }
    
    i = getWord(' ', condition, parameters, i);
    i = getWord(' ', b, parameters, i);
    int bI = getIndex(b);
    if (Tokens.at(bI).ifArray)
    {
        bcomp = "dword [" + arrayInitialization(i, bI) + "]";
    }
    else
    {
        string type;
        int offset = getWord('.', type, b, 0);
        bcomp = "dword " + Tokens.at(bI).getFullName();
    }
    condition = getJump(condition);
    
    codbuffer += sx() +   "cmp " + acomp + ", " + bcomp + "\n";
    
    inLayer++;
    layerId++;
    codbuffer += sx() + condition + "else" + to_string(inLayer) + to_string(layerId) + "\n";
    ifToElse.push_back(sx() + "else" + to_string(inLayer) + to_string(layerId) + ":\n");
    elseToEnd.push_back(sx() + "end" + to_string(inLayer) + to_string(layerId) + ":\n");
    jumpToEnd.push_back(sx() + "jmp end" + to_string(inLayer) + to_string(layerId) + "\n");
    savedIfToElse++;
    savedElseToEnd++;
    Syntax++;
}

void doElse()
{
    savedIfToElse--;
    savedElseToEnd--;
    codbuffer += jumpToEnd.back();
    codbuffer += ifToElse.back();
    jumpToEnd.pop_back();
    ifToElse.pop_back();
    isElse = true;
    Syntax++;
}

bool replace(string& str, const string& from, const string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void doInclude(int &i)
{
    int offset;
    string including;
    //string name;
    i -= 4;
    i = getWord('\n', including, parameters, i);
    replace(parameters, including, "");
    replace(including, "use ", "");
    includes2 = readFile(including);
    i = 0;
}

void makeNewType(int &index)
{
    string name;
    int offset = getWord('(', name, parameters, index);
    varbuffer += "\n" + name + ":\n";
    TypeNames.push_back(name);
    makeFunc(index);
    Token type;
    type = Tokens.at(getIndex(name));
    Tokens.erase(Tokens.begin()+getIndex(name));
    type.makeType(name);
    type.makePublic();
    isType = true;
    Tokens.push_back(type);
}

void makeNew(int &index)
{
    string type;
    string name;
    string ifStatic;
    bool isStatic = false;
    int offset = getWord(' ', ifStatic, parameters, index);
    if (ifStatic == "static")
    {
        index = offset;
        isStatic = true;
    }
    index = getWord(' ', type, parameters, index);
    index = getWord(' ', name, parameters, index);
    Token newType = Tokens.at(getIndex(type));
    if (isStatic)
    {
        //if making an new static class.
        //aka. total copy of the original class.
    }
    else
    {
        //dynamic malloc class.
        //in runtime made class.
        Token ptr;
        ptr.makeName(name);
        ptr.ifMirrored = true;
        ptr.origin = type;
        if (FunctionNames.size() > 0 && FunctionNames.back() == " ")
        {
            //it is public.
            ptr.makePublic();
        }
        else
        {
            //it is private.
            ptr.makePrivate(FunctionNames.back());
        }
        ptr.makePtr();
        varbuffer += ptr.getFullName() + " dd 0\n";
        //get the template class size.
        codbuffer += sx() + ";Give malloc Type size.\n";
        codbuffer += sx() + "push " + to_string(Tokens.at(getIndex(type)).Size) + "\n\n";
        codbuffer += sx() + ";Call malloc.\n";
        check("malloc");
        codbuffer += sx() + "call function_malloc\n\n";
        codbuffer += sx() + "pop dword [" + ptr.getFullName() + "]\n";
        codbuffer += sx() + ";deleteing the parameters from stack\n";
        codbuffer += sx() + "add esp, 4\n";
        codbuffer += sx() + ";Save new Type address in stack at(" + to_string(Stack.size() * 4) + ")\n";
        codbuffer += sx() + "push dword [" + ptr.getFullName() + "]\n\n";
        Tokens.push_back(ptr);
        Stack.push_back(ptr.Name);
    }
    
}

void doInterruption(int &index)
{
    string eax;
    string ebx;
    string ecx;
    string edx;
    string callingnumber;
    string carry;
    index = getWord(' ', eax, parameters, index);
    index = getWord(' ', ebx, parameters, index);
    index = getWord(' ', ecx, parameters, index);
    index = getWord(' ', edx, parameters, index);
    index = getWord(' ', callingnumber, parameters, index);
    index = getWord(' ', carry, parameters, index);
    int aI = getIndex(eax);
    int bI = getIndex(ebx);
    int cI = getIndex(ecx);
    int dI = getIndex(edx);
    codbuffer += "\n" + sx() + "push eax\n";
    if (isdigit(eax.at(0)) || eax.at(0) == '-')
    {
        codbuffer += sx() + "mov eax, " + eax + "\n";
    }
    else
    {
        codbuffer += sx() + "mov eax, dword [" + Tokens.at(aI).getFullName() + "]\n";
    }
    if (isdigit(ebx.at(0)) || ebx.at(0) == '-')
    {
        codbuffer += sx() + "mov ebx, " + ebx + "\n";
    }
    else
    {
        codbuffer += sx() + "mov ebx, dword [" + Tokens.at(bI).getFullName() + "]\n";
    }
    if (isdigit(ecx.at(0)) || ecx.at(0) == '-')
    {
        codbuffer += sx() + "mov ecx, " + ecx + "\n";
    }
    else
    {
        codbuffer += sx() + "mov ecx, dword [" + Tokens.at(cI).getFullName() + "]\n";
    }
    if (isdigit(edx.at(0)) || edx.at(0) == '-')
    {
        codbuffer += sx() + "mov edx, " + edx + "\n";
    }
    else
    {
        codbuffer += sx() + "mov edx, dword [" + Tokens.at(dI).getFullName() + "]\n";
    }
    
    codbuffer += sx() + "int " + callingnumber + "\n";
    codbuffer += sx() + "mov [" + carry + "], eax\n";
    codbuffer += sx() + "pop eax\n\n";
}

void makeNewString(int &index)
{
    string name;
    string is;
    string str;
    index = getWord(' ', name, parameters, index);
    index = getWord(' ', is, parameters, index);
    Token String;
    String.makeName(name);
    int Size = 0;
    if (FunctionNames.back() == " ")
    {
        String.makePublic();
    }
    else
    {
        String.makePrivate(FunctionNames.back());
    }
    if (is != ":")
    {
        index = getWord('"', str, parameters, index);
        index = getWord('"', str, parameters, index);
        varbuffer += String.getFullName() + " db \"" + str + "\", 0\n";
        Size = str.size();
    }
    else
    {
        index = getWord(' ', str, parameters, index);
        varbuffer += String.getFullName() + " times " + str + " db 1\n";
        varbuffer += " dd 0\n";
        Size = str.size();
    }
    String.makeString(Size);
    Tokens.push_back(String);
}

void useStr(int &index, string destination)
{
    string command;
    string b;

    index = getWord(' ', command, parameters, index);
    index = getWord(' ', b, parameters, index);
    int destI = getIndex(destination);
    int bI = getIndex(b);

    if (Tokens.at(bI).ifString)
    {
        codbuffer += "\n" + sx() + ";Load the destination.\n";
        codbuffer += sx() + "lea edi, [" + Tokens.at(destI).getFullName() + "]\n";
        codbuffer += sx() + ";Load the source.\n";
        codbuffer += sx() + "lea esi, [" + Tokens.at(bI).getFullName() + "]\n";
        codbuffer += sx() + "push esi\n";
    }
    else
    {
        codbuffer += "\n" + sx() + ";Load the destination.\n";
        codbuffer += sx() + "lea edi, [" + Tokens.at(destI).getFullName() + "]\n";
        codbuffer += sx() + ";Load the source.\n";
        prepareFunction(index, b);
    }

    codbuffer += "\n" + sx() + ";Load the destination.\n";
    codbuffer += sx() + "call len\n";
    codbuffer += sx() + "pop ecx\n";
    codbuffer += sx() + "repnz movsb \n\n";
}

void returnValue(int &index)
{
    string dest;
    index = getWord(' ', dest, parameters, index);
    if (dest == "->")
    {
        dest = "";
        index = getWord(' ', dest, parameters, index);
    }
    int destIndex = getIndex(dest);
    codbuffer += sx() + ";returning from stack frame\n";
    codbuffer += sx() + "mov esp, ebp\n" + sx() + "pop ebp\n\n";
    codbuffer += sx() + ";returning a value from function\n";
    codbuffer += sx() + "pop eax\n";
    codbuffer += sx() + "push dword [" + Tokens.at(destIndex).getFullName() + "]\n";
    Syntax--;
    codbuffer += sx() + "jmp eax\n\n";
    ifReturnValue = true;
    int func = getIndex(FunctionNames.back());
    Tokens.at(func).makeReturnable();
    varbuffer += "endVariables_" + FunctionNames.back() + ":\n\n";
    funcID++;
}

void While(int &index)
{
    string skip;
    string a;
    string b;
    index = getWord('(', skip, parameters, index);
    index = getWord(' ', a, parameters, index);
    index = getWord(':', skip, parameters, index);
    index = getWord(')', b, parameters, index);
    /*codbuffer += sx() + "push dword [" + a + "]\n";

    if (isdigit(b.at(0)) || b.at(0) == '-')
    {
        codbuffer += sx() + "push dword " + b + "\n";
    }
    else
    {
        codbuffer += sx() + "push dword [" + b + "]\n";
    }*/

    whileParams.push_back(b);
    whileParams.push_back(a);

    codbuffer += sx() + "While_" + to_string(WhileID) + ":\n";
    whiles.push_back("While_" + to_string(WhileID));
    WhileID++;
}

void getThis(int &index)
{
    string name;
    index = getWord(' ', name, parameters, index);
    useVar(index, name);
}

void TypeFetch(int &index, string statement)
{
    string type;
    string child;
    int offset = getWord('.', type, statement, 0);
    int childIndex = offset;
    offset = getWord(' ', child, statement, offset);
    if (Tokens.at(getIndex(child)).ifFunction)
    {
        child = "";
        offset = getWord('(', child, statement, childIndex);
        ThisParameters.push_back(Tokens.at(getIndex(type)).getFullName());
        prepareFunction(index, child);
    }
    else if (Tokens.at(getIndex(child)).ifVar)
    {
        useVar(index, statement);
    }
    else if (Tokens.at(getIndex(child)).ifArray)
    {

    }
}


void parser(string destination, string &file, int &continu, string &varbuffer1, string &codbuffer1, string &texbuffer1, string &includes1, string &bssbuffer1)
{
    codbuffer = "";
    texbuffer = "";
    varbuffer = "";
    bssbuffer = "";
    parameters = file;
    string destTest;

    if (destination.find('(') != -1)
    {
        int offset = getWord('(', destTest, destination, 0);
        check(destTest);
        if (int i = getIndex(destTest) != 0)
        {
            int recruit = getReversedIndex('(', destination, destination.size());
            destination = destTest;
            continu -= recruit;
        }
    }

    int dest = getIndex(destination);
    if (destination.size() > 0 && destination.at(0) == '#')
    {
        string comment;
        continu = getWord('\n', comment, parameters, continu);
        destination.at(0) = ' ';
        //codbuffer += "\n" + sx() + ";-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_;\n";
        codbuffer += "\n" + sx() + ";usr::" + destination + ' ' + comment + "\n";
        //codbuffer += sx() + ";-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_;\n";
    }
    else if (destination == "return")
    {
        returnValue(continu);
    }
    else if (destination == "var")
    {
        makeVar(continu);
    }
    else if (destination == "type")
    {
        makeNewType(continu);
    }
    else if (destination == "func")
    {
        makeFunc(continu);
    }
    else if (destination == ")")
    {
        doReturn();
        framesAmount--;
    }
    else if (destination == "(")
    {
        if (hasFunctionStackFrame)
        {
            hasFunctionStackFrame = false;
        }
        else
        {
            codbuffer += "\n" + sx() + ";making a stack frame start\n";
            codbuffer += sx() + "push ebp\n" + sx() + "mov ebp, esp\n\n";
        }
        framesAmount++;
    }
    else if (destination == "if")
    {
        doComparing(continu);
    }
    else if (destination == "else")
    {
        doElse();
    }
    else if (destination == "while")
    {
        While(continu);
    }
    else if (destination == "use")
    {
        doInclude(continu);
    }
    else if (destination == "new")
    {
        makeNew(continu);
    }
    else if (destination == "sys")
    {
        doInterruption(continu);
    }
    else if (destination == "str")
    {
        makeNewString(continu);
    }
    else if (destination == "->")
    {
        getThis(continu);
    }
    else if (Tokens.at(dest).ifString)
    {
        useStr(continu, destination);
    }
    else if (Tokens.at(dest).ifVar || Tokens.at(dest).ifArray) 
    {
        useVar(continu, destination);
    }
    else if (destination.find('.') != -1)
    {
        TypeFetch(continu, destination);
    }
    else if (Tokens.at(dest).ifFunction)
    {
        check(destination);
        callFunction(destination, continu);
    }

    file = parameters;
    varbuffer1 += varbuffer;
    bssbuffer1 += bssbuffer;
    codbuffer1 += codbuffer;
    texbuffer1 += texbuffer;
    includes1 += includes2;
    includes2 = "";
}

#endif