#include "car_detection.hpp"
#include "opencv2/opencv.hpp"
#include "helper_opencv.hpp"

/*** Trackbar variables ***/
static int greyThreshold = 40;
static const int greyThresholdMax = 200;
static string trackbarThresholdName = "Grey Threshold Trackbar";

static int greyDiffInterval = 9;
static const int greyDiffMax = 50;
static const string trackbarGreyDiffName = "Grey Diff Trackbar";

/*** Window names ***/
static const string windowName_greyThreshold = "Grey Threshold";
static const string windowName_greyDiff = "Grey Diff";
static const string windowName_contours = "Contours";
static const string windowName_contoursAndEllipses = "Contours and Ellipses";
static const string windowName_InputImage = "Input Image";
static const string windowName_CroppedImage = "Cropped Image";
static const string windowName_prevGrey = "Prev Grey Image";
static const string windowName_currGrey = "Current Grey Image";

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

 	namedWindow(windowName_InputImage);
 	moveWindow(windowName_InputImage, 100, 100);

 	namedWindow(windowName_CroppedImage);
 	moveWindow(windowName_CroppedImage, 100, 500);

 	namedWindow(windowName_prevGrey);
 	moveWindow(windowName_prevGrey, 100, 100);

 	namedWindow(windowName_currGrey);
 	moveWindow(windowName_currGrey, 100, 100);
}

int detect_vehicles(vector<Vehicle_Information_S> * vehicles)
{
	VideoCapture capture;
	capture.open("testvid00.mp4");

	if (!capture.isOpened())
	{
		printf("uh oh?\n");
		return -1;
	}

	while (1)
	{
		static Mat inputImage;
		static Mat croppedImage;
		static bool updateImage = false;
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
						helper_drawEllipseAroundContours(&contours_drawing, &contoursAndEllipse);
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

		char key = waitKey(1);
		if (key  == ' ')
		{
			updateImage = !updateImage;
			if (updateImage)
			{
				printf("Running video\n");
			}
			else
			{
				printf("Pausing video\n");
			}
		}
		else if (key == 'q')
		{
			return 0;
		}
	}


}
