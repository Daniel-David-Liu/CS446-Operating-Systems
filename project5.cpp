#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <queue>
#include <pthread.h>
#include <iomanip>
#include <semaphore.h>
//#include "MemoryFunction.h"
//#include "MemoryFunction.cpp"
//#include "configData.h"
//#include "configData.cpp"

using namespace std;
sem_t lock;
unsigned int address;

struct inputData {
	string header;
	string command;
	int runTime;
};
struct PCB {
	int processState;

};

//for processState, 1= START, 2 = READY, 3 = RUNNING, 4 = WAITING, 5 = EXIT
int timeToWait;
int getIncrement(int current, int total)
{
	if(current < total)
	{
		return current+1;
	}
	return 0;
}
//timeToWait declared globaly so it can be used by threads when created

float getSystemTime(clock_t &t, float &systemTime){
    t = clock()-t;
    systemTime += t;
    return ((float)systemTime/CLOCKS_PER_SEC);
}

void wait(int waitTime){
    clock_t endwait;
    endwait = clock () + waitTime * CLOCKS_PER_SEC/1000 ;
    while (clock() < endwait) {}
}
void* threadWait(void*)
{
	sem_wait(&lock);

	wait(timeToWait);

	sem_post(&lock);

	pthread_exit(0);
}


unsigned int allocateMemory(int totMem,int  blockSize)
{

	address += blockSize;
	return address - blockSize;
} //This Function is taken from Webcampus from the professor
string delSpaces(string &str)
{
	str.erase(remove(str.begin(),str.end(), ' '), str.end());
	return str;
}

