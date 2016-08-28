#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

const char * trackbars = "Config Trackbars";
const char * minThresholdStr = "Min Threshold";
const char * maxThresholdStr = "Max Threshold";
const char * minAreaStr = "Min Area";
const char * maxAreaStr = "Max Area";
const char * minCircularityStr = "Min Circularity";
const char * maxCircularityStr = "Max Circularity";
const char * minConvexityStr = "Min Convexity";
const char * maxConvexityStr = "Max Convexity";
const char * minInertiaStr = "Min Inertia";
const char * maxInertiaStr = "Max Inertia";

/*** Raw Values ***/
int minThresholdValue = 20;
int maxThresholdValue = 200;
int minAreaValue = 400;
int maxAreaValue = 3000;

/*** Percentage ***/
int minCircularityValue = 50;
int maxCircularityValue = 70;
int minConvexityValue = 60;
int maxConvexityValue = 90;
int minInertiaValue = 30;
int maxInertiaValue = 80;

static int minThresholdMax = 500;
static int maxThresholdMax = 500;
static int minAreaMax = 3000;
static int maxAreaMax = 3000;
static int minCircularityMax = 100;
static int maxCircularityMax = 100;
static int minConvexityMax = 100;
static int maxConvexityMax = 100;
static int minInertiaMax = 100;
static int maxInertiaMax = 100;

static void trackbarCb(int, void *)
{
	;
}

int main( int argc, char** argv )
{
	VideoCapture capture;
	capture.open("testvid00.avi");


	namedWindow(trackbars, WINDOW_NORMAL);
	createTrackbar(minThresholdStr, trackbars, &minThresholdValue, minThresholdMax, trackbarCb);
	createTrackbar(maxThresholdStr, trackbars, &maxThresholdValue, maxThresholdMax, trackbarCb);
	createTrackbar(minAreaStr, trackbars, &minAreaValue, minAreaMax, trackbarCb);
	createTrackbar(maxAreaStr, trackbars, &maxAreaValue, maxAreaMax, trackbarCb);
	createTrackbar(minCircularityStr, trackbars, &minCircularityValue, minCircularityMax, trackbarCb);
	createTrackbar(maxCircularityStr, trackbars, &maxCircularityValue, maxCircularityMax, trackbarCb);
	createTrackbar(minConvexityStr, trackbars, &minConvexityValue, minConvexityMax, trackbarCb);
	createTrackbar(maxConvexityStr, trackbars, &maxConvexityValue, maxConvexityMax, trackbarCb);
	createTrackbar(minInertiaStr, trackbars, &minInertiaValue, minInertiaMax, trackbarCb);
	createTrackbar(maxInertiaStr, trackbars, &maxInertiaValue, maxInertiaMax, trackbarCb);


	if (!capture.isOpened())
	{
		printf("uh oh?\n");
		return false;
	}
	while (1)
	{
		static Mat im;
		static bool updateImage = false;
		static bool start = true;
		bool validDiff = false;
		static uint32_t counter = 0;
		static uint8_t sampleInterval = 5; ///< Every sampleInterval frames we will grab one frame


		if (updateImage || start)
		{
			capture.read(im);
			start = false;
			counter++;
		}
		static Mat prev_grey_image;
		static Mat curr_grey_image;
		static Mat diff_grey_image;
		if (!(counter % sampleInterval))
		{
			curr_grey_image.copyTo(prev_grey_image);
			cvtColor(im, curr_grey_image, CV_BGR2GRAY);
			if (prev_grey_image.size().height > 0)
			{
				validDiff = true;
				absdiff(prev_grey_image, curr_grey_image, diff_grey_image);
			}
		}

		SimpleBlobDetector::Params params;
		// Change thresholds
		params.minThreshold = minThresholdValue;
		params.maxThreshold = maxThresholdValue;

		// Filter by Area.
		params.filterByArea = true;
		params.minArea = minAreaValue;
		params.maxArea = maxAreaValue;

		// Filter by Circularity
		params.filterByCircularity = true;
		params.minCircularity = (float)minCircularityValue / 100.0F;
		params.maxCircularity = (float)maxCircularityValue / 100.0F;

		// Filter by Convexity
		params.filterByConvexity = false;
		params.minConvexity = (float)minConvexityValue / 100.0F;
		params.maxConvexity = (float)maxConvexityValue / 100.0F;

		// Filter by Inertia
		params.filterByInertia = true;
		params.minInertiaRatio = (float)minInertiaValue / 100.0F;
		params.maxInertiaRatio = (float)maxInertiaValue / 100.0F;

		// Set up the detector with default parameters.
		Ptr<SimpleBlobDetector> d = SimpleBlobDetector::create(params);



		// Detect blobs.
		std::vector<KeyPoint> keypoints;
		d->detect( im, keypoints);

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
		Mat im_with_keypoints;
		drawKeypoints( im, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

		// Show blobs
		imshow("keypoints", im_with_keypoints );
		// And differences
		if (validDiff)
		{
			imshow("prev grey", prev_grey_image);
			imshow("curr grey", curr_grey_image);
			imshow("diff grey image", diff_grey_image);
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
