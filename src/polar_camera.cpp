#include "polar_camera.h"

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>
#include <vector>

#include <opencv2/opencv.hpp> 
#include <opencv2/video/video.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>



void RecordVideo(const std::vector<Arena::IImage*>& images,std::string filename)
{
	// Prepare video parameters
	std::cout << TAB1 << "Prepares video parameters (" << images[0]->GetWidth() << "x" << images[0]->GetHeight() << ", " << FRAMES_PER_SECOND << " FPS)\n";

	Save::VideoParams params(
		images[0]->GetWidth(),
		images[0]->GetHeight(),
		FRAMES_PER_SECOND);

	// Prepare video recorder
	std::cout << TAB1 << "Prepare video recorder for video " << filename << "\n";

	Save::VideoRecorder videoRecorder(
		params,
		filename.c_str());

	// Set codec, container, and pixel format
	std::cout << TAB1 << "Set codec to H264, container to MPEG-4, and pixel format to BGR8\n";

	videoRecorder.SetH264Mp4BGR8();

	// Open video
	std::cout << TAB1 << "Open video\n";

	
	videoRecorder.Open();
	

	// Append images
	std::cout << TAB2 << "Append images\n";

	for (size_t i = 0; i < images.size(); i++)
	{
		if (i % 25 == 0)
			std::cout << TAB2;
		std::cout << ".";
		if (i % images.size() == images.size() - 1)
			std::cout << "\n";
		else if (i % 25 == 24)
			std::cout << "\r" << ERASE_LINE << "\r";

		videoRecorder.AppendImage(images[i]->GetData());
	}


	videoRecorder.Close();

	
}

// =-=-=-=-=-=-=-=-=-
// =- PREPARATION -=-
// =- & CLEAN UP =-=-
// =-=-=-=-=-=-=-=-=-

int RecordSequencePol(int status, std:: string file)
{
	std::cout << "Start_camera\n";
	cv::Mat img;
	cv::VideoCapture cap(0);

	try
	{
		// prepare example
		Arena::ISystem* pSystem = Arena::OpenSystem();
		pSystem->UpdateDevices(1000);
		std::vector<Arena::DeviceInfo> deviceInfos = pSystem->GetDevices();

		if (deviceInfos.size() == 0)
		{
			std::cout << "\nNo camera connected\n";
			return 0;
		}
		else if (NUM_IMAGES == 0)
		{
			std::cout << "\nNUM_IMAGES should be greater than 0\n";
			return 0;
		}
		Arena::IDevice* pDevice = pSystem->CreateDevice(deviceInfos[0]);
		std::vector<Arena::IImage*> images;
		pDevice->StartStream();
		for (int i = 0; i < NUM_IMAGES; i++)
		{
			Arena::IImage* pImage = pDevice->GetImage(2000);
			images.push_back(Arena::ImageFactory::Convert(pImage, BGR8));
			img = cv::Mat((int)pImage->GetHeight(), (int)pImage->GetWidth(), CV_8UC1, (void *)pImage->GetData());

			cv::imshow("Arena_POL", img);
			cv::waitKey(1);
			pDevice->RequeueBuffer(pImage);
		//	if (status == 0)
		//		continue;
		}

		// run example
		std::cout << "Record_video\n\n";
		RecordVideo(images, file);
		std::cout << "\nRecord_video\n";


	
		for (size_t i = 0; i < images.size(); i++)
		{
			Arena::ImageFactory::Destroy(images[i]);
		}
		pSystem->DestroyDevice(pDevice);
		Arena::CloseSystem(pSystem);
	}
	catch (GenICam::GenericException& ge)
	{
		std::cout << "\nGenICam exception thrown: " << ge.what() << "\n";
		return -1;
	}
	catch (std::exception& ex)
	{
		std::cout << "\nStandard exception thrown: " << ex.what() << "\n";
		return -1;
	}
	catch (...)
	{
		std::cout << "\nUnexpected exception thrown\n";
		return -1;
	}

	std::cout << "Sequence complete\n";

	return 0;
}

