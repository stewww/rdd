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

	frame_counter = 0;
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

	frame_counter = 0;
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
	std::cout<<"Start Hough"<<std::endl;
	cv::Mat hough = original.clone();
	bool l1 = false, l2 = false, l3 = false, l4 = false, l5 = false, l6 = false;
	std::clock_t start;
	start = std::clock();
	double duration;
	//	double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC; // get duration in second
	//	duration *= 1000; // get duration in milisecond
	//	std::cout<<"Start "<<duration<<std::endl;
	int threshold = 60;
	//inverse transformation matrix value that I got from using ipm.getHinv() from the points orig & dst points
	//	cv::Mat transform = (cv::Mat_<float>(3,3) << -0.02255122438780657, 0.0136674087198828, -16.0628019323675,
	//			0.004747626186906658, -0.00187027698272079, -16.0628019323675,
	//			2.373813093453329e-005, 4.244090076174131e-005, -0.08031400966183749);
#if houghp
	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(canny_image, lines, rho, theta, threshold, minLineLength, maxLineGap);
#endif

#if stdhough
	//std::vector<cv::Vec2f> lines;
	lines.empty();
	cv::HoughLines(canny_image, lines, 1, CV_PI/180, threshold);

#endif

	/*	line(hough, cv::Point(x, y), cv::Point(x, y), cv::Scalar(a, b, c), 3, lineType)
	 *	x, y = 2d array. ex: x = [int][int]
	 *	a, b, c = RGB values 0-255
	 *	lineType = type of line that is to be used. See http://docs.opencv.org/3.1.0/d0/de1/group__core.html#gaf076ef45de481ac96e0ab3dc2c29a777
	 */
	cv::Point2d pt1, pt2, pt3, pt4, left_pt, cen_pt, right_pt, pt5, pt6, pt7, pt8, pt9, pt10, pt11, pt12;
	cv::Scalar line_color1 = cv::Scalar(255, 0, 0); 	// BGR color Blue
	cv::Scalar line_color2 = cv::Scalar(0, 255, 0); 	// Green
	cv::Scalar line_color3 = cv::Scalar(0, 0, 255); 	// Red
	cv::Scalar line_color4 = cv::Scalar(255, 255, 0);	// Cyan
	cv::Scalar line_color5 = cv::Scalar(255, 0, 255);	// Pink
	cv::Scalar line_color6 = cv::Scalar(0, 255, 255);	// Yellow
	int line_thickness = 4;
	line one, two, three, four, five, six;
	int line_cnt = 0;
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
		// Extract line segments from Hough Transform
