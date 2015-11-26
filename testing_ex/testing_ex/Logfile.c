//--------Library Includes--------//
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <WinBase.h>

//--------Project Includes--------//
#include "LogFile.h"

//--------Implementation--------//
BOOL CreateCorrectnessLogFile (TestObject *tests_array, int num_of_tests, char *correctness_log_file)
{
	FILE *output_file=NULL;
	errno_t err = -1;
	int i;
	BOOL return_value = FALSE;

	if (tests_array == NULL)
	{
		LOG_ERROR("couldn't create correctness log file: received NULL pointer to tests array");
		return FALSE;
	}

	err = fopen_s(&output_file, correctness_log_file, "w");
	if (err != 0)
	{
		LOG_ERROR("couldn't create correctness log file: couldn't open the file");
		return FALSE;
	}

	for (i=0 ; i < num_of_tests; i++) //walking through the array of tests
	{
		switch(tests_array[i].test_result) //distinguish between the diffrent results for each test and print appropriate line to the file
		{
		case SUCCEEDED :
			fprintf(output_file, "test #%d : Succeeded\n", i+1);
			break;				 
		case FAILED :			 
			fprintf(output_file, "test #%d : Failed\n", i+1);
			break;				 
		case CRASHED:			 
			fprintf(output_file, "test #%d : Crashed #%d\n", i+1, tests_array[i].exit_code);
			break;
		default:
			LOG_ERROR("test #%d : illegal value of test result\n",i+1);
			goto cleanup;
		}	
	}

	return_value = TRUE;

cleanup:
	if (output_file != NULL)
	{
		fclose(output_file);
	}
	
	return return_value;
}

BOOL CreateRuntimeLogFile (TestObject *tests_array, int num_of_tests, char *runtime_log_file)
{
	FILE *output_file=NULL;
	errno_t err = -1;
	int i;
	BOOL return_value = FALSE;

	if (tests_array == NULL)
	{
		LOG_ERROR("couldn't create runtime log file: received NULL pointer to tests array");
		return FALSE;
	}

	err = fopen_s(&output_file, runtime_log_file, "w");
	if (err != 0)
	{
		LOG_ERROR("couldn't create runtime log file: couldn't open the file");
		return FALSE;
	}

	for (i=0 ; i <num_of_tests; i++) //walking through the array of tests
	{
		fprintf(output_file, "test #%d : ", i+1);
		fprintf(output_file, "start_time=%02d:%02d:%02d:%3d ",
				tests_array[i].creation_time.wHour, 
				tests_array[i].creation_time.wMinute,
				tests_array[i].creation_time.wSecond, 
				tests_array[i].creation_time.wMilliseconds
		);
		fprintf(output_file, "total_time=%I64u ", tests_array[i].total_time);
		fprintf(output_file, "user_time=%I64u ", tests_array[i].user_time);
		fprintf(output_file, "kernel_time=%I64u\n", tests_array[i].kernel_time);
	
	}
	
	fclose(output_file);
	return TRUE;
}
