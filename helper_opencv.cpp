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

typedef std::vector<std::vector<cv::Point> > Cont;
typedef std::vector<cv::Vec4i> Hier;

void printExternalContours(cv::Mat img, Cont const& contours, Hier const& hierarchy, int const idx)
{
    //for every contour of the same hierarchy level
    for(unsigned int i = idx; i >= 0; i = hierarchy[i][0])
    {
        //print it
        cv::drawContours(img, contours, i, cv::Scalar(255));

        //for every of its internal contours
        for(int j = hierarchy[i][2]; j >= 0; j = hierarchy[j][0])
        {
            //recursively print the external contours of its children
            printExternalContours(img, contours, hierarchy, hierarchy[j][2]);
        }
    }
}

void helper_drawEllipseAroundVehicleContours(Mat * inputImage, Mat * outputImage, vector<lane_c> * lanes,
		                                     int minHeight, int minWidth, int maxHeight, int maxWidth,
											 vector<Vehicle_Information_S> * vehicles)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	/// Find contours
	findContours(*inputImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, Point(0, 0) );

	/// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect( contours.size() );
	vector<RotatedRect> minEllipse( contours.size() );

	//const int vehicleHeight =

	for(unsigned int i = 0; i < contours.size(); i++ )
	{

		minRect[i] = minAreaRect( Mat(contours[i]));
		if( contours[i].size() > 5)
		{
			minEllipse[i] = fitEllipse( Mat(contours[i]) );
		}
	}

	/// Draw contours + rotated rects + ellipses
	*outputImage = Mat::zeros( inputImage->size(), CV_8UC3 );
	for(unsigned int i = 0; i< contours.size(); i++ )
	{
		// Maximum a contour can be tilted to qualify as a vehicle must be less than 90
		const int maxTiltVehicleContour = 5;
		// Only use large bounding rectangles that are horizontal-ish
		if (minRect[i].size.height > minHeight && minRect[i].size.width > minWidth &&
			minRect[i].size.height < maxHeight && minRect[i].size.width < maxWidth &&
			(abs(helper_getRotatedRect180Deg(minRect[i]) - 90) < maxTiltVehicleContour))
		{
			Scalar green = Scalar(0, 255, 0);
			Scalar white = Scalar(255, 255, 255);
			Rect currentRect = minRect[i].boundingRect();
			// bounding contour
			drawContours( *outputImage , contours, i, white, 1, 8, vector<Vec4i>(), 0, Point() );
			//printExternalContours(*outputImage, contours, hierarchy, 0);
			// bounding ellipse
			ellipse( *outputImage , minEllipse[i], white, 2, 8 );

			// If this is a candidate for a vehicle (more than x number of bounding rectangles intersect, then color the circle green
			int intersections = 0;
			for(unsigned int rectIndex = 0; rectIndex < contours.size(); rectIndex++)
			{
				Rect comparedRect = minRect[rectIndex].boundingRect();
#if 0
				// If this rectangle intersects and is not the same rectangle as the rectangle in question
				if ((rectIndex != i) && ((minRect[rectIndex].boundingRect() & minRect[i].boundingRect()).area() > 0))
#endif
				// If the rectangle stacks up vertically with another rectangle (approximately) and is lower than the object
				if ((rectIndex != i) && (abs((comparedRect.x + comparedRect.width / 2)- (currentRect.x + currentRect.width / 2)) < 10) &&
										(abs(comparedRect.y - currentRect.y) < 50))
				{
					intersections++;
				}
			}
			const int intersectionsRequired = 2;
			if (intersections >= intersectionsRequired)
			{
				circle(*outputImage, minRect[i].center, 10, green, -1);

				// Check if the "vehicle" already exists vertically aligned, add/modify as necessary
				const int maxVehicleContourCenterOffsetHorizontal = 20;
				const int maxVehicleContourHeight = 50;
				bool addVehicle = true;
				for (unsigned int vehicleIdx = 0; vehicleIdx < vehicles->size(); vehicleIdx++)
				{
					// Vehicle aligns vertically with another vehicle and is within a reasonable vertical distance
					if ((abs((currentRect.x + currentRect.width / 2) - vehicles->at(vehicleIdx).actualPosX) < maxVehicleContourCenterOffsetHorizontal) &&
						(abs((currentRect.y + currentRect.height / 2) - vehicles->at(vehicleIdx).actualPosY) < maxVehicleContourHeight))
					{
						addVehicle = false;
						// Check if we need to modify this contour (take the lowest as it is the most reliable)
						if (currentRect.y > vehicles->at(vehicleIdx).actualPosY)
						{
							vehicles->at(vehicleIdx).actualPosX = currentRect.x + currentRect.width / 2;
							vehicles->at(vehicleIdx).actualPosY = currentRect.y + currentRect.height / 2;
						}
						break;
					}
				}
				// If we need to add this vehicle, add it here
				if (addVehicle)
				{
					Vehicle_Information_S newVehicle;
					newVehicle.actualPosX = currentRect.x + currentRect.width / 2;
					newVehicle.actualPosY = currentRect.y + currentRect.height / 2;

					// For now, set the extrapolated x/y position to the actual x/y position
					newVehicle.extrapolatedXPos = currentRect.x + currentRect.width / 2;
					newVehicle.extrapolatedYPos = currentRect.y + currentRect.height / 2;
					vehicles->push_back(newVehicle);
				}
			}

			// circle at the center
			//circle(*outputImage, minRect[i].center, 5, red, -1);
			// rotated rectangle
			Point2f rect_points[4]; minRect[i].points( rect_points );
			for( int j = 0; j < 4; j++ )
				line( *outputImage , rect_points[j], rect_points[(j+1)%4], white, 1, 8 );
		}
	}
	Scalar red = Scalar(0, 0, 255);
	// Point out all vehicles with red
	//printf("Size of vehicles array: %i\n", vehicles->size());
	for (unsigned int vehicleIdx = 0; vehicleIdx < vehicles->size(); vehicleIdx++)
	{
		Point2d vehiclePoint;
		vehiclePoint.x = vehicles->at(vehicleIdx).actualPosX;
		vehiclePoint.y = vehicles->at(vehicleIdx).actualPosY;
		circle(*outputImage, vehiclePoint, 15, red, -1);
		bool laneFound = false;
		for (unsigned int laneIdx = 0; laneIdx < lanes->size(); laneIdx++)
		{
			// If this is the lane the vehicle belongs to, assign the respective number
			if (lanes->at(laneIdx).in_lane(vehiclePoint))
			{
				vehicles->at(vehicleIdx).lane = lanes->at(laneIdx).get_number();
				laneFound = true;
				break;
			}
		}
		if (!laneFound)
		{
			vehicles->at(vehicleIdx).lane = -1;
		}
	}
}

