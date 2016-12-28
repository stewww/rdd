/*
 * imshow2.cpp
 *
 *  Created on: Oct 19, 2016
 *      Author: jonathan
 */

#include "imshow2.hpp"
//#include <chrono>
//using std::chrono::system_clock;
/*
 * imshow2() is an imshow() wrapper that also displays processing frame rate of the calling program (assumedly a while-loop).
 * Requires that std::time_t loop_top = time(0); is set at the start of the calling while-loop that imshow2() is in. #include <chrono>
 * Requires caller to have declared an int array of size 22 with all values 0, and pass its pointer into imshow2().
 */

/* About time_array[] input variable
 * Remember that all time_array[] values should have been initialized to 0 before passing it in to imshow2().
 * time_array[0 through 19] = the time_t values (casted as int) holding the time used to process one frame in the calling program.
 * time_array[20] = cumulative sum of time_t values.
 * time_array[21] = counter/indexer used to update the rolling mean used to calculate the processing frame rate.
 * processing fps = 1 / (time_array[20] / 20), requires scaling according to desired time unit.
 */

void imshow2(const string& winname, cv::Mat mat, double time_array[], system_clock::time_point loop_start){


	int index = time_array[21]; // Used to make code more readable.
	time_array[20] -= time_array[ index ]; // Remove (subtract) oldest time value from cumulative sum.
	double frame_process_time = chrono::duration_cast<chrono::milliseconds> (system_clock::now() - loop_start).count(); // time(ms) to compute current frame
	time_array[ index ] = frame_process_time; // Add current frame's process time to the array of time values.
	time_array[20] += time_array[ index ]; // Add the current frame's process time to cumulative sum.
	double avg_ms_per_frame = time_array[20] / 20; // Divide cumulative time(ms) sum by 20 to obtain avg_time(ms) that was required to compute the last 20 frames so far.
	double avg_s_per_frame = avg_ms_per_frame / 1000; // time_array values are stored as milliseconds, see where double frame_process_time is set above
	double rolling_avg_fps = 1 / avg_s_per_frame;

	//Display original frame and then also output the processing fps.
	cv::putText(mat, cv::format("Processing FPS:%.2f", rolling_avg_fps ), cv::Point(10, 170), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255));

	if( time_array[21] > 18)// time-array[21], the counter, should loop between 0-19
	{
		time_array[21] = 0;
	}
	else
	{
		time_array[21]++;
	}

	imshow(winname, mat);
}

