#ifndef HELPER_OPENCV_HPP_
#define HELPER_OPENCV_HPP_

#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;


typedef enum {
	VideoCapture_Webcam,
	VideoCapture_Video,
} VideoCapture_E ;

typedef struct {
	// idkDataType totalCPUCycles
	// idkDataType startCPUCycles
	int temp;
} idkDataType_S ;

typedef enum {
	Window_Camera,
	// All the other window displays should be here
} Window_List_E ;

typedef struct {
	Window_List_E windowEnum;
	string windowName;
	Mat * matrix;
	// All other relevant info for each display
} Display_Info_S ;


// Doxygen Comments TODO:
// Note, min value is defaulted to 0
void helper_trackbarSimple(const string trackbarName, const string windowName, int * variable, const int maxValue);

// Returns frame rate of this display, can easily be called with displayInfo.at(index)
uint helper_displayWindow(Display_Info_S displayInfo);

void helper_seekFrameByCount(VideoCapture * capturePtr, uint frameCount);

void helper_seekFrameByMs(VideoCapture * capturePtr, double ms);

// If the capture type is video, videoName can be omitted (hence defaulted to NULL)
void helper_getVideoCapture(VideoCapture * capturePtr, VideoCapture_E captureType, string videoName = NULL);

// Call this before a section where you want to track the total CPU usage of
void helper_startCounter(idkDataType_S * counterStructPtr);
// Call this after the section where you want to track the total CPU usage of
void helper_stopCounter(idkDataType_S * counterStructPtr);

//crops main image
void helper_cropImage(Mat * inputImage, Mat * croppedImage);

// creates ellipses around contours
void helper_drawEllipseAroundContours(Mat * inputImage, Mat * outputImage, int minSquaredArea);

#endif /* HELPER_OPENCV_HPP_ */
