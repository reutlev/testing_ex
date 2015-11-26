//Author://----// Adi Mashiah, ID:305205676//------//Reut Lev, ID:305259186//
//Belongs to project: testing_ex
//--------Library Includes--------//
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <WinBase.h>
#include <tchar.h>

//--------Project Includes--------//
#include "ParseTestSpec.h"
#include "ThreadRun.h"
#include "LogFile.h"
#include "FileCompare.h"

//--------Definitions--------//
#define INPUT_PARAMETERS_NUM (5)
typedef enum {
	CMD_PARAMETER_PATH_TO_EXE_OFFSET = 1,
	CMD_PARAMETER_TEST_SPEC_FILE_OFFSET,
	CMD_PARAMETER_CORRECTNESS_LOG_FILE_OFFSET,
	CMD_PARAMETER_RUNTIME_LOG_FILE_OFFSET,
} CmdParameter;

//--------Declarations--------//
/**
* This function creates a thread by calling Win32 Api's CreateThread()
* function, and setting some of the parameters to default value.
*
* parameters:
* ----------
* StartAddress - a pointer to the function that will be run by the thread.
* ParameterPtr - a pointer to the parameter that will be supplied to the
*                function run by the thread.
* ThreadIdPtr - return argument: a pointer to a DWORD variable into which
*               the function will write the created thread's ID.
*
* returns:
* --------
* On success, a handle to the created thread. On Failure - NULL.
*/
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,LPVOID ParameterPtr,LPDWORD ThreadIdPtr);

