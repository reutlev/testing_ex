//Author: Adi Mashiah, ID:305205676\Reut Lev, ID:305259186
//Belongs to project: testing_ex
//This module deals with the run of each thread. Means, creating the process within the thread, that runs a test.
//depending files: stdio.h,windows.h,stdlib.h,WinBase.h,tchar.h,string.h,ThreadRun.h,ParseTestSpec.h,common.h
//--------Project Includes--------//
#include "ThreadRun.h"

//--------Declarations--------//
//Receives CommandLine and pointer to process informations, and creates a new process.
//Returns NONZERO if succeeded, Zero if failed.
BOOL CreateProcessSimple(
	LPTSTR CommandLine,
	PROCESS_INFORMATION *ProcessInfoPtr
);

//Recieves a pointer to a test struct and updates its appropriate timing fields.
//Returns TRUE if succeeded, else FALSE.
BOOL AnalyzeTiming(TestObject *test, HANDLE hProcess);

//Taken from recitation:
// calculates the difference ( a - b ) between two times in FILETIME format.
// parameters: Late - the time that will be subtracted from ( a )
// Early - the time that will be subtracted ( b ) : should be earlier than 'Late'
// Returns: The difference in times: ( Late - Early )
FILETIME SubtractFiletimes(FILETIME Late, FILETIME Early);

// Converts wintime into unsigned long long (used for counting user and kernel times)
ULONGLONG WintoMiliseconds(FILETIME wintime);


//--------Implementation--------//
BOOL StartTest(TestObject *test)
{
	PROCESS_INFORMATION process_info;
	BOOL is_process_info_valid = FALSE;
	int retVal=0;
	char *command_line = NULL;
	int command_line_len = 0;
	DWORD wait_code;
	DWORD exit_code;
	DWORD thread_id = -1;
	BOOL result = FALSE;

	thread_id = GetCurrentThreadId();
	command_line_len = strlen(test->path_to_exe) + strlen(test->args) + 2;
	command_line = (char *)malloc(command_line_len);
	if (command_line == NULL)
	{
		LOG_ERROR("Thread #%d: Failed to allocate memory for the command line", thread_id);
		goto cleanup;
	}

	//Creating command_line to run:
	sprintf_s(command_line, command_line_len, "%s %s", test->path_to_exe, test->args);
	LOG_INFO("Thread #%d running command: %s", thread_id, command_line);

	//Creating the process:
	retVal = CreateProcessSimple(command_line, &process_info);

	if (retVal == FALSE)
	{
		LOG_ERROR("Thread #%d: Couldn't create process, error code %d\n", thread_id, GetLastError());
		goto cleanup;
	}
	is_process_info_valid = TRUE;

	//Waiting for termination of the process:
	wait_code = WaitForSingleObject(process_info.hProcess, INFINITE);

	if (wait_code != WAIT_OBJECT_0)
	{
		LOG_ERROR("Thread #%d: Unexpected output value of 0x%x from WaitForSingleObject()", thread_id, wait_code);
		goto cleanup;
	}

	if(!GetExitCodeProcess(process_info.hProcess, &exit_code))
	{
		LOG_ERROR("Thread #%d: Failed to get exit code from process", thread_id);
		goto cleanup;
	}

	LOG_INFO("Thread #%d: The process returned exit code #%d", thread_id, exit_code);
	test->exit_code = exit_code;

	//Analyzing timing:
	if(!AnalyzeTiming(test, process_info.hProcess))
	{
		LOG_ERROR("Failed to analyze timing of a process run");
		goto cleanup;
	}

	result = TRUE;

cleanup:
	if (is_process_info_valid)
	{
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);
	}
	
	if (command_line != NULL)
	{
		free(command_line);
	}
	
	return result;
}

BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr)
{
	STARTUPINFO    startinfo = { sizeof(STARTUPINFO), NULL, 0 }; /* <ISP> here we */
																 /* initialize a "Neutral" STARTUPINFO variable. Supplying this to */
																 /* CreateProcess() means we have no special interest in this parameter. */
																 /* This is equivalent to what we are doing by supplying NULL to most other */
																 /* parameters of CreateProcess(). */

	return CreateProcess(NULL, /*  No module name (use Command line). */
		CommandLine,            /*  Command line. */
		NULL,                    /*  Process handle not inheritable. */
		NULL,                    /*  Thread handle not inheritable. */
		FALSE,                    /*  Set handle inheritance to FALSE. */
		NORMAL_PRIORITY_CLASS,    /*  creation/priority flags. */
		NULL,                    /*  Use parent's environment block. */
		NULL,                    /*  Use parent's starting directory. */
		&startinfo,                /*  Pointer to STARTUPINFO structure. */
		ProcessInfoPtr            /*  Pointer to PROCESS_INFORMATION structure. */
		);
}


BOOL AnalyzeTiming (TestObject *test, HANDLE hProcess)
{
	SYSTEMTIME          UTC_Time, LocalTime;
	FILETIME            creation_time, exit_time, kernel_time, user_time, total_time;

	//Retrieve the process times:
	if(!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
	{
		LOG_ERROR("Failed to retrieve a test process times");
		return FALSE;
	}

	//Convert the creation time to local time:
	if(!FileTimeToSystemTime(
		&creation_time, /* input */
		&UTC_Time)  /* output */
	) {
		LOG_ERROR("Failed to convert creation time to local time");
		return FALSE;
	}

	if(!SystemTimeToTzSpecificLocalTime(
		NULL, /* use default time zone */
		&UTC_Time,     /* input */
		&LocalTime)  /* output */
	) {
		LOG_ERROR("Failed to convert UTC to a specified time zone's corresponding local time");
		return FALSE;
	}

	total_time = SubtractFiletimes(exit_time, creation_time);
	
	//update the timing fields of the test's struct:
	test->creation_time = LocalTime;
	test->total_time = WintoMiliseconds(total_time);
	test->user_time = WintoMiliseconds(user_time);
	test->kernel_time = WintoMiliseconds(kernel_time);

	return TRUE;
}

FILETIME SubtractFiletimes(FILETIME Late, FILETIME Early)
{
	typedef unsigned __int64 Unsigned64BitType;
	Unsigned64BitType Late64BitVal;
	Unsigned64BitType Early64BitVal;
	Unsigned64BitType Difference64BitVal;
	FILETIME DifferenceAsFILETIME;
	const Unsigned64BitType Low32BitsMask = 0x00000000FFFFFFFF;

	Late64BitVal = ((Unsigned64BitType)(Late.dwHighDateTime) << 32) +
		Late.dwLowDateTime;
	Early64BitVal = ((Unsigned64BitType)(Early.dwHighDateTime) << 32) +
		Early.dwLowDateTime;
	Difference64BitVal = Late64BitVal - Early64BitVal;

	DifferenceAsFILETIME.dwLowDateTime =
		(DWORD)(Difference64BitVal & Low32BitsMask);
	DifferenceAsFILETIME.dwHighDateTime =
		(DWORD)(Difference64BitVal >> 32);

	return DifferenceAsFILETIME;
}

ULONGLONG WintoMiliseconds(FILETIME wintime)
{
	ULARGE_INTEGER large;
	large.LowPart = wintime.dwLowDateTime;
	large.HighPart = wintime.dwHighDateTime;
	return large.QuadPart / 10000;
}
