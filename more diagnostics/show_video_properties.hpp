/*
 * show_video_properties.hpp
 *
 *  Created on: Dec 1, 2016
 *      Author: jonathan
 */

#ifndef SHOW_VIDEO_PROPERTIES_HPP_
#define SHOW_VIDEO_PROPERTIES_HPP_

#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core.hpp>
#include <sstream>
using namespace std;
using namespace cv;

void show_video_properties(VideoCapture& capture, Mat& frame);

#endif /* SHOW_VIDEO_PROPERTIES_HPP_ */