int testSequence() {
	
		//Open the default video camera
		cv::VideoCapture cap(0);

		// if not success, exit program
		if (cap.isOpened() == false)
		{
			std::cout << "Cannot open the video camera" << std::endl;
			std::cin.get(); //wait for any key press
			return -1;
		}

		double dWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
		double dHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

		std::cout << "Resolution of the video : " << dWidth << " x " << dHeight << std::endl;

		std::string window_name = "My Camera Feed";
		cv::namedWindow(window_name); //create a window called "My Camera Feed"

		while (true)
		{
			cv::Mat frame;
			bool bSuccess = cap.read(frame); // read a new frame from video 

			//Breaking the while loop if the frames cannot be captured
			if (bSuccess == false)
			{
				std::cout << "Video camera is disconnected" << std::endl;
				std::cin.get(); //Wait for any key press
				break;
			}

			//show the frame in the created window
			imshow(window_name, frame);

			//wait for for 10 ms until any key is pressed.  
			//If the 'Esc' key is pressed, break the while loop.
			//If the any other key is pressed, continue the loop 
			//If any key is not pressed withing 10 ms, continue the loop 
			if (cv::waitKey(10) == 27)
			{
				std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
				break;
			}
		}

		return 0;

	
}

int testSave() {

	// Create a VideoCapture object and use camera to capture the video
	cv::VideoCapture cap(0);

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		std::cout << "Error opening video stream" << std::endl;
		return -1;
	}

	// Default resolutions of the frame are obtained.The default resolutions are system dependent.
	int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

	// Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file.
	cv::VideoWriter video("outcpp.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, cv::Size(frame_width, frame_height));

	while (1) {

		cv::Mat frame;

		// Capture frame-by-frame
		cap >> frame;

		// If the frame is empty, break immediately
		if (frame.empty())
			break;

		// Write the frame into the file 'outcpp.avi'
		video.write(frame);

		// Display the resulting frame    
		imshow("Frame", frame);

		// Press  ESC on keyboard to  exit
		char c = (char)cv::waitKey(1);
		if (c == 27)
			break;
	}

	// When everything done, release the video capture and write object
	cap.release();
	video.release();

	// Closes all the frames
	cv::destroyAllWindows();
	return 0;
}

int testTimedSave(int frames) {

	// Create a VideoCapture object and use camera to capture the video
	cv::VideoCapture cap(0);

	int i = 0;

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		std::cout << "Error opening video stream" << std::endl;
		return -1;
	}

	// Default resolutions of the frame are obtained.The default resolutions are system dependent.
	int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

	// Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file.
	cv::VideoWriter video("outcpp.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, cv::Size(frame_width, frame_height));

	while (frames - i > 0) {

		i++;

		cv::Mat frame;

		// Capture frame-by-frame
		cap >> frame;

		// If the frame is empty, break immediately
		if (frame.empty())
			break;

		// Write the frame into the file 'outcpp.avi'
		video.write(frame);

		// Display the resulting frame    
		imshow("Frame", frame);

		// Press  ESC on keyboard to  exit
		char c = (char)cv::waitKey(1);
		if (c == 27)
			break;
	}

	// When everything done, release the video capture and write object
	cap.release();
	video.release();

	// Closes all the frames
	cv::destroyAllWindows();
	return 0;
}

class Settings
{
public:
	Settings() : goodInput(false) {}
	enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
	enum InputType { INVALID, CAMERA, VIDEO_FILE, IMAGE_LIST };

