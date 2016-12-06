#ifndef CAR_DETECTION_HPP_
#define CAR_DETECTION_HPP_

#include <vector>
using namespace std;

typedef struct {
	int lane;			///< Lane number incrementing left to right (-1 for unknown)
	int actualPosX;		///< Actual x position of the vehicle
	int actualPosY; 	///< Actual y position of the vehicle
	int extrapolatedXPos;	///< Estimated x position of the vehicle's center from top-down
	int extrapolatedYPos;	///< Estimated x position of the vehicle's center from top-down
} Vehicle_Information_S ;

void detect_initDebugInfo(void);
#if 0
int detect_vehicles_greydiff(vector<Vehicle_Information_S> * vehicles);
#endif // 0
int detect_vehicles_edgeDetection(vector<Vehicle_Information_S> * vehicles);
#if 0
int detect_vehicles_colorFilter(vector<Vehicle_Information_S> * vehicles);
#endif // 0

#endif /* CAR_DETECTION_HPP_ */
