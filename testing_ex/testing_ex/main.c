#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <WinBase.h>

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr);
int PrintingHello (int i);

int main ()
{
	int m=6;
	DWORD exitcode = 98;
	HANDLE Thread0; //handle to save the thread
	DWORD thread_id = 677; //contain the Is return from creating the handle.
	Thread0=CreateThreadSimple((LPTHREAD_START_ROUTINE)PrintingHello,&m,&thread_id);
	if (Thread0==NULL)
	{
		printf("failed to create hande thread! :(\n");
	}
	printf("the thread Id is %d\n",thread_id);
	WaitForSingleObject(Thread0,INFINITE);
	Sleep(10);
	GetExitCodeThread(Thread0, &exitcode);
	printf("exit code from thread is %ld",&exitcode);
	CloseHandle(Thread0);
	return 0;
}

int PrintingHello (int i)
{
	int j;
	errno_t err = -1;
	FILE *output_file=NULL;
	Sleep(100);
	err = fopen_s(&output_file, "c:\temp\threadPrintingHellow.txt", "w");
	if (err != 0)
	{
		return 1;
	}
	
	fprintf(output_file,"I got the number %d %d\n", &i, GetCurrentThreadId());
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
