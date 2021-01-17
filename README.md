# OS Process Simulation
A program to simulate the way an operating system handles multiple processes and allocates resources to them (such as CPU and SSD time).

## Getting Started

### Prerequisites
- GNU Compiler Collection (installed either directly or through a Linux-based runtime environment like Cygwin)

### Usage
To run the program:
- Open a Terminal window
- Navigate to the main directory
- To build the executable, type:
```bash
g++ main.cpp ProcessTable.cpp DeviceTable.cpp -std=c++14
```
- To run, type (on Windows/Linux):
```bash
a.exe inputfile.txt outputfile.txt
```
- If on Mac, instead type:
```bash
./a.out inputfile.txt outputfile.txt
```
Replace *inputfile.txt* with the name of your desired input file, and *outputfile.txt* with the name of your desired output file.

#### Input
- Two example input files are given, *input1.txt* and *input2.txt*. To make any changes to these or make your own inputs, the format is given below:
  - Every input file must begin with the line:
    ```text
    NCORES #
    ```
    representing the number of cores in the simulation followed by whitespace(s) and a number.
  - Each process is laid out linearly. A process begins with the lines:
    ```text
    START #
    PID #
    ```
    The number after **START** represents when the process starts (in ms). The number after **PID** represents the process ID.
  - Every line after these first two (until the next process or END) represent requests that the process makes,
    with the number indicating the time requested (in ms). A process can request time for:
    - CORE : CPU core processing time
    - SSD : SSD read/write
    - TTY : User I/O
- The simulation contains two queues for processes waiting for resources:
  - NI Queue : Non-interactive, for processes waiting for CPU or SSD time
  - I Queue : Interactive, for processes that are blocked while interacting with the user
- Processes in the interactive queue are always prioritized for resources before those in the non-interactive queue
#### Output
- The simulation will output a txt file with the name given in the program call. While processes are fed to the program
  linearly, the output will show the results of the processes *chronologically*, with events displayed for:
  - Process start
  - Requests (CPU or SSD)
  - Completions (CPU or SSD)
  - User interactions
  - Process completion
- Once the simulation has finished running, the output ends with a summary of some statistical data about the run

## Outline
+ **main.cpp** : The main runner. Calls on DeviceTable.cpp and ProcessTable.cpp, reads from input file, and writes to output file.
+ **DeviceTable.h** : Header for DeviceTable
+ **DeviceTable.cpp** : Handles the core requests, core completions, SSD requests, SSD completion, and user I/O
+ **ProcessTable.h** : Header for ProcessTable
+ **ProcessTable.cpp** : Organizes the text input from *main.cpp* into separate "Process" objects. Contains a vector to store
each object and a priority queue to dictate which order the process requests are handled.