	void write(cv::FileStorage& fs) const                        //Write serialization for this class
	{
		fs << "{"
			<< "BoardSize_Width" << boardSize.width
			<< "BoardSize_Height" << boardSize.height
			<< "Square_Size" << squareSize
			<< "Calibrate_Pattern" << patternToUse
			<< "Calibrate_NrOfFrameToUse" << nrFrames
			<< "Calibrate_FixAspectRatio" << aspectRatio
			<< "Calibrate_AssumeZeroTangentialDistortion" << calibZeroTangentDist
			<< "Calibrate_FixPrincipalPointAtTheCenter" << calibFixPrincipalPoint

			<< "Write_DetectedFeaturePoints" << writePoints
			<< "Write_extrinsicParameters" << writeExtrinsics
			<< "Write_gridPoints" << writeGrid
			<< "Write_outputFileName" << outputFileName

			<< "Show_UndistortedImage" << showUndistorted

			<< "Input_FlipAroundHorizontalAxis" << flipVertical
			<< "Input_Delay" << delay
			<< "Input" << input
			<< "}";
	}
	void read(const cv::FileNode& node)                          //Read serialization for this class
	{
		node["BoardSize_Width"] >> boardSize.width;
		node["BoardSize_Height"] >> boardSize.height;
		node["Calibrate_Pattern"] >> patternToUse;
		node["Square_Size"] >> squareSize;
		node["Calibrate_NrOfFrameToUse"] >> nrFrames;
		node["Calibrate_FixAspectRatio"] >> aspectRatio;
		node["Write_DetectedFeaturePoints"] >> writePoints;
		node["Write_extrinsicParameters"] >> writeExtrinsics;
		node["Write_gridPoints"] >> writeGrid;
		node["Write_outputFileName"] >> outputFileName;
		node["Calibrate_AssumeZeroTangentialDistortion"] >> calibZeroTangentDist;
		node["Calibrate_FixPrincipalPointAtTheCenter"] >> calibFixPrincipalPoint;
		node["Calibrate_UseFisheyeModel"] >> useFisheye;
		node["Input_FlipAroundHorizontalAxis"] >> flipVertical;
		node["Show_UndistortedImage"] >> showUndistorted;
		node["Input"] >> input;
		node["Input_Delay"] >> delay;
		node["Fix_K1"] >> fixK1;
		node["Fix_K2"] >> fixK2;
		node["Fix_K3"] >> fixK3;
		node["Fix_K4"] >> fixK4;
		node["Fix_K5"] >> fixK5;

		validate();
	}
	void validate()
	{
		goodInput = true;
		if (boardSize.width <= 0 || boardSize.height <= 0)
		{
			std::cerr << "Invalid Board size: " << boardSize.width << " " << boardSize.height << std::endl;
			goodInput = false;
		}
		if (squareSize <= 10e-6)
		{
			std::cerr << "Invalid square size " << squareSize << std::endl;
			goodInput = false;
		}
		if (nrFrames <= 0)
		{
			std::cerr << "Invalid number of frames " << nrFrames << std::endl;
			goodInput = false;
		}

		if (input.empty())      // Check for valid input
			inputType = INVALID;
		else
		{
			if (input[0] >= '0' && input[0] <= '9')
			{
				std::stringstream ss(input);
				ss >> cameraID;
				inputType = CAMERA;
			}
			else
			{
				if (isListOfImages(input) && readStringList(input, imageList))
				{
					inputType = IMAGE_LIST;
					nrFrames = (nrFrames < (int)imageList.size()) ? nrFrames : (int)imageList.size();
				}
				else
					inputType = VIDEO_FILE;
			}
			if (inputType == CAMERA)
				inputCapture.open(cameraID);
			if (inputType == VIDEO_FILE)
				inputCapture.open(input);
			if (inputType != IMAGE_LIST && !inputCapture.isOpened())
				inputType = INVALID;
		}
		if (inputType == INVALID)
		{
			std::cerr << " Input does not exist: " << input;
			goodInput = false;
		}

		flag = 0;
		if (calibFixPrincipalPoint) flag |= cv::CALIB_FIX_PRINCIPAL_POINT;
		if (calibZeroTangentDist)   flag |= cv::CALIB_ZERO_TANGENT_DIST;
		if (aspectRatio)            flag |= cv::CALIB_FIX_ASPECT_RATIO;
		if (fixK1)                  flag |= cv::CALIB_FIX_K1;
		if (fixK2)                  flag |= cv::CALIB_FIX_K2;
		if (fixK3)                  flag |= cv::CALIB_FIX_K3;
		if (fixK4)                  flag |= cv::CALIB_FIX_K4;
		if (fixK5)                  flag |= cv::CALIB_FIX_K5;

		if (useFisheye) {
			// the fisheye model has its own enum, so overwrite the flags
			flag = cv::fisheye::CALIB_FIX_SKEW | cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
			if (fixK1)                   flag |= cv::fisheye::CALIB_FIX_K1;
			if (fixK2)                   flag |= cv::fisheye::CALIB_FIX_K2;
			if (fixK3)                   flag |= cv::fisheye::CALIB_FIX_K3;
			if (fixK4)                   flag |= cv::fisheye::CALIB_FIX_K4;
			if (calibFixPrincipalPoint) flag |= cv::fisheye::CALIB_FIX_PRINCIPAL_POINT;
		}

		calibrationPattern = NOT_EXISTING;
		if (!patternToUse.compare("CHESSBOARD")) calibrationPattern = CHESSBOARD;
		if (!patternToUse.compare("CIRCLES_GRID")) calibrationPattern = CIRCLES_GRID;
		if (!patternToUse.compare("ASYMMETRIC_CIRCLES_GRID")) calibrationPattern = ASYMMETRIC_CIRCLES_GRID;
		if (calibrationPattern == NOT_EXISTING)
		{
			std::cerr << " Camera calibration mode does not exist: " << patternToUse << std::endl;
			goodInput = false;
		}
		atImageList = 0;

	}
	cv::Mat nextImage()
	{
		cv::Mat result;
		if (inputCapture.isOpened())
		{
			cv::Mat view0;
			inputCapture >> view0;
			view0.copyTo(result);
		}
		else if (atImageList < imageList.size())
			result = cv::imread(imageList[atImageList++], cv::IMREAD_COLOR);

		return result;
	}

