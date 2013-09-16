#include <string>
using namespace std;
	
/*
	Process_prop consists of the struct of a process, including name, creation_time
	priority, age, age_count, required_time;
	all properties will be given by the input text file except age_count
*/
struct Process_prop{
	string name;
	int creation_time;
	int priority;
	int age;
	int age_count;
	int required_time;
};

/*
	Process_result is used for recording output of the process.
*/
struct Process_result{
	string name;
	int start;
	int pause;
	int end;
	int ready;
	int running;
	int waiting;
	bool first_run;
};

