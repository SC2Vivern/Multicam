// basler_recorder.cpp : Defines the entry point for the console application.
//

// sorter_ver1_cpu.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "basler_camera.h"

#define saveImage 1
#define recordVideo 1
#define gpuProcessing 0






int RecordSequenceRGB(int status, std::string file)
{
	static const uint32_t c_countOfImagesToGrab = 10;
	Pylon::PylonAutoInitTerm autoInitTerm;

	try
	{
		Pylon::CInstantCamera camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
		GenApi::INodeMap& nodemap = camera.GetNodeMap();
		camera.Open();
		GenApi::CIntegerPtr width = nodemap.GetNode("Width");
		GenApi::CIntegerPtr height = nodemap.GetNode("Height");
		GenApi::CEnumerationPtr hw_trigger = nodemap.GetNode("TriggerMode");

		height->SetValue(300);

		std::string oldPixelFormat = hw_trigger->ToString();
		std::cout << "Old   : " << oldPixelFormat << std::endl;
		hw_trigger->FromString("Off");
		std::string filename = "basler" + file;
		cv::VideoWriter cvVideo;
		cv::Size framesize = cv::Size((int)width->GetValue(), (int)height->GetValue());



		cvVideo.open(filename, cvVideo.fourcc('M', 'J', 'P', 'G'), 30, framesize, true);
		//cvVideo.open(filename, cvVideo.fourcc('Y', 'U', 'V','2'), 30, framesize, true);


		camera.MaxNumBuffer = 5;
		Pylon::CImageFormatConverter formatConverter;
		formatConverter.OutputPixelFormat = Pylon::PixelType_BGR8packed;

		Pylon::CPylonImage pylonImage;

		int grabbedImages = 0;

		/*-------------------OpenCV-------------------------*/
		cv::VideoWriter cvVideoCreator;
		cv::Mat openCVImage;

		cv::Size frameSize = cv::Size((int)width->GetValue(), (int)height->GetValue());

		camera.StartGrabbing(c_countOfImagesToGrab, Pylon::GrabStrategy_LatestImages);
		Pylon::CGrabResultPtr ptrBuffer;


		int64 t0 = cv::getTickCount();

		while (camera.IsGrabbing())
		{


			//		camera.WaitForFrameTriggerReady(500, Pylon::TimeoutHandling_ThrowException);
			camera.RetrieveResult(500, ptrBuffer, Pylon::TimeoutHandling_ThrowException);
			if (ptrBuffer->GrabSucceeded())
			{
				formatConverter.Convert(pylonImage, ptrBuffer);
				openCVImage = cv::Mat(ptrBuffer->GetHeight(), ptrBuffer->GetWidth(), CV_8UC3, (uint8_t *)pylonImage.GetBuffer());
				cvVideo.write(openCVImage);
				//	cv::namedWindow("Raw image");
				//	cv::imshow("Raw image", openCVImage);
				//	cv::waitKey(1);
			}


		}


		int64 t1 = cv::getTickCount();
		double secs = (t1 - t0) / cv::getTickFrequency();

		std::cout << "Time: " << secs << std::endl;
		cv::waitKey(10000);
	}
	catch (const Pylon::GenericException &e)
	{
		// Error handling.
		std::cerr << "An exception occurred." << std::endl
			<< e.GetDescription() << std::endl;

	}

	return 0;
}



