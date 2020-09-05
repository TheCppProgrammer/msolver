#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32)) && !defined(__CYGWIN__)
    #define _WINDOWS_
#else 
    #define _UNIX_COLOR_
#endif

#include "main.h"
#include <vector>
#include <deque>
#include <stack>

//struct that holds operator and constant data
struct data
{
    double d = 0;
    uint8_t c = INVALID_OPERATOR;

    data(uint8_t e = 0)
    {
        c = e;
    }

    //tells if the object is holding any constant data
    bool isNum()
    {
        return c == 0;
    }
};

//operator stack
std::stack<uint8_t> _operator;

//output queue. A deque is used instead of a queue because
//deque allows user to get and erase any item using the index
std::deque<data> output;

//vector to store paranthesis information
std::vector<uint16_t> pNumber;

//adds operator to the operator stack
void addOperator(uint8_t o){
    if(_operator.size())
    while(_operator.size() && _operator.top() > o)
    {
        output.push_back(_operator.top());
        _operator.pop();
    }

    _operator.push(o);
}

//inserts constant in the output queue
void insertStr(std::string a)
{
    output.push_back(0);
    output.back().d = atof(a.c_str());
}

//remove subtraction operator from the operator stack
void remove_subtraction()
{
    _operator.pop();
    if(_operator.size())
        if(_operator.top() == O_ADDITION)
            _operator.pop();

    if(output.size())
        output.erase(output.begin()+(output.size()-1));
}

void showHelp(char *c)
{
    printf("Usage: %s [OPTION(s)] expression\n",c);
    puts("Note : if expression contains space(s), it must be enclosed in double quotes\n");
    puts("-nowarning : Don't display any warning");
    puts("-noerror : Don't display any error");
    puts("-degree : Change angle measurement unit to degree(default is radian)");
    puts("--help/-h : Displays this help\n");
    puts("Supported Operators:");
    puts("    +    : Addition Operator");
    puts("    -    : Subtraction Operator");
    puts("    *    : Multiplication Operator");
    puts("    /    : Division Operator");
    puts("    ^    : Exponentiation Operator");
    puts("    !    : Factorial Operator");
    puts("    %    : Modulo Operator\n");
    puts("Supported Functions(Must be Followed By Paranthesis):");
    puts("Trigonometric Functions :");
    puts("    sin(),cos(),tan(),asin(),acos(),atan()");
    puts("Other Functions :");
    puts("    log(),ln(),round(),floor()\n");
    puts("    ceil()    : ceiling function");
    puts("    mod()     : mod function");
    puts("    int()     : returns integer part of float");
    puts("  fraction()  : returns fraction part of float\n");
}

