#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


static void trackbarCb(int, void *)
{
	;
}

int main( int argc, char** argv )
{
	VideoCapture capture;
	capture.open("testvid00.avi");

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
		static uint8_t sampleInterval = 3; ///< Every sampleInterval frames we will grab one frame


		if (updateImage || start)
		{
			capture.read(im);
			start = false;
			counter++;
			static Mat prev_grey_image;
			static Mat curr_grey_image;
			static Mat diff_grey_image;
			vector<vector<Point> > contours;
			if (!(counter % sampleInterval))
			{
				Mat contours_drawing;
				Mat threshold_diff;
				curr_grey_image.copyTo(prev_grey_image);
				cvtColor(im, curr_grey_image, CV_BGR2GRAY);
				if (prev_grey_image.size().height > 0)
				{
					validDiff = true;
					absdiff(prev_grey_image, curr_grey_image, diff_grey_image);


					vector<vector<Point> > contours;
					vector<Vec4i> hierarchy;

					diff_grey_image.copyTo(threshold_diff);
					threshold(threshold_diff, threshold_diff, 25, 200, THRESH_BINARY);
					threshold_diff.copyTo(contours_drawing);
					findContours(contours_drawing, contours, hierarchy,
						CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
					int idx = 0;
					for( ; idx >= 0; idx = hierarchy[idx][0] )
					{
						Scalar color( rand()&255, rand()&255, rand()&255 );
						drawContours( contours_drawing, contours, idx, color, CV_FILLED, 8, hierarchy );
					}

					if (!contours_drawing.empty())
					{
						imshow("Threshold diff", threshold_diff);
						imshow("Contours", contours_drawing);
					}

				}

			}

			// Show Image
			imshow("image", im );

			// And differences
			if (validDiff)
			{
				imshow("prev grey", prev_grey_image);
				imshow("curr grey", curr_grey_image);
				imshow("diff grey image", diff_grey_image);
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
