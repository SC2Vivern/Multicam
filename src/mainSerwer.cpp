
#include "socketCS.h"

#include <iostream>
#include <chrono>
#include <mutex>
#include <vector>

using namespace std;

bool runThreads = true;
vector<Socket*> clients;

SocketServer* ssocket = nullptr;

void printMenu()
{
    cout << endl;
    cout << "1: ustaw nazwe pliku" << endl
        << "2: ODPAL klientow" << endl
        << "3: STOP klientow" << endl
		<< "4: ZAPIS klientow" << endl
		<< "5: OPCJE klientow" << endl
        << "0: wyjscie/koniec" << endl
        << "Twoj wybor: " << endl;
}

void acceptThreadFunction()
{
    while (runThreads)
    {
        try
        {
            Socket* cli = ssocket->Accept();
            clients.push_back(cli);
            cout << "nowy klient, aktualna liczba klientow: " << clients.size() << endl;
            printMenu();
        }
        catch (...)
        {
            if (runThreads)
                cout << "cos sie stalo z serwerem - wyjatek!" << endl;
            //Accept rzuca wyjatek, gdy zrobi sie .Close() na serwerze - a nie wiem jak inaczej go z Accept() 'wybudzic'
            //ale .Close() wolane jest po wylaczeniu flagi runThreads, wiec wtedy nie ma co sie wyjatkiem przejmowac
        }
    }
}

void readThreadFunction()
{
    while (runThreads)
    {
        this_thread::sleep_for(chrono::milliseconds(1));
        for (int i = 0; i < clients.size(); ++i) //sprawdzenie kolejnych klientow, czy czegos nie przyslali
        {
            string msg = clients[i]->ReceiveBytes();
            if (msg.length() > 0) //cos jest!
            {
                if (msg.back() != '\n')
                {   //niepelny komunikat odczytany - klient wysyla SendLine(), wiec '\n' musi byc na koncu
                    msg += clients[i]->ReceiveLine();
                }
                msg.erase(msg.end()-1); //usuniecie znaku konca linii, bo niepotrzebny...

                if (msg == "D") //klient skonczyl nagrywanie
                {
                    cout << "Klient " << i << " skonczyl nagrywanie" << endl;
                }
                else if (msg == "X") //klient zakonczyl dzialanie - trzeba usunac z wektora
                {
                    clients.erase(clients.begin() + i--); //trzeba zdekrementowac i, aby koleja iteracje petli wziela nastepnego...
                    cout << "klient zamkniety, aktualna liczba klientow: " << clients.size() << endl;
                }
                else //jakis nieznany komunikat
                {
                    cout << "Klient " << i << ": <" << msg << ">" << endl;
                }
            }
        }
    }
}

void sendFileNameToClients()
{
    cout << "Podaj nazwe pliku: ";
    string fname;
    cin >> fname;
    for (int i = 0; i < clients.size(); ++i)
        clients[i]->SendLine("N "+fname);
}

void sendFireToClients()
{
    for (int i = 0; i < clients.size(); ++i)
        clients[i]->SendLine("F");
}

void sendStopToClients()
{
    for (int i = 0; i < clients.size(); ++i)
        clients[i]->SendLine("S");
}

void sendRecToClients()
{
	for (int i = 0; i < clients.size(); ++i)
		clients[i]->SendLine("P");
}

void sendSetToClients()
{
	for (int i = 0; i < clients.size(); ++i)
		clients[i]->SendLine("O");
}

int main()
{
    cout << "Serwer Wita" << endl;

    ssocket = new SocketServer(9999, 20);
    thread acceptTh(&acceptThreadFunction);
    thread readTh(&readThreadFunction);

    int choice = -1;
    while (choice != 0)
    {
        printMenu();
        cin >> choice;
        if (choice == 1)
            sendFileNameToClients();
        else if (choice == 2)
            sendFireToClients();
        else if (choice == 3)
            sendStopToClients();
		else if (choice == 4)
			sendRecToClients();
		else if (choice == 5)
			sendSetToClients();
    }

    for (int i = 0; i < clients.size(); ++i)
    {
        clients[i]->Close();
        delete clients[i];
    }
    runThreads = false;
    ssocket->Close();
    acceptTh.join();
    readTh.join();
    delete ssocket;
}