
#include "pulse.h"

using namespace std;

HANDLE rsInit(const char* portName, int baudRate, int byteSize, int parity, int stopBits)
{
	//otwarcie portu szeregowego
	HANDLE hComm = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (hComm == INVALID_HANDLE_VALUE)
		return INVALID_HANDLE_VALUE;

	//ustawienie parametrów
	DCB dcb;
	GetCommState(hComm, &dcb);
	dcb.BaudRate = (DWORD)baudRate;
	dcb.ByteSize = (BYTE)byteSize;
	dcb.Parity = (BYTE)parity;
	dcb.StopBits = (BYTE)stopBits;
	SetCommState(hComm, &dcb);

	//wyczyszczenie buforów i b³êdów
	PurgeComm(hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
	ClearCommError(hComm, NULL, NULL);

	return hComm;
}

BOOL rsClose(HANDLE hComm)
{
	PurgeComm(hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
	return CloseHandle(hComm);
}

BOOL rsRead(HANDLE hComm, BYTE* buffer, unsigned long bytesToRead, unsigned long* bytesRead)
{
	ClearCommError(hComm, NULL, NULL);
	DWORD commEvent;
	if (!WaitCommEvent(hComm, &commEvent, NULL))
		cout << "Error in WaitCommEvent" << endl;
	return ReadFile(hComm, buffer, bytesToRead, bytesRead, NULL);
}

BOOL rsWrite(HANDLE hComm, BYTE* buffer, unsigned long bytesToWrite, unsigned long* bytesWritten)
{
	ClearCommError(hComm, NULL, NULL);
	return WriteFile(hComm, buffer, bytesToWrite, bytesWritten, NULL);
}

string generateOutFileName()
{
	time_t tt = time(0);
	tm* ltm = 0;
	localtime_s(ltm, &tt);

	int yyyy = ltm->tm_year + 1900;
	int MM = ltm->tm_mon + 1;
	int dd = ltm->tm_mday;
	int HH = ltm->tm_hour;
	int mm = ltm->tm_min;
	int ss = ltm->tm_sec;

	return to_string(yyyy) + "." + ((MM < 10) ? "0" : "") + to_string(MM) + "." + ((dd < 10) ? "0" : "") + to_string(dd)
		+ "_" + ((HH < 10) ? "0" : "") + to_string(HH) + "." + ((mm < 10) ? "0" : "") + to_string(mm) + "." + ((ss < 10) ? "0" : "") + to_string(ss)
		+ ".txt";
}

BOOL rsEventComsoleMsg = TRUE;

void rsEventMsg(BYTE* buf, int numBytes)
{
	if (rsEventComsoleMsg)
	{
		cout << "Przeczytano " << numBytes << " bajtow: ";
		for (int n = 0; n < numBytes; n++) cout << (int)buf[n] << " ";
		cout << endl;
	}
}

void waitForUserInput(BOOL& doRun)
{
	do
	{
		cout << "Enter 'q' or 'e' or 's' to quit/exit/stop application" << endl;
		string userInput;
		cin >> userInput;
		if ((userInput == "e") || (userInput == "q") || (userInput == "s") || (userInput == "exit") || (userInput == "quit") || (userInput == "stop"))
			doRun = FALSE;
	} while (doRun);
}

void stats(int& eventCounter, BOOL& doRun)
{
	static int prevCounter = 0;
	while (doRun)
	{
		this_thread::sleep_for(chrono::milliseconds(1000));
		//   cout << "num events: " << ((eventCounter > prevCounter) ? eventCounter - prevCounter : 300 + eventCounter - prevCounter) << endl;
		prevCounter = eventCounter;
	}
}

int RecordSequencePulse(int statuP, std::string port, std::string filepath)
{
	string portName = "COM6";	// zmienilam z com6 na com3 

	cout << "portNameaaa=" << portName << endl;


	//ofstream outFile(generateOutFileName());
	ofstream outFile("plik12.txt");
	outFile << "#signal_strength\tbar_graph\twaveform\tpulse_rate\tspO2\t";
	outFile << "byte0\tbyte1\tbyte2\tbyte3\tbyte4\tbyte5\tbyte6\tbyte7\tbyte8\t";
	outFile << endl;

	BYTE inputBuf[16];
	BYTE cmdBuf[9] = { (BYTE)0x7d, (BYTE)0x081, (BYTE)0x80, (BYTE)0x80, (BYTE)0x80, (BYTE)0x80, (BYTE)0x80, (BYTE)0x80, (BYTE)0x80 };
	unsigned long numBytes;

	BOOL doRun;

	int baudRate = 115200; //19200;
	HANDLE hComm = rsInit(portName.c_str(), baudRate, 8, NOPARITY, ONESTOPBIT);

	

	if (hComm == INVALID_HANDLE_VALUE)
	{
		cout << "Error opening port " << portName << endl;
		return -1;
	}
	if (!SetCommMask(hComm, EV_RXCHAR))
		cout << "Error setting communications event mask" << endl;


	cout << "portNameaaa=" << portName << endl;

	
	cmdBuf[2] = (BYTE)0xa7; //'stopstore' command	
	rsWrite(hComm, cmdBuf, 9, &numBytes);

	rsRead(hComm, inputBuf, 16, &numBytes);

	rsEventMsg(inputBuf, numBytes);

	cmdBuf[2] = (BYTE)0xa2; //'stopreal' command
	rsWrite(hComm, cmdBuf, 9, &numBytes);
	rsRead(hComm, inputBuf, 16, &numBytes);
	rsEventMsg(inputBuf, numBytes);

	cmdBuf[2] = (BYTE)0xac; //'realtimepi' command
	rsWrite(hComm, cmdBuf, 9, &numBytes);
	rsRead(hComm, inputBuf, 16, &numBytes);
	rsEventMsg(inputBuf, numBytes);

	cmdBuf[2] = (BYTE)0xa1; //'realtime' command
	rsWrite(hComm, cmdBuf, 9, &numBytes);
	
	if (statuP == 1)
		doRun = TRUE;
	else
		doRun = FALSE;

	cout << "Error" << doRun << endl;
	int eventCounter = 0;

	int signalStrength;
	int barGraph;
	int waveform;
	int pulseRate;
	int spO2;
	int lowPI;
	int highPI;

	thread userTh(waitForUserInput, ref(doRun));
	thread statsTh(stats, ref(eventCounter), ref(doRun));

	int index = 0;

	while (doRun)
	{

		
		index = index + 1;
		cout << index << endl;
		rsRead(hComm, inputBuf, 16, &numBytes);
		if ((inputBuf[0] == 0x01) && (numBytes == 9))// && (index < 25))
		{

			signalStrength = ((int)inputBuf[2]) & 0x00f;
			waveform = ((int)inputBuf[3]) & 0x07f;
			barGraph = ((int)inputBuf[4]) & 0x00f;
			pulseRate = ((int)inputBuf[5]) & 0x7f; //0x0ff;
			spO2 = ((int)inputBuf[6]) & 0x7f; //0x0ff;
			lowPI = ((int)inputBuf[7]) & 0x0ff;
			highPI = ((int)inputBuf[8]) & 0x0ff;
			outFile << signalStrength << "\t" << barGraph << "\t" << waveform << "\t" << pulseRate << "\t" << spO2 << "\t";
			for (unsigned int b = 0; b < numBytes; b++)
				outFile << std::hex << (((int)inputBuf[b]) & 0x0ff) << "\t" << std::dec;
			outFile << endl;

			cout << "HR " << pulseRate << endl;




		}
		else
		{
			if (numBytes != 0)
			{
				cout << "Niewlasciwy komunikat" << endl;
				rsEventMsg(inputBuf, numBytes);
			}
		}

		++eventCounter;
		if (eventCounter == 300) //60 packets per second, writing 'stayconnected' every 5 sec
		{
			cmdBuf[2] = (byte)0xaf; //'stayconnected' command
			rsWrite(hComm, cmdBuf, 9, &numBytes);
			eventCounter = 0;
		}
		
		cout << statuP << endl;

		if (statuP == 0)
			doRun = FALSE;

		
	}
	
	cmdBuf[2] = (BYTE)0xa2; //'stopreal' command
	rsWrite(hComm, cmdBuf, 9, &numBytes);
	rsRead(hComm, inputBuf, 16, &numBytes);

	rsClose(hComm);
	userTh.join();
	statsTh.join();
	return 0;

}
