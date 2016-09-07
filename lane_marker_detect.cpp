/*
 * lane_marker_detect.cpp
 *
 *  Created on: Sep 5, 2016
 *      Author: lhthinh
 */
#include "lane_marker_detect.hpp"

// Choose between HoughP or Hough.
#define houghp 0
#define stdhough 1
#define hsv 1
#define binarize 1

lane_marker_detect::lane_marker_detect() {
	xSigma = 3;
	ySigma = 3;

	upperThresh = 100;
	lowerThresh = 33;

	minLineLength = 2;
	maxLineGap = 2;
}

/*
 * Use this if you want to input your own values;
 */
lane_marker_detect::lane_marker_detect(int xSigmai, int ySigmai, int upperThreshi, int lowerThreshi, int minLineLengthi, int maxLineGapi) {
	xSigma = xSigmai;
	ySigma = ySigmai;

	upperThresh = upperThreshi;
	lowerThresh = lowerThreshi;

	minLineLength = minLineLengthi;
	maxLineGap = maxLineGapi;
}

/*
 *	Apply gaussian blur to smooth out edges, gets rid of unwanted noise.
 *	cv::GaussianBlur(image, image, kSize, sigmaY, sigmaX);
 *	kSize must be positive and odd
 *	Typical kSize is 5x5 matrix, and sigmaY and X values of 1
 */
cv::Mat lane_marker_detect::gaussian_blur(cv::Mat image) {
	cv::Size kSize = cv::Size(5, 5);
	cv::GaussianBlur(image, image, kSize, xSigma, ySigma);

	return image;
}

/*
 *	Sobel Edge Detection
 */
cv::Mat lane_marker_detect::sobel_edge(cv::Mat gaussian_image) {
	int scale = 1;
	int delta = 0;
	int ddepth = CV_8U;

	cv::Mat sobel;
	gaussian_image.copyTo(sobel);

	cv::Sobel(gaussian_image, sobel, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);

	return sobel;
}


/*
 *	Use canny edge algorithm to get edges
 *	cv::Canny(image, image, upperThresh, lowerThresh);
 *	upperThresh > lowerThresh
 *	lowerThresh may be around 1/3 upperThresh. Depends on use case
 */
cv::Mat lane_marker_detect::canny_edge(cv::Mat gaussian_image) {

	cv::Mat canny;
	gaussian_image.copyTo(canny);
	cv::Canny(gaussian_image, canny, upperThresh, lowerThresh);
	return canny;
}

/*
 *	Probabilistic hough transform
 *	Use this instead of standard hough transform because we can choose to filter out min/max lengths
 *	cv::HoughLinesP(canny_image, lines, rho, theta, threshold, minLineLength, minLineLength);
 */
