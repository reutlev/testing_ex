#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <WinBase.h>
#include <tchar.h>
#include "FileCompare.h"
#include "ParseTestSpec.h"

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr);
int PrintingHello (int * i);
BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr);

int main ()
{
	//const char *output_file = "C:\\Users\\HP\\Repositories\\testing_ex\\testing_ex\\testing_ex\\output.txt";
	//const char *expected_file = "C:\\Users\\HP\\Repositories\\testing_ex\\testing_ex\\testing_ex\\expected_1.txt";
	//TestObject *test = NULL;
	//
	//test = (TestObject *)malloc(sizeof(*test));

	//if(CompareOutputVersusExpected(output_file, expected_file, test))
	//{
	//	printf("FAILED");
	//	return;
	//}
	//printf("test->test_result = %d", test->test_result);
	//return;
	int m=6;
	//thread variables
	DWORD				exitcode = 98;
	DWORD               waitcode;
	HANDLE				Thread0; //handle to save the thread
	DWORD thread_id = 677; //contain the Is return from creating the handle.
	//process variable
	TCHAR               Command[] = _T("calc.exe");
	PROCESS_INFORMATION procinfo1;
	int retVal=0;
	//process///
	retVal=CreateProcessSimple(Command, &procinfo1);
	if (retVal == 0)
	{
		printf("Couldn't create process, error code %d\n", GetLastError());
		return;
	}
	
	CloseHandle(procinfo1.hThread);
	waitcode = WaitForSingleObject(procinfo1.hProcess, INFINITE);
	CloseHandle(procinfo1.hProcess);
	////Threads ////
	Thread0=CreateThreadSimple((LPTHREAD_START_ROUTINE)PrintingHello,(int *)&m,&thread_id);
	if (Thread0==NULL)
	{
		printf("failed to create handle thread! :(\n");
	}
	printf("the thread Id is %d\n",thread_id);
	WaitForSingleObject(Thread0,INFINITE);
	GetExitCodeThread(Thread0, &exitcode);
	printf("exit code from thread is %ld",&exitcode);
	CloseHandle(Thread0);
	return 0;
}

int PrintingHello (int * i)
{
	int j=9;
	errno_t err = -1;
	FILE *output_file=NULL;
	err = fopen_s(&output_file, "C:\\Users\\Reut\\Documents\\enginerring\\tichnotmaarachot\\newfile.txt", "w");
	if (err != 0)
	{
		return 1;
	}

	fprintf(output_file,"I got the number %d and my local number is %d and the Thread Id: %d\n",*i, j, GetCurrentThreadId());
	fclose(output_file);
	return 0;
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