#include "Netvars.h"

intptr_t GetOffset(RecvTable* table, const char* tableName, const char* netvarName)
{
	for (int i = 0; i < table->m_nProps; i++)
	{
		RecvProp prop = table->m_pProps[i];

		if (!_stricmp(prop.m_pVarName, netvarName))
		{
			return prop.m_Offset;
		}

		if (prop.m_pDataTable)
		{
			intptr_t offset = GetOffset(prop.m_pDataTable, tableName, netvarName);

			if (offset)
			{
				return offset + prop.m_Offset;
			}
		}
	}
	return 0;
}

intptr_t GetNetVarOffset(const char* tableName, const char* netvarName, ClientClass* clientClass)
{
	ClientClass* currNode = clientClass;

	for (auto currNode = clientClass; currNode; currNode = currNode->m_pNext)
	{
		if (!_stricmp(tableName, currNode->m_pRecvTable->m_pNetTableName))
		{
			return GetOffset(currNode->m_pRecvTable, tableName, netvarName);
		}
	}

	return 0;
}