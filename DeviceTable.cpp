/*
 * DEVICETABLE.CPP
 * Benjamin Berryman
 *
 */


#ifndef DEVICETABLE_CPP_
#define DEVICETABLE_CPP_
#include "DeviceTable.h"

/* Process requests a core. If none is available, process is added
 * to the I or NI queue and set to the READY state. Otherwise, core
 * is occupied for time in 'howLong' and process is in RUNNING state.
 */
string DeviceTable::coreRequest(ProcessTable &table, ProcessTable::Process &process, int howLong, bool isInter)
{
	string output;
	output += "Process " + to_string(process.PID) + " requests a core at time " + to_string(process.curTime) + " ms for " + to_string(howLong) + " ms.\n";

	if (freeCores == 0)
	{
		output += "Process " + to_string(process.PID) + " must wait for a core.\n";
		process.addAgain = false;
		ProcessTable::Process *point = &process;
		if (isInter)
		{
			interactive.push(point);
			output += "I Queue now contains " + to_string(interactive.size()) + " process(es) waiting for a core.\n";
		}
		else
		{
			noninteractive.push(point);
			output += "NI Queue now contains " + to_string(noninteractive.size()) + " process(es) waiting for a core.\n";
		}
		process.state = "READY";
	}
	else
	{
		coreTime += howLong;
		process.curTime += howLong;
		output += "Process " + to_string(process.PID) + " will release a core at time " + to_string(process.curTime) + " ms.\n";
		process.addAgain = true;
		process.state = "RUNNING";
		freeCores--;
	}

	return output;
}

/* Process releases a core. If I queue is not empty, top process gets the core next.
 * Else, if NI queue is not empty, top process gets the core next.
 * Else, simply free the core.
 */
string DeviceTable::coreRelease(ProcessTable &table, ProcessTable::Process &process)
{

	string output;
	output += "CORE completion event for process " + to_string(process.PID) + " at time " + to_string(process.curTime) + " ms.\n";
	freeCores++;
	ProcessTable::Process* proc;
	if (!interactive.empty())
	{
		proc = &(*interactive.front());
		proc->curTime = process.curTime;
		table.eventList.push(proc);
		output += coreRequest(table, *proc, proc->events.at(proc->PC-1).timeNeeded, true);
		interactive.pop();
	}
	else if (!noninteractive.empty())
	{
		proc = &(*noninteractive.front());
		proc->curTime = process.curTime;
		table.eventList.push(proc);
		output += coreRequest(table, *proc, proc->events.at(proc->PC-1).timeNeeded, false);
		noninteractive.pop();
	}

	process.state = "READY";
	return output;
}

/* Process requests the SSD. If it is not free, process is added to the SSD queue
 * and set to the READY state. Otherwise, SSD is occupied for time in 'howLong' and
 * process is in BLOCKED state.
 */
string DeviceTable::ssdRequest(ProcessTable &table, ProcessTable::Process &process, int howLong)
{
	string output;
	output += "Process " + to_string(process.PID) + " requests SSD access at time " + to_string(process.curTime) + " ms for " + to_string(howLong) + " ms.\n";
	if (ssdState.compare("FREE") == 0)
	{
		process.curTime += howLong;
		ssdTime += howLong;
		ssdAccesses++;
		output += "Process " + to_string(process.PID) + " will release the SSD at time " + to_string(process.curTime) + " ms.\n";
		process.state = "BLOCKED";
		process.addAgain = true;
		ssdState = "BUSY";
	}
	else
	{
		process.addAgain = false;
		process.state = "READY";
		output += "Process " + to_string(process.PID) + " must wait for SSD access.\n";
		ssd.push(&process);
	}

	return output;
}

/* Process releases the SSD. If SSD queue is not empty, top process gets the SSD next.
 * Releasing process set to READY state.
 */
string DeviceTable::ssdRelease(ProcessTable &table, ProcessTable::Process &process)
{
	string output;
	output += "SSD completion event for process " + to_string(process.PID) + " at time " + to_string(process.curTime) + " ms.\n";
	ssdState = "FREE";
	ProcessTable::Process* proc;
	if (!ssd.empty())
	{
		proc = &(*ssd.front());
		proc->curTime = process.curTime;
		table.eventList.push(proc);
		ssdRequest(table, *proc, proc->events.at(proc->PC-1).timeNeeded);
		ssd.pop();
	}
	process.state = "READY";
	return output;
}

