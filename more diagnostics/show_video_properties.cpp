/*
 * show_video_properties.cpp
 *
 *  Created on: Dec 1, 2016
 *      Author: jonathan
 */

#include "show_video_properties.hpp"

void show_video_properties(VideoCapture& capture, Mat& frame) {
        stringstream ss_frames;
        stringstream ss_fps;
        stringstream ss_msec;
        stringstream ss_avi_ratio;
        rectangle(frame, cv::Point(5, 142), cv::Point(250,240),
                  cv::Scalar(255,255,255), -1);
        // Show current frame in the video.
        ss_frames << "Frame: ";
        ss_frames << capture.get(CAP_PROP_POS_FRAMES);
        string frameNumberString = ss_frames.str();
        putText(frame, frameNumberString.c_str(), cv::Point(5, 155),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //Show video's original fps.
        ss_fps << "Original fps: ";
        ss_fps << capture.get(CAP_PROP_FPS);
        string fpsString = ss_fps.str();
        putText(frame, fpsString.c_str(), cv::Point(5, 185),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        // Show current time in the video.
        double video_time_position = capture.get(CAP_PROP_POS_MSEC);
        video_time_position /= 1000;
        cv::putText(frame, cv::format("Video time=%.2fs", video_time_position ), cv::Point(5, 200),
        		cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,0));
        // Show % position into the video (from 0 - 100%).
        double video_percent_position = capture.get(CAP_PROP_POS_FRAMES) / capture.get(CV_CAP_PROP_FRAME_COUNT) * 100;
        cv::putText(frame, cv::format("Video progress=%.2f%%", video_percent_position ), cv::Point(5, 215),
        		cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,0));

}