#if stdhough
		double rho = lines[i][0], theta = lines[i][1];
		if(rho < 0){
			if (theta > 1.64 && theta < 1.67){
				//std::cout<<"rho: "<<rho<<"   "<<"theta: "<<theta<<std::endl;
				std::cout<<"6: "<<theta<<" "<<rho<<std::endl;
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;

				cv::Point2d temp1, temp2;
				temp1.x = cvRound(x0 + 300 * (-b));
				temp1.y = cvRound(y0 + 300 * (a));
				temp2.x = cvRound(x0 - 300 * (-b));
				temp2.y = cvRound(y0 - 300 * (a));

				line_equation(temp1, temp2, &six);

				pt11.y = 15;
				pt12.y = 31;
				pt11.x = find_x(pt11.y, six);
				pt12.x = find_x(pt12.y, six);
				l6 = true;
			}
			else if (theta > 1.74 && theta < 1.8){
				//std::cout<<"rho: "<<rho<<"   "<<"theta: "<<theta<<std::endl;
				std::cout<<"5: "<<theta<<" "<<rho<<std::endl;
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;

				cv::Point2d temp1, temp2;
				temp1.x = cvRound(x0 + 300 * (-b));
				temp1.y = cvRound(y0 + 300 * (a));
				temp2.x = cvRound(x0 - 300 * (-b));
				temp2.y = cvRound(y0 - 300 * (a));

				line_equation(temp1, temp2, &five);

				pt9.y = 44;
				pt10.y = 70;
				pt9.x = find_x(pt9.y, five);
				pt10.x = find_x(pt10.y, five);
				l5 = true;
			}
			else if (theta > 2 && theta < 2.1){
				//std::cout<<"rho: "<<rho<<"   "<<"theta: "<<theta<<std::endl;
				std::cout<<"4: "<<theta<<" "<<rho<<std::endl;
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;

				cv::Point2d temp1, temp2;
				temp1.x = cvRound(x0 + 300 * (-b));
				temp1.y = cvRound(y0 + 300 * (a));
				temp2.x = cvRound(x0 - 300 * (-b));
				temp2.y = cvRound(y0 - 300 * (a));

				line_equation(temp1, temp2, &four);

				pt7.y = 80;
				pt8.y = 150;
				pt7.x = find_x(pt7.y, four);
				pt8.x = find_x(pt8.y, four);
				l4 = true;
			}
		}
		else if (rho > 0){
			if(theta < 1.1 && theta > 0.8){
				//std::cout<<"rho: "<<rho<<"   "<<"theta: "<<theta<<std::endl;
				std::cout<<"3: "<<theta<<" "<<rho<<std::endl;
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;

				pt5.x = cvRound(x0 + 300 * (-b));
				pt5.y = cvRound(y0 + 300 * (a));
				pt6.x = cvRound(x0 - 300 * (-b));
				pt6.y = cvRound(y0 - 300 * (a));

				line_equation(pt5, pt6, &three);

				pt5.y = 60;
				pt6.y = 127;
				pt5.x = find_x(pt5.y, three);
				pt6.x = find_x(pt6.y, three);
				l3 = true;
			}
			else if(theta < 1.37 && theta > 1.3){
				//std::cout<<"rho: "<<rho<<"   "<<"theta: "<<theta<<std::endl;
				std::cout<<"2: "<<theta<<" "<<rho<<std::endl;
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;

				pt3.x = cvRound(x0 + 300 * (-b));
				pt3.y = cvRound(y0 + 300 * (a));
				pt4.x = cvRound(x0 - 300 * (-b));
				pt4.y = cvRound(y0 - 300 * (a));

				line_equation(pt3, pt4, &two);
				l2 = true;
			}
			else if(theta < 1.5 && theta > 1.4){
				//std::cout<<"rho: "<<rho<<"   "<<"theta: "<<theta<<std::endl;
				std::cout<<"1: "<<theta<<" "<<rho<<std::endl;
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;

				pt1.x = cvRound(x0 + 300 * (-b));
				pt1.y = cvRound(y0 + 300 * (a));
				pt2.x = cvRound(x0 - 300 * (-b));
				pt2.y = cvRound(y0 - 300 * (a));

				line_equation(pt1, pt2, &one);
				l1 = true;
			}
		}
		else
			break;

		bool expired = false;

		duration = (std::clock() - start) / (double) CLOCKS_PER_SEC; // get duration in second
		duration *= 1000; // get duration in milisecond
		if(duration > 7)
			expired = true;
		/*
		 * Print out the result every 7 ms if the data is valid
		 * Ex: left point's x-coordinate should be smaller than the center point
		 */
		/*if(expired && left_pt.x != 0 && right_pt.x != 0 && right_pt.x > cen_pt.x && left_pt.x < cen_pt.x){
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
		}*/


