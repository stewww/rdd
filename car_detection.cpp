#include "car_detection.hpp"
#include "opencv2/opencv.hpp"
#include "helper_opencv.hpp"

/*** Trackbar variables ***/
static int greyThreshold = 40;
static const int greyThresholdMax = 255;
static string trackbarThresholdName = "Grey Threshold Trackbar";

static int greyDiffInterval = 9;
static const int greyDiffMax = 50;
static const string trackbarGreyDiffName = "Grey Diff Trackbar";

static int edgeDetectionGreyThreshold = 48;
static const int edgeDetectionGreyThresholdMax = 255;
static string trackbarEdgeDetectionThresholdName = "Edge Detection Grey Threshold Trackbar";


/*** Window names ***/
static const string windowName_greyThreshold = "Grey Threshold";
static const string windowName_greyDiff = "Grey Diff";
static const string windowName_contours = "Contours";
static const string windowName_contoursAndEllipses = "Contours and Ellipses";
static const string windowName_HSVImage = "HSV Image";
static const string windowName_InputImage = "Input Image";
static const string windowName_InputImageUp = "Input Image Up";
static const string windowName_InputImageDown = "Input Image Down";
static const string windowName_InputImageLeft = "Input Image Left";
static const string windowName_InputImageRight = "Input Image Right";
static const string windowName_EdgeDetection = "Edge Detection";
static const string windowName_EdgeDetectionGrey = "Edge Detection Grey";
static const string windowName_CroppedImage = "Cropped Image";
static const string windowName_prevGrey = "Prev Grey Image";
static const string windowName_currGrey = "Current Grey Image";
static const string windowName_SimpleColors = "Simple Colors";

void detect_initDebugInfo(void)
{
	namedWindow(windowName_greyThreshold);
	moveWindow(windowName_greyThreshold, 100, 100);
	helper_trackbarSimple(trackbarThresholdName, windowName_greyThreshold, &greyThreshold, greyThresholdMax);

	namedWindow(windowName_greyDiff);
	moveWindow(windowName_greyDiff, 100, 100);
	helper_trackbarSimple(trackbarGreyDiffName, windowName_greyDiff, &greyDiffInterval, greyDiffMax);

 	namedWindow(windowName_contours);
 	moveWindow(windowName_contours, 100, 100);

 	namedWindow(windowName_InputImage);
 	moveWindow(windowName_InputImage, 100, 100);

 	namedWindow(windowName_InputImageUp);
 	moveWindow(windowName_InputImageUp, 100, 100);

 	namedWindow(windowName_InputImageDown);
 	moveWindow(windowName_InputImageDown, 100, 100);

 	namedWindow(windowName_InputImageLeft);
 	moveWindow(windowName_InputImageLeft, 100, 100);

 	namedWindow(windowName_InputImageRight);
 	moveWindow(windowName_InputImageRight, 100, 100);

 	namedWindow(windowName_EdgeDetection);
 	moveWindow(windowName_EdgeDetection, 100, 100);

 	namedWindow(windowName_EdgeDetectionGrey);
 	moveWindow(windowName_EdgeDetectionGrey, 100, 100);
	helper_trackbarSimple(trackbarEdgeDetectionThresholdName, windowName_EdgeDetectionGrey, &edgeDetectionGreyThreshold, edgeDetectionGreyThresholdMax);

 	namedWindow(windowName_CroppedImage);
 	moveWindow(windowName_CroppedImage, 100, 500);

 	namedWindow(windowName_prevGrey);
 	moveWindow(windowName_prevGrey, 100, 100);

 	namedWindow(windowName_currGrey);
 	moveWindow(windowName_currGrey, 100, 100);
}

static VideoCapture capture;
static bool updateImage = false;
static bool programStart = false;

static bool openCapture(void)
{
	capture.open("ir_testvid01.mp4");

	if (!capture.isOpened())
	{
		printf("uh oh?\n");
		return false;
	}
	else
	{
		return true;
	}
}

static bool waitForNextFrame(void)
{
	char key = waitKey(1);
	if (key  == ' ')
	{
		updateImage = !updateImage;
		if (updateImage)
		{
			programStart = true;
			printf("Running video\n");
		}
		else
		{
			printf("Pausing video\n");
		}
	}
	else if (key == 'q')
	{
		return false;
	}
	return true;
}

