#ifndef FILE_COMPARE_H
#define FILE_COMPARE_H

#include "ParseTestSpec.h"
#include "common.h"

//This function receives strings of the output file name and the expected file name.
//Goes through those files and checks whether they are equal or not.
//returns test_results
test_result CompareOutputVersusExpected(
	char output_file,
	char expected_file
);

#endif