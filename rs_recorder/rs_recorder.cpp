// rs_recorder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "rs_camera.h"

int RecordSequenceDepth(int status, std::string file)
try
{
	// Declare depth colorizer for pretty visualization of depth data
	rs2::colorizer color_map;

	// Declare RealSense pipeline, encapsulating the actual device and sensors
	rs2::pipeline pipe;
	// Start streaming with default recommended configuration
	pipe.start();

	using namespace cv;
	const auto window_name = "Display Image";
	namedWindow(window_name, WINDOW_AUTOSIZE);
	int count = 0;

	int const maxFrames = 500;


	rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
	rs2::frame depth = data.get_depth_frame().apply_filter(color_map);
	rs2::frame videeo = data.get_color_frame();

	// Query frame size (width and height)
	const int w = depth.as<rs2::video_frame>().get_width();
	const int h = depth.as<rs2::video_frame>().get_height();

	const int wc = videeo.as<rs2::video_frame>().get_width();
	const int hc = videeo.as<rs2::video_frame>().get_height();

	cv::VideoWriter cvVideo;
	cv::Size framesize = cv::Size((int)w, (int)h);

	cv::VideoWriter cvVideoC;
	cv::Size framesizeC = cv::Size((int)wc, (int)hc);

	std::string filename = "depth_"+ file;
	std::string filenameC = "rgb_"+ file;

	cvVideo.open(filename, cvVideo.fourcc('M', 'J', 'P', 'G'), 30, framesize, true);
	cvVideoC.open(filenameC, cvVideoC.fourcc('M', 'J', 'P', 'G'), 30, framesizeC, true);

	//cvVideo.open(filename, cvVideo.fourcc('Y', 'U', 'V','2'), 30, framesize, true);

	cv::VideoWriter cvVideoCreator;
	cv::VideoWriter cvVideoCreatorC;

	while (waitKey(1) < 0 && getWindowProperty(window_name, WND_PROP_AUTOSIZE) >= 0 && count <= maxFrames)
	{
		rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
		rs2::frame depth = data.get_depth_frame().apply_filter(color_map);
		rs2::frame videeo = data.get_color_frame();

		Mat image(Size(w, h), CV_8UC3, (void*)depth.get_data(), Mat::AUTO_STEP);
		Mat imageRGB(Size(wc, hc), CV_8UC3, (void*)videeo.get_data(), Mat::AUTO_STEP);
		
		if (count <= maxFrames)
		{// Create OpenCV matrix of size (w,h) from the colorized depth data
			

			cvVideo.write(image);

			cvVideoC.write(imageRGB);
			count++;
		}

		// Update the window with new data
		imshow(window_name, image);
	}

	return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
	std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}