int detect_vehicles_greydiff(vector<Vehicle_Information_S> * vehicles)
{
	if (!openCapture())
	{
		return false;
	}

	while (1)
	{
		static Mat inputImage;
		static Mat croppedImage;
		static bool start = true;
		bool validDiff = false;
		static uint32_t counter = 0;

		if (updateImage || start)
		{
			capture.read(inputImage);
			helper_cropImage(&inputImage, &croppedImage);
			start = false;
			counter++;
			static Mat prev_grey_image;
			static Mat curr_grey_image;
			static Mat diff_grey_image;
			vector<vector<Point> > contours;
			if (!(counter % greyDiffInterval))
			{
				Mat contours_drawing;
				Mat contoursAndEllipse;
				Mat threshold_diff;
				cvtColor(croppedImage, curr_grey_image, CV_BGR2GRAY);
				if (prev_grey_image.size().height > 0)
				{
					validDiff = true;
					absdiff(prev_grey_image, curr_grey_image, diff_grey_image);

					diff_grey_image.copyTo(threshold_diff);
					threshold(threshold_diff, threshold_diff, greyThreshold, greyThresholdMax, THRESH_BINARY);

					if (!threshold_diff.empty())
					{
						imshow(windowName_greyThreshold, threshold_diff);
						threshold_diff.copyTo(contours_drawing);
						vector<Point> pointsOfInterest;
						helper_drawEllipseAroundContours(&contours_drawing, &contoursAndEllipse, 10,15,300,400, &pointsOfInterest);
						imshow(windowName_contours, contours_drawing);
						imshow(windowName_contoursAndEllipses, contoursAndEllipse);
					}
				}
				curr_grey_image.copyTo(prev_grey_image);

			}

			// Show Images
			imshow(windowName_InputImage, inputImage);
			imshow(windowName_CroppedImage, croppedImage);

			// And differences
			if (validDiff)
			{
				imshow(windowName_prevGrey, prev_grey_image);
				imshow(windowName_currGrey, curr_grey_image);
				imshow(windowName_greyDiff, diff_grey_image);
			}

		}
		// If we need to quit, then quit
		if (!waitForNextFrame())
		{
			return false;
		}
	}
}

