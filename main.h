#ifndef __MAIN_H_INCLUDED
#define __MAIN_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <math.h>

#ifdef _WINDOWS_
    #include <windows.h>
#endif

#define FACTORIAL_MAX 170

#define PARANTHESIS_OPEN 0
#define O_ADDITION 1
#define O_SUBTRACTION 2
#define O_MULTIPLICATION 3
#define O_DIVISION 4
#define O_EXPONENT 5
#define O_MODULUS 6
#define F_FACTORIAL 127
#define F_LOG 128
#define F_LN 129
#define F_SIN 130
#define F_COS 131
#define F_TAN 132
#define F_ROUND 133
#define F_FLOOR 134
#define F_CEIL 135
#define F_INT 136
#define F_FRACTION 137
#define F_MOD 138
#define F_ASIN 139
#define F_ACOS 140
#define F_ATAN 141
#define INVALID_OPERATOR 255

#define DEGREE_TO_RAD 0.0174532925199
#define RAD_TO_DEGREE 57.2957795131

bool isRadian = true;
bool _warning = true;
bool _error = true;

void printColored(bool &&color, const char* c, const uint16_t &len)
{
    #ifdef _UNIX_COLOR_
        printf("\033[0;%dm%.*s\033[0m",(color?31:35),len,c);
    #else
        #ifdef _WINDOWS_
            HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
            WORD w;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(h,&csbi);
            w = csbi.wAttributes;
            SetConsoleTextAttribute(h,(color?FOREGROUND_RED:FOREGROUND_RED|FOREGROUND_BLUE)|FOREGROUND_INTENSITY);
            printf("%.*s",len,c);
            SetConsoleTextAttribute(h,w);
        #else
            printf(" \"%.*s\" ",len,c);
        #endif
    #endif
}

void warning(const char* warning, int location = -1, const char* string = nullptr, uint16_t len = 1)
{
    if(!_warning)
        return;
    printf("Warning   : %s",warning);
    if(location!=-1)
    {
        printf("\nParameter : ");
        printf("%.*s",location,string);
        printColored(false,string+location,len);
        printf("%s",string+location+len);
    }

    printf("\n\n");
}

void error(const char* error, int location = -1, const char* string = nullptr, uint16_t len = 1)
{
    if(!_error)
        return;
    printf("Error     : %s",error);
    if(location!=-1)
    {
        printf("\nParameter : ");
        printf("%.*s",location,string);
        printColored(false,string+location,len);
        printf("%s",string+location+len);
    }

    printf("\n\n");
}

uint8_t toUnchar(const char &c)
{
    switch(c)
    {
        case '+':
            return O_ADDITION;
        case '-':
            return O_SUBTRACTION;
        case '*':
            return O_MULTIPLICATION;
        case '/':
            return O_DIVISION;
        case '(':
            return PARANTHESIS_OPEN;
        case '^':
            return O_EXPONENT;
        case '%':
            return O_MODULUS;
        case '!':
            return F_FACTORIAL;
        case 'S':
            return F_SIN;
        case 'C':
            return F_COS;
        case 'T':
            return F_TAN;
        case 'L':
            return F_LOG;
        case 'N':
            return F_LN;
        case 'R':
            return F_ROUND;
        case 'F':
            return F_FLOOR;
        case 'E':
            return F_CEIL;
        case 'I':
            return F_INT;
        case 'f':
            return F_FRACTION;
        case 'M':
            return F_MOD;
        case 's':
            return F_ASIN;
        case 'c':
            return F_ACOS;
        case 't':
            return F_ATAN;
        default:
            return INVALID_OPERATOR;
    }
}

inline bool isFunction(uint8_t c)
{
    return c>=F_FACTORIAL;
}

bool isvalid(std::string &s)
{
    bool decimal = false;
    for(uint16_t i = 0;i<s.length();++i)
    {
        if(s[i]=='.')
        {
            if(decimal)
                s.erase(i--,1);
            else
                decimal = true;
        }
    }

    if(s == ".")
    {
        s = "";
        return false;
    }

    else
        return s!="";
}

inline void factorial(double &a)
{
    if(a>FACTORIAL_MAX)
    {
        error("Factorial above #FACTORIAL_MAX is not supported");
        exit(1);
    }
    else if(a<0)
    {
        warning("Factorial of negative Numbers not allowed, Ignoring");
        return;
    }

    uint8_t b = round(a);
    double _result = 1.0f;
    for(uint8_t i = 1;i<=b;++i)
    {
        _result*=i;
    }

    a = _result;
}