#endif
	}

	if(l1 && l2 && l3 && l4 && l5 && l6){
		line_cnt += 6;
		std::cout<<"Detected all six lines"<<std::endl;
		std::cout<<"Line count: "<<line_cnt<<std::endl;
	}

	std::vector<line> detected_lines;
	std::vector<lane> defined_lanes;

	//	if(leftside_of(&one, &two) && leftside_of(&one, &three)){
	//		detected_line.push_back(one);
	//		detected_line[0] = one;
	//	}
	//	if(rightside_of(&two, &one) && leftside_of(&two, &three)){
	//		detected_line.push_back(two);
	//		detected_line[1] = two;
	//	}
	//	if(rightside_of(&three, &one) && rightside_of(&three, &two)){
	//		detected_line.push_back(three);
	//		detected_line[2] = three;
	//	}

	if(line_cnt == 6){
		for(int i = 0; i < line_cnt; i++){
			line temp;
			detected_lines.push_back(temp);
		}
		if(detected_lines.size() == 6){
			detected_lines[0] = one;
			detected_lines[1] = two;
			detected_lines[2] = three;
			detected_lines[3] = four;
			detected_lines[4] = five;
			detected_lines[5] = six;
		}
		std::cout<<"6 lines detected in one frame!"<<std::endl;
	}

	//std::cout<<"# of lines: "<<detected_lines.size()<<std::endl;
	if(detected_lines.size() > 0){
		for(std::size_t i = 0; i < detected_lines.size() - 1; i++){
			cv::Point2d center;
			lane temp;
			center = find_center_point(&detected_lines[i], &detected_lines[i+1]);
			//		temp.center = center;
			//		temp.left = detected_line[i];
			//		temp.right = detected_line[i+1];
			//		temp.number = i;
			defined_lanes.push_back(temp);
			defined_lanes[i] = define_lane(center, &detected_lines[i], &detected_lines[i+1], (int) i + 1);
		}
	}

	for(std::size_t i = 0; i < defined_lanes.size(); i++)
		std::cout<<defined_lanes[i].number<<std::endl;
	if(detected_lines.size() >= line_cnt && detected_lines.size() > 0){
		detected_lines.empty();
		defined_lanes.empty();
		std::cout<<"Deleted!"<<std::endl;
	}

	cv::line(hough, pt1, pt2, line_color1, line_thickness, cv::LINE_AA);
	cv::line(hough, pt3, pt4, line_color2, line_thickness, cv::LINE_AA);
	cv::line(hough, pt5, pt6, line_color3, line_thickness, cv::LINE_AA);
	cv::line(hough, pt7, pt8, line_color4, line_thickness, cv::LINE_AA);
	cv::line(hough, pt9, pt10, line_color5, line_thickness, cv::LINE_AA);
	cv::line(hough, pt11, pt12, line_color6, line_thickness, cv::LINE_AA);

	std::cout<<"End Hough"<<std::endl;
	return hough;
}

/*
 *	Show images
 */
void lane_marker_detect::show_windows(cv::Mat *hough_image, cv::Mat *canny_image, cv::Mat *gaussian_image, cv::Mat *original) {
	const std::string original_image = "Original Image";
	const std::string gaussian = "Gaussian Blur";
	const std::string canny = "Canny";
	const std::string hough_lines = "Hough Lines";

	//	cv::namedWindow(original_image, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(gaussian, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(canny, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(hough_lines, cv::WINDOW_AUTOSIZE);

	//cv::imshow(original_image, original);
	cv::imshow(gaussian, *gaussian_image);
	cv::imshow(canny, *canny_image);
	cv::imshow(hough_lines, *hough_image);


}


void lane_marker_detect::detect() {

	//int width = 700, height = 350;
	cv::VideoCapture cam = cv::VideoCapture("../videos/ir_testvid01.mp4");
	cv::Mat gray;
	cv::Point p_one, p_two;
	frame_counter = 0;
	p_one.x = 0;
	p_one.y = 320;
	p_two.x = 1280;
	p_two.y = 450;

	cv::namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
#if hsv


	int iLowH = 0;
	int iHighH = 67; //62

	int iLowS = 0;
	int iHighS = 236; //188

	int iLowV = 115; //128
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
		cam >> noncrop_original;
		std::cout<<"Frame no. "<<frame_counter<<std::endl;
		frame_counter++;
		cropImage(&noncrop_original, &original, p_one, p_two);
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
		//new_filter(&original, &canny_image);

		hough_image = hough_transform(canny_image, original);

		show_windows(&hough_image, &canny_image, &gaussian_image, &gaussian_image);
		if (cv::waitKey(30) >= 0)
			break;
	}
}

/*
 * check if a given either x or y coordinate is on a line in Cartesian coordinate
 * @param x, y --> x and y coordinates
 * @param l --> line struct, containing slope m and intercept b in the line equation y = mx + b
 */

bool lane_marker_detect::line_check(double x, double y, line l)
{
	bool check = false;
	if(x == ((y - l.b) / l.m))
		check = true;
	else if(y == (l.m * x + l.b))
		check = true;
	return check;
}
/*
 * Function to find slope and intercept (m and b) in the line equation y = mx + b
 * @param point1, point2 --> 2 points on the line
 * @param *m, *b --> double pointers to slope and intercept variable
 */

void lane_marker_detect::line_equation(cv::Point2d point1, cv::Point2d point2, line *l)
{
	l->m = (point2.y - point1.y) / (point2.x - point1.x);
	l->b = point2.y - point2.x * (l->m);
}

/*
 * Function to find x-coordinate given y and a line equation so that a point(x,y) would be on the line
 * @param y --> y-coordinate of a point
 * @param m, b --> the slope and intercept value of a line in cartesian coordinate
 */

