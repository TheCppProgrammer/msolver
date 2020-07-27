#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32)) && !defined(__CYGWIN__)
#define _WINDOWS_
#else 
#define _UNIX_COLOR_
#endif

#include <string>
#include <string.h>
#include <stack>
#include <vector>
#include <cmath>
#include "main.h"

std::stack<unsigned char> _operator;    //operator stack
std::vector<data> output;    //output queue. A vector is used instead of a queue because vector allows user to get and erase any item using the index

unsigned char toUnchar(char c)
{
    switch (c)
    {
        case '-':
            return 13;    //'-' is a valid operator, 12 is assigned for invalid operators
        case 'T':
            return 11;
        case 'C':
            return 10;
        case 'S':
            return 9;
        case 'N':
            return 8;
        case 'L':
            return 7;
        case '!':
            return 6;
        case '%':
            return 5;
        case '^':
            return 4;
        case '/':
            return 3;
        case 'x':
        case 'X':
        case '*':
            return 2;
        case '+':
            return 1;
        case '(':
            return 0;
        default:
            return 12;
    }
}

inline bool isfunction(unsigned char c)
{
    return c>5;
}

const char* getFuncStr(unsigned char& c)
{
    switch(c)
    {
        case 7:
            return "Log";
        case 8:
            return "Ln";
        case 9:
            return "Sin";
        case 10:
            return "Cos";
        case 11:
            return "Tan";
        default:
            return "The";
    }
}

bool isvalid(std::string &s)
{
    if(s=="."||s=="-."||s=="-")
    {
        s = "";
        return false;
    }
    else 
        return s!="";
}

inline void factorial(double &a)
{
    if(a>170)
    {
        error("Factorial above 170 is not supported\n");
        exit(1);
    }
    else if(a<0)
    {
        warning("Factorial of negative Numbers not allowed, Ignoring\n");
        return;
    }

    int b = (int)a;
    double _result = b;
    for(int i = b-1;i>1;--i)
    {
        _result*=i;
    }

    a = _result;
}

inline void apply(double &a, double &b, unsigned char& c)
{
    switch (c)
    {
        case 1:
            a += b;
            break;
        case 2:
            a *= b;
            break;
        case 3:
            if(b==0)
            {
                error("Divide by Zero error\n");
                exit(1);
            }
            a /= b;
            break;
        case 4:
            a = std::pow(a,b);
            break;
        case 5:
            a = fmod(a,b);
            break;
    }
}

inline void applyfunc(double &a,unsigned char &c)
{
    switch(c){
        case 6 :
            factorial(a);
            break;
        case 7 :
            a = log(a);
            break;
        case 8 :
            a = log(a)/log(e);
            break;
        case 9 :
            a = sin(a);
            break;
        case 10 :
            a = cos(a);
            break;
        case 11 :
            a = tan(a);
            break;
    }
}

std::string eae(unsigned char c,bool &&arg=false)   //eae - Empty Argument Error
{
    std::string err = "Empty Argument in " + std::string(getFuncStr(c))+" Function"+(arg?"":"\n");
    return err;
}

void addOperator(unsigned char o,bool b){
    if(_operator.size())
    while(_operator.size()&&_operator.top()>o)
    {
        if(b&&isfunction(_operator.top()))
        {
            error(eae(_operator.top()).c_str());
            exit(1);
        }
        output.push_back(_operator.top());
        _operator.pop();
    }

    _operator.push(o);
}

void insertStr(std::string a)
{
    bool point = false;
    for(uint16_t i = 0;i<a.length();++i)
    {
        if(a[i]=='.')
        {
            if(point)
                a.erase(i--,1);
            else
                point = true;
        }
    }
    
    output.push_back(0);
    output.back().d = atof(a.c_str());
}

