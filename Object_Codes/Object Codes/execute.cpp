#include <bits/stdc++.h>

using namespace std;
enum type {INTEGER = 0, STRING = 1, FLOATING = 2} ;

class Data {
	public :
		int val;
		string s;
		double num;
		type var;

	Data(int x) : val(x) { var = INTEGER ;};
	Data(string x) : s(x) { var = STRING ;} ;
	Data(double x) : num(x) { var = FLOATING ; };
	Data (Data &d) {
		this->num = d.num;
	}
};





//map<string,Data*> varlist;
map<string, float> varlist;
map<char, float> registers;
map<string,int>labels;
int cc;
int linenum=1;
void parseData (char* filename) {
	string line;
	registers['A'] = 0.0;
	registers['S'] = 0;
	registers['L'] = 0;
	registers['X'] = 0;

	ifstream opcode(filename);
	if(opcode.is_open()) {
		getline(opcode, line);
		while(getline(opcode,line)) {
			if(line != ".CODE") {
				int fpos = line.find(" ");
				int lpos = line.rfind(" ");
				string varname = line.substr(0,fpos);
				string instr = line.substr(fpos + 1, lpos - 2);
				string data = line.substr(lpos + 1, line.size());
				if(instr == "BYTE" || instr == "WORD") {
					if(instr == "WORD") {
//						Data d(data);
						varlist[varname] = 0.0;
					}
					else {
						const char *value = new char[data.size() + 1];
						value = data.c_str();
						if(instr.find('.') != string::npos) {
							Data d(atoi(value));
							varlist[varname] = d.var;
						}
						else {
							Data d(atof(value));
							varlist[varname] = d.num;
						}
					//	cout<<varlist[varname]->num<<endl;
					}
				}
				else if(instr == "RESB 1") {
					Data d(0.0);
					varlist[varname] = d.num;
				}
				else {
					Data d("");
					varlist[varname] = d.num;
				}
		//		cout<<varname<<" "<<varlist[varname]<<endl;
			}
			else
				break;
		}
	}
}

void parseProgram (char *filename) {
	FILE *fp;
	fp = fopen(filename, "rw");
	ifstream opcode(filename);
	string line;
	if(opcode.is_open()) {
		while(line != ".CODE") {
			//cout<<line<<endl;
			getline(opcode, line);
		}
	}
	while(getline(opcode,line)) {
		if(line ==  "END") {
			break;
		}
		
		int fpos = line.find(" ");
		int lpos = line.rfind(" ");
		string label = line.substr(0,fpos);
		labels[label]=linenum;
		cout<<label<<" "<<labels[label]<<endl;
		string instr = line.substr(fpos + 1, lpos - 5);
		string operand = line.substr(lpos + 1, line.size());
		//cout<<label<<"\t"<<instr<<"\t"<<operand<<endl;
		if(instr == "LDA") {
			float data = varlist[operand];
			registers['A'] = data;
		linenum++;
		}
		else if (instr=="LDX")
		{
			/* code */
			float data=varlist[operand];
			registers['X']=data;
			linenum++;
		}
		else if (instr == "STA") {
			Data d(registers['A']);
			varlist[operand] = d.num;
		linenum++;
		}
		else if(instr == "ADDF") {
			float a = varlist[operand];
			float b = registers['A'];
			//cout<<"ADD : a = "<<a<<" , b = "<<b<<endl;
			registers['A'] = a + b;
			linenum++;
		}
		else if(instr == "SUBF") {
			float a = varlist[operand];
			float b = registers['A'];
			//cout<<"SUB : a = "<<a<<" , b = "<<b<<endl;
			registers['A'] = b - a;
			linenum++;
		}
		else if(instr == "MULF") {
			float a = varlist[operand];
			float b = registers['A'];
			registers['A'] = a * b;
			linenum++;
		}
		else if(instr=="TIX"){
			registers['X']++;
			if(registers['X']==varlist[operand])
				cc=0;
			else if(registers['X']>varlist[operand])
				cc=1;
			else
				cc=2;
			linenum++;
		}
		else if(instr=="JEQ"){
			if(cc==0) {
				fseek(fp,labels[operand],SEEK_SET);
			}
		}
		else if(instr=="JLT") {
			if(cc==2)
				fseek(fp,labels[operand],SEEK_SET);
		}
		else if(instr=="JGT") {
			if(cc==1)
				fseek(fp,labels[operand],SEEK_SET);
		}
	}
}


void displayMemoryStatus() {
	cout<<"Register Status"<<endl;
	cout<<"---------------"<<endl;
	ofstream reg("reg.txt");
	ofstream mem("mem.txt");
	for(map<char, float>::iterator it = registers.begin(); it != registers.end(); it++) {
		cout<<it->first<<"\t"<<it->second<<endl;
		reg<<it->first<<","<<it->second<<endl;
	}
	cout<<endl<<endl;
	cout<<"Memory Status"<<endl;
	cout<<"-------------"<<endl;
	//cout<<varlist.size()<<endl;
	int flag = false;
	for(map<string, float>::iterator it = varlist.begin(); it != varlist.end(); it++) {
		if(flag) {
			cout<<it->first<<"\t"<<(it->second)<<endl;
			mem<<it->first<<","<<it->second<<endl;
		}
		flag = true;
	}
}

int main(void) {
	cout<<"Enter the filename : ";
	char filename[25];
	scanf("%s",filename);
	//char *filename = "Floating.txt";
	parseData(filename);
	parseProgram(filename);
	displayMemoryStatus();
}
