/*
 * memory_used_by_current_process.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: jonathan
 */
/*
 * Only for Linux.
 * Memory (and other) information for processes is constantly
 * updated inside the root/proc file. To get memory of a current process,
 * we can access /proc/self/stat. The stat file contains the status
 * of a processes including it's various attributes.
 * See Linux man page for proc, look for /proc/[pid]/status
 * (distinct from /proc/[pid]/stat and /proc/[pid]/statm)
 *
 * Linux /status file terminology of interest:
 * VmRSS - resident set size, number of pages the process has in real memory (no swap memory)
 * VmSize - virtual memory size
 */
/*
 * Contains three functions:
 * 1. int virtualMemoryUsedByCurrentProcess()
 * 2. int physicalMemoryUsedByCurrentProcess()
 * 3. int parseLine(char* line) - helper function
 * Uses ints since file is read directly and values are listed as kB in the Linux file.
 */

#include "memory_used_by_current_process.hpp"
//#include <stdio.h>

int virtualMemoryUsedByCurrentProcess(){ // Linux stores mem values in kB.
    FILE* linux_status_file = fopen("/proc/self/status", "r");
    int memory_int_value = -1;
    char file_data_chunk[128];

    // Grab 127 chars at a time while parsing each char set for the desired property
    // in this case "VmSize." Go through the entire file.
    while (fgets(file_data_chunk, 128, linux_status_file) != NULL){ //fgets will stop in this linux file when a newline is found
        if (strncmp(file_data_chunk, "VmSize:", 7) == 0){ // if VmSize is found
            memory_int_value = parseDataLine(file_data_chunk); // parse the entire line of data
            break;
        }
    }
    fclose(linux_status_file);
    return memory_int_value;
}

int physicalMemoryUsedByCurrentProcess(){ // Linux stores mem values in kB.
    FILE* linux_status_file = fopen("/proc/self/status", "r");
    int memory_int_value = -1;
    char file_data_chunk[128];

    // Grab 127 chars at a time while parsing each char set for the desired property
    // in this case "VmSize." Go through the entire file.
    while (fgets(file_data_chunk, 128, linux_status_file) != NULL){ //fgets will stop in this linux file when a newline is found
        if (strncmp(file_data_chunk, "VmRSS:", 6) == 0){ // if VmSize is found
            memory_int_value = parseDataLine(file_data_chunk); // parse the entire line of data
            break;
        }
    }
    fclose(linux_status_file);
    return memory_int_value;
}

// Helper function. Used to obtain values from the two above functions.
int parseDataLine(char* file_data_chunk){
    // Parse the data chunk for the property's int value.
	// Linux stores int values in the status page in "[int value] kB" as of 2016.
    int data = strlen(file_data_chunk);
    const char* pData = file_data_chunk; // Setup access to the char array input sent in.
    while (*pData <'0' || *pData > '9') pData++; // Increment across the data line until an integer is found.
    file_data_chunk[data-3] = '\0'; // The last 3 chars in a line are expected to be " kB" preceded by the int value we want.
    data = atoi(pData);
    return data;
}

