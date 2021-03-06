#include <iostream>
#include <queue>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include "ProcessStruct.hpp"
#include "Process.hpp"
#define PRIORITY 4

using namespace std;

class Scheduler{
private:
	int runtime;
	int total_burst;
	//rr stands for round robin time;
	int rr;
	vector <Process_result> output;
	queue <Process_prop> hpq_q1,lpq_q2;
	queue <Process_prop> hpq_p4,hpq_p5,hpq_p6;
	queue <Process_prop> ready_h,ready_l;
	vector <Process_prop> sub_hpq;

	/*
		swap
		swap the value in the vector
		@param	int a - index of first value
				int b - index of second value
				vector <Process_prop> &subq - a vector needs to be swap
	*/
	void swap(int a, int b, vector <Process_prop> &subq){
		Process_prop temp;
		temp = subq[a];
		subq[a] = subq[b];
		subq[b] = temp;
	}
	/*
		set_output
		create output information of a process,which are ready time and waiting
		@param	int index - index of process
	*/	
	void set_output(int index){
		output[index].running+=rr;
		if (output[index].first_run){
			output[index].ready=runtime;
			output[index].first_run=!output[index].first_run;
		}
		else{
			output[index].waiting=output[index].waiting+
									runtime-output[index].pause;
		}

	}
/*
	buildSub
	scheduler will push the higher process into differet sub queue 
	according to the priority of that process.
	after that, hpq_q1 will be empty.
*/

	void buildSub(){
		int priority;
		Process_prop current_process;
		while (hpq_q1.size()>0){
			current_process = hpq_q1.front();
			priority=current_process.priority;
			switch (priority){
				case 4:
					hpq_p4.push(current_process);
					break;
				case 5:
					hpq_p5.push(current_process);
					break;
				case 6:
					hpq_p6.push(current_process);
					break;
			}
			hpq_q1.pop();
		}
	}
	/*
		sort_ready
		CPU will get new processes after running, the new process is appended onto ready queue at first
		ready queue is sorted by creation time.
		@param	queue <Process_prop> ready - a ready queue 
		@return queue <Process_prop> sorted - a queue sorted by creation time
	*/
	
	queue <Process_prop> sort_ready(queue <Process_prop> ready){
		queue <Process_prop> sorted;
		vector <Process_prop> temp;
		while(ready.size()!=0){
			temp.push_back(ready.front());
			ready.pop();
		}
		int newest,push=0;
		int size=temp.size();
		for (int i=0; i<size; i++){
			newest= temp[i].creation_time;
			push=i;
			for (int j=i+1; j<size; j++){
				if (temp[j].creation_time<newest){
					newest = temp[j].creation_time;
					push=j;
				}
			}
			if (push!=i){
				swap(i,push,temp);
			}
			sorted.push(temp[i]);
		}
		
		return sorted;
	}

	/*
		check_creation
		when creation time equals current cpu run time, then push this process
		for high priority process, push into the running queue, which is sub_hpq, re-sort this queue by current priority of processes
		for low priority process, push into the low priority queue, which is lpq_q2.
	*/
	bool check_creation(){
		queue <Process_prop> temp_h,temp_l;
		vector <Process_prop> priority;
		bool nph=false,npl=false;
		if (ready_h.size() || ready_l.size()){
			while(ready_h.size()!=0){
				if (ready_h.front().creation_time==runtime){
					int pp = ready_h.front().priority;
					switch (pp){
					case 6:
						hpq_p6.push(ready_h.front());
						break;
					case 5:
						hpq_p5.push(ready_h.front());
						break;
					case 4:
						hpq_p4.push(ready_h.front());
						break;
					}
					nph=true;	
				}
				else{
					temp_h.push(ready_h.front());
				}
				ready_h.pop();
			}
			while(ready_l.size()!=0){
				if (ready_l.front().creation_time==runtime){
					lpq_q2.push(ready_l.front());
					npl=true;
				}
				else{
					temp_l.push(ready_l.front());
				}
				ready_l.pop();
			}
			
			ready_h=temp_h;
			ready_l=temp_l;
		}
		if(nph || npl){
			return true;
		}
		else{
			return false;	
		}
	}
public:
	Scheduler(int rr);
	void initial_scheduler(Process_prop *p,int number);
	void print_result();
	void run_process();
};

/*
	Constructor
	initial round robin unit and runtime to 0 as default;
	default round robin unit is 10;
*/
Scheduler::Scheduler(int rr=10){
	this->rr = rr;
	this->runtime=0;
}

/*
	initial_scheduler
	push processes into different queue by their priority and creation time
	if the creation time equals zero, it will be pushed into corresponding high priority or low priority running queue.
	Otherwise, it will be pushed into ready queue at first. then these processes will be sorted by creation time
	in order to be executed when it is "created".
	total cpu required time is sum up as well.
	the process name is added into output vector and set all first_run value to true in order to record first time gets cpu time
*/

