/*
 * 	PROCESSTABLE.CPP
 *	Benjamin Berryman
 *
 * 	Implementation of ProcessTable.h functions.
 *
 * 	The Process Table contains all of the actual data about the processes.
 * 	A separate class InputTable is first used to read the input file line by line
 * 	and add each line to a queue. The queue is then read by the ProcessTable
 * 	using the transfer() function, getting additional data from each line along
 * 	the way. Once the ProcessTable(vector processes) is fully constructed,
 * 	each element is added to the eventList (organized as a priority queue according
 * 	to SMALLEST curTime) for use in the DeviceTable's nextEvent() function.
 */

#include "ProcessTable.h"

/* Translates each of the lines from the given InputTable into processes,
 * then make an Event List out of those processes.
 */
void ProcessTable::transfer(InputTable &t)
{
	bool newProcess = false;
	Process *current;
	InputTable::Entry *temp;
	string op;
	int time;

	/* ===============================================================================
	 * FIRST, transfer contents of InputTable to ProcessTable
	 *
	 * The keyword 'START' signals when a new process begins in the input file,
	 * which means it also signals the end of the process before it. The bool
	 * variable 'newProcess' keeps track of this. When it is TRUE, a 'START' signal
	 * will push the currently building process to 'processes' and begin a new process.
	 * When it is FALSE, it will simply begin a new process.
	 * ===============================================================================
	 */

	while (!t.lines.empty())
	{
		temp = &t.lines.front();
		op = temp->operation;
		time = temp->time;

		if (op.compare("NCORES") == 0)
		{
			cores = time;
		}
		else if (op.compare("START") == 0)
		{
			if (newProcess)
			{
				processes.push_back(*current);
				newProcess = false;
			}

			current = new Process (time);

		}
		else if (op.compare("PID") == 0)
		{
			newProcess = true;
			current->PID = time;

		}
		else if (op.compare("CORE") == 0)
		{
			current->events.push_back(*new Process::Event("CORE", time));

		}
		else if (op.compare("SSD") == 0 )
		{
			current->events.push_back(*new Process::Event("SSD", time));
		}
		else if (op.compare("TTY") == 0)
		{
			current->events.push_back(*new Process::Event("TTY", time));
		}

		t.lines.pop();

	}
	//Push the final process that 'current' is still holding onto.
	processes.push_back(*current);

	/* ======================================================================
	 * SECOND, create pointers for every process in 'processes' and push
	 * each of them to 'eventList', which is a priority queue of 'Process'
	 * pointers, all referencing back to respective processes in 'processes'.
	 * ======================================================================
	 */

	Process *point;
	for (int i = 0; i < processes.size(); i++)
	{
		point = &processes.at(i);
		eventList.push(point);
	}
}

// Returns whether eventList is empty.
bool ProcessTable::isEmpty()
{
	return eventList.empty();
}

// Returns the address of the top process on 'eventList'.
ProcessTable::Process* ProcessTable::getTopProcess()
{
	return &(*eventList.top());
}

/* Prints the current state of ProcessTable, meaning
 * the state of each process (READY, RUNNING, BLOCKED, TERMINATED).
 * The method will not print out processes that have not arrived or
 * processes that have already been terminated and displayed.
 */
string ProcessTable::printTable()
{
	if (noActiveProcesses())
	{
		return "There are no active processes.\n";
	}
	string output;
	Process* point;
	for (int i = 0; i < processes.size(); i++)
	{
		point = &processes.at(i);
		if ((!point->terminated) && point->state.compare("N/A") != 0)
			output += "Process " + to_string(point->PID) + " is " + point->state + ".\n";
		if (point->state.compare("TERMINATED") == 0)
			point->terminated=true;
	}
	return output;
}

// Returns whether there are any active processes.
bool ProcessTable::noActiveProcesses()
{
	for (Process p: processes)
	{
		if (p.state.compare("N/A") != 0) //If the process' state is anything other than N/A
			return false; //There is at least one active process
	}
	return true;
}