float helper_getRotatedRect180Deg(RotatedRect rectangle)
{
	float angle = (rectangle.size.width < rectangle.size.height) ? rectangle.angle + 180 : rectangle.angle+90;
	return angle;
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

    for(unsigned int i = 0; i< contours.size(); i=hierarchy[i][0] ) 	// iterate through each contour.
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

bool mouseDrag = false;
bool mouseMove = false;
bool rectSelected = false;
Point initialPoint;
Point endPoint;
Rect rectFormed;
int vehicleNumber = 1;
double prevTimestamp = 0;

void recordVehicleLocation(string fileName, int x, int y, int vehicleNum, double timestamp)
{
	printf("we made it");
	if(!mouseMove && rectSelected)
	{
		printf("4\n");
		outputFile = fopen(fileName.c_str(), "a");
		if(outputFile == NULL)
		{
			perror("error opening file");
		}
		if(prevTimestamp == timestamp)
			vehicleNumber++;
		else
			vehicleNumber = 1;
		fprintf(outputFile, "Timestamp: %fms - Vehicle #%d (%d,%d) \n", timestamp, vehicleNumber, x, y);
		rectSelected = false;
		prevTimestamp = timestamp;
		fclose(outputFile);
	}

	if(mouseMove)
	{
		//rectangle()
	}
}

void clickAndDrag(int event, int x, int y, int flags, void* param)
{
	VideoCapture *frame = (VideoCapture*)param;

	if(event == CV_EVENT_LBUTTONDOWN && !mouseDrag)
	{
		initialPoint = Point(x,y);
		mouseDrag = true;
		printf("1\n");
	}
	if(event == CV_EVENT_MOUSEMOVE && mouseDrag)
	{
		endPoint = Point(x,y);
		mouseMove = true;
		printf("2\n");
	}
	if(event == CV_EVENT_LBUTTONUP && mouseDrag)
	{
		int xpos;
		int ypos;

		rectFormed = Rect(initialPoint, endPoint);
		xpos = rectFormed.x + rectFormed.width/2;
		ypos = rectFormed.y + rectFormed.height/2;

		mouseDrag = false;
		mouseMove = false;
		rectSelected = true;

		printf("3\n");
		recordVehicleLocation("/home/james/Documents/test.txt", xpos, ypos, vehicleNumber, frame->get(CV_CAP_PROP_POS_MSEC));
	}

}
