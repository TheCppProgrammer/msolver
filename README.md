# M-SOLVER

Mathematics expression parser and solver using shunting yard algorithm([Wikipedia](https://en.wikipedia.org/wiki/Shunting-yard_algorithm))

## Building :

makefile is provided with the project. If make utility is available, it is recommended

### Windows or macOS :
use any compiler to compile main.cpp

g++ is a widely used compiler. MingW package is available for windows to download : [Download MingW For Windows](http://www.mingw.org/)

use following command to build :

    g++ -o msolver.exe main.cpp
*exe should be ommited in case of macOS*

### Linux :
make sure that your terminal supports color

[check if color support is available in your linux terminal](https://codeyarns.com/2015/03/18/how-to-check-colors-supported-by-terminal/)

if the color support is available, steps are same as in macOS.

if the color support is not available, open main.cpp using your favourite text editor and remove 3rd and 4th line. Rest of the steps are same as in macOS.

## Usage :
Open the command prompt/terminal in the location where the executable is located and type :

#### windows :
    msolver.exe --help

#### macOS/Linux :
    ./msolver --help

### Supported Functions :

  * log
  * ln
  * sin
  * cos
  * tan
  * asin
  * acos
  * atan
  * round
  * floor
  * mod
  * ceil(ceiling function)
  * int(returns integer part of a float)
  * fraction(returns fraction part of a float)

**Functions must be followed by paranthesis '('**

### Supported Operators :

  * Addition Operator(+)
  * Subtraction Operator(-)
  * Multiplication Operator(*/X/x)
  * Division Operator(/)
  * Exponentiation Operator(^)
  * Factorial Operator(!)
  * Modulo Operator(%)