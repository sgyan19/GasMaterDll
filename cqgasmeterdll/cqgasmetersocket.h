#pragma once


using std::string;

namespace cqgasmeter
{
	class CqGasMeterSocket
	{
	private:
		CqGasMeterSocket();
		~CqGasMeterSocket();
		bool isSupported;
		string SendCommandCore(const string *lpCommand, int *lpCode);

	public:
		static CqGasMeterSocket *getInstance();
		string SendCommand(const string *lpCommand, int *lpCode);
	};
}