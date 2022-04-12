#pragma once
#include "ArenaApi.h"
#include "SaveApi.h"

#include <string>
#define TAB1 "  "
#define TAB2 "    "
#define ERASE_LINE "\t\t\t\t\t"


// =-=-=-=-=-=-=-=-=-
// =-=- SETTINGS =-=-
// =-=-=-=-=-=-=-=-=-

#define WIDTH 100
#define HEIGHT 100

#define PIXEL_FORMAT "Mono8"

#define NUM_IMAGES 500

#define MAX_PACKET_SIZE false

#define TIMEOUT 2000

#define FILE_NAME "video.mp4"

#define FRAMES_PER_SECOND 25

int RecordSequencePol(int status, std::string file);
int testSequence();
int settingsStuff();
int testTimedSave(int frames);