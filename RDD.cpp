#include "opencv2/opencv.hpp"
#include "helper_opencv.hpp"
#include "car_detection.hpp"

using namespace cv;
using namespace std;

#define DETECT_VEHICLES 1

int main( int argc, char** argv )
{
#if DETECT_VEHICLES
	vector<Vehicle_Information_S> vehicles;
	detect_initDebugInfo();
	return detect_vehicles_colorFilter(&vehicles);
#endif
}
