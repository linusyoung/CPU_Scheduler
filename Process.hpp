class Process{
private:
	int number;
	Process_prop *p;		

public:
	void initial(string &input, int number_process);
	Process_prop *getProcess();
	void print();
	int getNumber();
	~Process();
};

/*
	initial
	read in the process info from a text file, which includes fields of
	name, creation time, priority, age, required cpu time
	@precon input string is a valid test file name
	@param	string &input - test file name
			int number_process - number of process in the test file	

*/
void Process::initial(string &input, int number_process){
	ifstream rawdata;
	string line;
	p = new Process_prop [number_process];
	rawdata.open(input.c_str());
	int r=0,c;
	if (rawdata.is_open()){
		while(!rawdata.eof()){
			getline(rawdata,line);
			stringstream ss(line);
			ss >> p[r].name; 
			ss >> p[r].creation_time;
			ss >> p[r].priority;
			ss >> p[r].age;
			ss >> p[r].required_time;
			p[r].age_count=0;
			r++;
		}

	}
	rawdata.close();	
	this->number = number_process;
}

/*
	Process_prop *getProcess()
	return process to the cpu in order to run these processes
	@precon	processes have been initialised
	@return	Process_prop start_process - a pointer to the initial processes
*/
Process_prop * Process::getProcess(){
	Process_prop *start_process = new Process_prop[number];
	for(int i=0; i<number; i++){
		start_process[i] = p[i];
	}
	return start_process;
}

/*
	void print()
	print out the input test file;
*/
void Process::print(){
	cout << "Input:" << endl;
	cout << setw(10) << left << "process"
		<<setw(10) << left << "creation"
		<<setw(10) << left << "priority"
		<<setw(10) << left << "age"
		<<setw(10) << left << "required time" << endl;
	
	for (int i=0; i<number; i++){
		cout << setw(10) << left << p[i].name 
		<<setw(10) << left << p[i].creation_time
		<<setw(10) << left << p[i].priority
		<<setw(10) << left << p[i].age
		<<setw(10) << left << p[i].required_time << endl;
		
	}

}

int Process::getNumber(){
	return number;
}

Process::~Process(){
	delete [] p;
}