	static bool readStringList(const std::string& filename, std::vector<std::string>& l)
	{
		l.clear();
		cv::FileStorage fs(filename, cv::FileStorage::READ);
		if (!fs.isOpened())
			return false;
		cv::FileNode n = fs.getFirstTopLevelNode();
		if (n.type() != cv::FileNode::SEQ)
			return false;
		cv::FileNodeIterator it = n.begin(), it_end = n.end();
		for (; it != it_end; ++it)
			l.push_back((std::string)*it);
		return true;
	}

	static bool isListOfImages(const std::string& filename)
	{
		std::string s(filename);
		// Look for file extension
		if (s.find(".xml") == std::string::npos && s.find(".yaml") == std::string::npos && s.find(".yml") == std::string::npos)
			return false;
		else
			return true;
	}
public:
	cv::Size boardSize;              // The size of the board -> Number of items by width and height
	Pattern calibrationPattern;  // One of the Chessboard, circles, or asymmetric circle pattern
	float squareSize;            // The size of a square in your defined unit (point, millimeter,etc).
	int nrFrames;                // The number of frames to use from the input for calibration
	float aspectRatio;           // The aspect ratio
	int delay;                   // In case of a video input
	bool writePoints;            // Write detected feature points
	bool writeExtrinsics;        // Write extrinsic parameters
	bool writeGrid;              // Write refined 3D target grid points
	bool calibZeroTangentDist;   // Assume zero tangential distortion
	bool calibFixPrincipalPoint; // Fix the principal point at the center
	bool flipVertical;           // Flip the captured images around the horizontal axis
	std::string outputFileName;       // The name of the file where to write
	bool showUndistorted;        // Show undistorted images after calibration
	std::string input;                // The input ->
	bool useFisheye;             // use fisheye camera model for calibration
	bool fixK1;                  // fix K1 distortion coefficient
	bool fixK2;                  // fix K2 distortion coefficient
	bool fixK3;                  // fix K3 distortion coefficient
	bool fixK4;                  // fix K4 distortion coefficient
	bool fixK5;                  // fix K5 distortion coefficient

	int cameraID;
	std::vector<std::string> imageList;
	size_t atImageList;
	cv::VideoCapture inputCapture;
	InputType inputType;
	bool goodInput;
	int flag;

private:
	std::string patternToUse;


};

static inline void read(const cv::FileNode& node, Settings& x, const Settings& default_value = Settings())
{
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}

enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };

bool runCalibrationAndSave(Settings& s, cv::Size imageSize, cv::Mat&  cameraMatrix, cv::Mat& distCoeffs,
	std::vector<std::vector<cv::Point2f> > imagePoints, float grid_width, bool release_object);

