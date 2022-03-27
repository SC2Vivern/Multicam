
#include "polar_camera.h"


void AcquireImages(Arena::IDevice* pDevice)
{

	GenICam::gcstring acquisitionModeInitial = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionMode");

	std::cout << TAB1 << "Set acquisition mode to 'Continuous'\n";

	Arena::SetNodeValue<GenICam::gcstring>(
		pDevice->GetNodeMap(),
		"AcquisitionMode",
		"Continuous");


	std::cout << TAB1 << "Set buffer handling mode to 'NewestOnly'\n";

	Arena::SetNodeValue<GenICam::gcstring>(
		pDevice->GetTLStreamNodeMap(),
		"StreamBufferHandlingMode",
		"NewestOnly");

	std::cout << TAB1 << "Start stream\n";

	pDevice->StartStream();

	std::cout << TAB1 << "Getting " << NUM_IMAGES << " images\n";

	for (int i = 0; i < NUM_IMAGES; i++)
	{

		std::cout << TAB2 << "Get image " << i;

		Arena::IImage* pImage = pDevice->GetImage(TIMEOUT);


		size_t size = pImage->GetSizeFilled();
		size_t width = pImage->GetWidth();
		size_t height = pImage->GetHeight();
		GenICam::gcstring pixelFormat = GetPixelFormatName(static_cast<PfncFormat>(pImage->GetPixelFormat()));
		uint64_t timestampNs = pImage->GetTimestampNs();

		std::cout << " (" << size << " bytes; " << width << "x" << height << "; " << pixelFormat << "; timestamp (ns): " << timestampNs << ")";

		std::cout << " and requeue\n";

		pDevice->RequeueBuffer(pImage);
	}

	
	std::cout << TAB1 << "Stop stream\n";

	pDevice->StopStream();

	
	Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionMode", acquisitionModeInitial);
}


int64_t SetIntValue(GenApi::INodeMap* pNodeMap, const char* nodeName, int64_t value)
{

	GenApi::CIntegerPtr pInteger = pNodeMap->GetNode(nodeName);

	
	value = (((value - pInteger->GetMin()) / pInteger->GetInc()) * pInteger->GetInc()) + pInteger->GetMin();

	if (value < pInteger->GetMin())
	{
		value = pInteger->GetMin();
	}

	if (value > pInteger->GetMax())
	{
		value = pInteger->GetMax();
	}

	pInteger->SetValue(value);


	return value;
}


void AcquireImagesRapidly(Arena::IDevice* pDevice)
{
	// get node values that will be changed in order to return their values
	// at the end of the example
	int64_t widthInitial = Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "Width");
	int64_t heightInitial = Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "Height");
	GenICam::gcstring pixelFormatInitial = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "PixelFormat");
	int64_t deviceStreamChannelPacketSizeInitial;
	if (MAX_PACKET_SIZE)
	{
		deviceStreamChannelPacketSizeInitial = Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "DeviceStreamChannelPacketSize");
	}
	GenICam::gcstring exposureAutoInitial = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "ExposureAuto");
	double exposureTimeInitial = Arena::GetNodeValue<double>(pDevice->GetNodeMap(), "ExposureTime");

	// Set low width and height
	//    Reducing the size of an image reduces the amount of bandwidth required
	//    for each image. The less bandwidth required per image, the more images
	//    can be sent over the same bandwidth.
	std::cout << TAB1 << "Set low width and height";

	int64_t width = SetIntValue(
		pDevice->GetNodeMap(),
		"Width",
		WIDTH);

	int64_t height = SetIntValue(
		pDevice->GetNodeMap(),
		"Height",
		HEIGHT);

	std::cout << " (" << width << "x" << height << ")\n";

	// Set small pixel format
	//    Similar to reducing the ROI, reducing the number of bits per pixel also
	//    reduces the bandwidth required for each image. The smallest pixel
	//    formats are 8-bit bayer and 8-bit mono (i.e. BayerRG8 and Mono8).
	std::cout << TAB1 << "Set small pixel format (" << PIXEL_FORMAT << ")\n";

	Arena::SetNodeValue<GenICam::gcstring>(
		pDevice->GetNodeMap(),
		"PixelFormat",
		PIXEL_FORMAT);

	
	if (MAX_PACKET_SIZE)
	{
		std::cout << TAB1 << "Set maximum device stream channel packet size";

		GenApi::CIntegerPtr pDeviceStreamChannelPacketSize = pDevice->GetNodeMap()->GetNode("DeviceStreamChannelPacketSize");
		if (!pDeviceStreamChannelPacketSize || !GenApi::IsReadable(pDeviceStreamChannelPacketSize) || !GenApi::IsWritable(pDeviceStreamChannelPacketSize))
		{
			throw GenICam::GenericException("DeviceStreamChannelPacketSize node not found/readable/writable", __FILE__, __LINE__);
		}

		std::cout << " (" << pDeviceStreamChannelPacketSize->GetMax() << " " << pDeviceStreamChannelPacketSize->GetUnit() << ")\n";

		pDeviceStreamChannelPacketSize->SetValue(pDeviceStreamChannelPacketSize->GetMax());
	}


	std::cout << TAB1 << "Set minimum exposure time";

	Arena::SetNodeValue<GenICam::gcstring>(
		pDevice->GetNodeMap(),
		"ExposureAuto",
		"Off");

	GenApi::CFloatPtr pExposureTime = pDevice->GetNodeMap()->GetNode("ExposureTime");

	if (!pExposureTime || !GenApi::IsReadable(pExposureTime) || !GenApi::IsWritable(pExposureTime))
	{
		throw GenICam::GenericException("ExposureTime node not found/readable/writable", __FILE__, __LINE__);
	}

	std::cout << " (" << pExposureTime->GetMin() << " " << pExposureTime->GetUnit() << ")\n";

	pExposureTime->SetValue(pExposureTime->GetMax());

	
	std::cout << TAB1 << "Start stream with " << NUM_IMAGES << " buffers\n";
	std::vector<Arena::IImage*> images;

	pDevice->StartStream(NUM_IMAGES);

	for (int i = 1; i <= NUM_IMAGES; i++)
	{
	
		std::cout << TAB2 << "Get image " << i << ((i % 250 == 0 && i != 0) || i == NUM_IMAGES - 1 ? "\n" : "\r");

		Arena::IImage* pImage = pDevice->GetImage(TIMEOUT);
		images.push_back(pImage);
	}

	// requeue buffers
	std::cout << TAB1 << "Reqeue buffers\n";

	for (size_t i = 0; i < images.size(); i++)
	{
		pDevice->RequeueBuffer(images[i]);
	}

	// stop stream
	std::cout << TAB1 << "Stop stream\n";

	pDevice->StopStream();

	// return nodes to their initial values
	if (exposureAutoInitial == "Off")
	{
		Arena::SetNodeValue<double>(pDevice->GetNodeMap(), "ExposureTime", exposureTimeInitial);
	}
	Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "ExposureAuto", exposureAutoInitial);
	if (MAX_PACKET_SIZE)
	{
		Arena::SetNodeValue<int64_t>(pDevice->GetNodeMap(), "DeviceStreamChannelPacketSize", deviceStreamChannelPacketSizeInitial);
	}
	Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "PixelFormat", pixelFormatInitial);
	Arena::SetNodeValue<int64_t>(pDevice->GetNodeMap(), "Width", widthInitial);
	Arena::SetNodeValue<int64_t>(pDevice->GetNodeMap(), "Height", heightInitial);
}