int detect_vehicles_edgeDetection(vector<Vehicle_Information_S> * vehicles)
{
	if (!openCapture())
	{
		return false;
	}

	while (1)
	{
		static Mat hsvImage;
		static Mat inputImage;
		static Mat inputImageUp;
		static Mat inputImageDown;
		static Mat inputImageLeft;
		static Mat inputImageRight;
		static Mat edgeDetectionImage;
		static Mat edgeDetectionImageGrey;
		static Mat simpleColorImage;
		static Mat temp1;
		static Mat contoursAndEllipse;
		static bool start = true;
		static int edgeWidth = 1;

		if (updateImage || start)
		{
			const int division = 32;
			capture.read(inputImage);
			helper_cropImage(&inputImage, &inputImage);

			inputImage.copyTo(hsvImage);

			cvtColor(hsvImage, hsvImage, CV_BGR2HSV);
			inputImage.copyTo(simpleColorImage);

			// Simple color reduction
			colorReduce(simpleColorImage, division);

			// Zero out all Mats
			inputImageUp = Mat::zeros(simpleColorImage.size(), simpleColorImage.type());
			//inputImageDown = Mat::zeros(simpleColorImage.size(), simpleColorImage.type());
			//inputImageLeft = Mat::zeros(simpleColorImage.size(), simpleColorImage.type());
			//inputImageRight = Mat::zeros(simpleColorImage.size(), simpleColorImage.type());
			if (start)
			{
				edgeDetectionImage = Mat::zeros(simpleColorImage.size(), simpleColorImage.type());
			}

			// Shift all Mats
			simpleColorImage(cv::Rect(0,edgeWidth, simpleColorImage.cols,simpleColorImage.rows-edgeWidth)).copyTo(inputImageUp(cv::Rect(0,0,simpleColorImage.cols,simpleColorImage.rows-edgeWidth)));
			//simpleColorImage(cv::Rect(0,0, simpleColorImage.cols,simpleColorImage.rows-edgeWidth)).copyTo(inputImageDown(cv::Rect(0,edgeWidth,simpleColorImage.cols,simpleColorImage.rows-edgeWidth)));
			//simpleColorImage(cv::Rect(edgeWidth,0, simpleColorImage.cols-edgeWidth,simpleColorImage.rows)).copyTo(inputImageLeft(cv::Rect(0,0,simpleColorImage.cols-edgeWidth,simpleColorImage.rows)));
			//simpleColorImage(cv::Rect(0,0, simpleColorImage.cols-edgeWidth,simpleColorImage.rows)).copyTo(inputImageRight(cv::Rect(edgeWidth,0,simpleColorImage.cols-edgeWidth,simpleColorImage.rows)));

			// Take absolute diff of each input image
			absdiff(simpleColorImage, inputImageUp, inputImageUp);
			//absdiff(simpleColorImage, inputImageDown, inputImageDown);
			//absdiff(simpleColorImage, inputImageLeft, inputImageLeft);
			//absdiff(simpleColorImage, inputImageRight, inputImageRight);

			double filterSize = 4;
			// Add the diffs together (average for now)
			Scalar numerator = Scalar(filterSize - 1 + 0.5, filterSize - 1 + 0.5, filterSize - 1 + 0.5);
			Scalar denominator = Scalar(filterSize, filterSize, filterSize);
			if (!start)
			{
				multiply(edgeDetectionImage, numerator, edgeDetectionImage);
				divide(edgeDetectionImage, denominator, edgeDetectionImage);

				divide(inputImageUp, denominator, inputImageUp);
			}

			add(edgeDetectionImage, inputImageUp, edgeDetectionImage);
			//add(edgeDetectionImage, inputImageDown, edgeDetectionImage);
			//add(edgeDetectionImage, inputImageLeft, edgeDetectionImage);
			//add(edgeDetectionImage, inputImageRight, edgeDetectionImage);

//			inRange(edgeDetectionImage, Scalar(240, edgeDetectionGreyThreshold, edgeDetectionGreyThreshold), Scalar(255, 255, 255), edgeDetectionImageGrey);

			// Threshold from color diff, as it has more information
			threshold(edgeDetectionImage, edgeDetectionImageGrey, edgeDetectionGreyThreshold, edgeDetectionGreyThresholdMax, THRESH_BINARY);

			// Convert the diff into greyscale
			cvtColor(edgeDetectionImageGrey, edgeDetectionImageGrey, CV_BGR2GRAY);

			edgeDetectionImageGrey.copyTo(temp1);

			vector<Point> pointsOfInterest;

			// Draw circles on the center of each contour and add it to a vector
			helper_drawEllipseAroundContours(&temp1, &contoursAndEllipse,3,15,300,400, &pointsOfInterest);

			// Grab points around the points of interest
			for (int point = 0; point < pointsOfInterest.size(); point++)
			{
				int xPos = pointsOfInterest.at(point).x;
				int yPos = pointsOfInterest.at(point).y;
				// How far we are going to sample our points in the x axis
				int xWidth = 15;
				// How far we are going to sample our points in the y axis
				int yWidth = 10;
				// If we have enough space to sample the point
				if (xPos >= xWidth && xPos < (contoursAndEllipse.cols - xWidth) &&
					yPos >= yWidth && yPos < (contoursAndEllipse.rows - yWidth))
				{
					// Amount of difference allowed to the contour's nearby sample points
					int diffAmount = 40;
					// Variable to keep track of how similar the nearby sample points are to the contour's center
					int commonHSVCount = 0;
					// How many sample points that must match to declare this as a valid point
					int requiredHSVCount = 3;
					printf("X: %i, Y: %i\n", xPos - xWidth, yPos - yWidth);

#define SAMPLE_POINT(y,x,a)	contoursAndEllipse.at<Vec3b>(y,x)[a]

#define SHOW_SAMPLE_POINT(y,x)	SAMPLE_POINT(y,x,0) = 255; \
								SAMPLE_POINT(y,x,1) = 255; \
								SAMPLE_POINT(y,x,2) = 255
#define SHOW_VALID_SAMPLE(yOrigin, xOrigin, ySample, xSample)	\
					if (abs(SAMPLE_POINT(yOrigin,xOrigin,0) - SAMPLE_POINT(ySample,xSample,0) < diffAmount) && 	\
						abs(SAMPLE_POINT(yOrigin,xOrigin,1) - SAMPLE_POINT(ySample,xSample,1) < diffAmount) && 	\
						abs(SAMPLE_POINT(yOrigin,xOrigin,2) - SAMPLE_POINT(ySample,xSample,2) < diffAmount))	\
					{ \
						SHOW_SAMPLE_POINT(ySample, xSample); \
						printf("ysample: %i, xsample: %i\n", ySample, xSample); \
						commonHSVCount++; \
					}

					SHOW_VALID_SAMPLE(yPos, xPos, (yPos - yWidth), (xPos - xWidth));
					SHOW_VALID_SAMPLE(yPos, xPos, (yPos - yWidth), xPos);
					SHOW_VALID_SAMPLE(yPos, xPos, (yPos - yWidth), (xPos + xWidth));
					SHOW_VALID_SAMPLE(yPos, xPos, (yPos + yWidth), (xPos - xWidth));
					SHOW_VALID_SAMPLE(yPos, xPos, (yPos + yWidth), xPos);
					SHOW_VALID_SAMPLE(yPos, xPos, (yPos + yWidth), (xPos + xWidth));
					if (commonHSVCount >= requiredHSVCount)
					{
						Scalar green = Scalar(0, 255, 0);
						circle(inputImage, pointsOfInterest.at(point), 10, green, -1);
					}

				}
			}

			// Show Images
			imshow(windowName_InputImage, inputImage);
			imshow(windowName_HSVImage, hsvImage);
			imshow(windowName_SimpleColors, simpleColorImage);
//			imshow(windowName_InputImageUp, inputImageUp);
//			imshow(windowName_InputImageDown, inputImageDown);
//			imshow(windowName_InputImageLeft, inputImageLeft);
//			imshow(windowName_InputImageRight, inputImageRight);
			imshow(windowName_EdgeDetection, edgeDetectionImage);
			imshow(windowName_EdgeDetectionGrey, edgeDetectionImageGrey);
			imshow(windowName_contoursAndEllipses, contoursAndEllipse);
			start = false;
		}
		// If we need to quit, then quit
		if (!waitForNextFrame())
		{
			return false;
		}
	}
}

