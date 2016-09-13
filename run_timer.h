/*
 * run_timer.h
 *
 *  Created on: Sep 6, 2016
 *      Author: jonathan arroyo
 *
 *      The purpose of this class is to
 *  measure run time of lines of code. This is done
 *  by first creating an object of this class
 *  for each code block/line/function etc you want to measure.
 *  Then you simply call object.start() before the code begins,
 *  and then object.stop() when the function ends. get and print functions are then available.
 *
 *  	This class functions by using the chrono library
 *  included in the std library. Chrono's system_clock class
 *  is used to access the system_wide clock via its now() function.
 *  now() returns a time_point (another chrono class) which stores a value of
 *  the third chrono class (duration). Duration stores two variables:
 *  a signed integral type and a ratio (see ratio library).
 *  now() time_points are converted into nanosecond durations of type int
 *  using duration's duration_cast() function.
 *  After the conversion, typical math operations take place.
 *
 *  ---General example of run_timer class---
 *  run_timer myClass; //declare class
 *  ...
 *  myClass.start(); //start timer
 *  [code line or function or code block that you want to measure]
 *  myClass.stop(); //stop timer. stop() contains a function that prepares timer values.
 *  //now print_run_time() and get_run_time() are available.
 *
 *  So if you want to compare multiple sections of code in a program, you'd probably create a
 *  an array of run_timer objects and an array of corresponding ints to store average run times.
 *  Each int element would be set using get_average_run_time(), after you've ran start() and stop()
 *  for each code section.
 */

#include <chrono>
#include <iostream>
using namespace std;
using std::chrono::system_clock;

#ifndef RUN_TIMER_H_
#define RUN_TIMER_H_

class run_timer {
public:

	run_timer();
	virtual ~run_timer();

	void start(); //"Starts" the run time "timer". Works by setting startTime to now().
	void stop();  //Similar to start(). Stop() contains overhead() which does math and stores values.

	//prints only run if amountOfRunTimeMeasurements != 0, else an error displays to the console."
	void print_this_run_time(); //Prints run time (stop - start) for the iteration in nanoseconds(int).
	void print_average_run_time(); //Similar to above. Also displays "totalRunTimeSum / amountOfTimeMeasurements"

	//gets return if amountOfRunTimeMeasurements != 0, else a 0 is returned.
	int get_this_run_time(); //returns time difference (stop-start) for the iteration in nanoseconds
	int get_average_run_time();

private:
	system_clock::time_point startTime; //time_point are set using system_clock::now() in start().
	system_clock::time_point stopTime; //Identical to startTime above.
	int thisRunTime; //pseudocode: thisRunTime = cast<int>(stop-start)
	int amountOfRunTimeMeasurements; //gets ++'d in stop()
	int totalRunTimeSum; //gets +=thisRunTime'd in stop()
	int averageRunTime; // = totalRunTimeSum / amountOfRunTimeMeasurements

	void overhead(); //Conversions, math operations, and value storage.
}; //run_timer{} end

#endif /* RUN_TIMER_H_ */
