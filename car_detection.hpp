#ifndef CAR_DETECTION_HPP_
#define CAR_DETECTION_HPP_

#include <vector>
using namespace std;

typedef struct {
	int xPos;
	int yPos;
	// Enum for lane
	// Other RGB parameters?
} Vehicle_Information_S ;

void detect_initDebugInfo(void);
int detect_vehicles_greydiff(vector<Vehicle_Information_S> * vehicles);
int detect_vehicles_edgeDetection(vector<Vehicle_Information_S> * vehicles);
int detect_vehicles_colorFilter(vector<Vehicle_Information_S> * vehicles);

#endif /* CAR_DETECTION_HPP_ */
