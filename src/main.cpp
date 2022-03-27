
#include "socketCS.h"
#include "polar_camera.h"

#include "pulse.h"

//#include "../rs_recorder/rs_camera.h"
//#include "../basler_recorder/basler_camera.h"

#include <iostream>
#include <chrono>
#include <mutex>
#include <string>

using namespace std;

bool socketThread = true;
SocketClient *clientSocket = nullptr;

string fileName = "domyslnaNazwa";
int statu = 0;
int statuC = 0; int statuD = 0; int statuP = 0;
string name= "kamera";

void printMenu()
{
    cout << endl;
    cout << "0: wyjscie/koniec" << endl
        //<< "1: symuluj koniec nagrywania" << endl
        << "Twoj wybor: " << endl;
}

void printInfo()
{
    cout << "poprawne wywolanie: " << endl << "klient <adres_serwera> <numer_portu>" << endl;
}

void sendDoneToServer()
{
    //info do serwera, ze klient zakonczyl nagrywanie (naturalnie badz na rozkaz STOP)
    clientSocket->SendLine("D");
}

void sendClosingToServer()
{
    //info do serwera, ze aplikacja klienta konczy dzialanie
    clientSocket->SendLine("X");
    this_thread::sleep_for(chrono::milliseconds(1000)); //zeby wyslal, bo jak zrobi sie od razy Close() to nie wysyla...
}
void recordSequence()
{
	string filepath;
	fileName = fileName.substr(0, fileName.size()-1);
	string port;
	port = "COM3";

	filepath = fileName+name;
	cout << filepath << endl;
	//RecordSequencePulse(statuP, port, filepath);
	//RecordSequencePol(statu, filepath);
	//RecordSequenceRGB(statuC, filepath);
	//RecordSequenceDepth(statuD, filepath);
	testSequence();
	sendDoneToServer();

}

void calibrateTest()
{
	settingsStuff();
}

void saveRecTest()
{
	testTimedSave(100);
}

void socketThreadFunction()
{
    cout << "oczekiwanie na polecenia z serwera" << endl;
    while (socketThread)
    {
        this_thread::sleep_for(chrono::milliseconds(1));
        string received = clientSocket->ReceiveLine();
        if (received[0] == 'N')
        {
            fileName = received.substr(2);
            cout << endl << "nowa nazwa pliku: " << fileName << endl;
        }
        else if (received[0] == 'F')
        {
            cout << endl << "rozkaz ODPAL!" << endl;
			statu = 1;
			statuC = 1;
			statuD = 1; statuP = 1;
            //..rozpoczecie nagrywania...
            //nagrywanie trzeba zrobic w nowym watku, 
            //bo jesli bezposrednio tu, to nie odbierze kolejnych polecen z serwera, 
            //np. ze ma przerwac nagrywanie...
			//recordSequence();
			std::thread t1(recordSequence);
			t1.join();

            //po zakonczeniu nagrywania klient wysyla info do serwera: sendDoneToServer();
        }
        else if (received[0] == 'S')
        {
            cout << endl << "rozkaz STOP!" << endl;
			statu = 0;
			statuC = 0;
			statuD = 0;
			statuP = 0;
            //...zakonczenie nagrywania...

            //po zatrzymaniu nagrywania klient wysyla info do serwera 
            //(tu, albo tam gdzie faktycznie zatrzymanie bedzie robione): sendDoneToServer(); 
        }
		else if (received[0] == 'P')
		{
			saveRecTest();
		}
		else if (received[0] == 'O')
		{
			calibrateTest();
		}
    }
}



void changeStatus(int val)
{
	
}

int main()
{
	cout << "hello" << endl;
	string serv_addr;
	int port;
	string name;
	cout << "enter server address: " << endl;
	cin >> serv_addr;
	cout << "/n enter port: " << endl;
	cin >> port;
	cout << "/n enter name: " << endl;
	cin >> name;

   /* if (argc < 3)
    {
        printInfo();
        return -1;
    }
    string serv_addr = argv[1];
    int port;
	
	cout << "hello" << endl;

    try 
    { 
        port = stoi(argv[2]); 
    }
    catch (...)
    {
        printInfo();
        return -2;
    }
	try
	{
		name = string(argv[3]);
	}
	catch (...)
	{
		printInfo();
		return -2;
	}*/
    cout << "adres serwera: " << serv_addr << endl << "numer portu: " << port << endl << "Nazwa: " << name << endl;


    try
    {
        clientSocket = new SocketClient(serv_addr, port);
    }
    catch (...)
    {
        cout << "Nie udalo sie polaczyc z serwerem. Sprawdz adres i numer portu." << endl;
        return -3;
    }

    thread socketTh(&socketThreadFunction);

    int choice = -1;
    while (choice != 0)
    {
        printMenu();
        cin >> choice;
        if (choice == 1) //do testowania: symulacja zakonczenia nagrywania
            sendDoneToServer();

    } 

    socketThread = false;
    sendClosingToServer();
    clientSocket->Close();
    socketTh.join();

    delete clientSocket;
}