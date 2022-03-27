#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
# include <pylon/PylonGUI.h>
#endif



// =-=-=-=-=-=-=-=-=-
// =-=- SETTINGS =-=-
// =-=-=-=-=-=-=-=-=-



#define NUM_IMAGES 500

#define MAX_PACKET_SIZE false

#define TIMEOUT 2000

#define FRAMES_PER_SECOND 25

int RecordSequenceRGB(int status, std::string file);