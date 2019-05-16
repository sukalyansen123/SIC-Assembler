#include "notepad.h"
#include "ui_notepad.h"
//#include <string.h>
#include<QFileDialog>
#include<QFile>
#include <QMessageBox>
#include <QTextStream>
#include <bits/stdc++.h>
#include<QDoubleSpinBox>
#include<QLineEdit>
#include<QApplication>

using namespace std;

const string datastart(".DATA");
const string codestart(".CODE");
int length;
int dataLength;
int programCounter = 0;
int memoryPointer = 0;
char message[20];
bool ERROR_FLAG = false;
int ERROR_LINE = -1;
bool OPCODE_ERROR = false;
bool first = true;
//bool STRING_COMPARE_FLAG = false;
int COMPARE_FLAG = 0; // 0 : equal ; 1 : greater ; -1 : less

struct Program {
    string label;
    string opcode;
    string operand;
} program[200];

struct Data {
    string varname;
    bool reserve;
    bool word;
    bool hex;
    char value[100];
} dat[100];

struct Register {
    char value[100];
} reg[4];	// 0 : A , 1 : X , 2 : S, 3 : B

struct Memory {
    int number;
}mem[1000];


map<string, int> SYMTAB;

void saveLine(string &line, int pos) {
    int len = line.size();
    program[pos].label = "";
    program[pos].opcode = "";
    program[pos].operand = "";
    int i = 0;
    while(line[i] == ' ')
        i++;
    for(;i < len; i++) {
        if(line[i] != ' ')
            program[pos].label += line[i];
        else
            break;
    }
    while(line[i] == ' ')
        i++;
    for(;i < len; i++) {
        if(line[i] != ' ')
            program[pos].opcode += line[i];
        else
            break;
    }
    while(line[i] == ' ')
        i++;
    for(;i < len; i++) {
        if(line[i] != ' ')
            program[pos].operand += line[i];
        else
            break;
    }
    if(program[pos].label != "NULL" && SYMTAB.find(program[pos].label) == SYMTAB.end())
        SYMTAB[program[pos].label] = pos;
    else if(program[pos].label == "NULL") ;
    else
        ERROR_FLAG = true;
}

void printLines() {
    for(int i = 0; i < length; i++) {
        cout<<program[i].label<<"\t"<<program[i].opcode<<"\t"<<program[i].operand<<endl;
    }
    cout<<endl<<endl;
    for(map<string,int>:: iterator it = SYMTAB.begin(); it != SYMTAB.end(); it++)
        cout<<it->first<<"\t"<<it->second<<endl;
}

void printErrorMessage(int line) {
    ERROR_LINE = line;
    sprintf(message,"%d",line);
    //QMessageBox::critical(this,tr("Error in Line : "),tr(message));
    cout<<"Error in line number "<<line<<endl;
}

void saveDataLine(string &line) {
    int len = line.size();
    int count = 0;
    string op("");
    char oper[100];
    dat[dataLength].varname = "";
    dat[dataLength].reserve = false;
    dat[dataLength].hex = false;
    string val;
    int i = 0;
    while(line[i] == ' ')
        i++;
    for(;i < len; i++) {
        if(line[i] != ' ')
            dat[dataLength].varname += line[i];
        else
            break;
    }
    while(line[i] == ' ')
        i++;
    for(;i < len; i++) {
        if(line[i] != ' ') {
            op += line[i];
        }
        else
            break;
    }
    while(line[i] == ' ')
        i++;
    if(op != "WORD") {
        for(;i < len; i++) {
            if(line[i] != ' ') {
                oper[count++] = line[i];
            }
            else {
                oper[count] = '\0';
                break;
            }
        }
        dat[dataLength].word = false;
    }
    else {
        i++;
        for(;i < len; i++) {
            if(line[i] != '"') {
                oper[count++] = line[i];
            }
            else {
                oper[count] = '\0';
                break;
            }
        }
        strcpy(dat[dataLength].value,oper);
        dat[dataLength].word = true;
    }
    if(op == "RESB" || op == "RESW") {
        dat[dataLength].reserve = true;
    }
    if(op != "WORD" && oper[count - 1] == 'H' || oper[count - 1] == 'h') {
        dat[dataLength].hex = true;
    }
    if(op != "WORD" && dat[dataLength].reserve == false && dat[dataLength].hex == true) {
        oper[count - 1] = '\0';
        count--;
        strcpy(dat[dataLength].value,oper);
    }
    if(op != "WORD" && dat[dataLength].reserve == false && dat[dataLength].hex == false) {
        strcpy(dat[dataLength].value,oper);
    }
}

void getDataLines(ifstream &opcode) {
    string line;
    dataLength = 0;
    while(getline(opcode, line)) {
        while(line == "\n" || line == ".DATA") {
            getline(opcode,line);
        }
        saveDataLine(line);
        dataLength++;
    }
}

void printDataLines() {
    /*if(first == true) {
        ofstream outfile("Substringpre.txt");
        for(int i = 0; i < dataLength; i++) {
            string out(data[i].value);
            outfile<<data[i].varname<<";"<<out<<endl;
        }
    }
    else {
        ofstream outfile("Substringpost.txt");
        for(int i = 0; i < dataLength; i++) {
            string out(data[i].value);
            outfile<<data[i].varname<<";"<<out<<endl;
        }
    }*/
    for(int i = 0; i < dataLength; i++) {
        first = false;
        string out(dat[i].value);
        cout<<dat[i].varname<<"\t"<<dat[i].reserve<<"\t"<<dat[i].hex<<"\t"<<out<<endl;
    }
    /*
    for(int i = 0; i < dataLength; i++) {
        int number = atoi(data[i].value);
        char buffer[100];
        itoa(number,buffer,16);
        string out(buffer);
        cout<<data[i].varname<<"\t"<<data[i].reserve<<"\t"<<data[i].hex<<"\t"<<out<<endl;
    }*/
}

