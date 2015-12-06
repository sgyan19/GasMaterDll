#include "stdafx.h"
#include "struct.h"
#include "cqgasmetercore.h"
#include "cqgasmetersocket.h"
#include "cqgasmeteradb.h"
#include "cJSON.h"
#pragma comment(lib,"cJSON.lib")

#define BUFFER_LENGTH 10240

namespace cqgasmeter
{
	CqGasMeterCore::CqGasMeterCore()
	{
		lpCriticalSection = new CRITICAL_SECTION;
		head = NULL;
		tail = NULL;
		InitializeCriticalSection(lpCriticalSection);
	}

	CqGasMeterCore::~CqGasMeterCore()
	{
		FileNameNode *current = head;
		while (NULL != current)
		{
			string *lpFileName = current->lpFileName;
			delete lpFileName;
			FileNameNode *next = current->next;
			delete current;
			current = next;
		}
		if (NULL != lpMobilePath)
		{
			delete lpMobilePath;
		}
		DeleteCriticalSection(lpCriticalSection);
		delete lpCriticalSection;
	}

	CqGasMeterCore *CqGasMeterCore::getInstance()
	{
		static CqGasMeterCore cqGasMeterCore;
		return &cqGasMeterCore;
	}

	int CqGasMeterCore::Init()
	{
		if (NULL != lpMobilePath)
		{
			return OK;
		}

		cJSON *commandJSON;
		commandJSON = cJSON_CreateObject();
		cJSON_AddNumberToObject(commandJSON, "type", COMMAND_LIST);
		string command(FilterJsonEnter(cJSON_Print(commandJSON)));
		cJSON_Delete(commandJSON);
		int code;
		string resultLine = CqGasMeterSocket::getInstance()->SendCommand(&command, &code);
		if (OK == code)
		{
			cJSON *result = cJSON_Parse(resultLine.c_str());
			cJSON *dirJSON = cJSON_GetObjectItem(result, "dir");
			if (NULL == dirJSON)
			{
				cJSON_Delete(result);
				return MOBILE_DATA_ERROR;
			}
			char *dir = dirJSON->valuestring;
			if (NULL == dir || 0 == strlen(dir))
			{
				cJSON_Delete(result);
				return MOBILE_DATA_ERROR;
			}
			cJSON *listJSON = cJSON_GetObjectItem(result, "list");
			if (NULL == listJSON)
			{
				cJSON_Delete(result);
				return MOBILE_DATA_ERROR;
			}
			int size = cJSON_GetArraySize(listJSON);
			for (int i = 0; i < size; i++)
			{
				cJSON *nameJSON = cJSON_GetArrayItem(listJSON, i);
				if (NULL == nameJSON)
				{
					cJSON_Delete(result);
					return MOBILE_DATA_ERROR;
				}
				char *name = nameJSON->valuestring;
				if (NULL == name)
				{
					cJSON_Delete(result);
					return MOBILE_DATA_ERROR;
				}
				if (NULL == tail)
				{
					head = tail = new FileNameNode;
					tail->lpFileName = new string(name);
					tail->next = NULL;
				}
				else
				{
					tail = tail->next = new FileNameNode;
					tail->lpFileName = new string(name);
					tail->next = NULL;
				}
			}
			lpMobilePath = new string(dir);
			cJSON_Delete(result);
			return OK;
		}

		return code;
	}

	bool CqGasMeterCore::CheckLinkList(const string *lpName)
	{
		FileNameNode *current = head;
		while (NULL != current)
		{
			if (current->lpFileName->compare(*lpName) == 0)
			{
				return true;
			}
			current = current->next;
		}
		return false;
	}

	void CqGasMeterCore::AddLinkList(const string *lpName)
	{
		if (NULL == tail)
		{
			head = tail = new FileNameNode;
			tail->lpFileName = new string(*lpName);
			tail->next = NULL;
		}
		else
		{
			tail = tail->next = new FileNameNode;
			tail->lpFileName = new string(*lpName);
			tail->next = NULL;
		}
	}


	string CqGasMeterCore::FilterJsonEnter(const char *commond) 
	{
		char buffer[1024];
		int bIndex = 0;
		for (size_t i = 0; ; i++)
		{
			if (commond[i] == '\n' || commond[i] == '\t') 
			{
				continue;
			}
			if (commond[i] == '\0') {
				buffer[bIndex++] = '\n';
				buffer[bIndex++] = '\0';
				break;
			}
			buffer[bIndex++] = commond[i];
		}
		return string(buffer);
	}

