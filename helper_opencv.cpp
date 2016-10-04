#include "helper_opencv.hpp"

void helper_cropImage(Mat * inputImage, Mat * croppedImage)
{
	Rect rect;
	Point one, two;
	Rect box;

	one.x = 200;
	one.y = 400;
	two.x = 1100;
	two.y = 700;

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

void helper_drawEllipseAroundContours(Mat * inputImage, Mat * outputImage, int minSquaredArea)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	/// Find contours
	findContours(*inputImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, Point(0, 0) );

	/// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect( contours.size() );
	vector<RotatedRect> minEllipse( contours.size() );

	for( int i = 0; i < contours.size(); i++ )
	{

		minRect[i] = minAreaRect( Mat(contours[i]));
		if( contours[i].size() > 5 )
		{
			minEllipse[i] = fitEllipse( Mat(contours[i]) );
		}
	}

	/// Draw contours + rotated rects + ellipses
	*outputImage = Mat::zeros( inputImage->size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ )
	{
		// Only use large bounding rectangles
		if (minRect[i].size.height > minSquaredArea && minRect[i].size.width > minSquaredArea)
		{
			Scalar red = Scalar(0, 0, 255);
			Scalar green = Scalar(0, 255, 0);
			Scalar blue = Scalar(255, 0, 0);
			Scalar white = Scalar(255, 255, 255);
			// bounding contour
			drawContours( *outputImage , contours, i, white, 1, 8, vector<Vec4i>(), 0, Point() );
			// bounding ellipse
			ellipse( *outputImage , minEllipse[i], blue, 2, 8 );

			// If this is a candidate for a vehicle (more than x number of bounding rectangles intersect, then color the circle green
			int intersections = 0;
			for(int rectIndex = 0; rectIndex < contours.size(); rectIndex++)
			{
				// If this rectangle intersects and is not the same rectangle as the rectangle in question
				if ((rectIndex != i) && ((minRect[rectIndex].boundingRect() & minRect[i].boundingRect()).area() > 0))
				{
					intersections++;
				}
			}
			const int intersectionsRequired = 3;
			if (intersections >= intersectionsRequired)
			{
				circle(*outputImage, minRect[i].center, 10, green, -1);
			}

			// circle at the center
			circle(*outputImage, minRect[i].center, 5, red, -1);
			// rotated rectangle
			Point2f rect_points[4]; minRect[i].points( rect_points );
			for( int j = 0; j < 4; j++ )
				line( *outputImage , rect_points[j], rect_points[(j+1)%4], blue, 1, 8 );
		}
	}
}

void colorReduce(Mat& image, int div)
{
    int nl = image.rows;                    // number of lines
    int nc = image.cols * image.channels(); // number of elements per line

    for (int j = 0; j < nl; j++)
    {
        // get the address of row j
        uchar* data = image.ptr<uchar>(j);

        for (int i = 0; i < nc; i++)
        {
            // process if it's nonzero (ignore black)
        	if (data[i] > 0)
        	{
                data[i] = data[i] / div * div + div / 2;
        	}
        }
    }
}

// Will operate on
void drawBoundingContours(Mat& input, Mat& output)
{
	const int rectMinWidth = 1;
	const int maxWidth = 300;
	const int minWidth = 30;
	const int minHeight = 20;
	const float flatness = 1.5;
	const float verticalness = 1.5;
    input.copyTo(output);

    //GaussianBlur(input, output, Size(15, 15), 0);
    adaptiveThreshold(output, output,255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,15,-5);

    vector< vector <Point> > contours; // Vector for storing contour
    vector< Vec4i > hierarchy;
    findContours(output, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    for( int i = 0; i< contours.size(); i=hierarchy[i][0] ) // iterate through each contour.
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