int settingsStuff() {
	Settings s;
	const std::string inputSettingsFile = "default.xml";
	cv::FileStorage fs(inputSettingsFile, cv::FileStorage::READ); // Read the settings
	if (!fs.isOpened())
	{
		std::cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << std::endl;
		return -1;
	}
	fs["Settings"] >> s;
	fs.release();                                         // close Settings file


	 //FileStorage fout("settings.yml", FileStorage::WRITE); // write config as YAML
	//fout << "Settings" << s;

	if (!s.goodInput)
	{
		std::cout << "Invalid input detected. Application stopping. " << std::endl;
		return -1;
	}

	//int winSize = parser.get<int>("winSize");
	int winSize = 720;

	float grid_width = s.squareSize * (s.boardSize.width - 1);
	bool release_object = false;
	/*if (parser.has("d")) {
		grid_width = parser.get<float>("d");
		release_object = true;
	}*/

	std::vector<std::vector<cv::Point2f> > imagePoints;
	cv::Mat cameraMatrix, distCoeffs;
	cv::Size imageSize;
	int mode = s.inputType == Settings::IMAGE_LIST ? CAPTURING : DETECTION;
	clock_t prevTimestamp = 0;
	const cv::Scalar RED(0, 0, 255), GREEN(0, 255, 0);
	const char ESC_KEY = 27;

	//! [get_input]
	for (;;)
	{
		cv::Mat view;
		bool blinkOutput = false;

		view = s.nextImage();

		//-----  If no more image, or got enough, then stop calibration and show result -------------
		if (mode == CAPTURING && imagePoints.size() >= (size_t)s.nrFrames)
		{
			if (runCalibrationAndSave(s, imageSize, cameraMatrix, distCoeffs, imagePoints, grid_width,
				release_object))
				mode = CALIBRATED;
			else
				mode = DETECTION;
		}
		if (view.empty())          // If there are no more images stop the loop
		{
			// if calibration threshold was not reached yet, calibrate now
			if (mode != CALIBRATED && !imagePoints.empty())
				runCalibrationAndSave(s, imageSize, cameraMatrix, distCoeffs, imagePoints, grid_width,
					release_object);
			break;
		}
		//! [get_input]

		imageSize = view.size();  // Format input image.
		if (s.flipVertical)    flip(view, view, 0);

		//! [find_pattern]
		std::vector<cv::Point2f> pointBuf;

		bool found;

		int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;

		if (!s.useFisheye) {
			// fast check erroneously fails with high distortions like fisheye
			chessBoardFlags |= cv::CALIB_CB_FAST_CHECK;
		}

		switch (s.calibrationPattern) // Find feature points on the input format
		{
		case Settings::CHESSBOARD:
			found = findChessboardCorners(view, s.boardSize, pointBuf, chessBoardFlags);
			break;
		case Settings::CIRCLES_GRID:
			found = findCirclesGrid(view, s.boardSize, pointBuf);
			break;
		case Settings::ASYMMETRIC_CIRCLES_GRID:
			found = findCirclesGrid(view, s.boardSize, pointBuf, cv::CALIB_CB_ASYMMETRIC_GRID);
			break;
		default:
			found = false;
			break;
		}
		//! [find_pattern]
		//! [pattern_found]
		if (found)                // If done with success,
		{
			// improve the found corners' coordinate accuracy for chessboard
			if (s.calibrationPattern == Settings::CHESSBOARD)
			{
				cv::Mat viewGray;
				cvtColor(view, viewGray, cv::COLOR_BGR2GRAY);
				cornerSubPix(viewGray, pointBuf, cv::Size(winSize, winSize),
					cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.0001));
			}

			if (mode == CAPTURING &&  // For camera only take new samples after delay time
				(!s.inputCapture.isOpened() || clock() - prevTimestamp > s.delay*1e-3*CLOCKS_PER_SEC))
			{
				imagePoints.push_back(pointBuf);
				prevTimestamp = clock();
				blinkOutput = s.inputCapture.isOpened();
			}

			// Draw the corners.
			drawChessboardCorners(view, s.boardSize, cv::Mat(pointBuf), found);
		}
		//! [pattern_found]
		//----------------------------- Output Text ------------------------------------------------
		//! [output_text]
		std::string msg = (mode == CAPTURING) ? "100/100" :
			mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
		int baseLine = 0;
		cv::Size textSize = cv::getTextSize(msg, 1, 1, 1, &baseLine);
		cv::Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);

		if (mode == CAPTURING)
		{
			if (s.showUndistorted)
				msg = cv::format("%d/%d Undist", (int)imagePoints.size(), s.nrFrames);
			else
				msg = cv::format("%d/%d", (int)imagePoints.size(), s.nrFrames);
		}

		putText(view, msg, textOrigin, 1, 1, mode == CALIBRATED ? GREEN : RED);

		if (blinkOutput)
			bitwise_not(view, view);
		//! [output_text]
		//------------------------- Video capture  output  undistorted ------------------------------
		//! [output_undistorted]
		if (mode == CALIBRATED && s.showUndistorted)
		{
			cv::Mat temp = view.clone();
			if (s.useFisheye)
			{
				cv::Mat newCamMat;
				cv::fisheye::estimateNewCameraMatrixForUndistortRectify(cameraMatrix, distCoeffs, imageSize,
					cv::Matx33d::eye(), newCamMat, 1);
				cv::fisheye::undistortImage(temp, view, cameraMatrix, distCoeffs, newCamMat);
			}
			else
				undistort(temp, view, cameraMatrix, distCoeffs);
		}
		//! [output_undistorted]
		//------------------------------ Show image and check for input commands -------------------
		//! [await_input]
		imshow("Image View", view);
		char key = (char)cv::waitKey(s.inputCapture.isOpened() ? 50 : s.delay);

		if (key == ESC_KEY)
			break;

		if (key == 'u' && mode == CALIBRATED)
			s.showUndistorted = !s.showUndistorted;

		if (s.inputCapture.isOpened() && key == 'g')
		{
			mode = CAPTURING;
			imagePoints.clear();
		}
		//! [await_input]
	}

	// -----------------------Show the undistorted image for the image list ------------------------
	//! [show_results]
	if (s.inputType == Settings::IMAGE_LIST && s.showUndistorted && !cameraMatrix.empty())
	{
		cv::Mat view, rview, map1, map2;

		if (s.useFisheye)
		{
			cv::Mat newCamMat;
			cv::fisheye::estimateNewCameraMatrixForUndistortRectify(cameraMatrix, distCoeffs, imageSize,
				cv::Matx33d::eye(), newCamMat, 1);
			cv::fisheye::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Matx33d::eye(), newCamMat, imageSize,
				CV_16SC2, map1, map2);
		}
		else
		{
			initUndistortRectifyMap(
				cameraMatrix, distCoeffs, cv::Mat(),
				getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0), imageSize,
				CV_16SC2, map1, map2);
		}

		for (size_t i = 0; i < s.imageList.size(); i++)
		{
			view = cv::imread(s.imageList[i], cv::IMREAD_COLOR);
			if (view.empty())
				continue;
			remap(view, rview, map1, map2, cv::INTER_LINEAR);
			imshow("Image View", rview);
			char c = (char)cv::waitKey();
			if (c == ESC_KEY || c == 'q' || c == 'Q')
				break;
		}
	}
	//! [show_results]

	return 0;
}

