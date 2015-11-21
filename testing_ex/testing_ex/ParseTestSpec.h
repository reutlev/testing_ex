//Author: Adi Mashiah, ID:305205676\Reut Lev, ID:305259186
//Belongs to project: testing_ex
//This module parsing the specification file and creates an array of structures of data 
// need to be transfered to the threads workers with fields to fill in by the workers.
//depending files: HW1TextFileReader.h,HW1TextFileReader.c,common.h
#include "Hw1TextFileReader.h"

#ifndef PARSE_T
#define PARSE_T

typedef enum{
	SUCCEEDED=0,
	FAILED
} TestResult;

typedef struct TestObject_s{
	char *args;
	char *expeceted_output;
	char *output;
	const char *path_to_exe;
	TestResult test_result;
} TestObject;

// This function receive a string contaiang the input file path,a pointer to integer to be filled with number of tests in Specification file and path_to_exe which will be filled by 
//parse the input file,fill the num_of_test by the number of tests in the file
//initialise array of  num_of_tests TestObjects.
//return pointer to the array
//Use FreeTestsArray in order to free all the allocated memory in this module
TestObject **ParseSpecificationFile(char *input_file, int *num_of_tests, char * path_to_exe);
//free all the the array of tests allocatewd in this module
//receive a pointer to the array of structs (same pointer which was the output of ParseSpecificationFile
BOOL FreeTestsArray (TestObject **);
void FreeTestsArrayMemory(TestObject **tests_array,int tests_num);
#endif //PARSE_T
