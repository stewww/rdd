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

typedef struct line{
	double m;
	double b;
}line;

typedef struct lane{
	cv::Point2d center;
	line left, right;
	int number;
}lane;

class lane_marker_detect {
public:
	lane_marker_detect();
	lane_marker_detect(int xSigmai, int ySigmai, int upperThreshi, int lowerThreshi, int minLineLengthi, int maxLineGapi);

	cv::Point2d find_center_point(line *l1, line *l2);
	lane define_lane(cv::Point2d center, line *left, line *right, int num);
	void new_filter(cv::Mat * in, cv::Mat * out);
	void colorReduce(cv::Mat& image, int div);
	void cropImage(cv::Mat * inputImage, cv::Mat * croppedImage, cv::Point one, cv::Point two);
	bool line_check(double x, double y,line l);
	double find_x(double y, line l);
	double dev_calc(double dst_left, double dst_right);
	void line_equation(cv::Point2d point1, cv::Point2d point2, line *l);
	bool leftside_of(line *l1, line *l2);
	bool rightside_of(line *l1, line *l2);
	void detect();
	void lane_process();
	void show_windows(cv::Mat *hough_image, cv::Mat *canny_image, cv::Mat *gaussian_image, cv::Mat *original);
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
	cv::Mat noncrop_original;
	cv::Mat original;
	cv::Mat hough_image;

	std::vector<cv::Vec2f> lines;
	int frame_counter;
};





#endif /* LANE_MARKER_DETECT_HPP_ */
