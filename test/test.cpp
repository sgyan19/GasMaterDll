// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\cqgasmeterdll\cqgasmeterdll.h"
#include <Windows.h>
int main()
{
	//return PushFile("d.txt");
	return PullFile("test.txt");
}

