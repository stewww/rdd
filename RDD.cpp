#include "opencv2/opencv.hpp"
#include "helper_opencv.hpp"
#include "car_detection.hpp"

using namespace cv;
using namespace std;

#define DETECT_VEHICLES 1
#define MAX_VEHICLES	20

std::vector<Vehicle_Information_S> vehicles(MAX_VEHICLES);

int main( int argc, char** argv )
{
#if DETECT_VEHICLES
	vector<Vehicle_Information_S> vehicles;
	detect_initDebugInfo();
	return detect_vehicles_edgeDetection(&vehicles);
#endif
}