cv::Mat lane_marker_detect::hough_transform(cv::Mat canny_image, cv::Mat original) {
	cv::Mat hough = original.clone();
	std::clock_t start;
	start = std::clock();
	double duration;
//	double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC; // get duration in second
//	duration *= 1000; // get duration in milisecond
//	std::cout<<"Start "<<duration<<std::endl;
	double rho = 1.2;
	double theta = CV_PI / 180;
	int threshold = 100;
	//inverse transformation matrix value that I got from using ipm.getHinv() from the points orig & dst points
//	cv::Mat transform = (cv::Mat_<float>(3,3) << -0.02255122438780657, 0.0136674087198828, -16.0628019323675,
//			0.004747626186906658, -0.00187027698272079, -16.0628019323675,
//			2.373813093453329e-005, 4.244090076174131e-005, -0.08031400966183749);
#if houghp
	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(canny_image, lines, rho, theta, threshold, minLineLength, maxLineGap);
#endif

#if stdhough
	std::vector<cv::Vec2f> lines;
	cv::HoughLines(canny_image, lines, rho, theta, threshold);

#endif

	/*	line(hough, cv::Point(x, y), cv::Point(x, y), cv::Scalar(a, b, c), 3, lineType)
	 *	x, y = 2d array. ex: x = [int][int]
	 *	a, b, c = RGB values 0-255
	 *	lineType = type of line that is to be used. See http://docs.opencv.org/3.1.0/d0/de1/group__core.html#gaf076ef45de481ac96e0ab3dc2c29a777
	 */
	//	cv::Point pt1, pt2;
	cv::Point2d pt1, pt2, pt3, pt4, left_pt, cen_pt, right_pt;
	cv::Scalar line_color = cv::Scalar(0, 0, 255);
	cv::Scalar line_color2 = cv::Scalar(255, 0, 255);
	int line_thickness = 2;
	double l_m = 0, l_b = 0, r_m = 0, r_b = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
#if houghp
		pt1 = cv::Point(lines[i][0],
				lines[i][1]);
		pt2 = cv::Point(lines[i][2],
				lines[i][3]);
		line(hough, pt1, pt2, line_color, line_thickness, cv::LINE_AA);
#endif


		// The code below is for standard Hough Line Transform
#if stdhough
		// http://stackoverflow.com/questions/31147438/how-to-undo-a-perspective-transform-for-a-single-point-in-opencv/31201448#31201448
		// transform single points,  sadly, does not work with houghp :( , apply transformation to vector points
		//perspectiveTransform(lines, lines, transform);
		double rho2 = lines[i][0], theta2 = lines[i][1];
		//		double x0 = a*rho, y0 = b*rho;
		//		pt1.x = cvRound(x0 + 300 * (-b));
		//		pt1.y = cvRound(y0 + 300 * (a));
		//		pt2.x = cvRound(x0 - 300 * (-b));
		//		pt2.y = cvRound(y0 - 300 * (a));

		//		pt11 = ipm.applyHomographyInv(pt1);
		//		pt22 = ipm.applyHomographyInv(pt2);

		//		line_equation(pt22, pt11, &m, &b);
		//		line_equation(pt2, pt1, &m, &b);
		//		left_pt.y = 200;
		//		left_pt.x = find_x(left_pt.y, b, m);

		cen_pt.y = 450; cen_pt.x = 740; //center point

		//		line(hough, pt1, pt2, line_color, line_thickness, cv::LINE_AA);
		//		line(hough, pt11, pt22, line_color, line_thickness, cv::LINE_AA);
		//		line(hough, ori_pt, left_pt, line_color2, line_thickness, cv::LINE_AA);

		// Angles between 15 and 70 degress in rads
		if ( theta2 > 0.26 && theta2 < 1.22 ){
			double a = cos(theta2), b = sin(theta2);
			double x0 = a*rho2, y0 = b*rho2;
			pt1.x = cvRound(x0 +  100*(-b));
			pt1.y = cvRound(y0 +  100*(a));
			pt2.x = cvRound(x0 -  100*(-b));
			pt2.y = cvRound(y0 -  100*(a));

			line_equation(pt2, pt1, &l_m, &l_b);
			left_pt.y = 450;
			left_pt.x = find_x(left_pt.y, l_b, l_m);

			line(hough, pt1, pt2, line_color, 3, cv::LINE_AA);
			line(hough, cen_pt, left_pt, line_color2, line_thickness, cv::LINE_AA);
		}

		// Angles between 115 and 145 degrees in radians
		if ( theta2 < 2.5 && theta2 > 2 ) {
			double a = cos(theta2), b = sin(theta2);
			double x0 = -a*rho2, y0 = b*rho2;
			pt3.x = cvRound(x0 +  100*(b));
			pt3.y = cvRound(y0 +  100*(-a));
			pt4.x = cvRound(x0 -  100*(b));
			pt4.y = cvRound(y0 -  100*(-a));

			std::cout<<"rho: "<<rho2<<", theta: "<<theta2<<std::endl;
			std::cout<<"pt0: "<<x0<<", "<<y0<<std::endl;
			std::cout<<"pt3: "<<pt3.x<<", "<<pt3.y<<std::endl;
			std::cout<<"pt4: "<<pt4.x<<", "<<pt4.y<<std::endl;

			line_equation(pt4, pt3, &r_m, &r_b);
			right_pt.y = 450;
			right_pt.x = find_x(right_pt.y, r_b, r_m);

			line(hough, pt3, pt4, line_color, 3, cv::LINE_AA);
			line(hough, cen_pt, right_pt, line_color2, line_thickness, cv::LINE_AA);
		}
		bool expired = false;

		duration = (std::clock() - start) / (double) CLOCKS_PER_SEC; // get duration in second
		duration *= 1000; // get duration in milisecond
		if(duration > 10)
			expired = true;
		/*
		 * Print out the result every 7 ms if the data is valid
		 * Ex: left point's x-coordinate should be smaller than the center point
		 */
		if(expired && left_pt.x != 0 && right_pt.x != 0 && right_pt.x > cen_pt.x){
			double dst_left = cen_pt.x - left_pt.x;
			double dst_right = right_pt.x - cen_pt.x;
			double dev_pert = dev_calc(dst_left, dst_right);
			if(dev_pert == 0){
				std::cout<<"left x: "<<left_pt.x;
				std::cout<<" cen x: "<<cen_pt.x;
				std::cout<<" right x: "<<right_pt.x<<std::endl;
				std::cout<<"Neutral pos!"<<std::endl;
			}
			else if(dev_pert < 0){
				std::cout<<"left x: "<<left_pt.x;
				std::cout<<" cen x: "<<cen_pt.x;
				std::cout<<" right x: "<<right_pt.x<<std::endl;
				std::cout<<"Deviates to the left by "<<-dev_pert<<"%"<<std::endl;
			}
			else{
				std::cout<<"left x: "<<left_pt.x;
				std::cout<<" cen x: "<<cen_pt.x;
				std::cout<<" right x: "<<right_pt.x<<std::endl;
				std::cout<<"Deviates to the right by "<<dev_pert<<"%"<<std::endl;
			}
			start = std::clock();
			expired = false;
		}

#endif
	}

	return hough;
}

