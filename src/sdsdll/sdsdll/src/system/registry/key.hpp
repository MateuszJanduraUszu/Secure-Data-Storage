// key.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_REGISTRY_KEY_HPP_
#define _SDSDLL_SYSTEM_REGISTRY_KEY_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/type_traits.hpp>
#include <handleapi.h>
#include <minwindef.h>
#include <system/execution/process.hpp>
#include <type_traits>
#include <winerror.h>
#include <winreg.h>

_SDSDLL_BEGIN
// ENUM CLASS predefined_registry_key
enum class predefined_registry_key : unsigned long {
    none           = 0,
    classes_root   = 0x8000'0000, // HKEY_CLASSES_ROOT
    current_user   = 0x8000'0001, // HKEY_CURRENT_USER
    local_machine  = 0x8000'0002, // HKEY_LOCAL_MACHINE
    users          = 0x8000'0003, // HKEY_USERS
    current_config = 0x8000'0005 // HKEY_CURRENT_CONFIG
};

// ENUM CLASS registry_access
enum class registry_access : unsigned long {
    none = 0,
    read = (0x0002'0000 | 0x0001 | 0x0008 | 0x0010) & ~0x0010'0000, // KEY_READ
    all  = (0x001F'0000 | 0x0001 | 0x0002 | 0x0004 | 0x0008
        | 0x0010 | 0x0020) & ~0x0010'0000 // KEY_ALL_ACCESS
};

// FUNCTION _Create_registry_key
extern _NODISCARD HKEY _Create_registry_key(
    const predefined_registry_key _Key, const wchar_t* const _Subkey) noexcept;

// FUNCTION _Open_registry_key
extern _NODISCARD HKEY _Open_registry_key(const predefined_registry_key _Key,
    const wchar_t* const _Subkey, const registry_access _Access) noexcept;

// FUNCTION _Registry_key_exists
extern _NODISCARD bool _Registry_key_exists(
    const predefined_registry_key _Key, const wchar_t* const _Subkey) noexcept;

// CLASS registry_key
class _SDSDLL_API registry_key { // non-copyable registry key wrapper
public:
    registry_key() noexcept;
    registry_key(registry_key& _Other) noexcept;
    registry_key(registry_key&& _Other) noexcept;
    ~registry_key() noexcept;

    registry_key(const predefined_registry_key _Key, const wchar_t* const _Subkey,
        const registry_access _Access = registry_access::all) noexcept;

    registry_key& operator=(registry_key& _Other) noexcept;
    registry_key& operator=(registry_key&& _Other) noexcept;

    registry_key(const registry_key&) = delete;
    registry_key& operator=(const registry_key&) = delete;

    // returns a handle to an open/created key
    _NODISCARD operator HKEY() noexcept;

    // returns a handle to an open/created key for view
    _NODISCARD operator const HKEY() const noexcept;

    // tries to create a new key
    _NODISCARD bool create(const predefined_registry_key _Key, const wchar_t* const _Subkey) noexcept;

    // tries to open an existing key
    _NODISCARD bool open(const predefined_registry_key _Key,
        const wchar_t* const _Subkey, const registry_access _Access = registry_access::all) noexcept;

    // tries to remove an open/created key
    _NODISCARD bool remove() noexcept;

    // closes an open/created key
    void close() noexcept;

    // checks if any key is open
    _NODISCARD bool is_open() const noexcept;
    
    // checks if an open/created key has any subkey
    _NODISCARD bool is_tree() const noexcept;

private:
    struct _Cached_data { // stores the current key location (required by remove())
        predefined_registry_key _Key;
        const wchar_t* _Subkey;
    };

    HKEY _Myptr;
    union {
        _Cached_data _Mycache; // lazy initialization
    };
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_REGISTRY_KEY_HPP_