void displayMemory() {
    cout<<endl<<endl;
    cout<<"MEMORY STATUS"<<endl;
    cout<<"-------------"<<endl;
    for (int i = 0; i < memoryPointer; ++i) {
        cout<<"Location : "<<(i + 1)<<"\t:\t"<<mem[i].number<<endl;
    }
}

void PASS1(char *filename) {
    ifstream opcode(filename);
    string line;
    int cur_code = 0;
    if(opcode.is_open()) {
        getline(opcode, line);
        if(line == datastart || line == "END") {
            return;
        }
        if(line == codestart) {
            while(getline(opcode, line)) {
                if(line == datastart || line == "END")
                    break;
                saveLine(line,cur_code);
                /*if(ERROR_FLAG) {
                    printErrorMessage(cur_code);
                    return;
                }*/
                cur_code ++;

            }
        }
    }
    length = cur_code;
    //printLines();
    getDataLines(opcode);
    //printDataLines();
}

bool numberSort(const struct Memory &lhs, const struct Memory &rhs) {
    if(lhs.number > rhs.number)
        return false;
    else
        return true;
}

void checkError() {
    if(ERROR_FLAG == true) {
        cout<<"error in code"<<endl;
        printErrorMessage(programCounter);
        //exit(0);
    }
}

void ftoa(float number) {
    sprintf(reg[0].value,"%f",number);
}

int getDataPosition(string &operand) {
    //const char *point;
    //point = operand.c_str();
    for(int i = 0; i < dataLength; i++) {
        if(operand == dat[i].varname)
            return i;
    }
    //cout<<operand<<endl;
    ERROR_FLAG = true;
    return -1;
}

void LDA() {
    int pos = getDataPosition(program[programCounter].operand);
    checkError();
    strcpy(reg[0].value,dat[pos].value);
}

void LDX() {
    int pos = getDataPosition(program[programCounter].operand);
    //checkError();
    strcpy(reg[1].value,dat[pos].value);
}

void LDS() {
    int pos = getDataPosition(program[programCounter].operand);
    checkError();
    int accumulator = atoi(reg[0].value);
    int index = atoi(reg[1].value);
    char newstring[100];
    strcpy(newstring,dat[pos].value);
    strncpy(reg[2].value,newstring + index, accumulator);
}

void MOVAS() {
    strcpy(reg[0].value,reg[2].value);
}

void MOVSA() {
    strcpy(reg[2].value,reg[0].value);
}

void MOVAX() {
    strcpy(reg[0].value,reg[1].value);
    //printf("movax : %s \n",reg[0].value);
}

void MOVXA() {
    strcpy(reg[1].value,reg[0].value);
}

void MOVAB() {
    strcpy(reg[0].value,reg[3].value);
}

void MOVBA() {
    strcpy(reg[3].value,reg[0].value);
}

void TIX() {
    int number = atoi(reg[1].value);
    number++;

    sprintf(reg[1].value,"%d",number);
}

void JEQ(string &label) {
    if(COMPARE_FLAG == 0) {
        programCounter = SYMTAB[label];
    }
    else
        programCounter++;
}

void JLT(string &label) {
    if(COMPARE_FLAG == -1) {
        programCounter = SYMTAB[label];
    }
    else
        programCounter++;
}

void JGT(string &label) {
    if(COMPARE_FLAG == 1) {
        programCounter = SYMTAB[label];
    }
    else
        programCounter++;
}

void J(string &label) {
    programCounter = SYMTAB[label];
}

void STA() {
    int pos = getDataPosition(program[programCounter].operand);
    //checkError();
    strcpy(dat[pos].value,reg[0].value);
    printf("sta : %s \n",dat[pos].value);
}

void STX() {
    int pos = getDataPosition(program[programCounter].operand);
    checkError();
    strcpy(dat[pos].value,reg[1].value);
}

void ADD() {
    int pos = getDataPosition(program[programCounter].operand);
    checkError();
    int addend = atoi(dat[pos].value);
    int accumulator = atoi(reg[0].value);
    int sum = addend + accumulator;
    //cout<<endl<<endl<<"Sum : "<<sum<<" = "<<accumulator<<" + "<<addend<<endl<<endl;
    sprintf(reg[0].value,"%d",sum);
}

void SUB() {
    int pos = getDataPosition(program[programCounter].operand);
    checkError();
    int subtrahend = atoi(dat[pos].value);
    int accumulator = atoi(reg[0].value);
    int difference = accumulator - subtrahend;
    sprintf(reg[0].value,"%d",difference);
}

void MUL() {
    int pos = getDataPosition(program[programCounter].operand);
    //printf("%d ",pos);
    checkError();
    int multiplicand = atoi(dat[pos].value);
    int accumulator = atoi(reg[0].value);
    printf("mul : %d acc : %d\n",multiplicand,accumulator);
    int product = accumulator * multiplicand;
    printf("Product : %d",product);
    snprintf(reg[0].value,sizeof(reg[0].value),"%d",product);
}

void ADDF() {
    int pos = getDataPosition(program[programCounter].operand);
    checkError();
    float addend = atof(dat[pos].value);
    float accumulator = atof(reg[0].value);
    float sum = accumulator + addend;
    ftoa(sum);
}

void SUBF() {
    int pos = getDataPosition(program[programCounter].operand);
    checkError();
    float subtrahend = atof(dat[pos].value);
    float accumulator = atof(reg[0].value);
    float difference = accumulator - subtrahend;
    ftoa(difference);
}

void MULF() {
    int pos = getDataPosition(program[programCounter].operand);
    checkError();
    float multiplicand = atof(dat[pos].value);
    float accumulator = atof(reg[0].value);
    float product = accumulator * multiplicand;
    ftoa(product);
}

