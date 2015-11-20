
#include "FileCompare.h"
#include <stdlib.h>
#include <stdio.h>

BOOL CompareOutputVersusExpected(const char *output_file, const char *expected_file, TestObject *test)
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
			return FALSE;
	}

	error_expected = fopen_s(&expected_file_ptr, expected_file, "r");
	if ( error_expected != 0)
	{
			LOG_ERROR("Could not open an expected output file for compare");
			return FALSE;
	}

	do
	{
		i = fgetc(output_file_ptr);
		j = fgetc(expected_file_ptr);
		
		if( i == j && feof(expected_file_ptr) && feof(expected_file_ptr))
		{
			//then this is the last characther in the files
			break;
		}

		if( i == j && (!feof(expected_file_ptr)) && (!feof(output_file_ptr)))
		{
			//current characters are equivalent
			continue;
		}
		
		if( i !=j || 
			(feof(output_file_ptr) && (!feof(expected_file_ptr)))||
			(feof(expected_file_ptr) && (!feof(output_file_ptr))))
		{
			//then they are not equal
			printf("Found unequal output file and expected outputfile.");
			test_result = FAILED;
			test->test_result = test_result;
			return TRUE;
		}

	}while(1);

	test_result = SUCCEEDED;
	test->test_result = test_result;
	return TRUE;
}