int main(int argc,char** argv)
{
    if(argc<2){
        showHelp(argv[0]);
        return 0;
    }

    for(int a = 1;a<argc-1;++a)
    {
        if(!strcasecmp(argv[a],"-nowarning"))
            _warning = false;

        else if(!strcasecmp(argv[a],"-noerror"))
            _error = false;

        else if(!strcasecmp(argv[a],"-degree"))
            isRadian = false;

        else if(!strcasecmp(argv[a],"-h")||!strcasecmp(argv[a],"--help")){
            showHelp(argv[0]);
            return 0;
        }

        else{
            printf("error: unrecognized command line option \'%s\'\nTry \'%s --help\' for more information\n",argv[a],argv[0]);
            return -1;
        }
    }

    if(!strcasecmp(argv[argc-1],"-h") || !strcasecmp(argv[argc-1],"--help")){
        showHelp(argv[0]);
        return 0;
    }

    char* arg = argv[argc-1];
    const uint16_t len = strlen(arg);    //stores length of the expression
    std::string b = "";    //string to store temporary constant
    char prev = ' ';    //stores last valid character
    uint16_t uc = 0;    //tells temporary no of unhandled characters
    uint16_t pDepth = 0;    //stores depth of paranthesis
    
    for(uint16_t i = 0;i<len;++i)
    {
        char c = arg[i];
        switch (c)
        {
            case '.':
                b += c;
                break;

            case 48 ... 57:

                //if the previous valid character is ) or !,
                //add * before the number. for e.g : (5+4)2 is parsed as (5+4)*2
                if(prev == ')' || prev=='!')
                    addOperator(O_MULTIPLICATION);

                b += c;
                prev = c;
                break;


            case '+':
            case '-':
                if(prev == '(' || prev == ' ' || prev == 127)
                {
                    if(c == '-'){
                        insertStr("-1");
                        addOperator(O_SUBTRACTION);
                    }

                    prev = c;
                    break;
                }

            case 'x':
            case 'X':
            case '*':
            case '/':
            case '^':
            case '%':
            case '!':

                if(prev == '(' || prev == ' '){
                    warning("Operator without any constant before it, Ignoring",i,arg);
                    break;
                }

                if(toUnchar(prev) < F_FACTORIAL && prev != ')'){
                    warning("Multiple Operators Without any constant in between, Ignoring",i,arg);
                    break;
                }

                if(isvalid(b)){
                    insertStr(b);
                    b = "";
                }

                //'~' is internally used to remove last operator
                if(prev == '~'){
                    if(_operator.size()){
                        if(_operator.top() == O_SUBTRACTION)
                            remove_subtraction();
                        else
                            _operator.pop();
                    }
                }

                //x-y is interpreted as x+(-1)*y. it is to avoid a "bug"
                //in the original interpretation "2-1+1" is solved as "2-2 = 0"
                //because pedmas/ bodmas tells that + should be interpreted before - operator
                //in the example, the answer should be 2 but it used to give 0
                if(c == '-'){
                    addOperator(O_ADDITION);
                    insertStr("-1");
                    _operator.push(O_SUBTRACTION);
                }else
                    addOperator(toUnchar(c));

                prev = c;
                break;

                
            case '(':

                if(isvalid(b)){
                    insertStr(b);
                    b = "";
                    addOperator(O_MULTIPLICATION);
                }

                else if(prev == ')' || prev == '!')
                    addOperator(O_MULTIPLICATION);
                
                _operator.push(PARANTHESIS_OPEN);
                ++pDepth;
                pNumber.emplace_back(output.size());
                prev = c;
                break;

            case ')':

                if(pDepth == 0){
                    warning("\')\' without corresponding \'(\', Ignoring",i,arg);
                    break;
                }

                --pDepth;

                if(isvalid(b)){
                    insertStr(b);
                    b = "";
                }

                if((toUnchar(prev) < F_FACTORIAL && prev != '(') || prev == '~')
                {
                    warning("Expression Ends with an Operator, Ignoring",i,arg);
                    if(_operator.top() == O_SUBTRACTION)
                        remove_subtraction();
                    else
                        _operator.pop();
                }

                if(output.size() == pNumber[pDepth])
                {
                    //remove ( from operator stack and remove preceding operator
                    _operator.pop();

                    if(_operator.size())
                        if(_operator.top() > F_FACTORIAL)
                            _operator.pop();

                    pNumber.pop_back();
                    if(pNumber.size())
                        prev = (pNumber[pNumber.size()-1] == output.size())?'(':'~';
                    else
                        prev = (output.size())?'~':' ';
                    break;
                }

                pNumber.pop_back();
                while(_operator.size())
                {
                    if(_operator.top() == 0){
                        _operator.pop();
                        break;
                    }

                    output.push_back(_operator.top());
                    _operator.pop();
                }

                prev = c;
                break;


            //---------------------------------
            //       Check for functions
            //---------------------------------

            case 'A':
            case 'a':

                if(check_string_a(arg,i,c,uc))
                    goto functionHandling;

                ++uc;
                continue;

            case 'C':
            case 'c':

                if(check_string_c(arg,i,c,uc))
                    goto functionHandling;

                ++uc;
                continue;

            case 'F':
            case 'f':

                if(check_string_f(arg,i,c,uc))
                    goto functionHandling;

                ++uc;
                continue;


            case 'I':
            case 'i':

                if(check_string_i(arg,i,c,uc))
                    goto functionHandling;

                ++uc;
                continue;

            case 'L':
            case 'l':

                if(check_string_l(arg,i,c,uc))
                    goto functionHandling;

                ++uc;
                continue;

            case 'M':
            case 'm':

                if(check_string_m(arg,i,c,uc))
                    goto functionHandling;

                ++uc;
                continue;


            case 'R':
            case 'r':

                if(check_string_r(arg,i,c,uc))
                    goto functionHandling;

                ++uc;
                continue;

            case 'S':
            case 's':                

                if(check_string_s(arg,i,c,uc))
                    goto functionHandling;

                ++uc;
                continue;

            case 'T':
            case 't':

                if(!check_string_t(arg,i,c,uc)){
                    ++uc;
                    continue;
                }

            functionHandling:
                if(prev == ')')
                {
                    addOperator(O_MULTIPLICATION);
                    b = "";
                }

                else if(isvalid(b))
                {
                    insertStr(b);
                    b = "";
                    addOperator(O_MULTIPLICATION);
                }

                addOperator(toUnchar(c));
                break;

            default:
                if(!isspace(c)){
                    ++uc;
                    continue;
                }
        }

        //show warning for unhandled characters
        if(uc){
            warning("Unhandled character(s)",i-uc,arg,uc);
            uc = 0;
        }
    }

    if(isvalid(b))
        insertStr(b);

    //show warning for unhandled characters
    if(uc)
        warning("Unhandled character(s)",len-uc,arg,uc);

    if((toUnchar(prev) < F_FACTORIAL && prev != '(') || prev == '~')
    {
        warning("Expression Ends with an Operator, Ignoring");
        if(_operator.top() == O_SUBTRACTION)
            remove_subtraction();
        else
            _operator.pop();
    }

    while(_operator.size())
    {
        if(_operator.top() == 0)
        {
            warning("\'(\' without corresponding \')\', Ignoring");
            _operator.pop();
            if(output.size() == pNumber[--pDepth])
            {
                if(_operator.size()){
                    if(_operator.top() == O_SUBTRACTION)
                        remove_subtraction();

                    else if(_operator.top() > F_FACTORIAL){
                        _operator.pop();

                        if(_operator.size()){
                            if(_operator.top() == O_SUBTRACTION)
                                remove_subtraction();

                            else if(_operator.top() != PARANTHESIS_OPEN)
                                _operator.pop();
                        }
                    }

                    else if(_operator.top() != PARANTHESIS_OPEN)
                        _operator.pop();
                }

                pNumber.pop_back();
                continue;
            }

            pNumber.pop_back();
        }

        output.push_back(_operator.top());
        _operator.pop();
    }

    for(std::size_t a = 0;a<output.size();++a)
    {
        if(!output[a].isNum())
        {
            if(isFunction(output[a].c))
            {
                if(a<1)
                {
                    output.erase(output.cbegin());
                    --a;
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
                    --a;
                    continue;
                }
                
                apply(output[a-2].d,output[a-1].d,output[a].c);
                output.erase(output.cbegin()+a-1,output.cbegin()+a+1);
                a-=2;
            }
        }
    }

    if(output.size() >= 1)
    {
        if(!output[0].isNum())
        {
            error("Syntax Error");
            return -1;
        }
        
        printf("Result    : %f\n",output[0].d);
    }
    else
        printf("Result    : %f\n",0.0f);

    return 0;
}
