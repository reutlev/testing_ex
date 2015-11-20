
#include "FileCompare.h"
#include <stdlib.h>
#include <stdio.h>

bool CompareOutputVersusExpected(const char *output_file, const char *expected_file)
{
	errno_t error_output = -1;
	errno_t error_expected = -1;
	FILE *output_file_ptr=NULL;
	FILE *expected_file_ptr=NULL;

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



}