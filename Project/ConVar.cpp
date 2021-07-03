#include "SDK.h"

ConCommandBase* ConCommandBase::s_pConCommandBases = NULL;
static int			s_nCVarFlag = 0;
static int			s_nDLLIdentifier = -1;
static bool			s_bRegistered = false;

class CDefaultAccessor : public IConCommandBaseAccessor
{
public:
	virtual bool RegisterConCommandBase(ConCommandBase* pVar)
	{
		Cvar->RegisterConCommand(pVar);
		return true;
	}
};

static CDefaultAccessor s_DefaultAccessor;
IConCommandBaseAccessor* ConCommandBase::s_pAccessor = &s_DefaultAccessor;

void* Create_Console_Variable_Location;

void Create_Console_Variable(void* ECX, const char* Variable_Name, const char* Default_Value, int Flags, const char* Description, void* On_Value_Changed_Function)
{
	using Create_Console_Variable_Type = void(__thiscall*)(void* ECX,
		const char* Variable_Name,
		const char* Default_Value,
		int Flags,
		const char* Description,
		void* On_Value_Changed_Function);

	Create_Console_Variable_Type((unsigned long int)Create_Console_Variable_Location)(ECX, Variable_Name, Default_Value, Flags, Description, On_Value_Changed_Function);
}

void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor* pAccessor)
{
	if (s_bRegistered)
		return;

	s_bRegistered = true;
	s_nCVarFlag = nCVarFlag;
	s_nDLLIdentifier = Cvar->AllocateDLLIdentifier();

	ConCommandBase* pCur, * pNext;

	ConCommandBase::s_pAccessor = pAccessor ? pAccessor : &s_DefaultAccessor;
	pCur = ConCommandBase::s_pConCommandBases;
	while (pCur)
	{
		pNext = pCur->m_pNext;
		pCur->AddFlags(s_nCVarFlag);
		pCur->Init();
		pCur = pNext;
	}

	ConCommandBase::s_pConCommandBases = NULL;
}

//-------------------------------------------------------------
// ConVar methods
//-------------------------------------------------------------
ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags)
{
	Create(pName, pDefaultValue, flags);
}

ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString)
{
	Create(pName, pDefaultValue, flags, pHelpString);
}

ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax)
{
	Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax);
}

ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, FnChangeCallback_t callback)
{
	Create(pName, pDefaultValue, flags, pHelpString, false, 0.0, false, 0.0, callback);
}

ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback)
{
	Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax, callback);
}

ConVar::~ConVar(void)
{
	if (m_Value.m_pszString)
	{
		delete[] m_Value.m_pszString;
		m_Value.m_pszString = NULL;
	}
}


bool ConVar::IsFlagSet(int flag) const
{
	return (flag & m_pParent->m_nFlags) ? true : false;
}

const char* ConVar::GetHelpText(void) const
{
	return m_pParent->m_pszHelpString;
}

void ConVar::AddFlags(int flags)
{
	m_pParent->m_nFlags |= flags;
}

int ConVar::GetFlags(void) const
{
	return m_pParent->m_nFlags;
}

bool ConVar::IsRegistered(void) const
{
	return m_pParent->m_bRegistered;
}
bool ConVar::IsCommand(void) const
{
	return false;
}

const char* ConVar::GetName(void) const
{
	return m_pParent->m_pszName;
}
const char* ConVar::GetBaseName(void) const
{
	return m_pParent->m_pszName;
}
float ConVar::GetFloat(void) const
{
	return m_pParent->m_Value.m_fValue;
}
int ConVar::GetInt(void) const
{
	return m_pParent->m_Value.m_nValue;
}
const char* ConVar::GetString(void) const
{
	return m_pParent->m_Value.m_pszString;
}
int ConVar::GetSplitScreenPlayerSlot(void) const
{
	return 0;
}
void ConVar::InternalSetValue(const char* value)
{
	float fNewValue;
	char  tempVal[32];
	char* val;

	float flOldValue = m_Value.m_fValue;

	val = (char*)value;
	fNewValue = (float)atof(value);

	if (ClampValue(fNewValue))
	{
		val = tempVal;
	}

	// Redetermine value
	m_Value.m_fValue = fNewValue;
	m_Value.m_nValue = (int)(fNewValue);

	if (!(m_nFlags & (int)FCVAR_NEVER_AS_STRING))
	{
		ChangeStringValue(val, flOldValue);
	}
}
void ConVar::ChangeStringValue(const char* tempVal, float flOldValue)
{
	UNREFERENCED_PARAMETER(flOldValue);
	int len = strlen(tempVal) + 1;

	if (len > m_Value.m_StringLength)
	{
		if (m_Value.m_pszString)
		{
			delete[] m_Value.m_pszString;
		}

		m_Value.m_pszString = new char[len];
		m_Value.m_StringLength = len;
	}

	memcpy(m_Value.m_pszString, tempVal, len);

}
bool ConVar::ClampValue(float& value)
{
	if (m_bHasMin && (value < m_fMinVal))
	{
		value = m_fMinVal;
		return true;
	}

	if (m_bHasMax && (value > m_fMaxVal))
	{
		value = m_fMaxVal;
		return true;
	}

	return false;
}
void ConVar::InternalSetFloatValue(float fNewValue)
{
	if (fNewValue == m_Value.m_fValue)
		return;
	// Check bounds
	ClampValue(fNewValue);

	// Redetermine value
	float flOldValue = m_Value.m_fValue;
	m_Value.m_fValue = fNewValue;
	m_Value.m_nValue = (int)fNewValue;

	if (!(m_nFlags & (int)FCVAR_NEVER_AS_STRING))
	{
		char tempVal[32];
		ChangeStringValue(tempVal, flOldValue);
	}
}
void ConVar::InternalSetIntValue(int nValue)
{
	if (nValue == m_Value.m_nValue)
		return;

	float fValue = (float)nValue;
	if (ClampValue(fValue))
	{
		nValue = (int)(fValue);
	}

	float flOldValue = m_Value.m_fValue;
	m_Value.m_fValue = fValue;
	m_Value.m_nValue = nValue;

	if (!(m_nFlags & (int)FCVAR_NEVER_AS_STRING))
	{
		char tempVal[32];
		ChangeStringValue(tempVal, flOldValue);
	}
}