void RecordVideo(const std::vector<Arena::IImage*>& images)
{
	// Prepare video parameters
	std::cout << TAB1 << "Prepares video parameters (" << images[0]->GetWidth() << "x" << images[0]->GetHeight() << ", " << FRAMES_PER_SECOND << " FPS)\n";

	Save::VideoParams params(
		images[0]->GetWidth(),
		images[0]->GetHeight(),
		FRAMES_PER_SECOND);

	// Prepare video recorder
	std::cout << TAB1 << "Prepare video recorder for video " << FILE_NAME << "\n";

	Save::VideoRecorder videoRecorder(
		params,
		FILE_NAME);

	// Set codec, container, and pixel format
	std::cout << TAB1 << "Set codec to H264, container to MPEG-4, and pixel format to BGR8\n";

	videoRecorder.SetH264Mp4BGR8();



	// Open video
	std::cout << TAB1 << "Open video\n";

	std::cout << "\nFFMPEG OUTPUT---------------\n\n";
	videoRecorder.Open();
	std::cout << "\nFFMPEG OUTPUT---------------\n\n";

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

	// Close video
	std::cout << TAB1 << "Close video\n";

	std::cout << "\nFFMPEG OUTPUT---------------\n\n";
	videoRecorder.Close();
	std::cout << "\nFFMPEG OUTPUT---------------\n";
}

int RecordSequence(int status)
{
	std::cout << "Cpp_Record\n";

	try
	{
		// prepare example
		Arena::ISystem* pSystem = Arena::OpenSystem();
		pSystem->UpdateDevices(100);
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
			pDevice->RequeueBuffer(pImage);
			if (status == 0)
				continue;
		}

		// run 
		std::cout << "Commence example\n\n";
		RecordVideo(images);
		std::cout << "\nExample complete\n";

		// clean up 
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

	std::cout << "Press any key to complete\n";
	std::getchar();
	return 0;
}

int FastAquisition()
{
	std::cout << "Cpp_Acquisition_RapidAcquisition\n";

	try
	{
		// prepare example
		Arena::ISystem* pSystem = Arena::OpenSystem();
		pSystem->UpdateDevices(100);
		std::vector<Arena::DeviceInfo> deviceInfos = pSystem->GetDevices();
		if (deviceInfos.size() == 0)
		{
			std::cout << "\nNo camera(s) connected\n";
			return 0;
		}
		Arena::IDevice* pDevice = pSystem->CreateDevice(deviceInfos[0]);

		// run example
		std::cout << "Commence example\n\n";
		AcquireImagesRapidly(pDevice);
		std::cout << "\nExample complete\n";

		// clean up example
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

	std::cout << "Press any key to complete\n";
	std::getchar();
	return 0;
}