#pragma once


using std::string;

namespace cqgasmeter
{
	struct FileNameNode
	{
		string *lpFileName;
		FileNameNode *next;
	};

	class CqGasMeterCore
	{
	private:
		CRITICAL_SECTION *lpCriticalSection;
		FileNameNode *head;
		FileNameNode *tail;
		string *lpMobilePath;
		CqGasMeterCore();
		~CqGasMeterCore();
		int Init();
		bool CheckLinkList(const string *lpName);
		void AddLinkList(const string *lpName);
		void DeleteLinkList(const string *lpName);
		string FilterJsonEnter(const char *commond);
	public:
		static CqGasMeterCore *getInstance();

		int PushFileV2(const string *lpPathName);
		int PushFile(const string *lpPathName);
		int PullFile(const string *lpPullName, const string *lpPathName);
		int DeleteFile(const string *lpDeleteName);
		string GetFilesList(int *lpCode);

		enum {
			COMMAND_LIST = 0,
			COMMAND_PUSH_START,
			COMMAND_PUSH_END,
			COMMAND_INFO,
			COMMAND_DELETE
		};
	};
}