void CMP() {
    int pos = getDataPosition(program[programCounter].operand);//COMPARE_FLAG = strcmp(reg[1].value, dat[pos].value);
    int reg_val = atoi(reg[1].value);
    int data_val = atoi(dat[pos].value);
    //cout<<reg_val<<"\t"<<reg_val<<endl;
    COMPARE_FLAG = reg_val <= data_val ? (reg_val == data_val ? 0 : -1) : 1;
}

void CMPS() {
    COMPARE_FLAG = 1;
    int pos = getDataPosition(program[programCounter].operand);
    //printf("%s\t %s",reg[2].value,data[pos].value);
    COMPARE_FLAG = strcmp(reg[2].value, dat[pos].value);
}

void READ() {

    int read;
    //mem[memoryPointer] =
    //cin>>read;
    read = mem[memoryPointer++].number;
    cout<<"READ : "<<read<<endl;
    //sprintf(reg[1].value,"%d",read);
}

void READMEM() {
    int pos = getDataPosition(program[programCounter].operand);
    int read = mem[32].number;
    printf("Memory read : %d",read);
    sprintf(dat[pos].value,"%d",read);
}

void WRITE() {
    int pos = getDataPosition(program[programCounter].operand);
    printf("%s", dat[pos].value);
}

void SORT() {
    int pos = getDataPosition(program[programCounter].operand);
    int startVal = atoi(dat[pos].value);
    sort(mem + startVal, mem + memoryPointer, &numberSort);
}

void STAM() {
    int pos = getDataPosition(program[programCounter].operand);
    int location = atoi(reg[0].value);
    sprintf(dat[pos].value,"%d",mem[location].number);
    printf("STXM : %d", dat[pos].value);

    //dat[pos].value = itoa(mem[location].number);
}

void STXM() {
    int pos = getDataPosition(program[programCounter].operand);
    int location = atoi(reg[1].value);
    sprintf(dat[pos].value,"%d",mem[location].number);
    printf("STXM : %d", dat[pos].value);
}

void WRITEMEM() {
    int pos = getDataPosition(program[programCounter].operand);
    int location = atoi(reg[1].value);
    mem[location].number = atoi(dat[pos].value);
}

void executeCode() {
    while(true) {
        if(ERROR_FLAG == true) {
            printErrorMessage(programCounter);
            return;
        }
        bool normal_code = true;
        if(program[programCounter].opcode == "LDA") {
            LDA();
        }
        else if(program[programCounter].opcode == "LDX") {
            LDX();
        }
        else if(program[programCounter].opcode == "LDS") {
            LDS();
        }
        else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "S,A") {
            MOVSA();
        }
        else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "A,S") {
            MOVAS();
        }
        else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "A,X") {
            MOVAX();
        }
        else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "X,A") {
            MOVXA();
        }
        else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "A,B") {
            MOVAB();
        }
        else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "B,A") {
            MOVBA();
        }
        else if(program[programCounter].opcode == "TIX") {
            TIX();
            printf("X : %s\n",reg[1].value);
        }
        else if(program[programCounter].opcode == "JEQ") {
            JEQ(program[programCounter].operand);
            normal_code = false;
        }
        else if(program[programCounter].opcode == "JLT") {
            JLT(program[programCounter].operand);
            cout<<"Data Lines : "<<endl;
            printDataLines();
            cout<<endl<<endl;
            normal_code = false;
        }
        else if(program[programCounter].opcode == "J") {
            J(program[programCounter].operand);
            normal_code = false;
        }
        else if(program[programCounter].opcode == "JGT") {
            JGT(program[programCounter].operand);
            normal_code = false;
        }
        else if(program[programCounter].opcode == "STA") {
            STA();
        }
        else if(program[programCounter].opcode == "STX") {
            STX();
        }
        else if(program[programCounter].opcode == "ADD") {
            ADD();
        }
        else if(program[programCounter].opcode == "ADDF") {
            ADDF();
        }
        else if(program[programCounter].opcode == "SUB") {
            SUB();
        }
        else if(program[programCounter].opcode == "SUBF") {
            SUBF();
        }
        else if(program[programCounter].opcode == "MUL") {
            MUL();
        }
        else if(program[programCounter].opcode == "MULF") {
            MULF();
        }
        else if(program[programCounter].opcode == "CMP") {
            CMP();
        }
        else if(program[programCounter].opcode == "CMPS") {
            CMPS();
        }
        else if(program[programCounter].opcode == "READ") {
            READ();
        }
        else if(program[programCounter].opcode == "READMEM") {
            READMEM();
        }
        else if(program[programCounter].opcode == "WRITE") {
            WRITE();
        }
        else if(program[programCounter].opcode == "SORT") {
            SORT();
        }
        else if(program[programCounter].opcode == "STAM") {
            STAM();
        }
        else if(program[programCounter].opcode == "STXM") {
            STXM();
        }
        else if(program[programCounter].opcode == "WRITEMEM") {
            WRITEMEM();
        }
        else {
            if(programCounter == length)
                break;
            ERROR_FLAG = true;
            OPCODE_ERROR = true;
            printErrorMessage(programCounter);
            return;
        }
        if(programCounter == length)
            break;
        if(normal_code)
            programCounter++;
    }
    //printErrorMessage();
}


