#include "../includes_usr/constants.h"
#include "../includes_usr/dispatcher.h"
#include "../includes_usr/file_io.h"
#include "../includes_usr/joblist.h"
#include "../includes_usr/logger_single_thread.h"
#include <queue>

PCB runningPCB;

std::queue<PCB> readyQ, blockedQ;

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

	if (interrupt == SWITCH_PROCESS) { 		// is it a timeslice interrupt? == true
		if (!readyQ.empty()) {			// are there any jobs to Switch to == true
			PCB tmp = readyQ.front(); 		//get a tmpPCB from the readyQ

			if (runningPCB.cpu_time > 0) {		//check if current job is worthy of being added back to readyQ
				readyQ.push(runningPCB);
			}
			else if (runningPCB.cpu_time == 0 && runningPCB.io_time == 0) {
				readyQ.push(runningPCB);
			}
			runningPCB = tmp;	//make runningPCB = tmp
			readyQ.pop();
			return PCB_SWITCHED_PROCESSES;
		}
		else {		//are there any jobs to switch to? == false
			if (blockedQ.empty()) {
				return NO_JOBS;
			}
			else {
				return BLOCKED_JOBS;
			}
		}
	}
	else if(interrupt == IO_COMPLETE) {  // is it an io complete interrupt? == true

		if (!blockedQ.empty()) {		// are there any jobs to switch to? == true
			for (unsigned int i = 0; i < blockedQ.size(); i++) {   //add jobs from blockedQ to readyQ
				readyQ.push(blockedQ.front());
				blockedQ.pop();
			}
			return PCB_MOVED_FROM_BLOCKED_TO_READY;
		}
		else {
			return PCB_BLOCKED_QUEUE_EMPTY;
		}
	}

	else { //if it isn't an io complete or timeslice interrupt
		return PCB_UNIMPLEMENTED;
	}
}


//see flowchart
int dispatcher::doTick() {
	int returnval = 0;			//initalize

	if (runningPCB.cpu_time != UNINITIALIZED &&
		runningPCB.io_time != UNINITIALIZED &&
		runningPCB.process_number != UNINITIALIZED &&
		runningPCB.start_time != UNINITIALIZED) { 	// is there a runningPCB? == YES

		if (runningPCB.cpu_time == 0) {   // is currentJob finished? == YES

			if (runningPCB.io_time == 1) { //does runningPCB make blocking IO call? == YES
				runningPCB.io_time = 0; 	// first set io_time = 0
				blockedQ.push(runningPCB);		// push it to blockedQ
				returnval = PCB_ADDED_TO_BLOCKED_QUEUE;
			}

			else if (runningPCB.io_time == 0) { //does runningPCB make blocking IO call? == NO
				returnval = PCB_FINISHED;
			}
		}

		else { // if cpu_time > 0
			runningPCB.cpu_time--;	// decrement cpu_time to indicate work being done
			return PCB_CPUTIME_DECREMENTED;
		}
	}

	else { //is there a runningPCB == NO

		if (!readyQ.empty()) {		// are there jobs that can be loaded from the readyQ? == true
			runningPCB = readyQ.front();  //move from readyQ to runningPCB
			readyQ.pop();
			if (runningPCB.cpu_time != 0) { //also need to do work in this tick!
				runningPCB.cpu_time--;
			}

			return PCB_MOVED_FROM_READY_TO_RUNNING;
		}

		else if (readyQ.empty()) {
			if (blockedQ.empty()) {
				return NO_JOBS;
			}
			return BLOCKED_JOBS;
		}
	}

	//check if NOW cpu_time == 0

	//unload or mark runningPCB as invalid
	runningPCB.cpu_time = UNINITIALIZED;
	runningPCB.io_time = UNINITIALIZED;
	runningPCB.process_number = UNINITIALIZED;
	runningPCB.start_time = UNINITIALIZED;

	return returnval;
}

