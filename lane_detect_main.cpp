#include "lane_marker_detect.hpp"

std::vector<lane_c> LANE_;

int main(int argc, char** argv)
{
	lane_marker_detect LD;
	cv::VideoCapture cam = cv::VideoCapture("../videos/ir_testvid01.mp4");
	cv::Mat video_frame;
	LD.set_up();
	while(1){
		cam >> video_frame;
		LD.detect(&video_frame, &LANE_);
		if (cv::waitKey(30) >= 0)
			break;
	}
	//video stream >> frame;
	//frame >> lane detection(lane info);
	//frame, lane info >> vehicle detection;
	//draw on the frame;

	return 0;
}
