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


class line_c {
public:
	line_c() : m(0), b(0){};
	line_c(double intercept, double slope) {b = intercept; m = slope;};
	void operator = (const line_c &line) {m = line.m, b = line.b;};
	bool operator == (const line_c &line){
		bool equal = false;
		if(this->b == line.b && this->m == line.m)
			equal = true;
		return equal;
	};
	bool operator > (line_c *line){
		bool right = false;
		cv::Point2d one, two;
		one.y = 100; two.y = 100;
		one.x = line->find_x(one.y);
		two.x = line->find_x(two.y);

		if(one.x > two.x)
			right = true;
		return right;
	};
	bool operator < (line_c *line){
		bool left = false;
		cv::Point2d one, two;
		one.y = 100; two.y = 100;
		one.x = line->find_x(one.y);
		two.x = line->find_x(two.y);

		if(one.x < two.x)
			left = true;
		return left;
	};
	bool on_line(cv::Point2d *point){
		bool check = false;
		if(point->x == ((point->y - this->b) / this->m))
			check = true;
		return check;
	};
	void def_equation(cv::Point2d *p1, cv::Point2d *p2){
		this->m = (p2->y - p1->y) / (p2->x - p1->x);
		this->b = p2->y - p2->x * (this->m);
	};
	double find_x(double y){
		double x = (y - this->b) / this->m;
		return x;
	};
	~line_c(){};

protected:
	double m;
	double b;
};

class lane_c : public line_c
{
public:
	lane_c() : center(0,0), left(0,0), right(0,0), number(0) {};
	lane_c(cv::Point2d cen, int num, line_c lt, line_c rt){
		center = cen;
		number = num;
		left = lt;
		right = rt;
	};
	lane_c(int num, line_c lt, line_c rt){
		center = calculate_center_point(&lt, &rt);
		number = num;
		left = lt;
		right = rt;
	};
	void operator = (const lane_c &lane){
		center = lane.center;
		number = lane.number;
		left = lane.left;
		right = lane.right;
	};
	void def_lane (cv::Point2d cen, int num, line_c *lt, line_c *rt){
		this->center = cen;
		this->number = num;
		this->left = *lt;
		this->right = *rt;
	};
	bool operator == (const lane_c &lane){
		bool equal = false;
		if(this->center == lane.center && this->left == lane.left && this->right == lane.right)
			equal = true;
		return equal;
	};
	cv::Point2d get_center_point(lane_c *lane){
		return lane->center;
	};
	cv::Point2d calculate_center_point(line_c *lt, line_c *rt){
		cv::Point2d center, right, left;

		left.y = 100, right.y = 100, center.y = 100;
		left.x = lt->find_x(left.y);
		right.x = rt->find_x(right.y);
		center.x = (left.x + right.x) / 2;

		return center;
	};
	bool in_lane(cv::Point2d point, lane_c *lane){
		bool inside = false;
		cv::Point2d left_p, right_p;
		line_c left_l = lane->left, right_l = lane->right;

		left_p.y = point.y;
		right_p.y = point.y;
		left_p.x = left_l.find_x(left_p.y);
		right_p.x = right_l.find_x(right_p.y);
		if(point.x > left_p.x && point.x < right_p.x)
			inside = true;

		return inside;
	};
	int get_number(lane_c lane){
		return lane.number;
	};
	~lane_c(){};

protected:
	cv::Point2d center;
	line_c left, right;
	int number;
};

class lane_marker_detect : public lane_c{
public:
	lane_marker_detect();
	void set_up();
	void cropImage(cv::Mat * inputImage, cv::Mat * croppedImage, cv::Point one, cv::Point two);
	void detect(cv::Mat *vid_frame, std::vector<lane_c> * lane_);
	void show_windows(cv::Mat *hough_image, cv::Mat *canny_image, cv::Mat *gaussian_image, cv::Mat *original);
	cv::Mat gaussian_blur(cv::Mat image);
	cv::Mat canny_edge(cv::Mat gaussian_image);
	cv::Mat sobel_edge(cv::Mat gaussian_image);
	cv::Mat hough_transform(cv::Mat canny_image, cv::Mat original, std::vector<lane_c> * lane_);

	~lane_marker_detect(){};
private:
	int xSigma;
	int ySigma;

	int upperThresh;
	int lowerThresh;

	int minLineLength;
	int maxLineGap;

	int iLowH;
	int iHighH;

	int iLowS;
	int iHighS;

	int iLowV;
	int iHighV;

	int threshL;
	int threshH;

	cv::Mat gaussian_image;
	cv::Mat canny_image;
	cv::Mat noncrop_original;
	cv::Mat original;
	cv::Mat hough_image;

	std::vector<cv::Vec2f> lines;
	int frame_counter;
};

#endif /* LANE_MARKER_DETECT_HPP_ */
