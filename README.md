# SIC-Assembler

The following is the code for our Third Year System Programming assignment to make a custom SIC Assembler.The project is done in C++ using QT-Creator for creating the GUI. 

# What is a SIC-Assembler?

An Assembler is used to take in Assembly Language as its source code and converts that into machine code that is understandable by the computer. For a SIC-Assembler the source code is written in SIC or SIC/XE.

# Contents

* # assembler.pro

It contains the list of source files and header files used for the development of the project.

* main.cpp

Source file used to create an object of the Notepad class defined in the notepad.h header file and also to initialise the GUI.

* notepad.cpp

It contains all the definitions of the methods used in the GUI.

* notepad.h

It is a header file which contains the methods used in the GUI under the Notepad class. 

* notepad.ui

File used to style the user interface for the GUI. 

* assembler.exe

Executable file to run the assembler

# Steps to run the assembler

* Download the assembler.exe file on your computer
* Open Terminal 
* Go to the file location of the assembler.exe file
* Run the assembler by typing ./assembler.exe


# Functions Implemented :

* LDA 

Load Accumulator

* LDX

Load X register

* LDS

Load S register

* MOVAS

Copy data of Accumulator to S

* MOVSA

Copy data of S to Accumulator

* MOVAX

Copy data of Accumulator to Index

* MOVXA

Copy data of Index register to Accumulator

* MOVAB

Move data of Accumulator to B

* MOVBA

Move data of B register to Accumulator

* TIX



* JEQ

Jump on equal 

* JLT

Jump on less than

* JGT

Jump on Greater Than

* J

Jump

* STA

Store Accumulator

* STX

Store Index register

* ADD

Add function

* SUB

Subtract 

* MUL

Multiply

* ADDF

Add floating point

* SUBF

Subtract floating point

* MULF

Multiply floating point

* CMP

Compare

* CMPS
* STAM
* STXM





