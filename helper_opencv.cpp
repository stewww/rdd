#include "helper_opencv.hpp"
#include <stdlib.h>

static FILE * outputFile;

void helper_cropImage(Mat * inputImage, Mat * croppedImage)
{
	Rect rect;
	Point one, two;
	Rect box;

	one.x = 300;
	one.y = 275;
	two.x = 1100;
	two.y = 500;

	box.width = abs(one.x - two.x);
	box.height = abs(one.y - two.y);
	box.x = min(one.x, two.x);
	box.y = min(one.y, two.y);

	*croppedImage = (*inputImage)(box);
}

void helper_trackbarSimple(const string trackbarName, const string windowName, int * variable, const int maxValue)
{
	createTrackbar(trackbarName.c_str(), windowName.c_str(), variable, maxValue);
}

void helper_drawEllipseAroundContours(Mat * inputImage, Mat * outputImage, int minHeight, int minWidth, int maxHeight, int maxWidth,vector<Point> * pointsOfInterest)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	/// Find contours
	findContours(*inputImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, Point(0, 0) );

	/// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect( contours.size() );
	vector<RotatedRect> minEllipse( contours.size() );

	vector<bool> alreadyTracked(contours.size());
	fill(alreadyTracked.begin(), alreadyTracked.end(), false);

	for( int i = 0; i < contours.size(); i++ )
	{
		if( contours[i].size() > 5)
		{
			minRect[i] = minAreaRect( Mat(contours[i]));
			minEllipse[i] = fitEllipse( Mat(contours[i]) );
		}
	}

	/// Draw contours + rotated rects + ellipses
	*outputImage = Mat::zeros( inputImage->size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ )
	{
		// Only use large bounding rectangles
		if (minRect[i].size.height > minHeight && minRect[i].size.width > minWidth &&
			minRect[i].size.height < maxHeight && minRect[i].size.width < maxWidth)
		{
			Scalar red = Scalar(0, 0, 255);
			Scalar green = Scalar(0, 255, 0);
			Scalar blue = Scalar(255, 0, 0);
			Scalar white = Scalar(255, 255, 255);
			// bounding contour
			drawContours( *outputImage , contours, i, white, 1, 8, vector<Vec4i>(), 0, Point() );
			// bounding ellipse
			//ellipse( *outputImage , minEllipse[i], white, 2, 8 );

			// If this is a candidate for a vehicle (more than x number of bounding rectangles intersect, then color the circle green
			int intersections = 0;
			bool newVehicle = true;
			for(int rectIndex = 0; rectIndex < contours.size(); rectIndex++)
			{
#if 0
				// If this rectangle intersects and is not the same rectangle as the rectangle in question
				if ((rectIndex != i) && ((minRect[rectIndex].boundingRect() & minRect[i].boundingRect()).area() > 0))
#endif
				// If the rectangle stacks up vertically with another rectangle (approximately)
				if ((rectIndex != i) && (abs((minRect[rectIndex].boundingRect().x + minRect[rectIndex].boundingRect().width / 2) -
						                     (minRect[i].boundingRect().x + minRect[i].boundingRect().width / 2)) < 20) &&
										(abs(minRect[rectIndex].boundingRect().y - minRect[i].boundingRect().y) < 50))
				{
					// Vehicle is already being tracked, don't track the contour
					if (alreadyTracked[rectIndex])
					{
						newVehicle = false;
					}
					intersections++;
				}
			}
			const int intersectionsRequired = 3;
			if (intersections >= intersectionsRequired && (newVehicle == true))
			{
				alreadyTracked[i] = true;
				//circle(*outputImage, minRect[i].center, 10, green, -1);
				Point newCircle = (minRect[i].center);
				pointsOfInterest->push_back(newCircle);
			}

			// circle at the center
			//circle(*outputImage, minRect[i].center, 5, red, -1);
			// rotated rectangle
			Point2f rect_points[4]; minRect[i].points( rect_points );
			for( int j = 0; j < 4; j++ )
				;//line( *outputImage , rect_points[j], rect_points[(j+1)%4], white, 1, 8 );
		}
	}
}

