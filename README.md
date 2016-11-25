# SIC-Assembler

The following is the code for our Third Year System Programming assignment to make a custom SIC Assembler.The project is done in C++ using QT-Creator for creating the GUI. 

# What is a SIC-Assembler?

An Assembler is used to take in Assembly Language as its source code and converts that into machine code that is understandable by the computer. For a SIC-Assembler the source code is written in SIC or SIC/XE.

# Contents

*  *assembler.pro* 

It contains the list of source files and header files used for the development of the project.

* *main.cpp*

Source file used to create an object of the Notepad class defined in the notepad.h header file and also to initialise the GUI.

* *notepad.cpp*

It contains all the definitions of the methods used in the GUI.

* *notepad.h*

It is a header file which contains the methods used in the GUI under the Notepad class. 

* *notepad.ui*

File used to style the user interface for the GUI. 

* *assembler.exe*

Executable file to run the assembler

# Steps to run the assembler

* Download the assembler.exe file on your computer
* Open Terminal 
* Go to the file location of the assembler.exe file
* Run the assembler by typing ./assembler.exe


# Functions Implemented :

* *LDA* 

Load Accumulator

* *LDX*

Load X register

* *LDS*

Load S register

* *MOVAS*

Copy data of Accumulator to S

* *MOVSA*

Copy data of S to Accumulator

* *MOVAX*

Copy data of Accumulator to Index

* *MOVXA*

Copy data of Index register to Accumulator

* *MOVAB*

Move data of Accumulator to B

* *MOVBA*

Move data of B register to Accumulator

* *TIX*

Increment index register and compare

* *JEQ*

Jump on equal 

* *JLT*

Jump on less than

* *JGT*

Jump on Greater Than

* *J*

Jump

* *STA*

Store Accumulator

* *STX*

Store Index register

* *ADD*

Add function

* *SUB*

Subtract 

* *MUL*

Multiply

* *ADDF*

Add floating point

* *SUBF*

Subtract floating point

* *MULF*

Multiply floating point

* *CMP*

Compare

* *CMPS*

* *STAM*

Store accumulator to memory

* *STXM*

Store Index register to memory


# Modules used 


* void PASS1(char *filename)  : Performs the first pass of the given program. Saves references to SYMBOL TABLE.

* int getDataPosition(string &operand)  : A utility function to get data value from data segment.

* void executeCode() :  Executes the assembly language program.

* int debugCode()  :   Executes the assembly language program line by line returning appropriate values to the caller module.

* void printDataLines() : A utility function to print data lines to the console for debugging

* Every instruction is a separate function in the source file.

# GUI functions

* Notepad : A text edit element for writing, editing assembly language code.

* VARIABLES :  Line edit elements for all the variables.

* String :  For String Inputs.

* Number : For Numeric Inputs.

* Output  : For Numeric Outputs. 

* Register : Line edit elements in Qt

* Accumulator

* Index Register

* Base Register

* Program Counter

# Buttons  :

* Run  : To execute the assembled program.
* Clear  :  To clear the text editor ,variables and registers.
* Quit  : To quit the window.
* To hex  : To change variables, register and memory values to hexadecimal values.
* Clear Memory : To clear the memory.

* SYMBOL TABLE  : Qtable widget for implementing the symbol table.
	Rows : Number
	Columns : 1 -> Symbol , 2 -> Line
	
* Debugger : Qtable widget for debugging process.
	Debug button for instruction by instruction execution
	Clear Debug for clearing the table.
	Columns : 1 -> LABEL , 2 -> INSTRUCTION , 3 -> OPERAND
	Rows  : 1 -> PREVIOUSLY EXECUTED INSTRUCTION  ,  2 -> CURRENTLY EXECUTED INSTRUCTION  , 3 -> INSTRUCTION TO BE EXECUTED NEXT
	
# Special Features : 

* Execution Progress :  Tracks how much of the code is left to be executed in debug mode. 
* Save menu option  : Saves the edited file in text editor to the disk.
* Error Handling  : Alerts the user if any opcode or label is not found in the assembly language program by using QmessageDialog.


# Integrating UI with Simulator

A number of modules were used to integrate the developed User Interface with the actual simulator coded in command line interface.

* Notepad::Notepad(QWidget *parent) : For initialsing notepad. A constructor.
* Notepad::~Notepad()  : A destructor.
* void Notepad::on_actionOpen_triggered()  : When the notepad is opened.
* void Notepad::on_actionSave_triggered()  :  For saving changes made to workspace.
* void Notepad::on_pushButton_clicked()  :  For code execution.
* void Notepad::on_clear_clicked()  :  For clearing window elements.
* void Notepad::on_debug_clicked()  : To debug code instruction by instruction.
* void Notepad::on_pushButton_3_clicked()  : For writing to memory.
* void Notepad::on_pushButton_4_clicked()  : Clears debug.
* void Notepad::on_decimal_clicked()  : Converts data values to decimal.

#List of programs tested

* Nth Fibonacci Number
* Factorial of n
* Substring search
* Bubble sort