	void CqGasMeterCore::DeleteLinkList(const string *lpName)
	{
		FileNameNode *current = head;
		FileNameNode *previous = NULL;
		FileNameNode *next = NULL;
		while (NULL != current)
		{
			next = current->next;
			if (current->lpFileName->compare(*lpName) == 0)
			{
				delete current->lpFileName;
				delete current;
				if (NULL == previous)
				{
					head = next;
				}
				else
				{
					previous->next = next;
				}
				if (NULL == next)
				{
					tail = previous;
				}
				return;
			}
			previous = current;
			current = next;
		}
	}

	int CqGasMeterCore::PushFileV2(const string *lpPathName)
	{
		if (NULL == lpPathName || lpPathName->size() == 0)
		{
			return PARAM_ERROR;
		}
		char fname[BUFFER_LENGTH];
		char ext[BUFFER_LENGTH];
		int n = _splitpath_s(lpPathName->c_str(), NULL, 0, NULL, 0, fname, BUFFER_LENGTH, ext, BUFFER_LENGTH);
		if (0 != n)
		{
			return PARAM_ERROR;
		}
		string name = string(fname) + string(ext);
		EnterCriticalSection(lpCriticalSection);
		n = Init();
		if (OK != n)
		{
			LeaveCriticalSection(lpCriticalSection);
			return n;
		}
		cJSON *commandPushStartJSON;
		commandPushStartJSON = cJSON_CreateObject();
		cJSON_AddNumberToObject(commandPushStartJSON, "type", COMMAND_PUSH_START);
		cJSON_AddStringToObject(commandPushStartJSON, "name", name.c_str());
		string commandPushStart(FilterJsonEnter(cJSON_Print(commandPushStartJSON)));
		cJSON_Delete(commandPushStartJSON);
		cJSON *commandPushEndJSON;
		commandPushEndJSON = cJSON_CreateObject();
		cJSON_AddNumberToObject(commandPushEndJSON, "type", COMMAND_PUSH_END);
		cJSON_AddStringToObject(commandPushEndJSON, "name", name.c_str());
		string commandPushEnd(FilterJsonEnter(cJSON_Print(commandPushEndJSON)));
		cJSON_Delete(commandPushEndJSON);
		int code;
		CqGasMeterSocket::getInstance()->SendCommand(&commandPushStart, &code);
		if (OK != code)
		{
			LeaveCriticalSection(lpCriticalSection);
			return code;
		}
		bool s = CqGasMeterADB::getInstance()->PushFile(lpPathName, lpMobilePath);
		if (false == s)
		{
			CqGasMeterSocket::getInstance()->SendCommand(&commandPushEnd, &code);
			LeaveCriticalSection(lpCriticalSection);
			return SOCKET_ADB_ERROR;
		}
		//AddLinkList(&name);
		CqGasMeterSocket::getInstance()->SendCommand(&commandPushEnd, &code);
		LeaveCriticalSection(lpCriticalSection);
		return OK;
	}

	int CqGasMeterCore::PushFile(const string *lpPathName)
	{
		if (NULL == lpPathName || lpPathName->size() == 0)
		{
			return PARAM_ERROR;
		}
		char fname[BUFFER_LENGTH];
		char ext[BUFFER_LENGTH];
		int n = _splitpath_s(lpPathName->c_str(), NULL, 0, NULL, 0, fname, BUFFER_LENGTH, ext, BUFFER_LENGTH);
		if (0 != n)
		{
			return PARAM_ERROR;
		}
		string name = string(fname) + string(ext);
		EnterCriticalSection(lpCriticalSection);
		n = Init();
		if (OK != n)
		{
			LeaveCriticalSection(lpCriticalSection);
			return n;
		}
		if (CheckLinkList(&name))
		{
			LeaveCriticalSection(lpCriticalSection);
			return PUSH_FILE_EXISTS;
		}
		cJSON *commandPushStartJSON;
		commandPushStartJSON = cJSON_CreateObject();
		cJSON_AddNumberToObject(commandPushStartJSON, "type", COMMAND_PUSH_START);
		cJSON_AddStringToObject(commandPushStartJSON, "name", name.c_str());
		string commandPushStart(FilterJsonEnter(cJSON_Print(commandPushStartJSON)));
		cJSON_Delete(commandPushStartJSON);
		cJSON *commandPushEndJSON;
		commandPushEndJSON = cJSON_CreateObject();
		cJSON_AddNumberToObject(commandPushEndJSON, "type", COMMAND_PUSH_END);
		cJSON_AddStringToObject(commandPushEndJSON, "name", name.c_str());
		string commandPushEnd(FilterJsonEnter(cJSON_Print(commandPushEndJSON)));
		cJSON_Delete(commandPushEndJSON);
		int code;
		CqGasMeterSocket::getInstance()->SendCommand(&commandPushStart, &code);
		if (OK != code)
		{
			LeaveCriticalSection(lpCriticalSection);
			return code;
		}
		bool s = CqGasMeterADB::getInstance()->PushFile(lpPathName, lpMobilePath);
		if (false == s)
		{
			CqGasMeterSocket::getInstance()->SendCommand(&commandPushEnd, &code);
			LeaveCriticalSection(lpCriticalSection);
			return SOCKET_ADB_ERROR;
		}
		//AddLinkList(&name);
		CqGasMeterSocket::getInstance()->SendCommand(&commandPushEnd, &code);
		LeaveCriticalSection(lpCriticalSection);
		return OK;
	}