/*
 *	Show images
 */
void lane_marker_detect::show_windows(cv::Mat hough_image, cv::Mat canny_image, cv::Mat gaussian_image, cv::Mat original) {
	const std::string original_image = "Original Image";
	const std::string gaussian = "Gaussian Blur";
	const std::string canny = "Canny";
	const std::string hough_lines = "Hough Lines";

	//	cv::namedWindow(original_image, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(gaussian, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(canny, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(hough_lines, cv::WINDOW_AUTOSIZE);

	//cv::imshow(original_image, original);
	cv::imshow(gaussian, gaussian_image);
	cv::imshow(canny, canny_image);
	cv::imshow(hough_lines, hough_image);


}


void lane_marker_detect::detect() {

	//int width = 700, height = 350;
	cv::VideoCapture cam = cv::VideoCapture("../videos/testvid00.mp4");
	cv::Mat gray;

	std::vector<cv::Point2f> origPoints;
	std::vector<cv::Point2f> dstPoints;

	std::vector < std::vector<cv::Point> > contours;
	std::vector < cv::Vec4i > hierarchy;

	cv::namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
#if hsv


	int iLowH = 0;
	int iHighH = 62; //62

	int iLowS = 0;
	int iHighS = 188; //188

	int iLowV = 158; //128
	int iHighV = 255;

	//Create trackbars in "Control" window
	cv::createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cv::createTrackbar("HighH", "Control", &iHighH, 179);

	cv::createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cv::createTrackbar("HighS", "Control", &iHighS, 255);

	cv::createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cv::createTrackbar("HighV", "Control", &iHighV, 255);
#endif

#if binarize
	int threshL = 193;
	int threshH = 255;

	cv::createTrackbar("threshL", "Control", &threshL, 255); //Value (0 - 255)
	cv::createTrackbar("threshH", "Control", &threshH, 255);
#endif

	while (1) {
		cam >> original;
		//resize(original, original, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
		cv::cvtColor(original, gray, cv::COLOR_BGR2GRAY);
		gray.copyTo(gaussian_image);

#if hsv
		cvtColor(original, gaussian_image, cv::COLOR_BGR2HSV); //Convert frame from BGR to HSV

		inRange(gaussian_image, cv::Scalar(iLowH, iLowS, iLowV), cv::Scalar(iHighH, iHighS, iHighV), gaussian_image);
		inRange(gaussian_image, cv::Scalar(iLowH, iLowS, iLowV), cv::Scalar(iHighH, iHighS, iHighV), gaussian_image);

		erode(gaussian_image, gaussian_image, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
		dilate(gaussian_image, gaussian_image, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));

#endif

#if binarize
		threshold(gaussian_image, gaussian_image, threshL, threshH, cv::THRESH_BINARY_INV);
#endif

		gaussian_blur(gaussian_image);
		canny_image = canny_edge(gaussian_image);

		hough_image = hough_transform(canny_image, original);

		show_windows(hough_image, canny_image, gaussian_image, gaussian_image);
		if (cv::waitKey(30) >= 0)
			break;
	}
}

/*
 * check if a given either x or y coordinate is on a line in Cartesian coordinate
 * @param x, y --> x and y coordinates
 * @param m, b --> slope m and intercept b in the line equation y = mx + b
 */

bool lane_marker_detect::line_check(double x, double y, double b, double m)
{
	bool check = false;
	if(x == ((y - b) / m))
		check = true;
	else if(y == (m * x + b))
		check = true;
	return check;
}
/*
 * Function to find slope and intercept (m and b) in the line equation y = mx + b
 * @param point1, point2 --> 2 points on the line
 * @param *m, *b --> double pointers to slope and intercept variable
 */

void lane_marker_detect::line_equation(cv::Point2d point1, cv::Point2d point2, double *m, double *b)
{
	*m = (point2.y - point1.y) / (point2.x - point1.x);
	*b = point2.y - point2.x * (*m);
}

/*
 * Function to find x-coordinate given y so that a point(x,y) would be on a line
 * @param y --> y-coordinate of a point
 * @param m, b --> the slope and intercept value of a line in cartesian coordinate
 */

double lane_marker_detect::find_x(double y, double b, double m)
{
	double x = (y - b) / m;
	return x;
}

/*
 * Function to calculate the deviation percentage of a car
 * @param dst_left, dst_right --> distance from the left and right lane markers
 * return value is the percentage of deviation
 */

double lane_marker_detect::dev_calc(double dst_left, double dst_right)
{
	int pos = -1;
	double res = 0;
	double total = dst_left + dst_right;
	double min = dst_left;
	if(dst_right < min){
		min = dst_right;
		pos = 1;
	}
	else if(dst_right == min){
		pos = 0;
	}

	res = min / (total / 2);
	res = 1 - res;
	res *= 100;

	if(pos == -1) res = -res;

	return res;
}
