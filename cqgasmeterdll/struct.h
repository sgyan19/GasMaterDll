#pragma once
namespace cqgasmeter
{
	//������
	enum {
		OK = 0,
		SOCKET_NOT_SUPPORTED = 1,
		SOCKET_CREATE_ERROR = 2,
		SOCKET_CONNECT_ERROR = 3,
		SOCKET_SEND_ERROR = 4,
		SOCKET_ADB_ERROR = 5,
		MOBILE_DATA_ERROR = 6,
		PUSH_FILE_EXISTS = 7,
		PULL_FILE_NOT_EXISTS = 8,
		DELETE_FILE_NOT_EXISTS = 9,
		PARAM_ERROR = 10
	};
}