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

	iLowH = 0;
	iHighH = 0;

	iLowS = 0;
	iHighS = 0;

	iLowV = 0;
	iHighV = 0;

	threshL = 0;
	threshH = 0;

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
cv::Mat lane_marker_detect::hough_transform(cv::Mat canny_image, cv::Mat original, std::vector<lane_c> * lane_) {
	std::cout<<"Start Hough"<<std::endl;
	cv::Mat hough = original.clone();
	static int l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
	//std::clock_t start;
	//start = std::clock();
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
	line_c line_[6];
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
			if (theta > 1.64 && theta < 1.66){
				//std::cout<<"rho: "<<rho<<"   "<<"theta: "<<theta<<std::endl;
				std::cout<<"6: "<<theta<<" "<<rho<<std::endl;
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;

				cv::Point2d temp1, temp2;
				temp1.x = cvRound(x0 + 300 * (-b));
				temp1.y = cvRound(y0 + 300 * (a));
				temp2.x = cvRound(x0 - 300 * (-b));
				temp2.y = cvRound(y0 - 300 * (a));

				line_[5].def_equation(&temp1, &temp2);

				pt11.y = 15;
				pt12.y = 31;
				pt11.x = line_[5].find_x(pt11.y);
				pt12.x = line_[5].find_x(pt12.y);
				l6 = rho;
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

				line_[4].def_equation(&temp1, &temp2);

				pt9.y = 44;
				pt10.y = 70;
				pt9.x = line_[4].find_x(pt9.y);
				pt10.x = line_[4].find_x(pt10.y);
				l5 = rho;
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

				line_[3].def_equation(&temp1, &temp2);

				pt7.y = 80;
				pt8.y = 150;
				pt7.x = line_[3].find_x(pt7.y);
				pt8.x = line_[3].find_x(pt8.y);
				l4 = rho;
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

				line_[2].def_equation(&pt5, &pt6);

				pt5.y = 60;
				pt6.y = 127;
				pt5.x = line_[2].find_x(pt5.y);
				pt6.x = line_[2].find_x(pt6.y);
				l3 = rho;
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

				line_[1].def_equation(&pt3, &pt4);
				l2 = rho;
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

				line_[0].def_equation(&pt1, &pt2);
				l1 = rho;
			}
		}
		else
			break;

#endif
	}

	if(l1 && l2 && l3 && l4 && l5 && l6){
		line_cnt += 6;
		std::cout<<"Detected all six lines"<<std::endl;
		std::cout<<"Line count: "<<line_cnt<<std::endl;
	}

	if(line_cnt > 0){
		for(int i = 0; i < line_cnt - 1; i++){
			cv::Point2d center;
			lane_c temp;
			center = temp.calculate_center_point(&line_[i], &line_[i+1]);
			(*lane_).push_back(temp);
			(*lane_)[i].def_lane(center, (int) i + 1, &line_[i], &line_[i+1]);
		}
	}

	for(int i = 0; i < line_cnt; i++)
		std::cout<<get_number((*lane_)[i])<<std::endl;
	if((frame_counter % 10) == 0){
		l1 = 0; l2 = 0; l3 = 0; l4 = 0; l5 = 0; l6 = 0;
		(*lane_).empty();
		std::cout<<"Cleared!"<<std::endl;
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

void lane_marker_detect::set_up()
{
	cv::namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
#if hsv


	iLowH = 0;
	iHighH = 67; //62

	iLowS = 0;
	iHighS = 236; //188

	iLowV = 115; //128
	iHighV = 255;

	//Create trackbars in "Control" window
	cv::createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cv::createTrackbar("HighH", "Control", &iHighH, 179);

	cv::createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cv::createTrackbar("HighS", "Control", &iHighS, 255);

	cv::createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cv::createTrackbar("HighV", "Control", &iHighV, 255);
#endif

#if binarize
	threshL = 193;
	threshH = 255;

	cv::createTrackbar("threshL", "Control", &threshL, 255); //Value (0 - 255)
	cv::createTrackbar("threshH", "Control", &threshH, 255);
#endif


}


void lane_marker_detect::detect(cv::Mat *vid_frame, std::vector<lane_c> * lane_) {

	//int width = 700, height = 350;
	//	cv::VideoCapture cam = cv::VideoCapture("../videos/ir_testvid01.mp4");
	cv::Mat gray;
	cv::Point p_one, p_two;
	p_one.x = 0;
	p_one.y = 320;
	p_two.x = 1280;
	p_two.y = 450;

	noncrop_original = *vid_frame;
	std::cout<<"Frame no. "<<frame_counter<<std::endl;
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
	hough_image = hough_transform(canny_image, original, lane_);
	show_windows(&hough_image, &canny_image, &gaussian_image, &gaussian_image);
	frame_counter++;
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
