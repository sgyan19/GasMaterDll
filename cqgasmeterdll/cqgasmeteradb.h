#pragma once

#define MOBILE_PORT_STR "9919"
#define ADB_PORT_STR "9920"
#define ADB_PORT 9920

using std::string;

namespace cqgasmeter
{
	class CqGasMeterADB
	{
	private:
		CqGasMeterADB();
		~CqGasMeterADB();
		string *adbPath;

	public:
		static CqGasMeterADB *getInstance();
		bool PushFile(const string *lpPathName, const string *lpPushPath);
		bool PullFile(const string *lpPullPathName, const string *lpPathName);
		bool StartADB();
	};
}