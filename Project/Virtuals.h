#pragma once

#ifndef Virtuals_H
#define Virtuals_H

template <typename Type = void*>
Type Get_vFunction(void* pThis, size_t INDEX)
{
	return (*reinterpret_cast<Type**>(pThis))[INDEX];
}

#include <cstddef>

namespace VirtualMethod
{
    template <typename T, std::size_t Idx, typename ...Args>
    constexpr T call(void* classBase, Args... args) noexcept
    {
        return (*reinterpret_cast<T(__thiscall***)(void*, Args...)>(classBase))[Idx](classBase, args...);
    }
}

#define VIRTUAL_METHOD(returnType, name, idx, args, argsRaw) \
returnType name args noexcept \
{ \
    return VirtualMethod::call<returnType, idx>argsRaw; \
}

#endif