int debugCode() {
    /*if(ERROR_FLAG == true) {
        printErrorMessage(programCounter);
        return -1;
    }*/
    bool normal_code = true;
    if(program[programCounter].opcode == "LDA") {
        LDA();
    }
    else if(program[programCounter].opcode == "LDX") {
        LDX();
    }
    else if(program[programCounter].opcode == "LDS") {
        LDS();
    }
    else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "S,A") {
        MOVSA();
    }
    else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "A,S") {
        MOVAS();
    }
    else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "A,X") {
        MOVAX();
    }
    else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "X,A") {
        MOVXA();
    }
    else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "A,B") {
        MOVAB();
    }
    else if(program[programCounter].opcode == "MOV" && program[programCounter].operand == "B,A") {
        MOVBA();
    }
    else if(program[programCounter].opcode == "TIX") {
        TIX();
        printf("X : %s\n",reg[1].value);
    }
    else if(program[programCounter].opcode == "JEQ") {
        JEQ(program[programCounter].operand);
        normal_code = false;
    }
    else if(program[programCounter].opcode == "JLT") {
        JLT(program[programCounter].operand);
        cout<<"Data Lines : "<<endl;
        printDataLines();
        cout<<endl<<endl;
        normal_code = false;
    }
    else if(program[programCounter].opcode == "J") {
        J(program[programCounter].operand);
        normal_code = false;
    }
    else if(program[programCounter].opcode == "JGT") {
        JGT(program[programCounter].operand);
        normal_code = false;
    }
    else if(program[programCounter].opcode == "STA") {
        STA();
    }
    else if(program[programCounter].opcode == "STX") {
        STX();
    }
    else if(program[programCounter].opcode == "ADD") {
        ADD();
    }
    else if(program[programCounter].opcode == "ADDF") {
        ADDF();
    }
    else if(program[programCounter].opcode == "SUB") {
        SUB();
    }
    else if(program[programCounter].opcode == "SUBF") {
        SUBF();
    }
    else if(program[programCounter].opcode == "MUL") {
        MUL();
    }
    else if(program[programCounter].opcode == "MULF") {
       MULF();
    }
    else if(program[programCounter].opcode == "CMP") {
       CMP();
    }
    else if(program[programCounter].opcode == "CMPS") {
       CMPS();
    }
    else if(program[programCounter].opcode == "READ") {
        READ();
    }
    else if(program[programCounter].opcode == "READMEM") {
        READMEM();
    }
    else if(program[programCounter].opcode == "WRITE") {
        WRITE();
    }
    else if(program[programCounter].opcode == "SORT") {
        SORT();
    }
    else if(program[programCounter].opcode == "STAM") {
        STAM();
    }
    else if(program[programCounter].opcode == "STXM") {
        STXM();
    }
    else if(program[programCounter].opcode == "WRITEMEM") {
        WRITEMEM();
    }
    else {
        if(programCounter == length)
            return 0;
        //ERROR_FLAG = true;
        //OPCODE_ERROR = true;
        //printErrorMessage(programCounter);
        //return -1;
    }

    if(programCounter == length)
        return 0;
    if(normal_code)
        programCounter++;
    return 1;
    //printErrorMessage();
}


Notepad::Notepad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Notepad)
{
    ui->setupUi(this);
    for(int i = 0; i < 32; i++) {
        char *buff = "0";
        //sprintf(buff,"%d",mem[i].number);
        QTableWidgetItem *item = new QTableWidgetItem(buff);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,0,item);
    }
}

Notepad::~Notepad() {
    delete ui;
}
bool Notepad::quit(){

qApp->quit();
return 0;
}

void Notepad::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
               tr("Text Files (*.txt);;C++ Files (*.cpp *.h)"));

       if (!fileName.isEmpty()) {
           QFile file(fileName);
           if (!file.open(QIODevice::ReadOnly)) {
               QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
               return;
           }
           QTextStream in(&file);
           ui->textEdit->setFontPointSize(15);
           ui->textEdit->setText(in.readAll());
           file.close();
       }

       ui->T->setText(fileName);
}



void Notepad::on_actionSave_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
               tr("Text Files (*.txt);;C++ Files (*.cpp *.h)"));

       if (!fileName.isEmpty()) {
           QFile file(fileName);
           if (!file.open(QIODevice::WriteOnly)) {
               // error message
               QMessageBox::critical(this,tr("Error"),tr("Could not save file"));
               return;
           } else {
               QTextStream stream(&file);
               stream << ui->textEdit->toPlainText();
               stream.flush();
               file.close();
           }
       }
}

