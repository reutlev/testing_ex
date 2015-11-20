
#include "FileCompare.h"
#include <stdlib.h>
#include <stdio.h>

bool CompareOutputVersusExpected(const char *output_file, const char *expected_file, TestObject *test)
{
	errno_t error_output = -1;
	errno_t error_expected = -1;
	FILE *output_file_ptr=NULL;
	FILE *expected_file_ptr=NULL;
	int i = 0;
	int j = 0;
	TestResult test_result;

	error_output = fopen_s(&output_file_ptr, output_file, "r");
	if ( error_output != 0)
	{
			LOG_ERROR("Could not open an output file for compare");
			return 1;
	}

	if ( error_expected != 0)
	{
			LOG_ERROR("Could not open an expected output file for compare");
			return 1;
	}

	do
	{
		i = fgetc(output_file_ptr);
		j = fgetc(expected_file_ptr);
		if( i == j)
		{
			continue;
		}
		
		if( i !=j || 
			(feof(output_file_ptr) && (!feof(expected_file_ptr)))||
			(feof(expected_file_ptr) && (!feof(output_file_ptr))))
		{
			//then they are not equal
			printf("NOT EQUAL");
			test_result = FAILED;
			test->test_result = test_result;
			return 0;
		}

		test_result = SUCCEEDED;
		test->test_result = test_result;
		return 0;

	}while(1)



}