void ConVar::InternalSetColorValue(DWORD cValue)
{
	int color = (int)cValue;
	InternalSetIntValue(color);
}

void ConVar::Create(const char* pName, const char* pDefaultValue,
	int flags, const char* pHelpString, bool bMin, float fMin,
	bool bMax, float fMax, FnChangeCallback_t callback)
{
	static const char* empty_string = "";

	m_pParent = this;

	// Name should be static data
	m_pszDefaultValue = pDefaultValue ? pDefaultValue : empty_string;

	m_Value.m_StringLength = strlen(m_pszDefaultValue) + 1;
	m_Value.m_pszString = new char[m_Value.m_StringLength];
	memcpy(m_Value.m_pszString, m_pszDefaultValue, m_Value.m_StringLength);

	m_bHasMin = bMin;
	m_fMinVal = fMin;
	m_bHasMax = bMax;
	m_fMaxVal = fMax;

	m_fnChangeCallback = callback;
	m_Value.m_fValue = (float)atof(m_Value.m_pszString);
	m_Value.m_nValue = (int)m_Value.m_fValue;

	BaseClass::Create(pName, pHelpString, flags);
}
void ConVar::SetValue(const char* value)
{
	m_pParent->InternalSetValue(value);
}
void ConVar::SetValue(float value)
{
	m_pParent->InternalSetFloatValue(value);
}
void ConVar::SetValue(int value)
{
	m_pParent->InternalSetIntValue(value);
}
void ConVar::SetValue(DWORD value)
{
	m_pParent->InternalSetColorValue(value);
}
const char* ConVar::GetDefault(void) const
{
	return m_pParent->m_pszDefaultValue;
}

void ConVar::SetMemValue(const float Value, const int Flags)
{
	void* Shift = *(void**)((unsigned long int)this + 28);

	*(float*)((unsigned long int)Shift + 44) = Value;

	*(int*)((unsigned long int)Shift + 48) = (int)Value;

	if (Flags != -1) 
	{
		*(int*)((unsigned long int)this + 20) = Flags;
	}
}

SpoofedConvar::SpoofedConvar(const char* szCVar)
{
	m_pOriginalCVar = Cvar->FindVar(szCVar);
	Spoof();
}

SpoofedConvar::SpoofedConvar(ConVar* pCVar, const char* newname)
{
	m_pOriginalCVar = pCVar;
	if (!IsSpoofed() && m_pOriginalCVar)
	{
		//Save old name value and flags so we can restore the cvar lates if needed
		m_iOriginalFlags = m_pOriginalCVar->m_nFlags;
		strcpy(m_szOriginalName, m_pOriginalCVar->m_pszName);
		strcpy(m_szOriginalValue, m_pOriginalCVar->m_pszDefaultValue);

		sprintf_s(m_szDummyName, 128, "%s", newname[0] ? newname : m_szOriginalName);

		//Create the dummy cvar
		m_pDummyCVar = (ConVar*)malloc(sizeof(ConVar));
		if (!m_pDummyCVar) return;
		memcpy(m_pDummyCVar, m_pOriginalCVar, sizeof(ConVar));

		m_pDummyCVar->m_pNext = nullptr;
		//Register it
		Cvar->RegisterConCommand(m_pDummyCVar);

		//Fix "write access violation" bullshit
		DWORD dwOld;
		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, PAGE_READWRITE, &dwOld);

		//Rename the cvar
		strcpy((char*)m_pOriginalCVar->m_pszName, m_szDummyName);

		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, dwOld, &dwOld);

		SetFlags(FCVAR_NONE);
	}
}

SpoofedConvar::~SpoofedConvar()
{
	Restore();
}