void Notepad::on_pushButton_clicked() {
        QString text = ui->T->toPlainText();
        QByteArray array = text.toLocal8Bit();
        char* buffer = array.data();
        cout<<buffer;
        PASS1(buffer);
        int local_memory = 0;
        for(int i = 0; i < 32 ; i++) {
            QString str = ui->tableWidget->item(i,0)->text();
            QByteArray qbyte = str.toLocal8Bit();
            char *byte = qbyte.data();
            mem[local_memory++].number = atoi(byte);
        }
        QString t = ui->number->text();
        QByteArray a11 = t.toLocal8Bit();
        char *b11 = a11.data();
       // memoryPointer = atoi(b11) - 1;
        mem[32].number = atoi(b11);
        QString w = ui->strin->text();
        QByteArray a11l = w.toLocal8Bit();
        b11 = a11l.data();
        if(strlen(b11) != 0) {
            for(int i = 0; i < dataLength; i++) {
                if(dat[i].varname == "STR") {
                    int l = strlen(b11);
                    if(l != 0) {
                        strcpy(dat[i].value,b11);
                        for(int i = 0; i < dataLength; i++) {
                            if(dat[i].varname == "STRL") {
                                char buf[10];
                                sprintf(buf,"%d",l);
                                strcpy(dat[i].value,buf);
                            }
                        }
                    }
                }
            }
        }
        int row = 0;
        for(map<string,int>::iterator it = SYMTAB.begin(); it != SYMTAB.end(); it++,row++) {
            string symbol = it->first;
            int id = it->second;
            const char* buff1 = symbol.c_str();
            char buff2[10];
            sprintf(buff2,"%d",id);
            QTableWidgetItem *item1 = new QTableWidgetItem(buff1);
            item1->setTextAlignment(Qt::AlignCenter);
            QTableWidgetItem *item2 = new QTableWidgetItem(buff2);
            item2->setTextAlignment(Qt::AlignCenter);
            ui->symbol_table->setItem(row,0,item1);
            ui->symbol_table->setItem(row,1,item2);
        }
        /*QString t1 = ui->lineEdit_2->text();
        QByteArray a1 = t1.toLocal8Bit();
        char *b1 = a1.data();
        mem[local_memory++].number = atoi(b1);
        cout<<b1<<endl;
        QString t2 = ui->lineEdit_3->text();
        QByteArray a2 = t2.toLocal8Bit();
        char *b2 = a2.data();
        mem[local_memory++].number = atoi(b2);
        cout<<b2<<endl;
        QString t3 = ui->lineEdit_4->text();
        QByteArray a3 = t3.toLocal8Bit();
        char *b3 = a3.data();
        mem[local_memory++].number = atoi(b3);
        cout<<b3<<endl;
        QString t4 = ui->lineEdit_5->text();
        QByteArray a4 = t4.toLocal8Bit();
        char *b4 = a4.data();
        mem[local_memory++].number = atoi(b4);
        cout<<b4<<endl;
        QString t5 = ui->lineEdit_6->text();
        QByteArray a5 = t5.toLocal8Bit();
        char *b5 = a5.data();
        mem[local_memory++].number = atoi(b5);
        cout<<b5<<endl;
        QString t6 = ui->lineEdit_7->text();
        QByteArray a6 = t6.toLocal8Bit();
        char *b6 = a6.data();
        mem[local_memory++].number = atoi(b6);
        cout<<b6<<endl;
        QString t7 = ui->lineEdit_8->text();
        QByteArray a7 = t7.toLocal8Bit();
        char *b7 = a7.data();
        mem[local_memory++].number = atoi(b7);
        cout<<b7<<endl;
        QString t8 = ui->lineEdit_9->text();
        QByteArray a8 = t8.toLocal8Bit();
        char *b8 = a8.data();
        mem[local_memory++].number = atoi(b8);
        cout<<b8<<endl;
        QString t9 = ui->lineEdit_10->text();
        QByteArray a9 = t9.toLocal8Bit();
        char *b9 = a9.data();
        mem[local_memory++].number = atoi(b9);
        cout<<b9<<endl;
        QString t10 = ui->lineEdit_11->text();
        QByteArray a10 = t10.toLocal8Bit();
        char *b10 = a10.data();
        mem[local_memory++].number = atoi(b10);
        cout<<b10<<endl;
*/
       /* QString t11 = ui->number->text();
        QByteArray a11 = t11.toLocal8Bit();
        char *b11 = a11.data();*/
//        mem[10].number = atoi(b11);



        /*ui->lineEdit_4->setText(b1); ;
        ui->lineEdit_5->setText(b);*/
        int progress = ((float)programCounter / (float)length) * 100;
        ui->progressBar->setValue(progress);
        executeCode();
        progress = ((float)programCounter / (float)length) * 100;
        ui->progressBar->setValue(progress);
        if(ERROR_FLAG == false) {
            //ui->status->setAutoFillBackground(true);
            ui->status->setText("Program Executed Successfully");
        }
        else {
            ui->status->setText("Error in code");
            if(OPCODE_ERROR == true) {
                const char* mes = program[programCounter].opcode.c_str();
                char mess[25] = "Line Number : ";
                strcat(mess,message);
                strcat(mess,"\nOpcode : ");
                strcat(mess,mes);
                strcat(mess," NOT FOUND");
                QMessageBox::critical(this,tr("OPCODE ERROR"),tr(mess));
            }
            else {
                const char* mes = program[programCounter].operand.c_str();
                char mess[25] = "Line Number : ";
                strcat(mess,message);
                strcat(mess,"\nLabel : ");
                strcat(mess,mes);
                strcat(mess," NOT FOUND");
                QMessageBox::critical(this,tr("LABEL ERROR"),tr(mess));
                //QMessageBox::critical(this,tr("ERROR IN  LINE"),tr(message));
            }
            return;
        }

        /*if(strcmp(reg[1].value,"0")!=0 && strcmp(dat[6].value,"0")==0){
            ui->A->setText("FF");
        }
        else {
        ui->A->setText(dat[6].value);
        }

        ui->B->setText(dat[0].value);
        ui->C->setText(dat[5].value);   */

        ui->D->setText(reg[0].value);
        ui->E->setText(reg[1].value);
        ui->F->setText(reg[3].value);

        /*  SET VARIABLES   */
        for(int i = 0; i < dataLength; i++) {
            if(dat[i].varname == "RESULT") {
                ui->result->setText(dat[i].value);
            }
            else if(dat[i].varname == "STR") {
                ui->strin->setText(dat[i].value);
            }
            else if(dat[i].varname == "NUMBER") {
                ui->number->setText(dat[i].value);
            }
        }

        //printDataLines();
        /*if(memoryPointer != 0) {
            if(memoryPointer == 1) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
            }
            else if(memoryPointer == 2) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
                sprintf(buff,"%d",mem[1].number);
                ui->lineEdit_3->setText(buff);
            }
            else if(memoryPointer == 3) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
                sprintf(buff,"%d",mem[1].number);
                ui->lineEdit_3->setText(buff);
                sprintf(buff,"%d",mem[2].number);
                ui->lineEdit_4->setText(buff);
            }
            else if(memoryPointer == 4) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
                sprintf(buff,"%d",mem[1].number);
                ui->lineEdit_3->setText(buff);
                sprintf(buff,"%d",mem[2].number);
                ui->lineEdit_4->setText(buff);
                sprintf(buff,"%d",mem[3].number);
                ui->lineEdit_5->setText(buff);
            }
            else if(memoryPointer == 5) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
                sprintf(buff,"%d",mem[1].number);
                ui->lineEdit_3->setText(buff);
                sprintf(buff,"%d",mem[2].number);
                ui->lineEdit_4->setText(buff);
                sprintf(buff,"%d",mem[3].number);
                ui->lineEdit_5->setText(buff);
                sprintf(buff,"%d",mem[4].number);
                ui->lineEdit_6->setText(buff);
            }
            else if(memoryPointer == 6) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
                sprintf(buff,"%d",mem[1].number);
                ui->lineEdit_3->setText(buff);
                sprintf(buff,"%d",mem[2].number);
                ui->lineEdit_4->setText(buff);
                sprintf(buff,"%d",mem[3].number);
                ui->lineEdit_5->setText(buff);
                sprintf(buff,"%d",mem[4].number);
                ui->lineEdit_6->setText(buff);
                sprintf(buff,"%d",mem[5].number);
                ui->lineEdit_7->setText(buff);
            }
            else if(memoryPointer == 7) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
                sprintf(buff,"%d",mem[1].number);
                ui->lineEdit_3->setText(buff);
                sprintf(buff,"%d",mem[2].number);
                ui->lineEdit_4->setText(buff);
                sprintf(buff,"%d",mem[3].number);
                ui->lineEdit_5->setText(buff);
                sprintf(buff,"%d",mem[4].number);
                ui->lineEdit_6->setText(buff);
                sprintf(buff,"%d",mem[5].number);
                ui->lineEdit_7->setText(buff);
                sprintf(buff,"%d",mem[6].number);
                ui->lineEdit_8->setText(buff);
            }
            else if(memoryPointer == 8) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
                sprintf(buff,"%d",mem[1].number);
                ui->lineEdit_3->setText(buff);
                sprintf(buff,"%d",mem[2].number);
                ui->lineEdit_4->setText(buff);
                sprintf(buff,"%d",mem[3].number);
                ui->lineEdit_5->setText(buff);
                sprintf(buff,"%d",mem[4].number);
                ui->lineEdit_6->setText(buff);
                sprintf(buff,"%d",mem[5].number);
                ui->lineEdit_7->setText(buff);
                sprintf(buff,"%d",mem[6].number);
                ui->lineEdit_8->setText(buff);
                sprintf(buff,"%d",mem[7].number);
                ui->lineEdit_9->setText(buff);
            }
            else if (memoryPointer == 9) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
                sprintf(buff,"%d",mem[1].number);
                ui->lineEdit_3->setText(buff);
                sprintf(buff,"%d",mem[2].number);
                ui->lineEdit_4->setText(buff);
                sprintf(buff,"%d",mem[3].number);
                ui->lineEdit_5->setText(buff);
                sprintf(buff,"%d",mem[4].number);
                ui->lineEdit_6->setText(buff);
                sprintf(buff,"%d",mem[5].number);
                ui->lineEdit_7->setText(buff);
                sprintf(buff,"%d",mem[6].number);
                ui->lineEdit_8->setText(buff);
                sprintf(buff,"%d",mem[7].number);
                ui->lineEdit_9->setText(buff);
                sprintf(buff,"%d",mem[8].number);
                ui->lineEdit_10->setText(buff);
            }
            else if(memoryPointer == 10) {
                char buff[30];
                sprintf(buff,"%d",mem[0].number);
                ui->lineEdit_2->setText(buff);
                sprintf(buff,"%d",mem[1].number);
                ui->lineEdit_3->setText(buff);
                sprintf(buff,"%d",mem[2].number);
                ui->lineEdit_4->setText(buff);
                sprintf(buff,"%d",mem[3].number);
                ui->lineEdit_5->setText(buff);
                sprintf(buff,"%d",mem[4].number);
                ui->lineEdit_6->setText(buff);
                sprintf(buff,"%d",mem[5].number);
                ui->lineEdit_7->setText(buff);
                sprintf(buff,"%d",mem[6].number);
                ui->lineEdit_8->setText(buff);
                sprintf(buff,"%d",mem[7].number);
                ui->lineEdit_9->setText(buff);
                sprintf(buff,"%d",mem[8].number);
                ui->lineEdit_10->setText(buff);
                sprintf(buff,"%d",mem[9].number);
                ui->lineEdit_11->setText(buff);
            }
        }*/
        mem[32].number = 0;
        for(int i = 0; i < memoryPointer; i++) {
            char buff[10];
            sprintf(buff,"%d",mem[i].number);
            QTableWidgetItem *item = new QTableWidgetItem(buff);
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i,0,item);
        }
}