//! [compute_errors]
static double computeReprojectionErrors(const std::vector<std::vector<cv::Point3f> >& objectPoints,
	const std::vector<std::vector<cv::Point2f> >& imagePoints,
	const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
	const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
	std::vector<float>& perViewErrors, bool fisheye)
{
	std::vector<cv::Point2f> imagePoints2;
	size_t totalPoints = 0;
	double totalErr = 0, err;
	perViewErrors.resize(objectPoints.size());

	for (size_t i = 0; i < objectPoints.size(); ++i)
	{
		if (fisheye)
		{
			cv::fisheye::projectPoints(objectPoints[i], imagePoints2, rvecs[i], tvecs[i], cameraMatrix,
				distCoeffs);
		}
		else
		{
			projectPoints(objectPoints[i], rvecs[i], tvecs[i], cameraMatrix, distCoeffs, imagePoints2);
		}
		err = norm(imagePoints[i], imagePoints2, cv::NORM_L2);

		size_t n = objectPoints[i].size();
		perViewErrors[i] = (float)std::sqrt(err*err / n);
		totalErr += err * err;
		totalPoints += n;
	}

	return std::sqrt(totalErr / totalPoints);
}
//! [compute_errors]
//! [board_corners]
static void calcBoardCornerPositions(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners,
	Settings::Pattern patternType /*= Settings::CHESSBOARD*/)
{
	corners.clear();

	switch (patternType)
	{
	case Settings::CHESSBOARD:
	case Settings::CIRCLES_GRID:
		for (int i = 0; i < boardSize.height; ++i)
			for (int j = 0; j < boardSize.width; ++j)
				corners.push_back(cv::Point3f(j*squareSize, i*squareSize, 0));
		break;

	case Settings::ASYMMETRIC_CIRCLES_GRID:
		for (int i = 0; i < boardSize.height; i++)
			for (int j = 0; j < boardSize.width; j++)
				corners.push_back(cv::Point3f((2 * j + i % 2)*squareSize, i*squareSize, 0));
		break;
	default:
		break;
	}
}
//! [board_corners]
static bool runCalibration(Settings& s, cv::Size& imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
	std::vector<std::vector<cv::Point2f> > imagePoints, std::vector<cv::Mat>& rvecs, std::vector<cv::Mat>& tvecs,
	std::vector<float>& reprojErrs, double& totalAvgErr, std::vector<cv::Point3f>& newObjPoints,
	float grid_width, bool release_object)
{
	//! [fixed_aspect]
	cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
	if (!s.useFisheye && s.flag & cv::CALIB_FIX_ASPECT_RATIO)
		cameraMatrix.at<double>(0, 0) = s.aspectRatio;
	//! [fixed_aspect]
	if (s.useFisheye) {
		distCoeffs = cv::Mat::zeros(4, 1, CV_64F);
	}
	else {
		distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
	}

	std::vector<std::vector<cv::Point3f> > objectPoints(1);
	calcBoardCornerPositions(s.boardSize, s.squareSize, objectPoints[0], s.calibrationPattern);
	objectPoints[0][s.boardSize.width - 1].x = objectPoints[0][0].x + grid_width;
	newObjPoints = objectPoints[0];

	objectPoints.resize(imagePoints.size(), objectPoints[0]);

	//Find intrinsic and extrinsic camera parameters
	double rms;

	if (s.useFisheye) {
		cv::Mat _rvecs, _tvecs;
		rms = cv::fisheye::calibrate(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, _rvecs,
			_tvecs, s.flag);

		rvecs.reserve(_rvecs.rows);
		tvecs.reserve(_tvecs.rows);
		for (int i = 0; i < int(objectPoints.size()); i++) {
			rvecs.push_back(_rvecs.row(i));
			tvecs.push_back(_tvecs.row(i));
		}
	}
	else {
		int iFixedPoint = -1;
		if (release_object)
			iFixedPoint = s.boardSize.width - 1;
		rms = calibrateCameraRO(objectPoints, imagePoints, imageSize, iFixedPoint,
			cameraMatrix, distCoeffs, rvecs, tvecs, newObjPoints,
			s.flag | cv::CALIB_USE_LU);
	}

	if (release_object) {
		std::cout << "New board corners: " << std::endl;
		std::cout << newObjPoints[0] << std::endl;
		std::cout << newObjPoints[s.boardSize.width - 1] << std::endl;
		std::cout << newObjPoints[s.boardSize.width * (s.boardSize.height - 1)] << std::endl;
		std::cout << newObjPoints.back() << std::endl;
	}

	std::cout << "Re-projection error reported by calibrateCamera: " << rms << std::endl;

	bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

	objectPoints.clear();
	objectPoints.resize(imagePoints.size(), newObjPoints);
	totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints, rvecs, tvecs, cameraMatrix,
		distCoeffs, reprojErrs, s.useFisheye);

	return ok;
}