bool SpoofedConvar::IsSpoofed()
{
	return m_pDummyCVar != nullptr;
}

void SpoofedConvar::Spoof()
{
	if (!IsSpoofed() && m_pOriginalCVar)
	{
		//Save old name value and flags so we can restore the cvar lates if needed
		m_iOriginalFlags = m_pOriginalCVar->m_nFlags;
		strcpy(m_szOriginalName, m_pOriginalCVar->m_pszName);
		strcpy(m_szOriginalValue, m_pOriginalCVar->m_pszDefaultValue);
		sprintf_s(m_szDummyName, 128, "d_%s", m_szOriginalName);

		m_pDummyCVar = (ConVar*)malloc(sizeof(ConVar));
		if (!m_pDummyCVar) return;
		memcpy(m_pDummyCVar, m_pOriginalCVar, sizeof(ConVar));

		m_pDummyCVar->m_pNext = nullptr;
		Cvar->RegisterConCommand(m_pDummyCVar);

		DWORD dwOld;
		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, PAGE_READWRITE, &dwOld);

		strcpy((char*)m_pOriginalCVar->m_pszName, m_szDummyName);

		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, dwOld, &dwOld);

		SetFlags(FCVAR_NONE);
	}
}

void SpoofedConvar::Restore()
{
	if (IsSpoofed())
	{
		DWORD dwOld;

		SetFlags(m_iOriginalFlags);
		SetString(m_szOriginalValue);

		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, PAGE_READWRITE, &dwOld);
		strcpy((char*)m_pOriginalCVar->m_pszName, m_szOriginalName);
		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, dwOld, &dwOld);

		Cvar->UnregisterConCommand(m_pDummyCVar);
		//Interfaces->Cvar->RegisterConCommand(m_pOriginalCVar);
		free(m_pDummyCVar);
		m_pDummyCVar = nullptr;
	}
}
void SpoofedConvar::SetFlags(int flags)
{
	if (IsSpoofed()) {
		m_pOriginalCVar->m_nFlags = flags;
	}
}

int SpoofedConvar::GetFlags()
{
	return m_pOriginalCVar->m_nFlags;
}

void SpoofedConvar::SetInt(int iValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(iValue);
	}
}

void SpoofedConvar::SetBool(bool bValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(bValue);
	}
}

void SpoofedConvar::SetFloat(float flValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(flValue);
	}
}
void SpoofedConvar::SetString(const char* szValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(szValue);
	}
}

//-------------------------------------------------------------
// ConCommandBase methods
//-------------------------------------------------------------
ConCommandBase::ConCommandBase(void)
{
	m_bRegistered = false;
	m_pszName = NULL;
	m_pszHelpString = NULL;

	m_nFlags = 0;
	m_pNext = NULL;
}
ConCommandBase::ConCommandBase(const char* pName, const char* pHelpString /*=0*/, int flags /*= 0*/)
{
	Create(pName, pHelpString, flags);
}
ConCommandBase::~ConCommandBase(void)
{
}

bool ConCommandBase::IsCommand(void) const
{
	return true;
}
int ConCommandBase::GetDLLIdentifier() const
{
	return s_nDLLIdentifier;
}
void ConCommandBase::Create(const char* pName, const char* pHelpString /*= 0*/, int flags /*= 0*/)
{
	static const char* empty_string = "";

	m_bRegistered = false;

	m_pszName = pName;
	m_pszHelpString = pHelpString ? pHelpString : empty_string;

	m_nFlags = flags;

	if (!(m_nFlags & (int)FCVAR_UNREGISTERED))
	{
		m_pNext = s_pConCommandBases;
		s_pConCommandBases = this;
	}
	else
	{
		m_pNext = NULL;
	}

	Init();
}
void ConCommandBase::Init()
{
	if (s_pAccessor)
	{
		s_pAccessor->RegisterConCommandBase(this);
	}
}
const char* ConCommandBase::GetName(void) const
{
	return m_pszName;
}

void ConVar::InstallChangeCallback(FnChangeCallback_t callback)
{
	Assert(!m_pParent->m_fnChangeCallback || !callback);
	m_pParent->m_fnChangeCallback = callback;

	if (m_pParent->m_fnChangeCallback)
	{
		// Call it immediately to set the initial value...
		m_pParent->m_fnChangeCallback(this, m_Value.m_pszString, m_Value.m_fValue);
	}
}

bool ConCommandBase::IsFlagSet(int flag) const
{
	return (flag & m_nFlags) ? true : false;
}
void ConCommandBase::AddFlags(int flags)
{
	m_nFlags |= flags;
}
void ConCommandBase::RemoveFlags(int flags)
{
	m_nFlags &= ~flags;
}
int ConCommandBase::GetFlags(void) const
{
	return m_nFlags;
}
const char* ConCommandBase::GetHelpText(void) const
{
	return m_pszHelpString;
}
bool ConCommandBase::IsRegistered(void) const
{
	return m_bRegistered;
}