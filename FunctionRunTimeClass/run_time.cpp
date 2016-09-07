/*
 * run_time.cpp
 *
 *  Created on: Sep 6, 2016
 *      Author: jonathan arroyo
 */

/* Functions list in order:
 * run_time()
 * ~run_time()
 *
 * start()
 * stop()
 * overhead()
 * ----
 * print_this_run_time()
 * print_average_run_time()
 * ----
 * get_this_run_time()
 * get_average_run_time()
 */

#include "run_time.h"


//Constructor run_time()
run_time::run_time() {
	thisRunTime = 0;
	amountOfRunTimeMeasurements = 0;
	totalRunTimeSum = 0;
	averageRunTime = 0;
}//END constructor


//Destructor ~run_time()
run_time::~run_time() { //Destructor
}//END destructor


//start()
void run_time::start() {
	cout << "Starting run time timer." << endl;
	startTime = system_clock::now();
}//END start()


//stop()
void run_time::stop() {
	stopTime = system_clock::now();
	cout << "Stopping run time timer." << endl;
	overhead(); //See overhead() next below. Calculates time differences and stores them.
}//END stop()


//overhead(), a private function
void run_time::overhead() { //private function
	thisRunTime = chrono::duration_cast<chrono::nanoseconds> (stopTime - startTime).count();
	amountOfRunTimeMeasurements++;
	totalRunTimeSum += thisRunTime;
	if(amountOfRunTimeMeasurements != 0) {averageRunTime = totalRunTimeSum / amountOfRunTimeMeasurements;}
	else {
		cout << "Error: overhead. Amount of time measurements is 0. Do not want to divide by 0." << endl;
	}
}//END overhead()


//--------------Print functions

//print_this_run_time()
void run_time::print_this_run_time() {
	if(amountOfRunTimeMeasurements != 0) {cout << "This run time = [" << thisRunTime << "ns]" << endl;}
	else {
		cout << "Error: print_this. No measurements taken yet to print." << endl;
	}
}//END print_this_run_time().


//print_average_run_time()
void run_time::print_average_run_time() {
	if(amountOfRunTimeMeasurements != 0) {
		cout << "Average run time = (" << totalRunTimeSum << "ns / " << amountOfRunTimeMeasurements << " runs). = [[" << averageRunTime << "ns]]" << endl;
	}
	else {
		cout << "Error: print_average. No measurements taken yet to print." << endl;
	}
}//END print_average_run_time()


//-------------Get functions

//get_this_run_time()
int run_time::get_this_run_time() {
	if(amountOfRunTimeMeasurements != 0) {return thisRunTime;}
	else {
		cout << "Error: get_this. No measurements taken yet. Returning 0." << endl;
		return 0;
	}
}//END get_this_run_time()


//get_average_run_time()
int run_time::get_average_run_time() {
	if(amountOfRunTimeMeasurements != 0) {return averageRunTime;}
	else {
		cout << "Error: get_average. No measurements taken yet. Returning 0." << endl;
		return 0;
	}
}//END get_average_run_time() end