void Notepad::on_clear_clicked() {
    foreach(QLineEdit *widget, this->findChildren<QLineEdit*>()) {
        widget->clear();
        widget->update();
    }
    ui->textEdit->clear();
    ui->T->clear();
    ui->T->update();
    ui->symbol_table->clear();
    ui->symbol_table->update();
    ui->progressBar->setValue(0);
    length = 0;
    dataLength = 0;
    programCounter = 0;
    memoryPointer = 0;
    ERROR_FLAG = false;
    ERROR_LINE = -1;
    OPCODE_ERROR = false;
    first = true;
    COMPARE_FLAG = 0;
    SYMTAB.clear();
    qApp->processEvents();
}

void Notepad::on_pushButton_2_clicked() {
    stringstream sstream[14];
    //  NUMBER
    QString t = ui->number->text();
    QByteArray a = t.toLocal8Bit();
    char *b = a.data();
    int number = atoi(b);
    //stringstream sstream;
    sstream[0]<<hex<<number;
    string output = sstream[0].str();
    const char *c = output.c_str();
    ui->number->setText(c);
    // RESULT
    t = ui->result->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    //stringstream sstream1;
    sstream[1]<<hex<<number;
    output = sstream[1].str();
    const char *c1 = output.c_str();
    ui->result->setText(c1);
    //  Accumulator
    t = ui->D->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    //sstream.flush();
    sstream[2]<<hex<<number;
    output = sstream[2].str();
    const char *c2 = output.c_str();
    ui->D->setText(c2);
    //  Index
    t = ui->E->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[3]<<hex<<number;
    output = sstream[3].str();
    const char *c3 = output.c_str();
    ui->E->setText(c3);

    for(int i = 0; i < memoryPointer; i++) {
            QString str = ui->tableWidget->item(i,0)->text();
            QByteArray qba = str.toLocal8Bit();
            char *c = qba.data();
            int n = atoi(c);
            sstream[i + 4]<<hex<<n;
            output = sstream[i + 4].str();
            const char *cd = output.c_str();
            QTableWidgetItem *qtwi = new QTableWidgetItem(cd);
            qtwi->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i,0,qtwi);
    }
    /*  Memory Locations    */
   /* t = ui->lineEdit_2->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[4]<<hex<<number;
    output = sstream[4].str();
    const char *c4 = output.c_str();
    ui->lineEdit_2->setText(c4);

    t = ui->lineEdit_3->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[5]<<hex<<number;
    output = sstream[5].str();
    const char *c5 = output.c_str();
    ui->lineEdit_3->
Text(c5);

    t = ui->lineEdit_4->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[6]<<hex<<number;
    output = sstream[6].str();
    const char *c6 = output.c_str();
    ui->lineEdit_4->setText(c6);

    t = ui->lineEdit_5->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[7]<<hex<<number;
    output = sstream[7].str();
    const char *c7 = output.c_str();
    ui->lineEdit_5->setText(c7);

    t = ui->lineEdit_6->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[8]<<hex<<number;
    output = sstream[8].str();
    const char *c8 = output.c_str();
    ui->lineEdit_6->setText(c8);

    t = ui->lineEdit_7->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[9]<<hex<<number;
    output = sstream[9].str();
    const char *c9 = output.c_str();
    ui->lineEdit_7->setText(c9);

    t = ui->lineEdit_8->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[10]<<hex<<number;
    output = sstream[10].str();
    const char *c10 = output.c_str();
    ui->lineEdit_8->setText(c10);

    t = ui->lineEdit_9->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[11]<<hex<<number;
    output = sstream[11].str();
    const char *c11 = output.c_str();
    ui->lineEdit_9->setText(c11);

    t = ui->lineEdit_10->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[12]<<hex<<number;
    output = sstream[12].str();
    const char *c12 = output.c_str();
    ui->lineEdit_10->setText(c12);

    t = ui->lineEdit_11->text();
    a = t.toLocal8Bit();
    b = a.data();
    number = atoi(b);
    sstream[13]<<hex<<number;
    output = sstream[13].str();
    const char *c13 = output.c_str();
    ui->lineEdit_11->setText(c13);*/

}

