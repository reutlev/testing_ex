//Author: Adi Mashiah, ID:305205676\Reut Lev, ID:305259186
//Belongs to project: testing_ex
//This module deals with the run of each thread. Means, creating the process within the thread, that runs a test.
//depending files: stdio.h,windows.h,stdlib.h,WinBase.h,tchar.h,string.h,ThreadRun.h,ParseTestSpec.h,common.h

#ifndef THREAD_RUN_H
#define THREAD_RUN_H

//--------Library Includes--------//
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <WinBase.h>
#include <tchar.h>
#include <string.h>

//--------Project Includes--------//
#include "ParseTestSpec.h"
#include "common.h"

//--------Declarations--------//
//This is the main function of this module.
//Receives a pointer to a TestObject and runs the exe file as a process with the appropriate arguments.
//Updates the approproate time info and in TestObject, and also the test result field in case of crashing.
//Returns 0 if succeeded, else 1.
BOOL StartTest(
	TestObject *test
);

#endif
