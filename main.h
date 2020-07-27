#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED
#ifdef _WINDOWS_
#include <windows.h>
#endif
constexpr const double e = 2.71828182;

void printColored(bool &&color,char &c)
{
    #ifdef _UNIX_COLOR_
        printf("\033[0;%dm%c\033[0m",(color?31:35),c);
    #else
        #ifdef _WINDOWS_
            HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
            WORD w;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(h,&csbi);
            w = csbi.wAttributes;
            SetConsoleTextAttribute(h,(color?FOREGROUND_RED:FOREGROUND_RED|FOREGROUND_BLUE)|FOREGROUND_INTENSITY);
            printf("%c",c);
            SetConsoleTextAttribute(h,w);
        #else
            printf("'%c'",c);
        #endif
    #endif
}

void warning(const char* warning,int location = -1,char* string = nullptr)
{
    printf("Warning   : %s\n",warning);
    if(location!=-1)
    {
        printf("Parameter : ");
        uint16_t size = strlen(string);
        for(uint16_t i = 0;i<size;++i)
        {
            if(i == location)
            {                
                printColored(false,string[i]);
            }
            else
                printf("%c",string[i]);
        }

        printf("\n\n");
    }
}

void error(const char* error,int location = -1,char* string = nullptr)
{
    printf("Error     : %s\n",error);
    if(location!=-1)
    {
        printf("Parameter : ");
        uint16_t size = strlen(string);
        for(uint16_t i = 0;i<size;++i)
        {
            if(i == location)
            {
                printColored(true,string[i]);
            }
            else
                printf("%c",string[i]);
        }

        printf("\n\n");
    }
}

void showHelp(char *c)
{
    //HELPMENU
    printf("MSolver v1.0\n\n");
    printf("Usage: %s expression\n",c);
    printf("Supported Functions(Must be Followed By Paranthesis):\n");
    printf("    Log() : Log Function\n");
    printf("    Ln()  : Natural Log Function\n");
    printf("    Sin() : Sin Function\n");
    printf("    Cos() : Cos Function\n");
    printf("    Tan() : Tan Function\n");
    printf("Supported Operators:\n");
    printf("      +   : Add Operator\n");
    printf("      -   : Subtract Operator\n");
    printf("      *   : Multiply Operator\n");
    printf("      /   : Division Operator\n");
    printf("      ^   : Exponentiation Operator\n");
    printf("      !   : Factorial Operator\n");
    printf("      %%   : Modular Operator\n\n");
}

struct data
{
    double d;
    unsigned char c;
    data()
    {
        c = 12;
    }
    data(unsigned char e = 0)
    {
        c = e;
    }

    bool isVar()
    {
        return c == 0;
    }
};

bool checkStr(const char* a, char *b, uint16_t location)
{
    const uint16_t len = strlen(a);
    const uint16_t blen = strlen(b);
    
    if(blen-location < len)
        return false;
    
    for(uint16_t i = 0;i<len;++i)
    {
        if(toupper(a[i])!=toupper(b[i+location]))
            return false;
    }
    
    for(uint16_t i = location+len;i<blen;++i)
    {
        if(!isspace(b[i]))
        {
            if(b[i]=='(')
                return true;
            else
                return false;
        }
    }
    
    return false;
}

#endif // MAIN_H_INCLUDED
