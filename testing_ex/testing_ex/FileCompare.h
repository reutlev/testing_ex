#ifndef FILE_COMPARE_H
#define FILE_COMPARE_H

#include "ParseTestSpec.h"
#include "common.h"

//This function receives strings of the output file name and the expected file name, and a pointer to the TestObject
//Goes through those files and checks whether they are equal or not. Updates the test_result files in TestObject
//returns 0 if succeeded, 1 if failed.
BOOL CompareOutputVersusExpected(
	const char *output_file,
	const char *expected_file,
	TestObject *test
);

#endif