//--------Implementation--------//
int main(int argc, char *argv[])
{ 

	ErrorCode error_code = SUCCESS;
	char *path_to_exe = NULL;
	char *test_specification_file = NULL;
	char *correctness_log_file = NULL;
	char *runtime_log_file = NULL;
	TestObject *tests_array = NULL;
	int num_of_tests = 0;
	int i;
	HANDLE *threads_handles = NULL; //an array to holds the handles, the size isn't known during compilation time
	DWORD *threads_id = NULL;
	DWORD exitcode;
	DWORD wait_code;
	BOOL are_expected_and_output_matching;
	BOOL is_expecting_crash;
	int test_error_code;
	BOOL has_created_output_files = FALSE;
	
	//check for validity of arguments
	if (argc < INPUT_PARAMETERS_NUM)
	{
		LOG_ERROR("too few arguments were send to testing exercise process, exiting");
		error_code = WRONG_PARAMETERS;
		goto cleanup;
	}
	
	if (argc > INPUT_PARAMETERS_NUM)
	{
		LOG_ERROR("too many arguments were send to testing exercise process, exiting");
		error_code = WRONG_PARAMETERS;
		goto cleanup;
	}
	
	path_to_exe = argv[CMD_PARAMETER_PATH_TO_EXE_OFFSET];
	test_specification_file = argv[CMD_PARAMETER_TEST_SPEC_FILE_OFFSET];
	correctness_log_file = argv[CMD_PARAMETER_CORRECTNESS_LOG_FILE_OFFSET];
	runtime_log_file = argv[CMD_PARAMETER_RUNTIME_LOG_FILE_OFFSET];
	
	//parse the specification file, creating an array of structures.
	LOG_INFO("Parsing the specification file into an array");
	tests_array = ParseSpecificationFile(test_specification_file, &num_of_tests, path_to_exe);
	if (tests_array == NULL)
	{
		LOG_ERROR("failed to parse the specification file");
		error_code = PARSE_SPECIFICATION_FAILED;
		goto cleanup;
	}
	
	//creating an array of handles in num_of_tests size
	threads_handles = (HANDLE *)malloc(num_of_tests * sizeof(HANDLE));
	if (threads_handles == NULL)
	{
		LOG_ERROR("failed to malloc memory for threads_handle array");
		error_code = MALLOC_FAILED;
		goto cleanup;
	}
	
	//allocate memory for thread ID
	threads_id = (DWORD *)malloc(num_of_tests * sizeof(DWORD));
	if (threads_id == NULL)
	{
		LOG_ERROR("failed to malloc memory for threads_id array");
		error_code = MALLOC_FAILED;
		goto cleanup;
	}

	//Creating for each test, thread to run the test.
	for (i=0; i < num_of_tests; i++)
	{
		threads_handles[i] = CreateThreadSimple(
			(LPTHREAD_START_ROUTINE)StartTest,
			&(tests_array[i]),
			(LPDWORD)&(threads_id[i])
		);
		
		//cheack the creation of thread succeedded
		if (threads_handles[i] == NULL)
		{
			LOG_ERROR("failed to create thread");
			error_code = THREAD_CREATION_FAILED;
			goto cleanup;
		}

		LOG_INFO("Created thread number %d with id %d", i, threads_id[i]);
	}
	
	//Wait for all threads to end
	wait_code = WaitForMultipleObjects(
		num_of_tests,
		threads_handles,
		TRUE,       /* wait until all threads finish */
		INFINITE
	);

	if (wait_code != WAIT_OBJECT_0)
	{
		LOG_ERROR("Unexpected output value of 0x%x from WaitForMultipleObject()", wait_code);
		error_code = WAIT_FOR_MULTIPLE_OBJECT_FAILED;
		goto cleanup;
	}

	//get exit code of each thread and close the handle
	for (i = 0; i < num_of_tests; i++)
	{
		if (!GetExitCodeThread(threads_handles[i], &exitcode))
		{
			LOG_ERROR("Failed to get thread #%d exit code", i);
			error_code = THREAD_RUN_FAILED;
			goto cleanup;
		}
		LOG_INFO("Thread number %d returned with exit code %d\n", i, exitcode);
		if(exitcode != TRUE)
		{
			// A thread failure is a failure in the thread execution (but not in the exe execution)
			LOG_ERROR("Thread number %d failed", i);
			error_code = THREAD_RUN_FAILED;
			goto cleanup;
		}
		CloseHandle(threads_handles[i]);
		threads_handles[i] = NULL;

		if (!IsExpectingTestCrash(tests_array[i].expeceted_output, &is_expecting_crash, &test_error_code))
		{
			LOG_ERROR("Failed to check if the test was expecting a crash");
			error_code = OUTPUT_COMPARISSON_FAILED;
			goto cleanup;
		}

		if (tests_array[i].exit_code != 0)
		{
			if (is_expecting_crash && (test_error_code == tests_array[i].exit_code))
			{
				tests_array[i].test_result = SUCCEEDED;
			} else {
				tests_array[i].test_result = CRASHED;
			}
		} else {
			// Now compare output vs. expected
			if (!CompareOutputVersusExpected(tests_array[i].output, tests_array[i].expeceted_output, &are_expected_and_output_matching))
			{
				LOG_ERROR("Failed to compare the test output and expected output files");
				error_code = OUTPUT_COMPARISSON_FAILED;
				goto cleanup;
			}

			if (are_expected_and_output_matching)
			{
				tests_array[i].test_result = SUCCEEDED;
			} else {
				tests_array[i].test_result = FAILED;
			}
		}
	}
	
	//Creating the correctense and runtime log files
	if(error_code == SUCCESS) // check error code, if is diffrent from success do not create log files.
	{
		has_created_output_files = TRUE;
		if(!CreateCorrectnessLogFile(tests_array, num_of_tests, correctness_log_file))
		{
			LOG_ERROR("failed to create correctness log file");
			error_code = CREATE_LOG_FILE_FAILED;
			goto cleanup;
		}

		if(!CreateRuntimeLogFile (tests_array, num_of_tests, runtime_log_file))
		{
			LOG_ERROR("failed to create runtime log file");
			error_code = CREATE_LOG_FILE_FAILED;
			goto cleanup;
		}
	}

cleanup:
	if (threads_handles != NULL)
	{
		for (i = 0; i < num_of_tests; i++)
		{
			if (threads_handles[i] != NULL)
			{
				CloseHandle(threads_handles[i]);
			}
		}
		free(threads_handles);
	}

	if (threads_id != NULL)
	{
		free(threads_id);
	}


	if (tests_array != NULL)
	{
		FreeTestsArrayMemory(tests_array, num_of_tests);
	}
	
	//check the error code
	if ((error_code != SUCCESS) && has_created_output_files)
	{
		// in case of error, delete files of correcteness and runtime in case they already created;
		LOG_INFO("Deleting runtime and correctness log files (if existing) as a consequence of the program failure");
		DeleteFile(runtime_log_file);
		DeleteFile(correctness_log_file);
	}

	LOG_INFO("Tester exited with exit code %d", error_code);
	return error_code;
}


HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr)
{
	return CreateThread(
		NULL,            /*  default security attributes */
		0,                /*  use default stack size */
		StartAddress,    /*  thread function */
		ParameterPtr,    /*  argument to thread function */
		0,                /*  use default creation flags */
		ThreadIdPtr);    /*  returns the thread identifier */	
}
