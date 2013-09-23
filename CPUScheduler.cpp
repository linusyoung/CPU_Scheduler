
#include "Scheduler.hpp"

/*
	test
	print the result of input and output
	@precon	example test file stored in a text format
	@param	string test - file name of test file
	
*/

void test(string test){
	Process p;
	p.initial(test, 9);			
	Scheduler cpu(10);
	Process_prop *get;
	get = p.getProcess();
	int number=p.getNumber();
	cpu.initial_scheduler(get,number);
	cpu.run_process();
	p.print();
	cpu.print_result();
}
int main(){
	string test1="test1",test2="test2",test3="test3";
	test(test1);
	//test(test2);
	//test(test3);
}
