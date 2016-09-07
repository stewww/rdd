/*
 * main.cpp
 *
 *  Created on: Sep 6, 2016
 *      Author: jonathan arroyo
 *
 *      Testing functionality of run_time.h.
 */

#include <iostream>
#include "run_time.h"
using namespace std;

int main() {
	cout << "_____MAIN_START_____TESTING_RUN_TIMES_____" << endl;

	/* Use three for-loops to time the following three tasks:
	 * (1) initialize 2 integers,
	 * (2) initialize 1 integer,
	 * (3) intialize 1 integer.
	 *
	 * The code block for each task is identical.
	 * Each loop iteration uses get() and then cout to compare with print().
	 * Average time of each entire for-loop is displayed at the end, after all for-loops are complete.
	*/

//(1)
	cout << endl << endl << "== Run time to initialize ints a&b ==" << endl;
	run_time timeToInit2IntsAB;
	for(int i = 0; i < 3; i++) { //run the task 3 times
		timeToInit2IntsAB.start(); //start timer
		int a=2, b=3;
		timeToInit2IntsAB.stop(); //stop timer
		int taskOneTime = timeToInit2IntsAB.get_this_run_time(); //get_this_run_time()
		cout << "(" << i+1 << ")Time to store a&b: " << taskOneTime << "ns." << endl; //cout above's value
		timeToInit2IntsAB.print_this_run_time(); //print_this_run_time(). Should equal above's output.
		cout << endl;
	}
	int functionOneTimeAverage = timeToInit2IntsAB.get_average_run_time(); //get_average_run_time().
	cout << "-- Average time to initialize int a&b: [" << functionOneTimeAverage << "ns] --" << endl; //check average
	//print_average_run_time() is called after all functions are timed.

//(2)
	cout << endl << endl << "== Run time to initialize int c==" << endl;
	run_time timeToInit1IntC;
	for(int j = 0; j < 3; j++) { //run the task 3 times
		timeToInit1IntC.start(); //start timer
		int c = 2;
		timeToInit1IntC.stop(); //stop timer
		int taskTwoTime = timeToInit1IntC.get_this_run_time(); //get_this_run_time()
		cout << "(" << j+1 << ")Time to store c: " << taskTwoTime << "ns." << endl; //cout above's value
		timeToInit1IntC.print_this_run_time(); //print_this_run_time().
		cout << endl;
	}
	int functionTwoTimeAverage = timeToInit1IntC.get_average_run_time(); //get_average_run_time().
	cout << "-- Average time to initialize int c: [" << functionTwoTimeAverage << "ns] --" << endl; //check average

//(3)
	cout << endl << endl << "== Run time to initialize int d ==" << endl;
	run_time timeToInit1IntD;
	for(int k = 0; k < 3; k++) { //run the task 3 times
		timeToInit1IntD.start(); //start timer
		int d = 3;
		timeToInit1IntD.stop(); //stop timer
		int taskThreeTime = timeToInit1IntD.get_this_run_time(); //get_this_run_time()
		cout << "(" << k+1 << ")Time to store d: " << taskThreeTime << "ns." <<  endl; //cout above's value
		timeToInit1IntD.print_this_run_time(); //print_this_run_time()
		cout << endl;
	}
	int functionThreeTimeAverage = timeToInit1IntD.get_average_run_time(); //get_average_run_time().
	cout << "-- Average time to initialize int d: [" << functionThreeTimeAverage << "ns] --" <<  endl; //check average

	//For loops complete. Check average times again.
	cout << endl;
	cout << "For loops complete. Printing average task run times." << endl;
	timeToInit2IntsAB.print_average_run_time(); //print_average_run_time(). Should equal above's output.
	timeToInit1IntC.print_average_run_time(); //print_average_run_time(). Should equal above's output.
	timeToInit1IntD.print_average_run_time(); //print_average_run_time(). Should equal above's output.
	cout << endl;

	return 0;
}//main() end
