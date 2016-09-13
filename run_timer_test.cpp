/*
 * run_timer_test.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: jonathan
 */

#include "run_timer_test.h"

int run_timer_test()
{
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
	cout << endl << endl << "== Run time to initialize ints a&b 10^9 times ==" << endl;
	int a, b = 0;
	run_timer timeToInit2IntsAB;
	timeToInit2IntsAB.start();
	for(int i = 0; i < 1000000000; i++) { //run the task 10^9 times
		a=i, b=i+1;
	}
	timeToInit2IntsAB.stop();
	cout << "a=" << a << endl << "b=" << b << endl;
	int loopOneTimeAverage = timeToInit2IntsAB.get_average_run_time(); //get_average_run_time().
	cout << "-- Average time to initialize int a&b: [" << loopOneTimeAverage << "ms] --" << endl; //check average
	//print_average_run_time() is called after all functions are timed.
	timeToInit2IntsAB.print_this_run_time();
	timeToInit2IntsAB.print_average_run_time();


//(2)
	cout << endl << endl << "== Run time to initialize ints c&d 10^9 times ==" << endl;
	int c, d = 0;
	run_timer timeToInit2IntsCD;
	timeToInit2IntsCD.start();
	for(int i = 0; i < 1000000000; i++) { //run the task 10^9 times
		c=i, d=i+1;
	}
	timeToInit2IntsCD.stop();
	cout << "c=" << c << endl;
	cout << "d=" << d << endl;
	int loopTwoTimeAverage = timeToInit2IntsCD.get_average_run_time(); //get_average_run_time().
	cout << "-- Average time to initialize int c&d: [" << loopTwoTimeAverage << "ms] --" << endl; //check average
	//print_average_run_time() is called after all functions are timed.
	timeToInit2IntsCD.print_this_run_time();
	timeToInit2IntsCD.print_average_run_time();

//(3)
	cout << endl << endl << "== Run time to initialize ints e&f 10^9 times==" << endl;
	int e, f = 0;
	run_timer timeToInit2IntsEF;
	timeToInit2IntsEF.start();
	for(int i = 0; i < 1000000000; i++) { //run the task 10^9 times
		e=i, f=i+1;
	}
	timeToInit2IntsEF.stop();
	cout << "e=" << e << endl;
	cout << "f=" << f << endl;
	int loopThreeTimeAverage = timeToInit2IntsEF.get_average_run_time(); //get_average_run_time().
	cout << "-- Average time to initialize int e&f: [" << loopThreeTimeAverage << "ms] --" << endl; //check average
	//print_average_run_time() is called after all functions are timed.
	timeToInit2IntsEF.print_this_run_time();
	timeToInit2IntsEF.print_average_run_time();

	cout << endl << endl << "^^^^Testing Complete^^^^";

	return 1;
}
