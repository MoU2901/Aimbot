#pragma once

class Process
{
	HANDLE process = INVALID_HANDLE_VALUE;
public:
	Process(DWORD pid)
	{
		process = OpenProcess(PROCESS_VM_READ, false, pid);
	}
	Process(DWORD pid, bool write)
	{
		if(write == true)
			process = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, pid);
		else
			process = OpenProcess(PROCESS_VM_READ, false, pid);
	}

	~Process()
	{
		if (process != INVALID_HANDLE_VALUE)
			CloseHandle(process);
	}

	HANDLE get()
	{
		return process;
	}
};