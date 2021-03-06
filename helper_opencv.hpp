#ifndef HELPER_OPENCV_HPP_
#define HELPER_OPENCV_HPP_

#include "opencv2/opencv.hpp"
#include "lane_marker_detect.hpp"
#include "car_detection.hpp"
using namespace cv;
using namespace std;


typedef enum {
	VideoCapture_Webcam,
	VideoCapture_Video,
} VideoCapture_E ;

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

// Note, min value is defaulted to 0
void helper_trackbarSimple(const string trackbarName, const string windowName, int * variable, const int maxValue);

// Returns frame rate of this display, can easily be called with displayInfo.at(index)
uint helper_displayWindow(Display_Info_S displayInfo);

void helper_seekFrameByCount(VideoCapture * capturePtr, uint frameCount);

void helper_seekFrameByMs(VideoCapture * capturePtr, double ms);

// If the capture type is video, videoName can be omitted (hence defaulted to NULL)
void helper_getVideoCapture(VideoCapture * capturePtr, VideoCapture_E captureType, string videoName = NULL);

//crops main image
void helper_cropImage(Mat * inputImage, Mat * croppedImage);

// creates ellipses around contours and populates the vehicle objects
void helper_drawEllipseAroundVehicleContours(Mat * inputImage, Mat * outputImage, vector<lane_c> * lanes,
		                                     int minHeight, int minWidth, int maxHeight, int maxWidth,
											 vector<Vehicle_Information_S> * vehicles);

/* 0 for vertical, 90 for horizontal, 45 tilted left /, 135 = tilted right \ */
float helper_getRotatedRect180Deg(RotatedRect rectangle);

// Reduces the color gradient of the image
void colorReduce(Mat& image, int div=64);

// Draws bounding rectangles around all related contours
void drawBoundingContours(Mat& input, Mat& output);

// With the specified window, sets up the CSV dump interrupt to dump to the file specified. This should be a #define
void setCSVDump(string windowName, string fileName, Mat * matrix);

//testing module
void clickAndDrag(int event, int x, int y, int flags, void* param);
void recordVehicleLocation(string fileName, int x, int y, int vehicleNum, double timestamp);

#endif /* HELPER_OPENCV_HPP_ */
