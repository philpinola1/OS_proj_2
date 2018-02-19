#include <iostream>
#include "../includes_usr/constants.h"
#include "../includes_usr/dispatcher.h"
#include "../includes_usr/file_io.h"
#include "../includes_usr/joblist.h"

#include <vector>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

//assumme the worst
bool joblistHasJobs = false;

std::vector<PCB> myVec;


//loads data from filename and sorts
//it by job start time
int joblist::init(const char* filename) {

		return loadData(filename);

//		//clears vector holding process_stats structs
//		myVec.clear();
//
//		std::string line;
//		std::string token;
//		PCB myStats;
//		std::stringstream ss;
//
//		ifstream infile;
//		infile.open(filename);
//
//		//attempt to open file 'filename' to read
//		if (infile.is_open()) {
//
//			while (!infile.eof()) {
//				std::getline(infile, line); //get a line from the file
//				ss.str(line);
//
//				//get rid of the old values
//				//mymeasurement.clear();
//
//				//!!!!!! NO ERROR CHECK HERE DONT ASSUMME PERFECT DATA 		!!!!!!
//				//!!!!!! if any of the following fail retValue should		!!!!!!
//				//!!!!!! be set to false, break out of loop, then return;	!!!!!!
//
//				//get process_number
//				if (std::getline(ss, token, CHAR_TO_SEARCH_FOR))
//					myStats.process_number = std::stoi(token.c_str());
//
//				//get start_time
//				if ( std::getline(ss, token, CHAR_TO_SEARCH_FOR))
//					myStats.start_time = stoi(token.c_str());
//
//				//get cpu_time
//				if( std::getline(ss, token, CHAR_TO_SEARCH_FOR))
//					myStats.cpu_time = stoi(token.c_str());
//
//				//get IO_time
//				if (std::getline(ss, token, CHAR_TO_SEARCH_FOR))
//					myStats.io_time = stoi(token.c_str());
//
//				//finally add to vector (makes a COPY of myMeasurement)
//				myVec.push_back(myStats);
//
//				//clear stream so it will work for next read
//				ss.clear();
//
//				}
//
//			}
//		return SUCCESS;
}

//gets next job from file_IO
PCB joblist::getNextJob() {
	return getNext();
}

//if no jobs in vector managed by file_io return NO_JOBS
//otherwise see if its time to load a process by comparing current tick
//to the next process in lines start_time.
//If its time to load return ADD_JOB_TO_DISPATCHER
//if not return WAITING_TO_ADD_JOB_TO_DISPATCHER
int joblist::doTick(int currentTick) {

	if (size() == 0) {
		return NO_JOBS;
	}


	if (currentTick == peekNextStartTime()) {
		return ADD_JOB_TO_DISPATCHER;
	}

	else{
		return WAITING_TO_ADD_JOB_TO_DISPATCHER;
	}

	return ADD_JOB_TO_DISPATCHER;

}





