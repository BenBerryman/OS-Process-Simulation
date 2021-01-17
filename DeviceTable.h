/*
 * DEVICETABLE.H
 * Benjamin Berryman
 *
 * The Device Table holds the I, NI, and SSD queues, as well as
 * all the request and release functions for the core(s) and SSD.
 * It also holds the function nextEvent(), which is the main driver
 * for the simulation, and finalStats(), which gives info about the
 * simulation once it has been completed.
 */

#ifndef DEVICETABLE_H_
#define DEVICETABLE_H_
#include "ProcessTable.h"

class DeviceTable
{
	int numCores;
	int freeCores;
	string ssdState; // FREE or BUSY
	queue<ProcessTable::Process*> interactive; //I Queue
	queue<ProcessTable::Process*> noninteractive; //NI Queue
	queue<ProcessTable::Process*> ssd; //SSD Queue

	int elapsedTime; //Only updated at TERMINATION events
	int ssdAccesses; //Number of times the SSD was accessed.
	int coreTime; //Total amount of time core(s) was/were used.
	int ssdTime; //Total amount of time SSD was used.

	/* Process requests a core. If none is available, process is added
	 * to the I or NI queue and set to the READY state. Otherwise, core
	 * is occupied for time in 'howLong' and process is in RUNNING state.
	 */
	string coreRequest(ProcessTable &table, ProcessTable::Process &process, int howLong, bool isInter);

	/* Process releases a core. If I queue is not empty, top process gets a core next.
	 * Else, if NI queue is not empty, top process gets the core next.
	 * Else, simply free the core.
	 */
	string coreRelease(ProcessTable &table, ProcessTable::Process &process);

	/* Process requests the SSD. If it is not free, process is added to the SSD queue
	 * and set to the READY state. Otherwise, SSD is occupied for time in 'howLong' and
	 * process is in BLOCKED state.
	 */
	string ssdRequest(ProcessTable &table, ProcessTable::Process &process, int howLong);

	/* Process releases the SSD. If SSD queue is not empty, top process gets the SSD next.
	 * Releasing process set to READY state.
	 */
	string ssdRelease(ProcessTable &table, ProcessTable::Process &process);

	//Process interacts with the user for time 'howLong'. Process state set to BLOCKED.
	string userRequest(ProcessTable &table, ProcessTable::Process &process, int howLong);

	//Process completes interaction with user. Next core request set to INTERACTIVE.
	void userRelease(ProcessTable &table, ProcessTable::Process &process);

public:
	DeviceTable(ProcessTable &p) : numCores(p.getCores()), freeCores(p.getCores()),
									ssdState("FREE"),elapsedTime(0), ssdAccesses(0),
									coreTime(0), ssdTime(0){};

	//MAIN DRIVER FUNCTION: takes top process from the given ProcessTable's 'eventList' and processes it.
	string nextEvent(ProcessTable &p);

	//Once simulation has ended, prints out info about it.
	string finalStats(ProcessTable &p);
};

#endif /* DEVICETABLE_H_ */