	int CqGasMeterCore::PullFile(const string *lpPullName, const string *lpPathName)
	{
		if (NULL == lpPullName || lpPullName->size() == 0 || NULL == lpPathName || lpPathName->size() == 0)
		{
			return PARAM_ERROR;
		}
		EnterCriticalSection(lpCriticalSection);
		int n = Init();
		if (OK != n)
		{
			LeaveCriticalSection(lpCriticalSection);
			return n;
		}
		char buffer[1024];
		if (lpMobilePath->at(lpMobilePath->size() - 1) == '/')
		{
			sprintf_s(buffer,1024, "%s%s", lpMobilePath->data(), lpPullName->data());
		}
		else
		{
			sprintf_s(buffer, 1024, "%s/%s", lpMobilePath->data(), lpPullName->data());
		}
		string lpPullPathName(buffer);
		bool s = CqGasMeterADB::getInstance()->PullFile(&lpPullPathName, lpPathName);
		if (false == s)
		{
			LeaveCriticalSection(lpCriticalSection);
			return SOCKET_ADB_ERROR;
		}
		LeaveCriticalSection(lpCriticalSection);
		return OK;
	}

	int CqGasMeterCore::DeleteFile(const string *lpDeleteName)
	{
		if (NULL == lpDeleteName || lpDeleteName->size() == 0)
		{
			return PARAM_ERROR;
		}
		EnterCriticalSection(lpCriticalSection);
		int n = Init();
		if (OK != n)
		{
			LeaveCriticalSection(lpCriticalSection);
			return n;
		}
		if (!CheckLinkList(lpDeleteName))
		{
			LeaveCriticalSection(lpCriticalSection);
			return DELETE_FILE_NOT_EXISTS;
		}
		cJSON *commandDeleteJSON;
		commandDeleteJSON = cJSON_CreateObject();
		cJSON_AddNumberToObject(commandDeleteJSON, "type", COMMAND_DELETE);
		cJSON_AddStringToObject(commandDeleteJSON, "name", lpDeleteName->c_str());
		string commandDelete(FilterJsonEnter(cJSON_Print(commandDeleteJSON)));
		cJSON_Delete(commandDeleteJSON);
		int code;
		CqGasMeterSocket::getInstance()->SendCommand(&commandDelete, &code);
		LeaveCriticalSection(lpCriticalSection);
		return code;
	}

	string CqGasMeterCore::GetFilesList(int *lpCode)
	{
		if (NULL == lpCode)
		{
			*lpCode = PARAM_ERROR;
			return "";
		}
		EnterCriticalSection(lpCriticalSection);
		int n = Init();
		if (OK != n)
		{
			LeaveCriticalSection(lpCriticalSection);
			*lpCode = n;
			return "";
		}
		cJSON *arrayJSON = cJSON_CreateArray();
		FileNameNode *current = head;
		while (NULL != current)
		{
			cJSON_AddItemToArray(arrayJSON, cJSON_CreateString(current->lpFileName->c_str()));
			current = current->next;
		}
		string filesList(cJSON_Print(arrayJSON));
		cJSON_Delete(arrayJSON);
		*lpCode = OK;
		LeaveCriticalSection(lpCriticalSection);
		return filesList;
	}
}