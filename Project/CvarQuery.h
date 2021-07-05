#ifndef CvarQuery_H
#define CvarQuery_H

#include <string>

class DesiredConVarsValueInfo
{
	std::string Name, Value; int Status;
public:
	inline void Set(const char* name, const char* value, int status)
	{
		Name = name, Value = value, Status = status;
	}

	inline void Set(const char* value, int status)
	{
		Value = value, Status = status;
	}

	inline const char* GetName()
	{
		return Name.c_str();
	}

	inline const char* GetValue()
	{
		return Value.c_str();
	}

	inline int GetStatus()
	{
		return Status;
	}
};

#endif