//Process interacts with the user for time 'howLong'. Process state set to BLOCKED.
string DeviceTable::userRequest(ProcessTable &table, ProcessTable::Process &process, int howLong)
{
	string output;
	output += "Process " + to_string(process.PID) + " will interact with a user at time " + to_string(process.curTime) + " ms for " + to_string(howLong) + " ms.\n";
	process.curTime += howLong;
	output += "Process " + to_string(process.PID) + " will complete the interaction at time " + to_string(process.curTime) + " ms.\n";
	process.state = "BLOCKED";
	return output;
}

//Process completes interaction with user. Next core request set to INTERACTIVE.
void DeviceTable::userRelease(ProcessTable &table, ProcessTable::Process &process)
{
	process.events.at(process.PC).isInteractive = true;
}

//MAIN DRIVER FUNCTION: takes top process from the given ProcessTable's 'eventList' and processes it.
string DeviceTable::nextEvent(ProcessTable &p)
{
	ProcessTable::Process *process = p.getTopProcess();
	p.eventList.pop();
	string output;

	output += "==========================================================\n";

	/* =============================================================
	 * FIRST, check if the process just finished an event and if so,
	 * call the appropriate completion event.
	 * =============================================================
	 */
	if (process->state.compare("RUNNING") == 0 || process->state.compare("BLOCKED") == 0)
	{
		if (process->events.at(process->PC-1).eventType.compare("CORE") == 0)
		{
			output += coreRelease(p, *process) + "\n";
		}
		else if (process->events.at(process->PC-1).eventType.compare("SSD") == 0)
		{
			output += ssdRelease(p, *process);
		}
		else if (process->events.at(process->PC-1).eventType.compare("TTY") == 0)
		{
			userRelease(p, *process);
		}
	}

	/* ==========================================================
	 * SECOND, check if the process is on its first event. If so,
	 * print out arrival event.
	 * ==========================================================
	 */
	if (process->onFirstLine)
	{
		output += "ARRIVAL event for process " + to_string(process->PID)+ " at time " + to_string(process->curTime) + " ms.\n";
		output += "Process Table:\n" + p.printTable() + "\n";
		process->state = "READY";
		process->onFirstLine = false;
	}

	/* =====================================================================================
	 * THIRD, Check if the completion event just processed was the last event of the
	 * current process. If so, pop it from the processes list and mark state as TERMINATED.
	 * =====================================================================================
	 */
	if (process->PC >= process->events.size())
	{
		output += "Process " + to_string(process->PID) + " terminates at time " + to_string(process->curTime) + " ms.\n";
		process->addAgain = false;
		process->state = "TERMINATED";
		output += "Process Table:\n" + p.printTable();
		elapsedTime = process->curTime;
		if (!p.isEmpty())
		{
			output += "==========================================================\n";
			output += "                       |                       \n";
			output += "                       |                       \n";
		}
		else
			output += "\n";

		return output;
	}

	/* ===============================================================================
	 * FOURTH, the actual event processing. Take the event at the index pointer and process it
	 * depending on the event type.
	 * ===============================================================================
	 */
	ProcessTable::Process::Event event = process->events.at(process->PC);
	if (event.eventType.compare("CORE") == 0)
	{
		output += coreRequest(p, *process, event.timeNeeded, event.isInteractive);
	}
	else if (event.eventType.compare("SSD") == 0)
	{
		output += ssdRequest(p, *process, event.timeNeeded);
	}
	else if (event.eventType.compare("TTY") == 0)
	{
		output += userRequest(p, *process, event.timeNeeded);
	}
	process->PC++;

	/* ============================================================
	 * If not in a queue, reinsert the process into the event list.
	 * ============================================================
	 */
	if (process->addAgain)
	{
		p.eventList.push(process);
	}
	output += "==========================================================\n";
	output += "                       |                       \n";
	output += "                       |                       \n";

	return output;
}

//Once simulation has ended, prints out info about it.
string DeviceTable::finalStats(ProcessTable &p)
{
	string output;
	output += "================SUMMARY================\n";
	output += "Total elapsed time: " + to_string(elapsedTime) + " ms\n";
	output += "Number of completed processes: " + to_string(p.processes.size()) + "\n";
	output += "Total number of SSD accesses: " + to_string(ssdAccesses) + "\n";
	output += "Average number of busy cores: " + to_string((float)coreTime/elapsedTime)+ "\n";
	output += "SSD utilization: " + to_string((float)ssdTime/elapsedTime);

	return output;
}

#endif /* DEVICETABLE_CPP_ */
