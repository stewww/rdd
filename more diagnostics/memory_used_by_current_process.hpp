/*
 * memory_used_by_current_process.hpp
 *
 *  Created on: Oct 26, 2016
 *      Author: jonathan
 */
#ifndef MEMORY_USED_BY_CURRENT_PROCESS_HPP_
#define MEMORY_USED_BY_CURRENT_PROCESS_HPP_

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int parseDataLine(char* file_data_chunk); // Helper function for the two memory functions

int virtualMemoryUsedByCurrentProcess(); // in KB

int physicalMemoryUsedByCurrentProcess(); // in KB

#endif /* MEMORY_USED_BY_CURRENT_PROCESS_HPP_ */

