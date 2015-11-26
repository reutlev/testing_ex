//Author://----// Adi Mashiah, ID:305205676//------//Reut Lev, ID:305259186//
//Belongs to project: testing_ex

#ifndef LOGFILE_H
#define LOGFILE_H

//This module can create the log files: correctness log file and runtime log file, using:
//
//Deleting the files using:
//The module dependecies: "ParseTestSpec.h", "ParseTestSpec.c"
//--------Library Includs--------//
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <WinBase.h>
#include <tchar.h>
//--------Project Includs--------//
#include "ParseTestSpec.h"
//--------Definitions------------//
#define FILETIME_UNITS_PER_MILLISECOND 10000 


//--------Declarations-----------//
//this module receive a pointer to an array of tests results and it's size 
//the array type TestObject * as declered in "ParseTestSpec.h"
//and create the correctness log file
BOOL CreateCorrectnessLogFile (TestObject *tests_array, int num_of_tests, char *correctness_log_file);
//this module receive a pointer to an array of tests results- type TestObject as declered in "ParseTestSpec.h,
//and create the runtime log file
BOOL CreateRuntimeLogFile (TestObject *tests_array, int num_of_tests, char *runtime_log_file);

#endif //LOGFILE_H