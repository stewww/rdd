/*
 * lane_marker_detect.hpp
 *
 *  Created on: Sep 6, 2016
 *      Author: lhthinh
 */

#ifndef LANE_MARKER_DETECT_HPP_
#define LANE_MARKER_DETECT_HPP_

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ctime>

class lane_marker_detect {
public:
	lane_marker_detect();
	lane_marker_detect(int xSigmai, int ySigmai, int upperThreshi, int lowerThreshi, int minLineLengthi, int maxLineGapi);

	bool line_check(double x, double y, double b, double a);
	double find_x(double y, double b, double m);
	double dev_calc(double dst_left, double dst_right);
	void line_equation(cv::Point2d point1, cv::Point2d point2, double *m, double *b);
	void detect();
	void show_windows(cv::Mat hough_image, cv::Mat canny_image, cv::Mat gaussian_image, cv::Mat original);
	cv::Mat gaussian_blur(cv::Mat image);
	cv::Mat canny_edge(cv::Mat gaussian_image);
	cv::Mat sobel_edge(cv::Mat gaussian_image);
	cv::Mat hough_transform(cv::Mat canny_image, cv::Mat original);

private:
	int xSigma;
	int ySigma;

	int upperThresh;
	int lowerThresh;

	int minLineLength;
	int maxLineGap;

	cv::Mat gaussian_image;
	cv::Mat canny_image;
	cv::Mat original;
	cv::Mat hough_image;
};





#endif /* LANE_MARKER_DETECT_HPP_ */
