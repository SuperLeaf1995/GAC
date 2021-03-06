#ifndef _LEXER_H_
#define _LEXER_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
using namespace std;
vector<string> keyWords;
string includes1 = "";
string codbuffer1;
string varbuffer1 = "\n\nsection .data\n\n";
string texbuffer1 = "\n\nsection .text\n\n";
string bssbuffer1 = "\n\nsection .bss\n\n";
#include "parser.h"
#include "asmTools.h"
vector <clock_t> parsed;

void initializeKeyWords()
{
    FunctionNames.push_back(" ");
    Token TokenNull;
    TokenNull.makeName("nullnullTokennullnull");
    Tokens.push_back(TokenNull);

    setters.insert(make_pair("size", init_size));
    setters.insert(make_pair("malloc", init_malloc));
    setters.insert(make_pair("char", init_char));
    setters.insert(make_pair("num", init_num));
    setters.insert(make_pair("reverse", init_reverse));
    setters.insert(make_pair("gout", init_gout));

}

string readFile(string name)
{
    if (name.length() == 0)
    {
        throw runtime_error("Filename was empty");
    }

    ifstream file(name, ios::ate | ios::binary);

    if (!file.is_open()) 
    {
        throw runtime_error("Couldn't open the file");
    }

    int length = file.tellg();
    file.seekg(0, file.beg);

    char* buffer = new char[length];
    memset(buffer, 0, length);

    file.read(buffer, length);
    file.close();

    return string(buffer);
}

int getWord(char end, string &destination, string source, int continu)
{
    for (;source[continu] == ' ';)
    {
        continu++;
    }
    for (int i = continu; i < source.size();i++)
    {
        if (source[i] != end && source[i] != '\n')
        {
            destination += source[i];
        }
        else
        {
            i++;
            return i;
        }
        
    }

    return source.size();
}

int getError(char end, string &destination, string source, int continu, string &ended)
{
    for (int i = continu; i < source.size();i++)
    {
        if (source[i] != end && source[i] != '\n')
        {
            destination += source[i];
        }
        else
        {
            ended = source[i];
            i++;
            return i;
        }
        
    }

    return source.size();
}

int getReversedIndex(char end, string source, int continu)
{
    int i = continu;
    int index = 0;
    while (i > 0)
    {
        if (source[i] != end)
        {
            i--;
            index++;
        }
        else
        {
            return index;
        }
        
    }
    return 0;
}
int getReversedWord(char end,string &destination, string source, int continu)
{
    int i = continu;
    for (;i > 0;i--)
    {
        if (source[i] != end && source[i] != '\n')
        {
            destination += source[i];
        }
        else
        {
            return i;
        }
        
    }
    return 0;
}

void writeFile(string name, string buffer)
{
    ofstream file(name);

    if (file.is_open()) {
        file.write(buffer.c_str(), buffer.length());
    }
    
    file.close();
}

void lexer(string file, string &outbuffer)
{
    string destination;
    int continu = 0;
    for (int i = 0; i < file.size(); i++)
    {
        destination = "";
        continu = getWord(' ', destination, file, continu);

        if (i != file.size()) 
        {
            clock_t parserTimerS = clock();
            parser(destination, file, continu, varbuffer1, codbuffer1, texbuffer1, includes1, bssbuffer1);
            clock_t parserTimerE = clock();
            parsed.push_back(parserTimerE - parserTimerS);
            destination = "";
            if (includes1.size() > 2)
            {
                file = includes1 + file;
                includes1 = "";
                i = 0;
            }
        }
    }
    outbuffer = includes1 + texbuffer1 + codbuffer1 + varbuffer1 + bssbuffer1;
}

#endif