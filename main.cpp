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
        case 'T':
            return 12;
            break;
        case 'C':
            return 11;
            break;
        case 'S':
            return 10;
            break;
        case 'N':
            return 9;
            break;
        case 'L':
            return 8;
            break;
        case '!':
            return 7;
            break;
        case '%':
            return 6;
            break;
        case '^':
            return 5;
            break;
        case '/':
            return 4;
            break;
        case 'x':
        case 'X':
        case '*':
            return 3;
            break;
        case '+':
            return 2;
            break;
        case '-':
            return 1;
            break;
        case '(':
            return 0;
            break;
        default:
            return 13;
            break;
    }
}

inline bool isfunction(unsigned char c){
    return c>6;
}

bool isvalid(std::string &s)
{
    if(s=="."||s=="-.")
    {
        s = "";
        return false;
    }
    else 
        return s!="";
}

inline void factorial(double *a){
    if(*a>170){
        error("Factorial above 170 is not supported\n");
        exit(1);
    }else if(*a<0){
        warning("Factorial of negative Numbers not allowed, Ignoring\n");
        return;
    }

    int b = (int)*a;
    double _result = b;
    for(int i = b-1;i>1;--i){
        _result*=i;
    }
    *a = _result;
}

inline void apply(double *a, double *b, unsigned char& c)
{
    switch (c)
    {
        case 1:
            *a -= *b;
            break;
        case 2:
            *a += *b;
            break;
        case 3:
            *a *= *b;
            break;
        case 4:
            if(*b==0)
            {
                error("Divide by Zero error\n");
                exit(1);
            }
            *a /= *b;
            break;
        case 5:
            *a = std::pow(*a,*b);
            break;
        case 6:
            *a = fmod(*a,*b);
            break;
    }
}

inline void applyfunc(double *a,unsigned char &c){
    switch(c){
        case 7 :
            factorial(a);
            break;
        case 8 :
            *a = log(*a);
            break;
        case 9 :
            *a = log(*a)/log(e);
            break;
        case 10 :
            *a = sin(*a);
            break;
        case 11 :
            *a = cos(*a);
            break;
        case 12 :
            *a = tan(*a);
            break;
    }
}

void addOperator(unsigned char o){
    if(_operator.size())
    while(_operator.size()&&_operator.top()>o){
        output.push_back(_operator.top());
        _operator.pop();
    }

    _operator.push(o);
}

void insertStr(std::string a)
{
    bool point = false;
    for(std::size_t i = 0;i<a.length();++i)
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
    *(output.back().d) = atof(a.c_str());
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

    const std::size_t len = strlen(argv[1]);    //stores length of the expression
    bool d = true;    //boolean used to allow user to add +/- to constant at appropriate locations
    std::string b = "";    //string to store current constant
    char prev = ' ';    //stores previous valid character
    
    for(std::size_t i = 0;i<len;++i){
        char c = argv[1][i];
        switch (c)
        {
            case 48 ... 57:
                prev = c;
            case '.':
                //if the previous valid character is ) or !, add * before the number. For e.g : (5+4)2 is parsed as (5+4)*2
                if(prev == ')'|| prev=='!')
                {
                    addOperator(toUnchar('*'));
                }
                
                b+=c;
                
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
            case '!':
                if(d){
                    warning("Operator without any constant before it",i,argv[1]);
                    break;
                }

                if(toUnchar(prev)!=13){
                    warning("Multiple Operators Without any constant in between",i,argv[1]);
                    break;
                }

                if(isvalid(b)){
                    insertStr(b);
                    b = "";
                }
                addOperator(toUnchar(c));
                prev = c;
                break;

                
            case '(':
                d = true;
                if(isvalid(b))
                {
                    insertStr(b);
                    b = "";
                    addOperator(toUnchar('*'));
                }else if(prev==')'||prev=='!')
                    addOperator(toUnchar('*'));
                
                _operator.push(0);
                prev = c;
                break;

            case ')':
                if(d)
                    d = false;

                if(isvalid(b)){
                    insertStr(b);
                    b = "";
                }

                if(_operator.size())
                {
                    while(_operator.size())
                    {
                        if(_operator.top()==0)
                            break;
                        output.push_back(_operator.top());
                        _operator.pop();
                    }

                    if(_operator.size())
                        _operator.pop();    //To Remove '(' character from the operator stack

                    else{
                        error("\')\' without a previous \'(\'",i,argv[1]);
                        return -1;
                    }
                }
                else
                {
                    warning("\')\' at inappropriate location, Ignoring",i,argv[1]);
                    d = true;
                    break;
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
                if(prev == ')'){
                    addOperator(toUnchar('*'));
                    b = "";
                }

                if(isvalid(b)){
                    insertStr(b);
                    b = "";
                    addOperator(toUnchar('*'));
                }

                addOperator(toUnchar(c));
                break;

            default:
                if(!isspace(c))
                    warning("Unhandled character",i,argv[1]);
                break;
        }
    }

    if(isvalid(b))
        insertStr(b);

    if((prev<'0'||prev>'9')&&prev!=')'&&prev!='!')
    {
        error("Expression Ends with an Operator");
        return -1;
    }

    while(_operator.size())
    {
        if(_operator.top() == 0){
            warning("\'(\' without a \')\', Ignoring");
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
                    error("Internal Error Occured, Error %d",__LINE__);
                    return -1;
                }
                    
                applyfunc(output[a-1].d,output[a].c);
                output.erase(output.cbegin()+a);
                --a;
            }
            else
            {
                if(a<2)
                {
                    error("Internal Error Occured, Error %d",__LINE__);
                    return -1;
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
            error("Internal Error Occured, Error %d",__LINE__);
            return -1;
        }
        
        printf("\nResult    : %f\n",*(output[0].d));
        return 0;
    }

    return -1;
}
