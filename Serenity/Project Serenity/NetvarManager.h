#pragma once
#include "SDK.h"

class CNetVars
{
private:
	std::string lastTable = std::string();
public:
	CNetVars();
	~CNetVars();

	void GetOffset(const char* tableName, const char* propName, unsigned long* pOffset, int change = 0);

	RecvVarProxyFn GetProxyFunction(const char* tableName, const char* propName);

	void HookProp(const char* tableName, const char* propName, RecvVarProxyFn function);

private:
	int GetProp(const char* tableName, const char* propName, RecvProp** prop = 0);
	int GetProp(RecvTable* recvTable, const char* propName, RecvProp** prop = 0);

	RecvTable *GetTable(const char* tableName);

	std::vector<RecvTable*> m_tables;
};
