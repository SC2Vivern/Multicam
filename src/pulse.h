#pragma once
#include <Windows.h>

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <thread>

int RecordSequencePulse(int statuP, std::string port, std::string filepath);