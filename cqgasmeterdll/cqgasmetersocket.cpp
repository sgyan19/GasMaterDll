#include "stdafx.h"
#include "cqgasmetersocket.h"
#include "struct.h"
#include "cqgasmeteradb.h"

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_LENGTH 10240

namespace cqgasmeter
{
	CqGasMeterSocket::CqGasMeterSocket()
	{
		WSADATA wsaData;
		if(NO_ERROR == WSAStartup(MAKEWORD(2,2), &wsaData))
		{
			isSupported = true;
		}
		else
		{
			isSupported = false;
		}
	}

	CqGasMeterSocket::~CqGasMeterSocket()
	{
		WSACleanup();
	}

	CqGasMeterSocket *CqGasMeterSocket::getInstance()
	{
		static CqGasMeterSocket cqGasMeterSocket;
		return &cqGasMeterSocket;
	}

	string CqGasMeterSocket::SendCommandCore(const string *lpCommand, int *lpCode)
	{
		if(false == isSupported)
		{
			*lpCode = SOCKET_NOT_SUPPORTED;
			return "";
		}

		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(INVALID_SOCKET == s)
		{
			*lpCode = SOCKET_CREATE_ERROR;
			return "";
		}

		sockaddr_in name;
		name.sin_family = AF_INET;
		name.sin_addr.s_addr = inet_addr("127.0.0.1");
		name.sin_port = htons(ADB_PORT);
		int i = connect(s, (SOCKADDR *)&name, sizeof(name));
		if(NO_ERROR != i)
		{
			closesocket(s);
			*lpCode = SOCKET_CONNECT_ERROR;
			return "";
		}

		string command = *lpCommand + "\n";
		if(SOCKET_ERROR == send(s, command.c_str(), (int)command.size(), 0))
		{
			closesocket(s);
			*lpCode = SOCKET_SEND_ERROR;
			return "";
		}
		if(SOCKET_ERROR == send(s, "\n", 1, 0))
		{
			closesocket(s);
			*lpCode = SOCKET_SEND_ERROR;
			return "";
		}

		string line = "";
		char recvbuf[BUFFER_LENGTH];
		int recvResult;
		do {
			recvResult = recv(s, recvbuf, BUFFER_LENGTH, 0);
			if (recvResult > 0)
			{
				int i = 0;
				for (; i < recvResult; i++)
				{
					if('\n' == recvbuf[i])
					{
						break;
					}
				}
				line = line + string(recvbuf, i);
				if ('\n' == recvbuf[i])
				{
					break;
				}
			}
		} while (recvResult > 0);

		closesocket(s);
		*lpCode = OK;
		return line;
	}

	string CqGasMeterSocket::SendCommand(const string *lpCommand, int *lpCode)
	{
		string line = SendCommandCore(lpCommand, lpCode);
		if (SOCKET_CONNECT_ERROR != *lpCode)
		{
			if (OK == *lpCode && line.compare("") == 0)
			{
				*lpCode = SOCKET_CONNECT_ERROR;
				return line;
			}
			return line;
		}
		
		if(CqGasMeterADB::getInstance()->StartADB())
		{
			Sleep(1000);
			string line = SendCommandCore(lpCommand, lpCode);
			if (OK == *lpCode && line.compare("") == 0)
			{
				*lpCode = SOCKET_CONNECT_ERROR;
				return line;
			}
			return line;
		}
		else
		{
			*lpCode = SOCKET_ADB_ERROR;
			return "";
		}
	}
}