inline void apply(double &a, const double &b, const unsigned char& c)
{
    switch (c)
    {
        case O_ADDITION:
            a += b;
            break;

        case O_MULTIPLICATION:

        //O_SUBTRACTION is added to resolve empty paranthesis errors
        case O_SUBTRACTION:
            a *= b;
            break;

        case O_DIVISION:
            if(b==0)
            {
                error("Divide by Zero error");
                exit(1);
            }

            a /= b;
            break;

        case O_EXPONENT:
            if((a == b) && (a == 0.0f))
            {
                warning("0^0 is not defined, assuming 0^0 = 1");
                a = 1.0f;
                break;
            }

            a = std::pow(a,b);
            break;

        case O_MODULUS:
            a = fmod(a,b);
    }
}

inline void applyfunc(double &a, const unsigned char &c)
{
    switch(c)
    {
        case F_FACTORIAL:
            factorial(a);
            break;

        case F_LOG:
            a = log10(a);
            break;

        case F_LN:
            a = log(a);
            break;

        case F_SIN:
            if(isRadian)
                a = sin(a);
            else
                a = sin(a*DEGREE_TO_RAD);

            break;

        case F_COS:
            if(isRadian)
                a = cos(a);
            else
                a = cos(a*DEGREE_TO_RAD);
            break;

        case F_TAN:
            if(isRadian)
                a = tan(a);
            else
                a = tan(a*DEGREE_TO_RAD);
            break;

        case F_ROUND:
            a = round(a);
            break;

        case F_FLOOR:
            a = floor(a);
            break;

        case F_CEIL:
            a = ceil(a);
            break;

        case F_INT:
            modf(a,&a);
            break;

        case F_FRACTION:
            a = modf(a,&a);
            break;

        case F_MOD:
            a = fabs(a);
            break;

        case F_ASIN:
            a = asin(a)*(isRadian?1.0f:RAD_TO_DEGREE);
            break;

        case F_ACOS:
            a = acos(a)*(isRadian?1.0f:RAD_TO_DEGREE);
            break;

        case F_ATAN:
            a = atan(a)*(isRadian?1.0f:RAD_TO_DEGREE);
    }
}

bool checkStr(const char* a, const char *arg, uint16_t& location, uint16_t& uc)
{
    const uint16_t len = strlen(a);
    const uint16_t arglen = strlen(arg);
    uint16_t counter = 0;
    
    if(arglen-location < len)
        return false;
    
    uint16_t i = location;
    for(;i<arglen && counter<len;++i)
    {
        if(isspace(arg[i]))
            continue;

        if(toupper(a[counter]) != toupper(arg[i]))
            return false;
        else
            ++counter;
    }

    if(counter < len)
        return false;

    //show warning for unhandled characters
    if(uc){
        warning("Unhandled character(s)",location-uc,arg,uc);
        uc = 0;
    }

    location = i-2;
    return true;
}

inline bool check_string_a(const char* arg, uint16_t &location, char& c, uint16_t& uc)
{
    if(checkStr("asin(",arg,location,uc)){
        c = 's';
        return true;
    }else if(checkStr("acos(",arg,location,uc)){
        c = 'c';
        return true;
    }else if(checkStr("atan(",arg,location,uc)){
        c = 't';
        return true;
    }

    return false;
}

inline bool check_string_c(const char* arg, uint16_t &location, char& c, uint16_t& uc)
{
    if(checkStr("cos(",arg,location,uc)){
        c = 'C';
        return true;
    }else if(checkStr("ceil(",arg,location,uc)){
        c = 'E';
        return true;
    }

    return false;
}

inline bool check_string_f(const char* arg, uint16_t &location, char& c, uint16_t& uc)
{
    if(checkStr("floor(",arg,location,uc)){
        c = 'F';
        return true;
    }else if(checkStr("fraction(",arg,location,uc)){
        c = 'f';
        return true;
    }

    return false;
}

inline bool check_string_i(const char* arg, uint16_t &location, char& c, uint16_t& uc)
{
    if(checkStr("int(",arg,location,uc)){
        c = 'I';
        return true;
    }

    return false;
}

inline bool check_string_l(const char* arg, uint16_t& location, char& c, uint16_t& uc)
{
    if(checkStr("log(",arg,location,uc)){
        c = 'L';
        return true;
    }else if(checkStr("ln(",arg,location,uc)){
        c = 'N';
        return true;
    }

    return false;
}

inline bool check_string_m(const char* arg, uint16_t &location, char& c, uint16_t& uc)
{
    if(checkStr("mod(",arg,location,uc)){
        c = 'M';
        return true;
    }

    return false;
}

inline bool check_string_r(const char* arg, uint16_t &location, char& c, uint16_t& uc)
{
    if(checkStr("round(",arg,location,uc)){
        c = 'R';
        return true;
    }

    return false;
}

inline bool check_string_s(const char* arg, uint16_t &location, char& c, uint16_t& uc)
{
    if(checkStr("sin(",arg,location,uc)){
        c = 'S';
        return true;
    }

    return false;
}

inline bool check_string_t(const char* arg, uint16_t &location, char& c, uint16_t& uc)
{
    if(checkStr("tan(",arg,location,uc)){
        c = 'T';
        return true;
    }

    return false;
}

#endif