double lane_marker_detect::find_x(double y, line l)
{
	double x = (y - l.b) / l.m;
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

void lane_marker_detect::cropImage(cv::Mat * inputImage, cv::Mat * croppedImage, cv::Point one, cv::Point two)
{
	cv::Rect rect;
	cv::Rect box;

	box.width = abs(one.x - two.x);
	box.height = abs(one.y - two.y);
	box.x = std::min(one.x, two.x);
	box.y = std::min(one.y, two.y);

	*croppedImage = (*inputImage)(box);
}

void lane_marker_detect::new_filter(cv::Mat * in, cv::Mat * out)
{
	static cv::Mat inputImage;
	static cv::Mat channel[3];
	static cv::Mat simpleColorImage;
	static cv::Mat edgeDetectionImageGrey;
	static cv::Mat temp1;
	static cv::Mat contoursAndEllipse;
	const int division = 32;

	inputImage = *in;

	inputImage.copyTo(simpleColorImage);

	// Simple color reduction
	colorReduce(simpleColorImage, division);

	// Split into H,S and V
	split(simpleColorImage, channel);

	// Taking hue only
	channel[0] = cv::Mat::zeros(simpleColorImage.rows, simpleColorImage.cols, CV_8UC1);//Set hue channel to 0
	channel[1] = cv::Mat::zeros(simpleColorImage.rows, simpleColorImage.cols, CV_8UC1);//Set saturation channel to 0

	// Merge the channels back together
	merge(channel, 3, simpleColorImage);

	simpleColorImage.copyTo(edgeDetectionImageGrey);

	// Copy over the channels before calculating greyscale
	// Split into H,S and V
	split(edgeDetectionImageGrey, channel);

	// Taking hue only
	channel[2].copyTo(channel[0]);
	channel[2].copyTo(channel[1]);

	// Merge the channels back together
	merge(channel, 3, edgeDetectionImageGrey);


	// Convert to greyscale
	cvtColor(edgeDetectionImageGrey, edgeDetectionImageGrey, CV_BGR2GRAY);

	*out = edgeDetectionImageGrey;
}

void lane_marker_detect::colorReduce(cv::Mat& image, int div)
{
	int nl = image.rows;                    // number of lines
	int nc = image.cols * image.channels(); // number of elements per line

	for (int j = 0; j < nl; j++)
	{
		// get the address of row j
		uchar* data = image.ptr<uchar>(j);

		for (int i = 0; i < nc; i+=3)
		{
			if ((data[i + 0] >= 0 && data[i + 0] < 20) && // saturation???
					(data[i + 1] > 105) && // value?
					(data[i + 2] >= 0  && data[i + 2] < 140)) // hue....?
			{
				data[i + 0] = 0;
				data[i + 1] = 0;
				data[i + 2] = 0;
			}
			else
			{
				data[i + 0] = data[i + 0] / div * div + div / 2;
				data[i + 1] = data[i + 1] / div * div + div / 2;
				data[i + 2] = data[i + 2] / div * div + div / 2;
			}
		}
	}
}

bool lane_marker_detect::leftside_of(line *l1, line *l2)
{
	bool check = false;

	cv::Point2d one, two;
	one.y = 100; two.y = 100;
	one.x = find_x(one.y, *l1);
	two.x = find_x(two.y, *l2);

	if(one.x < two.x)
		check = true;

	return check;
}

bool lane_marker_detect::rightside_of(line *l1, line *l2)
{
	bool check = false;

	cv::Point2d one, two;
	one.y = 100; two.y = 100;
	one.x = find_x(one.y, *l1);
	two.x = find_x(two.y, *l2);

	if(one.x > two.x)
		check = true;

	return check;
}

cv::Point2d lane_marker_detect::find_center_point(line *l1, line *l2)
{
	cv::Point2d left, right, center;

	left.y = 100, right.y = 100, center.y = 100;
	left.x = find_x(left.y, *l1);
	right.x = find_x(right.y, *l2);
	center.x = (left.x + right.x) / 2;

	return center;
}

lane lane_marker_detect::define_lane(cv::Point2d center, line *left, line *right, int num)
{
	lane ln;

	ln.center = center;
	ln.left = *left;
	ln.right = *right;
	ln.number = num;

	return ln;
}

void lane_marker_detect::lane_process()
{

}
