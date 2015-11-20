#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "ParseTestSpec.h"
#include "Hw1TextFileReader.h"
//function declaretion:
char* CreateAndCopyString(const char *string_to_copy);

TestObject **ParseSpecificationFile(char *input_file, int *num_of_tests, char *path_to_exe)
{
	int tests_num=0;
	int i,line_i,word_i,j,non_empty_line=0,length=0,test_num=0;
	TestObject ** tests_array;
	TextFileReader reader;
	reader = CreateTextFileReader(input_file);
	if (reader.IsInitialized == FALSE)
	{
		LOG_ERROR("Failed to initialize the Text Reader");
		return NULL;
	}
	if( reader.NumOfLines == 0)
	{
		LOG_INFO("Specification file is empty");
		return NULL;
	}
	//Find the number of tests
	//the reader ignore new lines but not ignore tab,
	// dealing with diffrent spec file which containing tabs instead of only new lines.
    for ( line_i = 0 ; line_i < reader.NumOfLines ; line_i++ )
    {
		if(reader.WordsInLine[line_i] != 0)
		{
			tests_num++;
		}
    }
	//dividing by 2, because each two lines represent a test
	tests_num=tests_num/2;

	tests_array= (TestObject **)malloc(tests_num*sizeof(TestObject *));
	if (tests_array == NULL)
	{
		LOG_ERROR("failed to allocate memory");
		return NULL; 
	}
	//allocate memory for each entry - which is struct
	for (i = 0; i<tests_num ; i++)
	{
		tests_array[i]= (TestObject *)malloc(sizeof(TestObject));
			if (tests_array[i] == NULL)
		{
			LOG_ERROR("failed to allocate memory");
			for (j=0; j<i; j++)//free all the allocated entries which already aloccated.
			{
				free(tests_array[j]);
			}
			free(tests_array);//free already alloaceted memory in this function
			return NULL; 
		}
			memset(tests_array[i], '\0', sizeof(*tests_array[i]));//initialized all to zero
			tests_array[i]->args=NULL;
	}
	
    for ( line_i = 0 ; line_i < reader.NumOfLines ; line_i++ )
    {
		
		if (reader.WordsInLine[line_i] != 0)
		{
			if(non_empty_line%2==0)//even non-empty line
			{//saving the last which is output file in case of even non-empty line
				test_num=non_empty_line/2;//the number of test if always dividing of 2 of non_empty_line, because each test will have 2 lines
				length=strlen(reader.WordsArr[line_i][reader.WordsInLine[line_i]-1]);
				tests_array[test_num]->output=CreateAndCopyString(reader.WordsArr[line_i][reader.WordsInLine[line_i]-1]);
				if(tests_array[test_num]->output==NULL)
				{
					LOG_ERROR("failed to create new string and copy exsisting");
					FreeTestsArrayMemory(tests_array,tests_num);//free already allocated memory
					return NULL;
				}	
				//saving the arguments with the last which is output file
				for ( word_i = 0; word_i < (reader.WordsInLine[line_i]) ; word_i++ ) 
				  {//calcullating the total length with the terminating null character\additional space between words
					  length =length+ strlen (reader.WordsArr[line_i][word_i])+1;//+1 is for space char\end of string special char
					  tests_array[test_num]->args=(char *)realloc(tests_array[test_num]->args,(length)*sizeof(char));
					  if(tests_array[test_num]->args==NULL)
					  {
						  LOG_ERROR("Failed to realloc memory");
						  FreeTestsArrayMemory(tests_array,tests_num); //free already allocated memory
						  return NULL;
					  }
					  strcat(tests_array[test_num]->args," ");//add space to the string between each two arguments
					  strcat(tests_array[test_num]->args,reader.WordsArr[line_i][word_i]);//concatenate the new arg with the previous already concatenate
				  }
			}
			else//odd line
			{
				tests_array[test_num]->expeceted_output=CreateAndCopyString(reader.WordsArr[line_i][0]);
				if(tests_array[test_num]->expeceted_output==NULL)
				{
					LOG_ERROR("failed to create new string and copy exsisting");
					FreeTestsArrayMemory(tests_array,tests_num);//free already allocated memory
					return NULL;
				}	
			}
			non_empty_line++;
		//$// even_line=1-even_line;//change odd-even status: 0-odd non-empty line, 1- even non empty line. the "1-even_line" inverse 1 to 0 and 0 to 1
		}
    }

*num_of_tests=tests_num;

	return tests_array;
}

//malloc memory for new string and coppy existing string to it
char* CreateAndCopyString(const char *string_to_copy)
{
	char *string_empty=NULL;
	int length=0;
	length = strlen (string_to_copy);
		string_empty = (char *)malloc((length+1)*sizeof(char));
	if (string_empty == NULL)
	{
		LOG_ERROR ("failed to malloc memory");
		return NULL;
	}
	memcpy(string_empty, string_to_copy, length+1);
	return string_empty;
}

void FreeTestsArrayMemory(TestObject **tests_array,int tests_num)
{
	int i;
	if (tests_array != NULL)
	{
		 for (i=0; i<tests_num; i++)
		{
			if (tests_array[i] != NULL)
			{
				if(tests_array[i]->args != NULL)
				{
					free(tests_array[i]->args);
				}
				if(tests_array[i]->output != NULL)
				{
					free(tests_array[i]->output);
				}
				if(tests_array[i]->expeceted_output != NULL)
				{
					free(tests_array[i]->expeceted_output);
				}
			free(tests_array[i]);
			}
		}
	free(tests_array);
	}
}