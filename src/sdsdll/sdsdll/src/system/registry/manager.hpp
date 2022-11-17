// manager.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_REGISTRY_MANAGER_HPP_
#define _SDSDLL_SYSTEM_REGISTRY_MANAGER_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/string_view.hpp>
#include <cstdint>
#include <string>
#include <system/registry/key.hpp>
#include <system/registry/view.hpp>
#include <type_traits>
#include <winerror.h>
#include <winnt.h>
#include <winreg.h>

// STD types
using _STD wstring;

_SDSDLL_BEGIN
// CLASS registry_manager
class _SDSDLL_API registry_manager : public registry_view_manager {
public:
    registry_manager() noexcept;
    ~registry_manager() noexcept;

    registry_manager(const registry_manager&) = delete;
    registry_manager& operator=(const registry_manager&) = delete;

    explicit registry_manager(registry_key& _Key);
    explicit registry_manager(registry_key&& _Key);

    // inserts a new key and writes an integer into it
    _NODISCARD bool insert_integer(const wchar_t* const _Name, const uint32_t _Value) noexcept;
    _NODISCARD bool insert_integer(const wstring_view _Name, const uint32_t _Value) noexcept;
    _NODISCARD bool insert_integer(const wstring& _Name, const uint32_t _Value);

    // inserts a new key and writes a string into it
    _NODISCARD bool insert_string(const wchar_t* const _Name, const wchar_t* const _Value) noexcept;
    _NODISCARD bool insert_string(const wstring_view _Name, const wstring_view _Value) noexcept;
    _NODISCARD bool insert_string(const wstring& _Name, const wstring& _Value);

    // inserts a new key and writes an integer into it
    _NODISCARD bool modify_integer(const wchar_t* const _Name, const uint32_t _New_value) noexcept;
    _NODISCARD bool modify_integer(const wstring_view _Name, const uint32_t _New_value) noexcept;
    _NODISCARD bool modify_integer(const wstring& _Name, const uint32_t _New_value);

    // inserts a new key and writes a string into it
    _NODISCARD bool modify_string(const wchar_t* const _Name, const wchar_t* const _New_value) noexcept;
    _NODISCARD bool modify_string(const wstring_view _Name, const wstring_view _New_value) noexcept;
    _NODISCARD bool modify_string(const wstring& _Name, const wstring& _New_value);
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_REGISTRY_MANAGER_HPP_