int firstClicked = true;

void Notepad::on_debug_clicked() {
    if(firstClicked) {
        QString text = ui->T->toPlainText();
        QByteArray array = text.toLocal8Bit();
        char* buffer = array.data();
        cout<<buffer;

        PASS1(buffer);
        int local_memory = 0;
        for(int i = 0; i < 32 ; i++) {
            QString str = ui->tableWidget->item(i,0)->text();
            QByteArray qbyte = str.toLocal8Bit();
            char *byte = qbyte.data();
            mem[local_memory++].number = atoi(byte);
        }
        QString t = ui->number->text();
        QByteArray a11 = t.toLocal8Bit();
        char *b11 = a11.data();
       // memoryPointer = atoi(b11) - 1;
        mem[32].number = atoi(b11);
        QString w = ui->strin->text();
        QByteArray a11l = w.toLocal8Bit();
        b11 = a11l.data();
        if(strlen(b11) != 0) {
            for(int i = 0; i < dataLength; i++) {
                if(dat[i].varname == "STR") {
                    int l = strlen(b11);
                    if(l != 0) {
                        strcpy(dat[i].value,b11);
                        for(int i = 0; i < dataLength; i++) {
                            if(dat[i].varname == "STRL") {
                                char buf[10];
                                sprintf(buf,"%d",l);
                                strcpy(dat[i].value,buf);
                            }
                        }
                    }
                }
            }
        }
        int row = 0;
        for(map<string,int>::iterator it = SYMTAB.begin(); it != SYMTAB.end(); it++,row++) {
            string symbol = it->first;
            int id = it->second;
            const char* buff1 = symbol.c_str();
            char buff2[10];
            sprintf(buff2,"%d",id);
            QTableWidgetItem *item1 = new QTableWidgetItem(buff1);
            item1->setTextAlignment(Qt::AlignCenter);
            QTableWidgetItem *item2 = new QTableWidgetItem(buff2);
            item2->setTextAlignment(Qt::AlignCenter);
            ui->symbol_table->setItem(row,0,item1);
            ui->symbol_table->setItem(row,1,item2);
        }
       // memoryPointer = atoi(b11) - 1;
        //mem[32].number = atoi(b11);
        firstClicked = false;

    }
    int status = debugCode();
    int progress = ((float)programCounter / (float)length) * 100;
    if(progress == 100) {
        ui->status->setText("Program Executed Successfully");
    }
    ui->progressBar->setValue(progress);
    cout<<"Progress : "<<progress<<endl;
    if(true) {
        char ptr[10];
        sprintf(ptr,"%d",programCounter);
        ui->pc->setText(ptr);
        if(programCounter != 1) {
            const char *label = program[programCounter - 2].label.c_str();
            const char *opcode = program[programCounter - 2].opcode.c_str();
            const char *operand = program[programCounter - 2].operand.c_str();
            QTableWidgetItem *lab = new QTableWidgetItem(label);
            QTableWidgetItem *opc = new QTableWidgetItem(opcode);
            QTableWidgetItem *opr = new QTableWidgetItem(operand);
            ui->tableWidget_2->setItem(0,0,lab);
            ui->tableWidget_2->setItem(0,1,opc);
            ui->tableWidget_2->setItem(0,2,opr);
        }
        if(programCounter != length) {
            const char *label = program[programCounter].label.c_str();
            const char *opcode = program[programCounter].opcode.c_str();
            const char *operand = program[programCounter].operand.c_str();
            QTableWidgetItem *lab = new QTableWidgetItem(label);
            QTableWidgetItem *opc = new QTableWidgetItem(opcode);
            QTableWidgetItem *opr = new QTableWidgetItem(operand);
            ui->tableWidget_2->setItem(2,0,lab);
            ui->tableWidget_2->setItem(2,1,opc);
            ui->tableWidget_2->setItem(2,2,opr);
        }
        const char *label = program[programCounter - 1].label.c_str();
        const char *opcode = program[programCounter - 1].opcode.c_str();
        const char *operand = program[programCounter - 1].operand.c_str();
        QTableWidgetItem *lab = new QTableWidgetItem(label);
        QTableWidgetItem *opc = new QTableWidgetItem(opcode);
        QTableWidgetItem *opr = new QTableWidgetItem(operand);
        lab->setTextColor(QColor::fromRgb(255,0,0));
        opc->setTextColor(QColor::fromRgb(255,0,0));
        opr->setTextColor(QColor::fromRgb(255,0,0));
        ui->tableWidget_2->setItem(1,0,lab);
        ui->tableWidget_2->setItem(1,1,opc);
        ui->tableWidget_2->setItem(1,2,opr);
        ui->D->setText(reg[0].value);
        ui->E->setText(reg[1].value);
        ui->F->setText(reg[3].value);

        // Data Update
        for(int i = 0; i < dataLength; i++) {
            if(dat[i].varname == "RESULT") {
                ui->result->setText(dat[i].value);
            }
            else if(dat[i].varname == "STR") {
                ui->strin->setText(dat[i].value);
            }
            else if(dat[i].varname == "NUMBER") {
                ui->number->setText(dat[i].value);
            }
        }

        //MEMORY UPDATE
        for(int i = 0; i < memoryPointer; i++) {
            char buff[10];
            sprintf(buff,"%d",mem[i].number);
            QTableWidgetItem *item = new QTableWidgetItem(buff);
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i,0,item);
        }
        if(programCounter >= length) {
            for(int k = 0; k < 3; k++) {
                QTableWidgetItem *lab = new QTableWidgetItem("");
                ui->tableWidget_2->setItem(2,k,lab);
                    //ui->tableWidget_2->setItem(2,1,lab);
                    //ui->tableWidget_2->setItem(2,2,lab);
            }
        }
    }
    else if(status == -1) {
        if(ERROR_FLAG == false) {
            //ui->status->setAutoFillBackground(true);
            ui->status->setText("Program Executed Successfully");
        }
        else {
            ui->status->setText("Error in code");
            if(OPCODE_ERROR == true) {
                const char* mes = program[programCounter].opcode.c_str();
                char mess[25] = "Line Number : ";
                strcat(mess,message);
                strcat(mess,"\nOpcode : ");
                strcat(mess,mes);
                strcat(mess," NOT FOUND");
                QMessageBox::critical(this,tr("OPCODE ERROR"),tr(mess));
            }
            else {
                const char* mes = program[programCounter].operand.c_str();
                char mess[25] = "Line Number : ";
                strcat(mess,message);
                strcat(mess,"\nLabel : ");
                strcat(mess,mes);
                strcat(mess," NOT FOUND");
                QMessageBox::critical(this,tr("LABEL ERROR"),tr(mess));
                //QMessageBox::critical(this,tr("ERROR IN  LINE"),tr(message));
            }
            return;
        }
    }
}

