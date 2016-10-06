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

static int edgeDetectionGreyThreshold = 40;
static const int edgeDetectionGreyThresholdMax = 255;
static string trackbarEdgeDetectionThresholdName = "Edge Detection Grey Threshold Trackbar";

static int contourMinSquaredArea = 5;
static const int contourMinSquaredAreaMax = 40;
static string trackbarContourMinSquaredAreaName = "Contour Min Squared Area Trackbar";

/*** Window names ***/
static const string windowName_greyThreshold = "Grey Threshold";
static const string windowName_greyDiff = "Grey Diff";
static const string windowName_contours = "Contours";
static const string windowName_contoursAndEllipses = "Contours and Ellipses";
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

 	namedWindow(windowName_contoursAndEllipses);
 	moveWindow(windowName_contoursAndEllipses, 1000, 500);
	helper_trackbarSimple(trackbarContourMinSquaredAreaName, windowName_contoursAndEllipses, &contourMinSquaredArea, contourMinSquaredAreaMax);


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
	capture.open("testvid00.mp4");

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
						helper_drawEllipseAroundContours(&contours_drawing, &contoursAndEllipse, contourMinSquaredArea);
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
		static Mat inputImage;
		static Mat inputImageUp;
		static Mat inputImageDown;
		static Mat inputImageLeft;
		static Mat inputImageRight;
		static Mat edgeDetectionImage;
		static Mat edgeDetectionImageGrey;
		static Mat temp1;
		static Mat contoursAndEllipse;
		static bool start = true;
		static int edgeWidth = 4;

		if (updateImage || start)
		{
			capture.read(inputImage);
			helper_cropImage(&inputImage, &inputImage);
			start = false;

			// Zero out all Mats
			inputImageUp = Mat::zeros(inputImage.size(), inputImage.type());
			inputImageDown = Mat::zeros(inputImage.size(), inputImage.type());
			inputImageLeft = Mat::zeros(inputImage.size(), inputImage.type());
			inputImageRight = Mat::zeros(inputImage.size(), inputImage.type());
			edgeDetectionImage = Mat::zeros(inputImage.size(), inputImage.type());

			// Shift all Mats
			inputImage(cv::Rect(0,edgeWidth, inputImage.cols,inputImage.rows-edgeWidth)).copyTo(inputImageUp(cv::Rect(0,0,inputImage.cols,inputImage.rows-edgeWidth)));
			inputImage(cv::Rect(0,0, inputImage.cols,inputImage.rows-edgeWidth)).copyTo(inputImageDown(cv::Rect(0,edgeWidth,inputImage.cols,inputImage.rows-edgeWidth)));
			inputImage(cv::Rect(edgeWidth,0, inputImage.cols-edgeWidth,inputImage.rows)).copyTo(inputImageLeft(cv::Rect(0,0,inputImage.cols-edgeWidth,inputImage.rows)));
			inputImage(cv::Rect(0,0, inputImage.cols-edgeWidth,inputImage.rows)).copyTo(inputImageRight(cv::Rect(edgeWidth,0,inputImage.cols-edgeWidth,inputImage.rows)));

			// Take absolute diff of each input image
			absdiff(inputImage, inputImageUp, inputImageUp);
			absdiff(inputImage, inputImageDown, inputImageDown);
			absdiff(inputImage, inputImageLeft, inputImageLeft);
			absdiff(inputImage, inputImageRight, inputImageRight);

			// Add the diffs together
			add(edgeDetectionImage, inputImageUp, edgeDetectionImage);
			add(edgeDetectionImage, inputImageDown, edgeDetectionImage);
			add(edgeDetectionImage, inputImageLeft, edgeDetectionImage);
			add(edgeDetectionImage, inputImageRight, edgeDetectionImage);

//			inRange(edgeDetectionImage, Scalar(240, edgeDetectionGreyThreshold, edgeDetectionGreyThreshold), Scalar(255, 255, 255), edgeDetectionImageGrey);

			// Threshold from color diff, as it has more information
			threshold(edgeDetectionImage, edgeDetectionImageGrey, edgeDetectionGreyThreshold, edgeDetectionGreyThresholdMax, THRESH_BINARY);

			// Convert the diff into greyscale
			cvtColor(edgeDetectionImageGrey, edgeDetectionImageGrey, CV_BGR2GRAY);

			edgeDetectionImageGrey.copyTo(temp1);
			helper_drawEllipseAroundContours(&temp1, &contoursAndEllipse, contourMinSquaredArea);

			// Show Images
			imshow(windowName_InputImage, inputImage);
//			imshow(windowName_InputImageUp, inputImageUp);
//			imshow(windowName_InputImageDown, inputImageDown);
//			imshow(windowName_InputImageLeft, inputImageLeft);
//			imshow(windowName_InputImageRight, inputImageRight);
			imshow(windowName_EdgeDetection, edgeDetectionImage);
			imshow(windowName_EdgeDetectionGrey, edgeDetectionImageGrey);
			imshow(windowName_contoursAndEllipses, contoursAndEllipse);
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

	while (1)
	{
		static Mat actualInputImage;
		static Mat inputImage;
		static Mat channel[3];
		static Mat simpleColorImage;
		static Mat edgeDetectionImageGrey;
		static Mat temp1;
		static Mat contoursAndEllipse;
		static bool start = true;
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

			// Split into H,S and V
			split(simpleColorImage, channel);

			// Taking hue only
			channel[0] = Mat::zeros(simpleColorImage.rows, simpleColorImage.cols, CV_8UC1);//Set hue channel to 0
			channel[1] = Mat::zeros(simpleColorImage.rows, simpleColorImage.cols, CV_8UC1);//Set saturation channel to 0

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
