#include "../includes_usr/constants.h"
#include "../includes_usr/dispatcher.h"
#include "../includes_usr/file_io.h"
#include "../includes_usr/joblist.h"
#include "../includes_usr/logger_single_thread.h"
#include <queue>

PCB runningPCB;

std::queue<PCB> readyQ, blockedQ;
std::deque<PCB> blah;


//clears ready_Q and blocked_Q these are queues of PCB structures,
//initializes runningPCB to default values in constants (see PCB structure)
void dispatcher::init() {

	std::queue<PCB> empty;
	std::swap (readyQ, empty);
	std::swap (blockedQ, empty);

	PCB newPCB;
	runningPCB = newPCB;
}

//used for testing, return a copy of runningPCB
PCB dispatcher::getCurrentJob() {
	return runningPCB;
}

//add a job to the ready queue
void dispatcher::addJob(PCB &myPCB) {
	readyQ.push(myPCB);
}


//interrupt can be either;
//a switch process interrupt in which case the function performs the appropriate tasks and returns PCB_SWITCHED_PROCESSES
//or a io_complete interrupt in which case it pulls ALL processes off of the blockedQ and returns either PCB_MOVED_FROM_BLOCKED_TO_READY (if there were any)
//or PCB_BLOCKED_QUEUE_EMPTY if there were none.
int dispatcher::processInterrupt(int interrupt) {

	if (interrupt == SWITCH_PROCESS) { 		//5 = timeslice interrupt
		if (!readyQ.empty()) {
			PCB tmp = readyQ.front(); //front???
			readyQ.push(runningPCB);
			runningPCB = tmp; 	//should probably be a deep copy??
			return PCB_SWITCHED_PROCESSES;
		}
		else {
			if (blockedQ.empty()) {
				return NO_JOBS;
			}
			else {
				return BLOCKED_JOBS;
			}
		}
	}
	else if(interrupt == IO_COMPLETE) {

		if (!blockedQ.empty()) {
			for (unsigned int i = 0; i < blockedQ.size(); i++) {
				readyQ.push(blockedQ.front());
				blockedQ.pop();
			}
			return PCB_MOVED_FROM_BLOCKED_TO_READY;
		}
		else {
			return PCB_BLOCKED_QUEUE_EMPTY;
		}
	}

	else {
		return PCB_UNIMPLEMENTED;
	}
}


//see flowchart
int dispatcher::doTick() {
	int returnval = 0;

	if (runningPCB.cpu_time != UNINITIALIZED &&
		runningPCB.io_time != UNINITIALIZED &&
		runningPCB.process_number != UNINITIALIZED &&
		runningPCB.start_time != UNINITIALIZED) { 	// is there a runningPCB? == YES

		runningPCB.cpu_time -= 1;	//if running, decrement cpu_time by 1 --- indicate work done

		if (runningPCB.cpu_time <= 0) { // is current job finished? == YES
			if (runningPCB.io_time == 1) { // does runningPCB make a blocking IO call? == YES
				runningPCB.io_time = 0;		//set io_time to 0
				blockedQ.push(runningPCB);  //then move to blockedQ
				returnval = PCB_ADDED_TO_BLOCKED_QUEUE;
			}
			else if (runningPCB.io_time == 0){	// does runningPCB make a blocking IO call? == NO
				returnval = PCB_FINISHED; //HERE, SHOULD POP DEAD PROCESS???
			}

			else { // does runningPCB make a blocking IO call? == neither 1 or 0
				return FAIL;
			}
		}

		else {	// is current job finished? == NO
			return PCB_CPUTIME_DECREMENTED;
		}
	}

	else if (runningPCB.cpu_time == UNINITIALIZED &&
			runningPCB.io_time == UNINITIALIZED &&
			runningPCB.process_number == UNINITIALIZED &&
			runningPCB.start_time == UNINITIALIZED) { // is there a running PCB? == NO

		if (readyQ.empty()) {		//is the readyQ empty? == YES

					if (blockedQ.empty()) { //both readyQ and blockedQ are empty
						return NO_JOBS;
					}
					else { //just readyQ is empty
						return BLOCKED_JOBS;
					}
				}
				else {			//is the readyQ empty? == NO
					//***********Do some logic to see which job deserves priority!!

//					runningPCB = readyQ.front(); //load job into runningPCB
//					readyQ.pop();
					//return PCB_MOVED_FROM_READY_TO_RUNNING;


					bool flag = true;
					const int rQSize = readyQ.size();
					if (rQSize > 1) {
						while (flag) {
							runningPCB = readyQ.front(); //load job into runningPCB

							if (runningPCB.cpu_time == 0) {
								blockedQ.push(runningPCB); //dont think you can go straight to blockedQ
															//that decision might have to be made UP there while
															//it is the current runningPCB
							}
							else if (runningPCB.cpu_time > 0) {
								flag = false;
							}
							readyQ.pop();
						}
					}

					else if (rQSize == 1) {
						runningPCB = readyQ.front(); //load job into runningPCB
						readyQ.pop();
					}

					return PCB_MOVED_FROM_READY_TO_RUNNING;
				}
	}

	//unload or mark runningPCB as invalid
	runningPCB.cpu_time = UNINITIALIZED;
	runningPCB.io_time = UNINITIALIZED;
	runningPCB.process_number = UNINITIALIZED;
	runningPCB.start_time = UNINITIALIZED;

	return returnval;
}

