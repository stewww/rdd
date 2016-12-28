/*
 * imshow2.hpp
 *
 *  Created on: Oct 19, 2016
 *      Author: jonathan
 */

#ifndef IMSHOW2_HPP_
#define IMSHOW2_HPP_

#include <iostream>
#include <chrono>
using namespace std;
using std::chrono::system_clock;
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core.hpp>
void imshow2(const string &winname, cv::Mat mat, double time_array[], system_clock::time_point loop_start);

#endif /* IMSHOW2_HPP_ */

