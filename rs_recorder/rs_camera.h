#pragma once
#include <librealsense2/rs.hpp> 
#include <opencv2/opencv.hpp> 
#include <opencv2/video/video.hpp>



// =-=-=-=-=-=-=-=-=-
// =-=- SETTINGS =-=-
// =-=-=-=-=-=-=-=-=-



#define NUM_IMAGES 500

#define MAX_PACKET_SIZE false

#define TIMEOUT 2000

#define FILE_NAME "video.mp4"

#define FRAMES_PER_SECOND 25

int RecordSequenceDepth(int status, std::string file);