// Print camera parameters to the output file
static void saveCameraParams(Settings& s, cv::Size& imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
	const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
	const std::vector<float>& reprojErrs, const std::vector<std::vector<cv::Point2f> >& imagePoints,
	double totalAvgErr, const std::vector<cv::Point3f>& newObjPoints)
{
	cv::FileStorage fs(s.outputFileName, cv::FileStorage::WRITE);

	time_t tm;
	time(&tm);
	struct tm t2;
	localtime_s(&t2, &tm);
	char buf[1024];
	strftime(buf, sizeof(buf), "%c", &t2);

	fs << "calibration_time" << buf;

	if (!rvecs.empty() || !reprojErrs.empty())
		fs << "nr_of_frames" << (int)std::max(rvecs.size(), reprojErrs.size());
	fs << "image_width" << imageSize.width;
	fs << "image_height" << imageSize.height;
	fs << "board_width" << s.boardSize.width;
	fs << "board_height" << s.boardSize.height;
	fs << "square_size" << s.squareSize;

	if (!s.useFisheye && s.flag &cv::CALIB_FIX_ASPECT_RATIO)
		fs << "fix_aspect_ratio" << s.aspectRatio;

	if (s.flag)
	{
		std::stringstream flagsStringStream;
		if (s.useFisheye)
		{
			flagsStringStream << "flags:"
				<< (s.flag & cv::fisheye::CALIB_FIX_SKEW ? " +fix_skew" : "")
				<< (s.flag & cv::fisheye::CALIB_FIX_K1 ? " +fix_k1" : "")
				<< (s.flag & cv::fisheye::CALIB_FIX_K2 ? " +fix_k2" : "")
				<< (s.flag & cv::fisheye::CALIB_FIX_K3 ? " +fix_k3" : "")
				<< (s.flag & cv::fisheye::CALIB_FIX_K4 ? " +fix_k4" : "")
				<< (s.flag & cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC ? " +recompute_extrinsic" : "");
		}
		else
		{
			flagsStringStream << "flags:"
				<< (s.flag & cv::CALIB_USE_INTRINSIC_GUESS ? " +use_intrinsic_guess" : "")
				<< (s.flag & cv::CALIB_FIX_ASPECT_RATIO ? " +fix_aspectRatio" : "")
				<< (s.flag & cv::CALIB_FIX_PRINCIPAL_POINT ? " +fix_principal_point" : "")
				<< (s.flag & cv::CALIB_ZERO_TANGENT_DIST ? " +zero_tangent_dist" : "")
				<< (s.flag & cv::CALIB_FIX_K1 ? " +fix_k1" : "")
				<< (s.flag & cv::CALIB_FIX_K2 ? " +fix_k2" : "")
				<< (s.flag & cv::CALIB_FIX_K3 ? " +fix_k3" : "")
				<< (s.flag & cv::CALIB_FIX_K4 ? " +fix_k4" : "")
				<< (s.flag & cv::CALIB_FIX_K5 ? " +fix_k5" : "");
		}
		fs.writeComment(flagsStringStream.str());
	}

	fs << "flags" << s.flag;

	fs << "fisheye_model" << s.useFisheye;

	fs << "camera_matrix" << cameraMatrix;
	fs << "distortion_coefficients" << distCoeffs;

	fs << "avg_reprojection_error" << totalAvgErr;
	if (s.writeExtrinsics && !reprojErrs.empty())
		fs << "per_view_reprojection_errors" << cv::Mat(reprojErrs);

	if (s.writeExtrinsics && !rvecs.empty() && !tvecs.empty())
	{
		CV_Assert(rvecs[0].type() == tvecs[0].type());
		cv::Mat bigmat((int)rvecs.size(), 6, CV_MAKETYPE(rvecs[0].type(), 1));
		bool needReshapeR = rvecs[0].depth() != 1 ? true : false;
		bool needReshapeT = tvecs[0].depth() != 1 ? true : false;

		for (size_t i = 0; i < rvecs.size(); i++)
		{
			cv::Mat r = bigmat(cv::Range(int(i), int(i + 1)), cv::Range(0, 3));
			cv::Mat t = bigmat(cv::Range(int(i), int(i + 1)), cv::Range(3, 6));

			if (needReshapeR)
				rvecs[i].reshape(1, 1).copyTo(r);
			else
			{
				//*.t() is MatExpr (not Mat) so we can use assignment operator
				CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
				r = rvecs[i].t();
			}

			if (needReshapeT)
				tvecs[i].reshape(1, 1).copyTo(t);
			else
			{
				CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
				t = tvecs[i].t();
			}
		}
		fs.writeComment("a set of 6-tuples (rotation vector + translation vector) for each view");
		fs << "extrinsic_parameters" << bigmat;
	}

	if (s.writePoints && !imagePoints.empty())
	{
		cv::Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
		for (size_t i = 0; i < imagePoints.size(); i++)
		{
			cv::Mat r = imagePtMat.row(int(i)).reshape(2, imagePtMat.cols);
			cv::Mat imgpti(imagePoints[i]);
			imgpti.copyTo(r);
		}
		fs << "image_points" << imagePtMat;
	}

	if (s.writeGrid && !newObjPoints.empty())
	{
		fs << "grid_points" << newObjPoints;
	}
}

//! [run_and_save]
bool runCalibrationAndSave(Settings& s, cv::Size imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
	std::vector<std::vector<cv::Point2f> > imagePoints, float grid_width, bool release_object)
{
	std::vector<cv::Mat> rvecs, tvecs;
	std::vector<float> reprojErrs;
	double totalAvgErr = 0;
	std::vector<cv::Point3f> newObjPoints;
	
	bool ok = runCalibration(s, imageSize, cameraMatrix, distCoeffs, imagePoints, rvecs, tvecs, reprojErrs,
		totalAvgErr, newObjPoints, grid_width, release_object);
	std::cout << (ok ? "Calibration succeeded" : "Calibration failed")
		<< ". avg re projection error = " << totalAvgErr << std::endl;

	if (ok)
		saveCameraParams(s, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, reprojErrs, imagePoints,
			totalAvgErr, newObjPoints);



	
	return 0;
}