int main(int arg, char* fileName[])
{
	sem_init(&lock, 0, 1);
	PCB pcb1;
	pcb1.processState = 1;
	string fileInputPath;
	inputData allCommands[400];
	inputData tempStruct[400];

	string fileOutputPath;
	string log;
	int memoryLocation;
	pthread_t originalThread;
	pthread_attr_t attribute;
	pthread_attr_init(&attribute);

	int printerOutput = 0;
	int printerInput =0;
	int harddriveOutput= 0;
	int harddriveInput = 0;

	string tempStr;
	string lineData;
	int metaData[12];
	ifstream configFile;
	configFile.open (fileName[1]);
	int monitorLog = 0;
	int fileLog = 0;
	int cpuCode = 0;
	string cpuSchedule;


	if(!configFile)
	{
		cout << "Config File cant be opened" << endl;
		return 0;

	}
	if(configFile.eof())
	{
		cout << "Config File is Empty" << endl;
		return 0;

	}


	for(int i = 0; i <22; i++)
	{
		getline(configFile, lineData);
		tempStr = lineData;
		tempStr = delSpaces(tempStr);
		if(!tempStr.find("LogFilePath:") )
		{

				fileOutputPath = tempStr.substr(tempStr.find(":")+1, 256);

		}
		else if(tempStr.find("Log:") != string::npos)
		{
			if(tempStr.find("Both") != string::npos)
			{
				monitorLog = 1;
				fileLog = 1;
			}
			if(tempStr.find("Monitor")!= string::npos)
			{
				monitorLog = 1;
				fileLog = 0;
			}
			if(tempStr.find("File")!= string::npos)
			{
				monitorLog = 0;
				fileLog = 1;
			}

			log = tempStr.substr(tempStr.find(":")+1, 256);
		}
		else
		if(tempStr.find("FilePath:")!= string::npos)
		{
			fileInputPath = tempStr.substr(tempStr.find(":")+1, 256);

		}
		else if(tempStr.find("Processorcycle")!= string::npos)
		{

			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[0] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Monitor")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[1] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Hard drive cycle")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[2] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Printer cycle")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[3] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Keyboard")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[4] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Memorycycle")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[5] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Mouse")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[6] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Speaker")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[7] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("System")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[8] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Memoryblock")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[9] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Printerquantity")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[10] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("Harddrivequantity")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[11] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("ProcessorQuantum")!= string::npos)
		{
			string inputStr = tempStr.substr(tempStr.find(":")+1, 256);
			metaData[12] = atoi(inputStr.c_str());
		}
		else if(tempStr.find("CPUScheduling")!= string::npos)
		{
			if(tempStr.find("FIFO") != string::npos)
			{
				//cout << "FIFO" << endl;
				cpuCode = 0;

			}
			if(tempStr.find("PS")!= string::npos)
			{
				//cout << "PS" << endl;
				cpuCode= 1;

			}
			if(tempStr.find("SJF")!= string::npos)
			{
				//cout << "SJF" << endl;
				cpuCode = 2;

			}
			cpuSchedule = tempStr.substr(tempStr.find(":")+1, 256);
		}

	}




	configFile.close();
	ofstream outputFile(fileOutputPath.c_str());
	//outputFile.setf(ios_base::fixed);
	outputFile.precision(6);
	//cout.setf(ios_base::fixed);
	cout.precision(6);
	float time = 0;
	clock_t systemTimer;



	ifstream inputFile;
	inputFile.open(fileInputPath.c_str());

	if(!inputFile.good())
	{
		cout << "Input File cant be opened" << endl;
		return 0;

	}
	if(inputFile.eof())
	{
		cout << "Input File is Empty" << endl;
		return 0;

	}
	getline(inputFile, lineData);
	getline(inputFile, lineData);
	tempStr = lineData;
	tempStr = delSpaces(tempStr);
	int notifier;
	int iterator=0;
	//cout << tempStr << endl;
	string tempChar;
	int commandAmnt =0;

	while(!inputFile.eof())
	{
		if( tempStr.find("(") != string::npos)
		{
			tempChar = tempStr.substr(tempStr.find("(") -1, 1 );
			//cout << tempChar << endl;
			allCommands[commandAmnt].header = tempChar;
			tempChar = tempStr.substr(tempStr.find("(")+1 , tempStr.find(")")-tempStr.find("(") - 1 );
			//cout << tempChar << endl;
			allCommands[commandAmnt].command = tempChar;
			if(tempStr.find_first_of("0123456789") != string::npos)
			{
				notifier = 0;

				string tempChar = "";
				while(1)
				{

					notifier = tempStr.find_first_of("0123456789");
					tempChar += tempStr.substr(notifier, 1);

					tempStr = tempStr.substr(notifier+1, 256);

					//cout << tempStr << endl;
					//cout << tempChar << endl;
					if(tempStr.substr(0,1)!= "0" && tempStr.substr(0,1)!= "1" && tempStr.substr(0,1)!= "2" && tempStr.substr(0,1)!= "3" && tempStr.substr(0,1)!= "4" && tempStr.substr(0,1)!= "5" && tempStr.substr(0,1)!= "6" && tempStr.substr(0,1)!= "7" && tempStr.substr(0,1)!= "8" && tempStr.substr(0,1)!= "9")
					{
					//cout << notifier << endl;
					//while()
						break;
					}
				}

				allCommands[commandAmnt].runTime = atoi(tempChar.c_str());
				//cout << allCommands[commandAmnt].runTime << endl;
				commandAmnt++;
			iterator++;
				//cout << tempChar << endl;


			}

		}
		else
		{

			getline(inputFile, lineData);
			tempStr = lineData;
			tempStr = delSpaces(tempStr);

		}

			//allCommands[commandAmnt].







	}
	inputFile.close();
	int proccessTotal = 0;
	int proccessTasks = 0;
	int proccessTask2 = 0;
	int processCounter = 0;
	int loopRan = 0;
	float timePrint;
	int copyTracker = 0;
	int counting = 1;
	int position = 2;
	int tasksInProccess[40][3] = {0};
	int tempArray[1][2] = {0};
	for(int i = 0; i<39 ; i++)
	{
		tasksInProccess[i][1] = i;
	}
	if(cpuCode == 1)
	{
		for(int i=1; i< iterator; i++)
		{

			if(allCommands[i].header == "A" && allCommands[i].command == "start")
			{

				proccessTotal++;
			}

		}
		position = 1;
		cout << "Total Proccess : " << proccessTotal << endl;
		for(int i=0;i<proccessTotal; i++)
		{

			while(!(allCommands[position].header == "A" && allCommands[position].command == "start"))
			{

				position++;
			}
			while(allCommands[position].command != "end")
			{

				if(allCommands[position].header == "I" || allCommands[position].header == "O")
				{
					tasksInProccess[i][2]++;
				}
				if(allCommands[position].header == "S" && allCommands[position].command == "end")
				{
					tasksInProccess[i][0]--;
				}
				tasksInProccess[i][0]++;
				position++;
			}
			tasksInProccess[i][0]++;
			position++;

		}
		position = 1;
		for(int i=0; i<proccessTotal-1; i++)
		{

			for(int j=0; j<proccessTotal-1-i ; j++)
			{
				if(tasksInProccess[j][2] < tasksInProccess[j+1][2])
				{
					tempArray[0][0] = tasksInProccess[j][0];
					tempArray[0][1] = tasksInProccess[j][1];
					tasksInProccess[j][0] = tasksInProccess[j+1][0];
					tasksInProccess[j][1] = tasksInProccess[j+1][1];
					tasksInProccess[j+1][0] = tempArray[0][0];
					tasksInProccess[j+1][1] = tempArray[0][1];
				}
			}
		}
		copyTracker=1;
		for(int i =0; i< proccessTotal; i++)
		{

			for(int j = 0; j< proccessTotal; j++)
			{
				if(tasksInProccess[j][1] < tasksInProccess[i][1])
				{
					copyTracker+= tasksInProccess[j][0];
				}
			}
			for(int bepis = copyTracker; bepis < copyTracker +tasksInProccess[i][0]; bepis++)
			{
				tempStruct[position].command = allCommands[bepis].command;
				tempStruct[position].header = allCommands[bepis].header;
				tempStruct[position].runTime = allCommands[bepis].runTime;
				position++;
			}
			copyTracker=1;
		}
		for(int i=1; i < iterator-1; i++)
		{
			allCommands[i].command = tempStruct[i].command;
			allCommands[i].header = tempStruct[i].header;
			allCommands[i].runTime = tempStruct[i].runTime;
		}
	}
	if(cpuCode == 2)
	{
		for(int i=1; i< iterator; i++)
		{

			if(allCommands[i].header == "A" && allCommands[i].command == "start")
			{

				proccessTotal++;
			}

		}
		position = 1;
		cout << "Total Proccess : " << proccessTotal << endl;
		for(int i=0;i<proccessTotal; i++)
		{

			while(!(allCommands[position].header == "A" && allCommands[position].command == "start"))
			{

				position++;
			}
			while(allCommands[position].command != "end")
			{

				tasksInProccess[i][0]++;
				position++;
			}
			tasksInProccess[i][0]++;
			position++;

		}
		position = 1;
		for(int i=0; i<proccessTotal-1; i++)
		{

			for(int j=0; j<proccessTotal-1-i ; j++)
			{
				if(tasksInProccess[j][0] > tasksInProccess[j+1][0])
				{
					tempArray[0][0] = tasksInProccess[j][0];
					tempArray[0][1] = tasksInProccess[j][1];
					tasksInProccess[j][0] = tasksInProccess[j+1][0];
					tasksInProccess[j][1] = tasksInProccess[j+1][1];
					tasksInProccess[j+1][0] = tempArray[0][0];
					tasksInProccess[j+1][1] = tempArray[0][1];
				}
			}
		}
		copyTracker=1;
		for(int i =0; i< proccessTotal; i++)
		{

			for(int j = 0; j< proccessTotal; j++)
			{
				if(tasksInProccess[j][1] < tasksInProccess[i][1])
				{
					copyTracker+= tasksInProccess[j][0];
				}
			}
			for(int bepis = copyTracker; bepis < copyTracker +tasksInProccess[i][0]; bepis++)
			{
				tempStruct[position].command = allCommands[bepis].command;
				tempStruct[position].header = allCommands[bepis].header;
				tempStruct[position].runTime = allCommands[bepis].runTime;
				position++;
			}
			copyTracker=1;
		}
		for(int i=1; i < iterator-1; i++)
		{
			allCommands[i].command = tempStruct[i].command;
			allCommands[i].header = tempStruct[i].header;
			allCommands[i].runTime = tempStruct[i].runTime;
		}

	}


	pcb1.processState = 1;
	pcb1.processState = 2;
	cout.setf(ios_base::fixed);
	outputFile.setf(ios_base::fixed);
	systemTimer  = clock();
	timePrint = getSystemTime(systemTimer, time);
	systemTimer  = clock();



	pcb1.processState = 3;
	for(int i =0; i <= iterator;i++)
	{

		if(allCommands[i].command.length() > 11)
		{
			timePrint = getSystemTime(systemTimer, time);
			systemTimer  = clock();
			if(monitorLog == 1)
			cout << timePrint <<" Invalid Descriptor " << endl;
			if(fileLog == 1)
			outputFile << timePrint << "Invalid Descriptor " << endl;
			continue;
		}
		if(allCommands[i].command.length() < 3)
		{
			timePrint = getSystemTime(systemTimer, time);
			systemTimer  = clock();
			if(monitorLog == 1)
			cout << timePrint << " Invalid Descriptor " << endl;
			if(fileLog == 1)
			outputFile << timePrint << " Invalid Descriptor " << endl;
			continue;
		}
		if(allCommands[i].runTime < 0)
		{

			timePrint = getSystemTime(systemTimer, time);
			systemTimer  = clock();
			if(monitorLog == 1)
			cout << " No cycle value or incorrect value" << endl;
			if(fileLog == 1)
			outputFile << " No cycle value or incorrect value" << endl;
			continue;
		}


		if(allCommands[i].header == "S")
		{

			timePrint = getSystemTime(systemTimer, time);
			systemTimer  = clock();
			if(allCommands[i].command == "start")
			{
				if(monitorLog == 1)
				cout << timePrint << " - Simulator program starting" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Simulator program starting" << endl;
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();


			}
			if(allCommands[i].command == "end")
			{
				/*if(monitorLog == 1)
				cout << "End Program Meta-Data Code." << endl;
				if(fileLog == 1)
				outputFile << "End Program Meta-Data Code." << endl;*/
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - OS: process " << processCounter << " completed" <<endl;
				if(fileLog == 1)
				outputFile << timePrint << " - OS: process " << processCounter << " completed" <<endl;
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Simulator program ending" <<endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Simulator program ending" <<endl;

			}

		}
		if(allCommands[i].header == "A")
		{

			timePrint = getSystemTime(systemTimer, time);
			systemTimer  = clock();

			if(allCommands[i].command == "start")
			{
				processCounter++;
				if(monitorLog == 1)
				cout << timePrint << " - OS: preparing process " << processCounter << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - OS: preparing process " << processCounter << endl;
				timeToWait = allCommands[i].runTime;
				wait(timeToWait);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - OS: starting process " << processCounter << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - OS: starting process " << processCounter << endl;
			}
			if(allCommands[i].command == "end")
			{
				if(monitorLog == 1)
				cout << timePrint << " - End process " << processCounter << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - End process " << processCounter << endl;
			}
			continue;
		}
		if(allCommands[i].header == "P")
		{

			timePrint = getSystemTime(systemTimer, time);
			systemTimer  = clock();
			if(allCommands[i].command == "run")
			{

				if(monitorLog == 1)
				cout << timePrint << " - Process "<< processCounter << ": start processing action" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process 1"<< processCounter << ": start processing action" << endl;
				timeToWait = allCommands[i].runTime * metaData[0];
				//cout << allCommands[i].runTime << endl;
				wait(timeToWait);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Process "<< processCounter << ": interrupt processing action" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process "<< processCounter << ": interrupt processing action" << endl;


			}
		}
		if(allCommands[i].header == "I")
		{
			//PCB changed to waiting until thread is complete

			timePrint = getSystemTime(systemTimer, time);
			systemTimer  = clock();

			pcb1.processState = 4;
			if(allCommands[i].command == "harddrive")
			{

				timeToWait = allCommands[i].runTime * metaData[2];
				if(monitorLog == 1)
				cout << timePrint << " - Process "<< processCounter << ": start hard drive input on HDD " << harddriveInput << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process "<< processCounter << ": start hard drive input on HDD " << harddriveInput << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				//void result;
				pthread_join(originalThread, NULL);

				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(harddriveInput < metaData[11] -1)
				{
					harddriveInput ++;
				}
				else
				{
					harddriveInput = 0;
				}

				if(monitorLog == 1)
				cout << timePrint << " - Process "<< processCounter << ": end hard drive input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process "<< processCounter << ": end hard drive input"  << endl;
			}
			if(allCommands[i].command == "mouse")
			{
				timeToWait = allCommands[i].runTime * metaData[6];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": start mouse input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter << ": start mouse input" << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();

				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": end mouse input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter << ": end mouse input"  << endl;

			}
			if(allCommands[i].command == "keyboard")
			{
				timeToWait = allCommands[i].runTime * metaData[4];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": start keyboard input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter << ": start keyboard input" << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": end keyboard input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter << ": end keyboard input" << endl;
			}
			if(allCommands[i].command == "monitor")
			{
				timeToWait = allCommands[i].runTime * metaData[1];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": start monitor input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process 1: start monitor input" << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": end monitor input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process 1: end monitor input" << endl;
			}
			if(allCommands[i].command == "printer")
			{
				timeToWait = allCommands[i].runTime * metaData[3];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": start printer input on PRNTR " << printerInput << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter << ": start printer input on PRNTR " << printerInput << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(printerInput < metaData[10] - 1)
				{
					printerInput ++;
				}
				else
				{
					printerInput = 0;
				}
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": end printer input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter << ":end printer input" << endl;

			}
			if(allCommands[i].command == "speaker")
			{
				timeToWait = allCommands[i].runTime * metaData[7];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": start speaker input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter << ": start speaker input" << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": end speaker input" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter << ": end speaker input" << endl;

			}
			pcb1.processState = 3;

		}
		if(allCommands[i].header == "O")
		{
			pcb1.processState = 4;
			timePrint = getSystemTime(systemTimer, time);
			systemTimer  = clock();
			if(allCommands[i].command == "harddrive")
			{
				timeToWait = allCommands[i].runTime * metaData[2];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": start hard drive output on HDD " << harddriveOutput << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process 1: start hard drive output on HDD " << harddriveOutput << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(harddriveOutput < metaData[11] -1)
				{
					harddriveOutput ++;
				}
				else
				{
					harddriveOutput = 0;
				}
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": end hard drive output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter << ": end hard drive output" << endl;
			}
			if(allCommands[i].command == "mouse")
			{
				timeToWait = allCommands[i].runTime * metaData[6];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter << ": start mouse output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": start mouse output" << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Process 1: end mouse output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": end mouse output" << endl;
			}
			if(allCommands[i].command == "keyboard")
			{
				timeToWait = allCommands[i].runTime * metaData[4];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": start keyboard output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": start keyboard output" << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ":end keyboard output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": end keyboard output" << endl;
			}
			if(allCommands[i].command == "monitor")
			{
				timeToWait = allCommands[i].runTime * metaData[1];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": start monitor output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": start monitor output" << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": end monitor output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": end monitor output" << endl;
			}
			if(allCommands[i].command == "printer")
			{
				timeToWait = allCommands[i].runTime * metaData[3];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": start printer output on PRNTR " << printerOutput << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": start printer output on PRNTR " << printerOutput << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(printerOutput < metaData[10] -1)
				{
					printerOutput ++;
				}
				else
				{
					printerOutput = 0;
				}
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": end printer output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ":end printer output" << endl;
			}
			if(allCommands[i].command == "speaker")
			{
				timeToWait = allCommands[i].runTime * metaData[3];
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": start speaker output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": start speaker output" << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": end speaker output" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": end speaker output" << endl;
			}
			pcb1.processState = 3;

		}
		if(allCommands[i].header == "M")
		{

			timePrint = getSystemTime(systemTimer, time);
			systemTimer  = clock();
			if(allCommands[i].command == "block")
			{
				timeToWait = 100;
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": start memory blocking" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": start memory blocking" << endl;
				pthread_create(&originalThread, NULL, &threadWait, NULL);
				pthread_join(originalThread, NULL);
				timePrint = getSystemTime(systemTimer, time);
				systemTimer  = clock();
				if(monitorLog == 1)
				cout << timePrint << " - Process " << processCounter <<  ": end memory blocking" << endl;
				if(fileLog == 1)
				outputFile << timePrint << " - Process " << processCounter <<  ": end memory blocking" << endl;
			}
			if(allCommands[i].command == "allocate")
			{
				memoryLocation = allocateMemory(metaData[8], metaData[9]);
				if(monitorLog == 1)
				cout << internal << setfill('0');
				cout <<  timePrint << " - Process " << processCounter <<  ": memory allocated at 0x" << hex << setw(9) << memoryLocation << dec << endl;
				if(fileLog == 1)
				outputFile << internal << setfill('0');
				outputFile <<  timePrint << " - Process " << processCounter <<  ": memory allocated at 0x" << hex << setw(9) << memoryLocation << dec << endl;
			}
		}



		pcb1.processState = 5;
	}
	outputFile.close();

	//cout << "Printers = " << metaData[10] << endl;
	//cout << "Hard Drives = " << metaData[11] << endl;



return 0;
}
