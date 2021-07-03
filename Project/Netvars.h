#pragma once

#ifndef Netvars_H
#define Netvars_H

#include <cstdint>
#include <string.h>
#include "Vector.h"
#include "Virtuals.h"

#include "FindPattern.h"

class RecvProp;

class RecvTable
{
public:
	RecvProp* m_pProps;
	int				m_nProps;
	void* m_pDecoder;
	const char* m_pNetTableName;
	bool			m_bInitialized;
	bool			m_bInMainList;
};

class RecvProp
{
public:

	const char* m_pVarName;
	int m_RecvType;
	int				m_Flags;
	int				m_StringBufferSize;

	bool			m_bInsideArray;

	const void* m_pExtraData;

	RecvProp* m_pArrayProp;
	void* m_ArrayLengthProxy;

	void* m_ProxyFn;
	void* m_DataTableProxyFn;	// For RDT_DataTable.

	RecvTable* m_pDataTable;		// For RDT_DataTable.
	int						m_Offset;

	int						m_ElementStride;
	int						m_nElements;

	const char* m_pParentArrayPropName;
};

typedef void* (*CreateClientClassFn)(int entnum, int serialNum);
typedef void* (*CreateEventFn)();

struct ClientClass
{
	CreateClientClassFn		m_pCreateFn;
	CreateEventFn			m_pCreateEventFn;
	char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	int						m_ClassID;

	const char* GetName()
	{
		return m_pNetworkName;
	}
};


typedef enum
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY, // Only encodes the XY of a vector, ignores Z
	DPT_String,
	DPT_Array,	// An array of the base types (can't be of datatables).
	DPT_DataTable,
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
	DPT_Quaternion,
#endif

#ifdef SUPPORTS_INT64
	DPT_Int64,
#endif

	DPT_NUMSendPropTypes

} SendPropType;

class DVariant
{
public:
	DVariant() { m_Type = DPT_Float; }
	DVariant(float val) { m_Type = DPT_Float; m_Float = val; }

	/*const char* ToString()
	{
		static char text[128];

		switch (m_Type)
		{
		case DPT_Int:
			snprintf(text, sizeof(text), "%i", m_Int);
			break;
		case DPT_Float:
			snprintf(text, sizeof(text), "%.3f", m_Float);
			break;
		case DPT_Vector:
			snprintf(text, sizeof(text), "(%.3f,%.3f,%.3f)",
				m_Vector[0], m_Vector[1], m_Vector[2]);
			break;
		case DPT_VectorXY:
			snprintf(text, sizeof(text), "(%.3f,%.3f)",
				m_Vector[0], m_Vector[1]);
			break;
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
		case DPT_Quaternion:
			Q_snprintf(text, sizeof(text), "(%.3f,%.3f,%.3f %.3f)",
				m_Vector[0], m_Vector[1], m_Vector[2], m_Vector[3]);
			break;
#endif
		case DPT_String:
			if (m_pString)
				return m_pString;
			else
				return "NULL";
			break;
		case DPT_Array:
			snprintf(text, sizeof(text), "Array");
			break;
		case DPT_DataTable:
			snprintf(text, sizeof(text), "DataTable");
			break;
#ifdef SUPPORTS_INT64
		case DPT_Int64:
			snprintf(text, sizeof(text), "%I64d", m_Int64);
			break;
#endif
		default:
			snprintf(text, sizeof(text), "DVariant type %i unknown", m_Type);
			break;
		}

		return text;
	}*/

	union
	{
		float	m_Float;
		int		m_Int;
		const char* m_pString;
		void* m_pData;	// For DataTables.
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
		float	m_Vector[4];
#else
		Vector	m_Vector;
#endif

#ifdef SUPPORTS_INT64
		int64	m_Int64;
#endif
	};
	SendPropType	m_Type;
};

class CRecvProxyData
{
public:
	const RecvProp* m_pRecvProp;		// The property it's receiving.

	DVariant		m_Value;			// The value given to you to store.

	int				m_iElement;			// Which array element you're getting.

	int				m_ObjectID;			// The object being referred to.
};

typedef void(__cdecl* RecvVarProxyFn) (const CRecvProxyData*, void*, void*);

class IBaseClientDLL
{
public:
	ClientClass* GetAllClasses()
	{
		return Get_vFunction<ClientClass* (__thiscall*)(void*)>(this, 8)(this);
	}

	bool WriteUsercmdDeltaToBuffer(void* buf, int from, int to, bool isnewcommand)
	{
		return Get_vFunction<bool(__thiscall*)(void*, void*, int, int, bool)>(this, 23)(this, buf, from, to, isnewcommand);
	}
};

extern intptr_t GetOffset(RecvTable* table, const char* tableName, const char* netvarName);

extern intptr_t GetNetVarOffset(const char* tableName, const char* netvarName, ClientClass* clientClass);

#define NETVAR(funcname, class_name, var_name, ...) \
auto funcname() -> std::add_lvalue_reference_t<__VA_ARGS__> \
{ \
    uintptr_t Offset = GetNetVarOffset(class_name, var_name, BaseClientDLL->GetAllClasses()); \
	return *reinterpret_cast<std::add_pointer_t<__VA_ARGS__>>((unsigned int)this + Offset); \
}

#endif