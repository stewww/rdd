#include "opencv2/opencv.hpp"
#include "helper_opencv.hpp"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
	VideoCapture capture;
	capture.open("testvid00.avi");

	if (!capture.isOpened())
	{
		printf("uh oh?\n");
		return false;
	}

	static int greyThreshold = 40;
	const int greyThresholdMax = 200;
	string trackbarThresholdName = "Grey Threshold Trackbar";
	string windowName_GreyThreshold = "Grey Threshold";

	static int greyDiffInterval = 9;
	const int greyDiffMax = 50;
	string trackbarGreyDiffName = "Grey Diff Trackbar";
	string windowName_GreyDiff = "Grey Diff";

	namedWindow(windowName_GreyThreshold);
	moveWindow(windowName_GreyThreshold, 100, 100);
	helper_trackbarSimple(trackbarThresholdName, windowName_GreyThreshold, &greyThreshold, greyThresholdMax);

	namedWindow(windowName_GreyDiff);
	moveWindow(windowName_GreyDiff, 100, 100);
	helper_trackbarSimple(trackbarGreyDiffName, windowName_GreyDiff, &greyDiffInterval, greyDiffMax);


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
						imshow(windowName_GreyThreshold, threshold_diff);
						threshold_diff.copyTo(contours_drawing);
						helper_drawEllipseAroundContours(&contours_drawing, &contoursAndEllipse);
						imshow("Contours", contours_drawing);
						moveWindow("Contours", 100, 100);
						imshow("Contours and Ellipses", contoursAndEllipse);
						moveWindow("Contours and Ellipses", 1000, 500);
					}
				}
				curr_grey_image.copyTo(prev_grey_image);

			}

			// Show Images
			imshow("Input Image", inputImage);
			imshow("Cropped Image", croppedImage);
			moveWindow("Cropped Image", 0, 500);

			// And differences
			if (validDiff)
			{
				imshow("prev grey", prev_grey_image);
				imshow("curr grey", curr_grey_image);
				imshow(windowName_GreyDiff, diff_grey_image);
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
