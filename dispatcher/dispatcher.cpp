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
	runningPCB = getNext();
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
	int returnval;


	// **********NUKE THIS NEXT TIME FAM ******************

	PCB currentJob = dispatcher::getCurrentJob();

	if (runningPCB.cpu_time != UNINITIALIZED &&
			runningPCB.io_time != UNINITIALIZED &&
			runningPCB.process_number != UNINITIALIZED &&
			runningPCB.start_time != UNINITIALIZED &&
			runningPCB.start_time >= 0 &&
			runningPCB.cpu_time > 0) {//CHECK to see if there is a running PCB

		runningPCB.cpu_time -= 1;	//if running, decrement cpu_time by 1 --- indicate work done
	}
	else { 	//if no runningPCB, check readyQ

		if (readyQ.empty()) {

			if (blockedQ.empty()) { //both readyQ and blockedQ are empty
				return NO_JOBS;
			}
			else { //just readyQ is empty
				return BLOCKED_JOBS;
			}
		}
		//hi
		else {		//if the readyQ isn't empty, LOAD next job into runningPCB
			PCB tmp2 = getNext();
			runningPCB = tmp2;
			return PCB_MOVED_FROM_READY_TO_RUNNING;
		}

	}

	if (runningPCB.cpu_time == 0) {
		if (runningPCB.io_time == 1) { //process makes a blocking call
			blockedQ.push(runningPCB);
			returnval = PCB_ADDED_TO_BLOCKED_QUEUE;
		}
		else {							//process doesn't make a blocking call
			returnval = PCB_FINISHED;
		}
	}
	else {
		return PCB_CPUTIME_DECREMENTED;
	}

	//unload or mark runningPCB as invalid
	runningPCB.cpu_time = 0;
	runningPCB.io_time = 0;
	runningPCB.process_number = 0;
	runningPCB.start_time = 0;

	return returnval;
}

	//	if (runningPCB == empty) {
	//		if (readyQ == empty) {
	//			return NO_JOBS;
	//		}
	//		else if (readyQ != empty) {
	//			runningPCB.start = readyQ[i];
	//			return PCB_MOVED_FROM_READY_TO_RUNNING;
	//		}
	//	}
	//	else if (runningPCB != empty) {
	//		runningPCBs->cpu_time --;
	//	}
	//
	//	if (runningPCB.cpu_time == 0) {
	//		if (runningPCB has blockingIOCall == true) {
	//			blocked_Q.pushBack(runningPCB[i]);
	//			returnval = PCB_ADDED_TO_BLOCKED_QUEUE;
	//		}
	//		else if (runningPCB has blockingIOCall == false) {
	//			returnval = PCB_FINISHED;
	//		}
	//	}
	//	else if (runningPCB.cpu_time != 0) {
	//		return PCB_CPUTIME_DECREMENTED;
	//	}
	//
	//	//unblock/mark runningPCB as invalid
	//	return returnval;