void Scheduler::initial_scheduler(Process_prop *p,int number){
	total_burst=0;
	vector <Process_prop> priority;
	for (int i=0; i<number; i++){
		if(p[i].priority>=PRIORITY){
			if (p[i].creation_time==0){
				hpq_q1.push(p[i]);
			}
			else{
				ready_h.push(p[i]);
			}
		}
		else{
			if(p[i].creation_time==0){
				lpq_q2.push(p[i]);
			}
			else{
				ready_l.push(p[i]);
			}	
		}
	}
	buildSub();
	if (ready_l.size()){
		ready_l = sort_ready(ready_l);
	}
	for (int i=0; i<number; i++){
		output.push_back(Process_result());
		output[i].name=p[i].name;
		output[i].first_run=true;
		total_burst=total_burst+p[i].required_time;
	}
}
/*
	print_result
	screen out the output data as specific format;
*/
void Scheduler::print_result(){
	cout << "Output:" << endl;
	cout << setw(10) << left << "process"
		<<setw(10) << left << "start"
		<<setw(10) << left << "end"
		<<setw(10) << left << "ready"
		<<setw(10) << left << "running"
		<<setw(10) << left << "waiting" << endl;
	int task=output.size();
	int turnaround[task];
	int waiting[task];
	for (int i=0; i<task; i++){
		cout << setw(10) << left << output[i].name 
		<<setw(10) << left << output[i].start
		<<setw(10) << left << output[i].end
		<<setw(10) << left << output[i].ready
		<<setw(10) << left << output[i].running
		<<setw(10) << left << output[i].waiting << endl;
		turnaround[i]=output[i].end-output[i].start;
		waiting[i]=output[i].ready+output[i].waiting;
	}
	int max_turnaround=turnaround[0];
	int max_waiting=waiting[0];
	for(int i=1; i<task; i++){
		if(turnaround[i]>max_turnaround){
			max_turnaround = turnaround[i];
		}
		if(waiting[i]>max_waiting){
			max_waiting = waiting[i];
		}
	}
	double sum_turnaround=0;
	double sum_waiting=0;
	for(int i=0; i<task; i++){
		sum_turnaround+=turnaround[i];
		sum_waiting+=waiting[i];
	}
	double avg_t,avg_w;
	avg_t=sum_turnaround/task;
	avg_w=sum_waiting/task;
	cout << "Taskcompleted = " << task << endl;
	cout << "Maxturnaroundtime = " << max_turnaround << endl;
	cout << "Averageturnaroundtime = " << avg_t << endl;
	cout << "Maxwaitingtime = " << max_waiting << endl;
	cout << "Averagewaitingtime = " <<  avg_w << endl;
}


/*
	run_process
	this is the main function for scheduling the process
	the scheduler will run the process by the order of priority 
	if the higher queue is not empty, then running this queue. 
	after 2 runs, the priority of this process will be decreased by 1.
	after each run, the process will be pushed into the end of current queue or low priority queue according to the priority
	the process in the low priority queue has a age mechanism that will be promoted by 1 after running for a certain unit time(e.g 300)
	every process will be run a quantum unit time, which is 10 units in this case.
	scheduler will check if any new process has been created after executing one quantum unit time.
	
*/

void Scheduler::run_process(){
	Process_prop running;
	int index;
	bool serve=true,current_q2=false;
	string id;
	while(total_burst>0){
		if(hpq_p6.size()!=0){
			running= hpq_p6.front();
			hpq_p6.pop();
			id = running.name;
			index = id[1]-'0';
			if (output[index].first_run){
				output[index].start=running.creation_time;
			}
			set_output(index);
			running.required_time-=rr;
			if (running.required_time>0){
				if (output[index].running % 20!=0){
					hpq_p6.push(running);
				}
				else{
					running.priority--;
					if (running.priority<PRIORITY){
						lpq_q2.push(running);
					}
					else{
						hpq_p5.push(running);
					}
				}
			}
		}
		else if (hpq_p5.size()!=0){
			running= hpq_p5.front();
			hpq_p5.pop();
			id = running.name;
			index = id[1]-'0';
			if (output[index].first_run){
				output[index].start=running.creation_time;
			}
			set_output(index);
			running.required_time-=rr;
			if (running.required_time>0){
				if (output[index].running % 20!=0){
					hpq_p5.push(running);
				}
				else{
					running.priority--;
					if (running.priority<PRIORITY){
						lpq_q2.push(running);
					}
					else{
						hpq_p4.push(running);
					}
				}
			}

		} 
		else if(hpq_p4.size()!=0){
			current_q2=false;
			running= hpq_p4.front();
			hpq_p4.pop();
			id = running.name;
			index = id[1]-'0';
			if (output[index].first_run){
				output[index].start=running.creation_time;
			}
			set_output(index);
			running.required_time-=rr;
			if (running.required_time>0){
				if (output[index].running % 20!=0){
					hpq_p4.push(running);
				}
				else{
					running.priority--;
					if (running.priority<PRIORITY){
						lpq_q2.push(running);
					}
				}
			}
		}
		else{
			current_q2=true;
			if (serve){
	        	running=lpq_q2.front();
            	id = running.name;
           	 	index = id[1]-'0';
				serve=false;
			}
			if (output[index].first_run){
				output[index].start=running.creation_time;
			}

            set_output(index);
            running.required_time-=rr;
            if (running.required_time>0){
                running.age_count++;
            	if (running.age_count==10){
	    			running.age++;
            	}               
            	if (running.age==3){
            		running.priority++;
                	running.age=0;
            	}
            	if (running.priority>=PRIORITY){
	        		hpq_p4.push(running);
					serve=true;
            	}
            }
			else{
				serve=true;
            	lpq_q2.pop();
			}
		}
		//runtime increases rr time and total burst time decreases
		runtime+=rr;
		if (running.required_time==0){
			output[index].end=runtime;
		}
		else{
			output[index].pause=runtime;
		}
		total_burst-=rr;
		bool new_process=check_creation();	
		/* preemptive low priority queue
			while current process that is not finished is in low priority 
			queue, if there is new process coming, the current one will 
			be push into the end of current queue.
		*/
		if (running.required_time>0){
			if (new_process && current_q2){
				lpq_q2.pop();
				lpq_q2.push(running);
				serve=true;	
			}
		}
	}
}