int detect_vehicles_colorFilter(vector<Vehicle_Information_S> * vehicles)
{
	if (!openCapture())
	{
		return false;
	}

	static Mat actualInputImage;
	static Mat inputImage;
	static Mat channel[3];
	static Mat simpleColorImage;
	static Mat edgeDetectionImageGrey;
	static Mat temp1;
	static Mat contoursAndEllipse;
	static bool start = true;

	setCSVDump(windowName_InputImage, "IDK.csv", &inputImage);
	while (1)
	{
		const int division = 32;

		if (updateImage || start)
		{
			capture.read(actualInputImage);
			actualInputImage.copyTo(inputImage);
			start = false;

			helper_cropImage(&inputImage, &inputImage);

			// HSV?
			cvtColor(inputImage, inputImage, CV_BGR2HSV);

		}
		if (programStart)
		{
			inputImage.copyTo(simpleColorImage);

			// Simple color reduction
			colorReduce(simpleColorImage, division);

			simpleColorImage.copyTo(edgeDetectionImageGrey);

			// Convert to greyscale
			cvtColor(edgeDetectionImageGrey, edgeDetectionImageGrey, CV_BGR2GRAY);

	#if 0
			Canny(edgeDetectionImageGrey, edgeDetectionImageGrey, division - 1, division + 1);
	#else
			edgeDetectionImageGrey.copyTo(contoursAndEllipse);
			drawBoundingContours(edgeDetectionImageGrey, contoursAndEllipse);
	#endif

			// Show Images
			imshow("Actual input image", actualInputImage);
			imshow(windowName_InputImage, inputImage);
			imshow(windowName_SimpleColors, simpleColorImage);
			imshow(windowName_EdgeDetectionGrey, edgeDetectionImageGrey);
			imshow(windowName_contoursAndEllipses, contoursAndEllipse);
			// If we need to quit, then quit
			if (!waitForNextFrame())
			{
				return false;
			}
		}
		else
		{
			// If we need to quit, then quit
			if (!waitForNextFrame())
			{
				return false;
			}
		}
	}
}
