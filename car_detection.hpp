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
int detect_vehicles(vector<Vehicle_Information_S> * vehicles);

#endif /* CAR_DETECTION_HPP_ */