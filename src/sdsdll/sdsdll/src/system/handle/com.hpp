// com.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_HANDLE_COM_HPP_
#define _SDSDLL_SYSTEM_HANDLE_COM_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <combaseapi.h>
#include <core/api.hpp>
#include <objbase.h>

_SDSDLL_BEGIN
// STRUCT com_instance
struct _SDSDLL_API com_instance { // initializes and uninitializes a COM instance for the current thread
    // initializes a COM instance
    static void initialize() noexcept;

    // uninitializes the COM instance
    static void uninitialize() noexcept;
};

// CLASS com_instance_guard
class _SDSDLL_API com_instance_guard {
public:
    com_instance_guard() noexcept;
    ~com_instance_guard() noexcept;

    com_instance_guard(const com_instance_guard&) = delete;
    com_instance_guard& operator=(const com_instance_guard&) = delete;
};

// FUNCTION TEMPLATE release_com_object
template <class _Ty>
constexpr void release_com_object(_Ty* _Obj) noexcept {
    if (_Obj) {
        _Obj->Release();
        _Obj = nullptr;
    }
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_HANDLE_COM_HPP_