void colorReduce(Mat& image, int div)
{
#define FIRSTMETHOD 0
    int nl = image.rows;                    // number of lines
    int nc = image.cols * image.channels(); // number of elements per line
#if FIRSTMETHOD
    double hueConstSquared = -7.995004;
    double hueConst = 167.322176;
    double satConstSquared = -0.003410;
    double satConst = 1.419707;
    double valConstSquared = 0.007036;
    double valConst = -0.588139;
    double hsvStdev = 14.302897;
    double hsvConst = 1000;
#else
    double meanH = 24.315079;
    double stdevH = 5.106813;
    double meanS = 42.879132;
    double stdevS = 12.956450;
    double meanV = 65.678858;
    double stdevV = 9.244539;
#endif
    double zscoreMax = 4;

    for (int j = 0; j < nl; j++)
    {
        // get the address of row j
        uchar* data = image.ptr<uchar>(j);

        for (int i = 0; i < nc; i+=3)
        {
        	// If the calculated zscore of the pixel is within reasonable standard deviations, then accept it as the road
#if FIRSTMETHOD
			if (abs(hueConstSquared * data[i + 0] * data[i + 0] + hueConst * data[i + 0] +
					satConstSquared * data[i + 1] * data[i + 1] + satConst * data[i + 1] +
					valConstSquared * data[i + 2] * data[i + 2] + valConst * data[i + 2]
					- hsvConst) < hsvStdev * zscoreMax)
#else
			if ((abs(meanH - data[i + 0]) < zscoreMax * stdevH) &&
				(abs(meanS - data[i + 1]) < zscoreMax * stdevS) &&
				(abs(meanV - data[i + 2]) < zscoreMax * stdevV))
#endif
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

// Will operate on
void drawBoundingContours(Mat& input, Mat& output)
{
	const int rectMinWidth = 1;
	const int maxWidth = 400;
	const int minWidth = 10;
	const int minHeight = 10;
	const float flatness = 1;
	const float verticalness = 1;
    input.copyTo(output);

    //GaussianBlur(input, output, Size(15, 15), 0);
    adaptiveThreshold(output, output,255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,15,-5);

    vector< vector <Point> > contours; // Vector for storing contour
    vector< Vec4i > hierarchy;
    findContours(output, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    for( int i = 0; i< contours.size(); i=hierarchy[i][0] ) 	// iterate through each contour.
    {
        Rect r= boundingRect(contours[i]);
        if ((r.width < maxWidth) &&								// Must be smaller than a large portion of the screen
        	(r.width > minWidth || r.height > minHeight) &&		// Must be larger than a small blip
			((r.width > r.height * flatness) || 				// Horizontal contours
        	(r.height > r.width * verticalness)))				// Vertical contours (motorcycles)
        {
            if(hierarchy[i][2]<0) //Check if there is a child contour
              rectangle(output,Point(r.x-rectMinWidth,r.y-rectMinWidth), Point(r.x+r.width+rectMinWidth,r.y+r.height+rectMinWidth), Scalar(255,255,255),2,8,0); //Opened contour
            else
              rectangle(output,Point(r.x-rectMinWidth,r.y-rectMinWidth), Point(r.x+r.width+rectMinWidth,r.y+r.height+rectMinWidth), Scalar(255,255,255),2,8,0); //closed contour
        }
    }
}

static void csvDumpCallbackFunc(int event, int x, int y, int flags, void* userdata)
{
#if 0
    Mat* rgb = (Mat*) userdata;
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        printf("%d %d: %d, %d, %d\n",
        x, y,
        (int)(*rgb).at<Vec3b>(y, x)[0],
        (int)(*rgb).at<Vec3b>(y, x)[1],
        (int)(*rgb).at<Vec3b>(y, x)[2]);
    }
#else
    Mat* rgb = (Mat*) userdata;
	// If the mouse is pressed down and the cursor was moved or pressed, then we grab the pixel channel values
	if ((EVENT_FLAG_LBUTTON == flags) &&
		(EVENT_MOUSEMOVE == event ||
		 EVENT_LBUTTONDOWN == event))
	{
        fprintf(outputFile, "%d, %d, %d\n",
        (int)(*rgb).at<Vec3b>(y, x)[0],
        (int)(*rgb).at<Vec3b>(y, x)[1],
        (int)(*rgb).at<Vec3b>(y, x)[2]);
	}
#endif
}

void setCSVDump(string windowName, string fileName, Mat * matrix)
{
	outputFile = fopen(fileName.c_str(), "w");
    //set the callback function for any mouse event to the file name
   setMouseCallback(windowName, csvDumpCallbackFunc, matrix);
}
