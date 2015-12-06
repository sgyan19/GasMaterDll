#include "stdafx.h"
#include "cqgasmeteradb.h"

#define BUFFER_LENGTH 10240

namespace cqgasmeter
{
	CqGasMeterADB::CqGasMeterADB()
	{
		adbPath = new string("adb\\adb.exe");
		/*
		char buf[BUFFER_LENGTH];
		int length = GetModuleFileNameA(NULL, buf, BUFFER_LENGTH);
		if (0 == length)
		{
			adbPath = NULL;
		}
		else
		{
			adbPath = new string(string(buf, length) + "\\adb\\adb.exe");
		}
		*/
	}

	CqGasMeterADB::~CqGasMeterADB()
	{
		if (NULL != adbPath)
		{
			delete adbPath;
		}
	}

	CqGasMeterADB *CqGasMeterADB::getInstance()
	{
		static CqGasMeterADB cqGasMeterADB;
		return &cqGasMeterADB;
	}

	bool CqGasMeterADB::PushFile(const string *lpPathName, const string *lpPushPath)
	{
		if (NULL == adbPath)
		{
			return false;
		}

		string command = *adbPath + " push " + *lpPathName + " " + *lpPushPath + "/GasMeterA.json";

		PROCESS_INFORMATION processInformation = {0};
		STARTUPINFOA startupInfo = {0};
		if (false == CreateProcessA(NULL, command.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInformation);)
		{
			return false;
		}

		if (WAIT_OBJECT_0 != WaitForSingleObject(processInformation.hProcess, 10000))
		{
			return false;
		}

		DWORD exitCode;
		if (false == GetExitCodeProcess(processInformation.hProcess, &exitCode)
		{
			return false;
		}

		if (0 != exitCode)
		{
			return false;
		}

		return true;
	}

	bool CqGasMeterADB::PullFile(const string *lpPullPathName, const string *lpPathName)
	{
		if (NULL == adbPath)
		{
			return false;
		}

		string command = *adbPath + " pull " + *lpPullPathName + " " + *lpPathName;

		PROCESS_INFORMATION processInformation = {0};
		STARTUPINFOA startupInfo = {0};
		if (false == CreateProcessA(NULL, command.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInformation);)
		{
			return false;
		}

		if (WAIT_OBJECT_0 != WaitForSingleObject(processInformation.hProcess, 10000))
		{
			return false;
		}

		DWORD exitCode;
		if (false == GetExitCodeProcess(processInformation.hProcess, &exitCode)
		{
			return false;
		}

		if (0 != exitCode)
		{
			return false;
		}

		return true;
	}

	bool CqGasMeterADB::StartADB()
	{
		if (NULL == adbPath)
		{
			return false;
		}

		string command = *adbPath + " forward tcp:" + ADB_PORT_STR + " tcp:" +  MOBILE_PORT_STR;

		PROCESS_INFORMATION processInformation = {0};
		STARTUPINFOA startupInfo = {0};
		if (false == CreateProcessA(NULL, command.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInformation);)
		{
			return false;
		}

		if (WAIT_OBJECT_0 != WaitForSingleObject(processInformation.hProcess, 10000))
		{
			return false;
		}

		DWORD exitCode;
		if (false == GetExitCodeProcess(processInformation.hProcess, &exitCode)
		{
			return false;
		}

		if (0 != exitCode)
		{
			return false;
		}

		return true;
	}
}