int main(int argc,char** argv)
{
    //check if the number of arguments is not equal to 1
    if(argc != 2)
    {
        showHelp(argv[0]);
        return -1;
    }
    else if(!strcmp(argv[1],"--help") || !strcmp(argv[1],"-h"))
    {
        showHelp(argv[0]);
        return -1;
    }

    const uint16_t len = strlen(argv[1]);    //stores length of the expression
    bool d = true;    //boolean used to allow user to add +/- to constant at appropriate locations
    std::string b = "";    //string to store current constant
    char prev = ' ';    //stores previous valid character
    bool funcBool = false;
    uint16_t pBrach = 0;
    
    for(std::size_t i = 0;i<len;++i){
        char c = argv[1][i];
        switch (c)
        {
            case 48 ... 57:
            case '.':
                //if the previous valid character is ) or !, add * before the number. For e.g : (5+4)2 is parsed as (5+4)*2
                if(prev == ')'|| prev=='!')
                {
                    addOperator(toUnchar('*'),funcBool);
                }
                
                b+=c;
                
                if(c!='.')
                    prev=c;

                if(d)
                    d = false;
                break;
                
            case '+':
            case '-':
                if(d)
                {
                    //never add + character to string
                    if(c=='-')
                        b+=c;
                    d = false;
                    break;
                }
            case 'x':
            case 'X':
            case '*':
            case '/':
            case '^':
            case '%':
            case '!':
                if(d){
                    warning("Operator without any constant before it, Ignoring",i,argv[1]);
                    break;
                }

                if(toUnchar(prev)!=12&&prev!='!'){
                    warning("Multiple Operators Without any constant in between, Ignoring",i,argv[1]);
                    break;
                }

                if(isvalid(b)){
                    if(funcBool)
                        funcBool = false;
                    insertStr(b);
                    b = "";
                }

                if(c == '-')
                {
                    addOperator(toUnchar('+'),funcBool);
                    insertStr("-1");
                    addOperator(toUnchar('*'),funcBool);
                }else
                    addOperator(toUnchar(c),funcBool);
                prev = c;
                break;

                
            case '(':
                d = true;
                if(isvalid(b))
                {
                    if(funcBool)
                        funcBool = false;
                    insertStr(b);
                    b = "";
                    addOperator(toUnchar('*'),funcBool);
                }else if(prev==')'||prev=='!')
                    addOperator(toUnchar('*'),funcBool);
                
                _operator.push(0);
                prev = c;
                ++pBrach;
                break;

            case ')':
                if(pBrach == 0)
                {
                    warning("\')\' without corresponding \'(\', Ignoring",i,argv[1]);
                    break;
                }

                --pBrach;

                if(d)
                    d = false;

                if(isvalid(b))
                {
                    if(funcBool)
                        funcBool = false;
                    insertStr(b);
                    b = "";
                }
                else if(prev=='(')
                {
                    _operator.pop();
                    if(_operator.size())
                    {
                        if(funcBool&&_operator.top()>6)
                        {
                            error(eae(_operator.top(),true).c_str(),i,argv[1]);
                            return -1;
                        }
                        else
                        {
                            _operator.pop();
                        }
                    }
                    if(!output.size())
                        d=true;
                    prev=' ';
                    break;
                }

                if(_operator.size())
                {
                    if((prev<'0'||prev>'9')&&prev!=')'&&!isfunction(prev)&&prev!='(')
                    {
                        warning("Expression Ends with an Operator, Ignoring",i,argv[1]);
                        if(prev=='-')
                        {
                            _operator.pop();
                            if(_operator.size()){
                                _operator.pop();
                            }

                            if(output.size()){
                                output.erase(output.begin()+(output.size()-1));
                            }
                        }else
                            _operator.pop();
                    }

                    while(_operator.size())
                    {
                        if(_operator.top()==0)
                        {
                            _operator.pop();
                            break;
                        }

                        output.push_back(_operator.top());
                        _operator.pop();

                    }

                    if(_operator.size())
                        if(funcBool&&_operator.top()>6)
                        {
                            error(eae(_operator.top(),true).c_str(),i,argv[1]);
                            return -1;
                        }
                }
                
                prev = c;
                break;

            case 'l':
            case 'L':
                if(checkStr("log",argv[1],i))
                {
                    c = 'L';
                    i+=2;
                }
                else if(checkStr("ln",argv[1],i))
                {
                    c = 'N';
                    i+=1;
                }
                else
                {
                    warning("Unhandled character",i,argv[1]);
                    break;
                }
                
                goto functionHandling;
            
            case 'S':
            case 's':                
                if(checkStr("sin",argv[1],i))
                {
                    c = 'S';
                    i+=2;
                }
                else
                {
                    warning("Unhandled character",i,argv[1]);
                    break;
                }
                
                goto functionHandling;
            
            case 'C':
            case 'c':                
                if(checkStr("cos",argv[1],i))
                {
                    c = 'C';
                    i+=2;
                }
                else
                {
                    warning("Unhandled character",i,argv[1]);
                    break;
                }
                
                goto functionHandling;

            case 'T':
            case 't':
                if(checkStr("tan",argv[1],i))
                {
                    c = 'T';
                    i+=2;
                }
                else
                {
                    warning("Unhandled character",i,argv[1]);
                    break;
                }
                
            functionHandling:
                if(prev == ')')
                {
                    addOperator(toUnchar('*'),funcBool);
                    b = "";
                }

                if(isvalid(b))
                {
                    if(funcBool)
                        funcBool = false;
                    insertStr(b);
                    b = "";
                    addOperator(toUnchar('*'),funcBool);
                }

                addOperator(toUnchar(c),funcBool);
                funcBool = true;
                break;

            default:
                if(!isspace(c))
                    warning("Unhandled character",i,argv[1]);
                break;
        }
    }

    if(isvalid(b))
    {
        if(funcBool)
            funcBool = false;
        insertStr(b);
    }

    if(funcBool)
    {
        if(_operator.size())
        {
            while(_operator.size())
            {
                if(_operator.top()>6)
                {
                    error(eae(_operator.top()).c_str());
                    return -1;
                }

                _operator.pop();
            }
        }
        else
            error("Empty Argument in The Function\n");
        return -1;
    }

    if((prev<'0'||prev>'9')&&prev!=')'&&prev!='!'&&prev!=' ')
    {
        warning("Expression Ends with an Operator, Ignoring");
        if(_operator.size())
            _operator.pop();
    }

    while(_operator.size())
    {
        if(_operator.top() == 0)
        {
            warning("\'(\' without corresponding \')\', Ignoring");
            _operator.pop();
            continue;
        }
        output.push_back(_operator.top());
        _operator.pop();
    }

    for(int a = 0;a<output.size();++a)
    {
        if(!output[a].isVar())
        {
            if(isfunction(output[a].c))
            {
                if(a<1)
                {
                    output.erase(output.cbegin());
                    continue;
                }
                    
                applyfunc(output[a-1].d,output[a].c);
                output.erase(output.cbegin()+a);
                --a;
            }
            else
            {
                if(a<2)
                {
                    output.erase(output.cbegin()+a);
                    continue;
                }
                
                apply(output[a-2].d,output[a-1].d,output[a].c);
                output.erase(output.cbegin()+a-1,output.cbegin()+a+1);
                a-=2;
            }
        }
    }

    if(output.size()==1)
    {
        if(!output[0].isVar())
        {
            error("Syntax Error\n");
            return -1;
        }
        
        printf("\nResult    : %f\n",output[0].d);
        return 0;
    }
    else
    {
        error("Syntax Error\n");
        return -1;
    }
}
