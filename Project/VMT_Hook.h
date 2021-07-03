#pragma once

#ifndef VMTHook_H
#define VMTHook_H

#include <cstdint>
#include <Windows.h>
#include <unordered_map>
#include "FindPattern.h"

class VMTHook
{
public:
	VMTHook(void* classptr)
	{
		if (!classptr) return;

		this->m_class_pointer = reinterpret_cast<void***>(classptr);
		m_old_vmt = *m_class_pointer;

		size_t table_size = 0;
		while (m_old_vmt[table_size] && Tools::IsCodePtr(m_old_vmt[table_size]))
			table_size++;

		m_new_vmt = new void* [table_size + 1];
		memcpy(&m_new_vmt[1], m_old_vmt, sizeof(void*) * table_size);
		m_new_vmt[0] = m_old_vmt[-1];

		*m_class_pointer = &m_new_vmt[1];
	}

	~VMTHook()
	{
		*m_class_pointer = m_old_vmt;
		delete[] m_new_vmt;
	}

	template<typename fn = void*>
	fn Hook(size_t index, void* new_function)
	{
		if (new_function)
			m_new_vmt[index + 1] = new_function;
		return reinterpret_cast<fn>(m_old_vmt[index]);
	}

private:
	void*** m_class_pointer = nullptr;
	void** m_old_vmt = nullptr;
	void** m_new_vmt = nullptr;
};

#endif