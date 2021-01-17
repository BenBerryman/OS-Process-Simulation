/*
 * 	PROCESSTABLE.H
 * 	Benjamin Berryman
 *
 * 	The Process Table contains all of the actual data about the processes.
 * 	A separate class InputTable is first used to read the input file line by line
 * 	and add each line to a queue. The queue is then read by the ProcessTable
 * 	using the transfer() function, getting additional data from each line along
 * 	the way. Once the ProcessTable(vector processes) is fully constructed,
 * 	each element is added to the eventList (organized as a priority queue according
 * 	to SMALLEST curTime) for use in the DeviceTable's nextEvent() function.
 */

#ifndef PROCESSTABLE_H_
#define PROCESSTABLE_H_
#include <vector>
#include <string>
#include <queue>

using namespace std;

class InputTable
{
	friend class ProcessTable;
	struct Entry
	{
		const string operation;
		const int time;

		Entry(const string &op, const int &t) : operation(op), time(t){};
	};
	queue<Entry> lines;

public:
	void add (const string op, const int time)
	{
		lines.push(Entry(op, time));
	}

};

class ProcessTable
{
	friend class DeviceTable;
	struct Process
	{
		struct Event
		{
			Event(string type, const int time) : eventType(type), timeNeeded(time), isInteractive(false){};
			string eventType;
			int timeNeeded;
			bool isInteractive;
		};

		Process(const int &time) : PID(-1), curTime(time), PC(0),
				onFirstLine(true), state("N/A"), addAgain(true), terminated(false){};
		int PID;
		int curTime; //Current time
		vector<Event> events;
		int PC; //Program counter
		bool onFirstLine;
		bool addAgain; //Stores whether nextEvent() should reinsert the process into 'eventList' at the end
		string state; //READY, RUNNING, BLOCKED, TERMINATED, or N/A if not arrived yet.
		bool terminated; //Stores whether a process has been terminated
	};

	//Custom comparator that compares the 'curTime' of each Process.
	//Exclusively for use in the priority queue 'eventList'.
	struct curTimeComp
	{
		bool operator()(const Process *first, const Process *second)
		{
			return first->curTime > second->curTime;
		}
	};

	vector<Process> processes;
	priority_queue<Process*, vector<Process*>, curTimeComp > eventList;

	int cores; //Intermediate variable for assigning numCores variable in DeviceTable

	/* Returns the number of cores.
	 * NOTE: This is a private function because 'cores' is only used as an intermediate
	 * variable to get to 'numCores' in DeviceTable (used in its constructor).
	 */
	int getCores() {return cores;}

public:

	/* Translates each of the lines from the given InputTable into processes,
	 * then make an Event List out of those processes.
	 */
	void transfer(InputTable &t);

	// Returns whether eventList is empty.
	bool isEmpty();

	// Returns the address of the top process on 'eventList'.
	Process* getTopProcess();

	/* Prints the current state of ProcessTable, meaning
	 * the state of each process (READY, RUNNING, BLOCKED, TERMINATED).
	 * The method will not print out processes that have not arrived or
	 * processes that have already been terminated and displayed.
	 */
	string printTable();

	// Returns whether there are any active processes.
	bool noActiveProcesses();
};

#endif /* PROCESSTABLE_H_ */
