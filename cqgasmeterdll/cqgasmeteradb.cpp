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

		string command = *adbPath + " push " + *lpPathName + " " + *lpPushPath + "/gasMeter.db";
		if(0 != system(command.c_str()))
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
		if(0 != system(command.c_str()))
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
		if(0 != system(command.c_str()))
		{
			return false;
		}

		return true;
	}
}