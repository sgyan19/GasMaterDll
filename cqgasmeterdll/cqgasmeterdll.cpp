// cqgasmeterdll.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "cqgasmeterdll.h"
#include "struct.h"
#include "cqgasmetercore.h"

CQGASMETERDLL_API int PushFile(const char *lpPathName)
{
	int result = cqgasmeter::CqGasMeterCore::getInstance()->PushFileV2(&string(lpPathName));
	return result;
}

CQGASMETERDLL_API int PullFile(const char *lpPathName) {
	return cqgasmeter::CqGasMeterCore::getInstance()->PullFile(&string("GasMeterB.json"), &string(lpPathName));
}