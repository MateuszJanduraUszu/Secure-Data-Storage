// view.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_REGISTRY_VIEW_HPP_
#define _SDSDLL_SYSTEM_REGISTRY_VIEW_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/string_view.hpp>
#include <cstddef>
#include <cstdint>
#include <minwindef.h>
#include <string>
#include <system/registry/key.hpp>
#include <type_traits>
#include <winerror.h>
#include <winreg.h>

// STD types
using _STD wstring;

_SDSDLL_BEGIN
// ENUM CLASS registry_value_type
enum class registry_value_type : unsigned long {
    none             = 0, // REG_NONE
    string           = 1, // REG_SZ
    expand_string    = 2, // REG_EXPAND_SZ
    binary           = 3, // REG_BINARY
    dword            = 4, // REG_DWORD or REG_DWORD_LITTLE_ENDIAN
    dword_big_endian = 5, // REG_DWORD_BIG_ENDIAN
    link             = 6, // REG_LINK
    multi_string     = 7, // REG_MULTI_SZ
    qword            = 11 // REG_QWORD or REG_QWORD_LITTLE_ENDIAN
};

// FUNCTION _Is_string_type
extern _NODISCARD constexpr bool _Is_string_type(const registry_value_type _Type) noexcept;

// CLASS registry_manager
class registry_manager;

// CLASS registry_view_manager
class _SDSDLL_API registry_view_manager {
private:
    friend registry_manager;

public:
    registry_view_manager() noexcept;
    ~registry_view_manager() noexcept;

    registry_view_manager(const registry_view_manager&) = delete;
    registry_view_manager& operator=(const registry_view_manager&) = delete;

    explicit registry_view_manager(registry_key& _Key);
    explicit registry_view_manager(registry_key&& _Key);

    // returns the current key
    _NODISCARD registry_key& key() noexcept;

    // returns the current key for view
    _NODISCARD const registry_key& key() const noexcept;

    // reads the default value (converts to a string if integer)
    _NODISCARD wstring read_default_value();

    // reads the selected value (expected integer type)
    _NODISCARD uint32_t read_integer(const wchar_t* const _Name) noexcept;
    _NODISCARD uint32_t read_integer(const wstring_view _Name) noexcept;
    _NODISCARD uint32_t read_integer(const wstring& _Name);

    // reads the selected value (expected string type)
    _NODISCARD wstring read_string(const wchar_t* const _Name);
    _NODISCARD wstring read_string(const wstring_view _Name);
    _NODISCARD wstring read_string(const wstring& _Name);

private:
    // returns bytes count required for the string
    _NODISCARD DWORD _Get_buffer_size(const wchar_t* const _Name) const noexcept;
    
    registry_key _Mykey;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_REGISTRY_VIEW_HPP_