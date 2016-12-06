/*
 * lane_marker_detect.hpp
 *
 *  Created on: Sep 6, 2016
 *  Author: lhthinh
 *	Header file for lane marker detection
 *	Contain line, lane, and lane_marker_detect classes
 */

#ifndef LANE_MARKER_DETECT_HPP_
#define LANE_MARKER_DETECT_HPP_

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <cstdio>

/*
	Line classes
	Lines are defined in format y = mx + b
*/
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
	/*	Function to determine if a line is on the right side of another line */
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
	/*	Function to determine if the line is on the left side of another line */
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
	/*	Function to return tre if a point is on the line */
	bool on_line(cv::Point2d *point){
		bool check = false;
		if(point->x == ((point->y - this->b) / this->m))
			check = true;
		return check;
	};
	/*	Function to define a line in the format y = mx + b */
	void def_equation(cv::Point2d *p1, cv::Point2d *p2){
		this->m = (p2->y - p1->y) / (p2->x - p1->x);
		this->b = p2->y - p2->x * (this->m);
	};
	/*	Function to find x coordinate of a point on the line with a given y value */
	double find_x(double y){
		double x = (y - this->b) / this->m;
		return x;
	};
	~line_c(){};

protected:
	double m;
	double b;
};

/*	Lane class
		A lane is defined by two lines, left and right
		It also contains the number to indicate the relative position of the lane
		Center point is the point that indicates middle of the lane
*/

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

	/*	Assignment operator */
	void operator = (const lane_c &lane){
		center = lane.center;
		number = lane.number;
		left = lane.left;
		right = lane.right;
	};

	/* Function to define a lane given all needed information, e.g center point, number, left and right lines	*/
	void def_lane (cv::Point2d cen, int num, line_c *lt, line_c *rt){
		this->center = cen;
		this->number = num;
		this->left = *lt;
		this->right = *rt;
	};

	/*
			Equality operator
			Return true if two lanes are the same
	*/
	bool operator == (const lane_c &lane){
		bool equal = false;
		if(this->center == lane.center && this->left == lane.left && this->right == lane.right)
			equal = true;
		return equal;
	};

	/*	Return the center point of the lane	*/
	cv::Point2d get_center_point(){
		return this->center;
	};

	/*
			Function to calculate the center point of a lane
			The center point is calculated with respect to the two lines that define the lane
	*/
	cv::Point2d calculate_center_point(line_c *lt, line_c *rt){
		cv::Point2d center, right, left;

		left.y = 60, right.y = 60, center.y = 60;
		left.x = lt->find_x(left.y);
		right.x = rt->find_x(right.y);
		center.x = (left.x + right.x) / 2;

		return center;
	};

	/*
			Function to check if a point is inside of the lane
			Return true when a point is inside of the lane
	*/
	bool in_lane(cv::Point2d point){
		bool inside = false;
		cv::Point2d left_p, right_p;
		line_c left_l = this->left, right_l = this->right;

		left_p.y = point.y;
		right_p.y = point.y;
		left_p.x = left_l.find_x(left_p.y);
		right_p.x = right_l.find_x(right_p.y);
		if(point.x > left_p.x && point.x < right_p.x)
			inside = true;

		return inside;
	};

	/*	Return the lane's number	*/
	int get_number(void){
		return number;
	};
	~lane_c(){};

protected:
	cv::Point2d center;
	line_c left, right;
	int number;
};

/*
		Lane_marker_detect class
		The lane_marker_detect object would take in a video frame, crop the frame (optional),
		then apply filters, such as hsv, grey scale, and binary thresholding
		The output of the series of filters will go into Hough transform algorithm,
		which would detect and return line segments in form of rho and theta value.
*/
class lane_marker_detect : public lane_c{
public:
	lane_marker_detect();
	void set_up();
	void cropImage(cv::Mat * inputImage, cv::Mat * croppedImage, cv::Point one, cv::Point two);
	cv::Mat detect(cv::Mat *vid_frame, std::vector<lane_c> * lane_);
	void show_windows(cv::Mat *hough_image, cv::Mat *canny_image, cv::Mat *gaussian_image, cv::Mat *original);
	double cir_queue_average(double * vp, int size);
	void cir_queue_push(double * vp, double value);
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