void Notepad::on_pushButton_3_clicked() {
    for(int i = 0; i < 100 ; i++) {
        mem[i].number = 0;
    }
    for(int i = 0; i < 100; i++) {
        char buff[10];
        sprintf(buff,"%d",mem[i].number);
        QTableWidgetItem *item = new QTableWidgetItem(buff);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,0,item);
    }

}

void Notepad::on_decimal_clicked() {
    ui->D->setText(reg[0].value);
    ui->E->setText(reg[1].value);
    ui->F->setText(reg[3].value);

    /*  SET VARIABLES   */
    for(int i = 0; i < dataLength; i++) {
        if(dat[i].varname == "RESULT") {
            ui->result->setText(dat[i].value);
        }
        else if(dat[i].varname == "STR") {
            ui->strin->setText(dat[i].value);
        }
        else if(dat[i].varname == "NUMBER") {
            ui->number->setText(dat[i].value);
        }
    }
    for(int i = 0; i < memoryPointer; i++) {
        char buff[10];
        sprintf(buff,"%d",mem[i].number);
        QTableWidgetItem *item = new QTableWidgetItem(buff);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,0,item);
    }

}

void Notepad::on_pushButton_4_clicked() {
    length = 0;
    dataLength = 0;
    programCounter = 0;
    memoryPointer = 0;
    ERROR_FLAG = false;
    ERROR_LINE = -1;
    OPCODE_ERROR = false;
    firstClicked = true;
    first = true;
    COMPARE_FLAG = 0;
    SYMTAB.clear();
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            QTableWidgetItem *lab = new QTableWidgetItem("");
            ui->tableWidget_2->setItem(i,j,lab);
        }
    }
    qApp->processEvents();
}
