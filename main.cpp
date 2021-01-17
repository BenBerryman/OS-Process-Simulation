/* ==============================================================================================================
 * 	Benjamin Berryman
 *
 * 	The following program takes an input file through the argument vector that contains one or more processes
 * 	to be scheduled by an operating system along with each process' events, and outputs the arrival, request, and
 * 	termination events that occur.
 *
 *  =============================================================================================================
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include "ProcessTable.h"
#include "DeviceTable.h"

int main(int argc, char *argv[])
{
	/* 	===========================================================================================
	 * 	Read file and copy to InputTable, which inputs to ProcessTable with the transfer() function
	 *
	 * 	argv[0] = assignment1.cpp
	 * 	argv[1] = Input File Name
	 * 	argv[2] = Output File Name
	 * 	===========================================================================================
	 */
	ifstream inFile(argv[1], ios::in);
	ofstream outFile(argv[2], ios::out);
	InputTable input;

	if (inFile.is_open())
	{
		string str;
		getline(inFile, str);
		while (str != "END")
		{
			std::string op = str.substr(0,str.find(" "));
			std::string temp = str.substr(str.find(" "));
			temp.erase(remove_if(temp.begin(), temp.end(),::isspace), temp.end());
			int num = stoi(temp);
			input.add(op, num); //Adding every line to InputTable, with op as the string key, and num as the int value
			getline(inFile,str);
		}
		inFile.close();
	}
	else
		throw std::invalid_argument("Unable to read file!");

	ProcessTable p;
	p.transfer(input); //Transfer contents of 'input' to 'p'
	DeviceTable d (p); //DeviceTable created using ProcessTable in order to receive the number of cores in the simulation.

	/* ==============================================================================================
	 * Keep calling nextEvent() until the Process Table is empty (when all processes have terminated)
	 * ==============================================================================================
	 */
	while (!p.isEmpty())
	{
		outFile << d.nextEvent(p);
	}

	/* ================================================================
	 * Print out some final statistics about the simulation as a whole.
	 * ================================================================
	 */
	outFile << d.finalStats(p);

	outFile.close();

	return 0;
};
