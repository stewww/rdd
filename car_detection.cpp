#include "car_detection.hpp"
#include "opencv2/opencv.hpp"
#include "helper_opencv.hpp"
#include "lane_marker_detect.hpp"


#define DEBUG_INFO_ENABLED	0

/*** Trackbar variables ***/


static int edgeDetectionGreyThreshold = 48;
static const int edgeDetectionGreyThresholdMax = 255;


#if DEBUG_INFO_ENABLED
static int greyThreshold = 40;
static const int greyThresholdMax = 255;
static string trackbarThresholdName = "Grey Threshold Trackbar";

static int greyDiffInterval = 9;
static const int greyDiffMax = 50;
static const string trackbarGreyDiffName = "Grey Diff Trackbar";

static string trackbarEdgeDetectionThresholdName = "Edge Detection Grey Threshold Trackbar";

static int inputImageVehicleCount = 1;
static const int inputImageVehicleCountMax = 10;
static const string trackbarInputImageVehicleCountName = "Vehicle Number Trackbar";

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
#endif // DEBUG_INFO_ENABLED

void detect_initDebugInfo(void)
{
#if DEBUG_INFO_ENABLED
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
 	helper_trackbarSimple(trackbarInputImageVehicleCountName, windowName_InputImage, &inputImageVehicleCount, inputImageVehicleCountMax);

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
#endif // DEBUG_INFO_ENABLED
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

#if 0
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
						helper_drawEllipseAroundVehicleContours(&contours_drawing, &contoursAndEllipse, 10,15,300,400);
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
#endif // 0

#define MAX_LANES		6
std::vector<lane_c> LANE_(MAX_LANES);

int detect_vehicles_edgeDetection(vector<Vehicle_Information_S> * vehicles)
{
	if (!openCapture())
	{
		return false;
	}

	lane_marker_detect LD; //new
	LD.set_up(); //new
#if DEBUG_INFO_ENABLED
	setMouseCallback(windowName_InputImage, clickAndDrag, &capture);
#endif // DEBUG_INFO_ENABLED

	while (1)
	{
		static Mat inputImage;
		static Mat inputImageUp;
		static Mat inputImageDown;
		static Mat inputImageLeft;
		static Mat inputImageRight;
		static Mat edgeDetectionImage;
		static Mat edgeDetectionImageGrey;
		static Mat simpleColorImage;
		static Mat temp1;
		static Mat vehicleContoursAndEllipses;
		static Mat laneTracking;
		static bool start = true;
		static int edgeWidth = 1;

		if (updateImage || start)
		{

			const int division = 32;
			capture.read(inputImage);

			// Clear all vehicle objects
			vehicles->clear();

			helper_cropImage(&inputImage, &inputImage);
			laneTracking = LD.detect(&inputImage, &LANE_); //new
			//cvtColor(inputImage, inputImage, CV_BGR2HSV);

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
			//simpleColorImage(cv::Rect(0,0, simpleColocapturerImage.cols,simpleColorImage.rows-edgeWidth)).copyTo(inputImageDown(cv::Rect(0,edgeWidth,simpleColorImage.cols,simpleColorImage.rows-edgeWidth)));
			//simpleColorImage(cv::Rect(edgeWidth,0, simpleColorImage.cols-edgeWidth,simpleColorImage.rows)).copyTo(inputImageLeft(cv::Rect(0,0,simpleColorImage.cols-edgeWidth,simpleColorImage.rows)));
			//simpleColorImage(cv::Rect(0,0, simpleColorImage.cols-edgeWidth,simpleColorImage.rows)).copyTo(inputImageRight(cv::Rect(edgeWidth,0,simpleColorImage.cols-edgeWidth,simpleColorImage.rows)));

			// Take absolute diff of each input image
			absdiff(simpleColorImage, inputImageUp, inputImageUp);
			//absdiff(simpleColorImage, inputImageDown, inputImageDown);
			//absdiff(simpleColorImage, inputImageLeft, inputImageLeft);
			//absdiff(simpleColorImage, inputImageRight, inputImageRight);

			double filterSize = 4;
			// Add the diffs together (average for now)
			Scalar numerator = Scalar(filterSize - 1, (filterSize - 1) + 0.5, filterSize - 1);
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
			helper_drawEllipseAroundVehicleContours(&temp1, &vehicleContoursAndEllipses, &LANE_, 1,10,300,400, vehicles);
			//drawBoundingContours(temp1, contoursAndEllipse);
			// Show Images
			//imshow(windowName_InputImage, simpleColorImage);
//			imshow(windowName_InputImageUp, inputImageUp);
//			imshow(windowName_InputImageDown, inputImageDown);
//			imshow(windowName_InputImageLeft, inputImageLeft);
//			imshow(windowName_InputImageRight, inputImageRight);
			//imshow(windowName_EdgeDetection, edgeDetectionImage);
			//imshow(windowName_EdgeDetectionGrey, edgeDetectionImageGrey);
			//imshow(windowName_contoursAndEllipses, contoursAndEllipse);
			Mat D;
			add(inputImage, laneTracking, D);
			add(D, vehicleContoursAndEllipses, D);
			for (unsigned int x = 0; x < vehicles->size(); x++)
			{
				Vehicle_Information_S vehicle = vehicles->at(x);
				if (vehicle.lane != -1)
				{
					printf("Time: %f, PosX: %i, PosY: %i, Lane: %i\n", capture.get(CV_CAP_PROP_POS_MSEC),
							vehicle.extrapolatedXPos, vehicle.extrapolatedYPos, vehicle.lane);
				}
			}
			imshow("LOOKATME", D);
			start = false;
		}
		// If we need to quit, then quit
		if (!waitForNextFrame())
		{
			return false;
		}
	}
}

#if 0
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
#endif // 0
