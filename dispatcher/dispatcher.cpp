#include "../includes_usr/constants.h"
#include "../includes_usr/dispatcher.h"
#include "../includes_usr/file_io.h"
#include "../includes_usr/joblist.h"
#include "../includes_usr/logger_single_thread.h"
#include <queue>

PCB runningPCB;

void dispatcher::init() {
}

PCB dispatcher::getCurrentJob() {
	return runningPCB;
}

void dispatcher::addJob(PCB &myPCB) {
}

int dispatcher::processInterrupt(int interrupt) {
	return NO_JOBS;
}

int dispatcher::doTick() {

	PCB tmp = dispatcher::getCurrentJob();

	if (tmp.cpu_time > 0) {			//CHECK to see if there is a running PCB
		runningPCB.cpu_time -= 1;	//if running, decrement cpu_time by 1
	}
	else if (tmp.cpu_time <= 0) {
		//if not running, check if readyQ is empty

		if (peekNextStartTime() == NO_JOBS) { //if readyqQ is empty, were done here
			return NO_JOBS;
		}

		else {									//if the readyQ isn't empty, LOAD next job into runningPCB
			PCB tmp2 = getNext();
			addJob(&tmp2);
		}

	}



	return NO_JOBS;
}

