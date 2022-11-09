// key.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/registry/key.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Create_registry_key
_NODISCARD HKEY _Create_registry_key(
    const predefined_registry_key _Key, const wchar_t* const _Subkey) noexcept {
    if (!this_process::is_elevated()) {
        return nullptr;
    }

    if (_Registry_key_exists(_Key, _Subkey)) { // key already exists
        return nullptr;
    }

    HKEY _Result;
    return ::RegCreateKeyExW(
        reinterpret_cast<HKEY>(_Key), _Subkey, 0, nullptr, 0, static_cast<DWORD>(registry_access::all),
            nullptr, &_Result, nullptr) == ERROR_SUCCESS ? _Result : nullptr;
}

// FUNCTION _Open_registry_key
_NODISCARD HKEY _Open_registry_key(const predefined_registry_key _Key,
    const wchar_t* const _Subkey, const registry_access _Access) noexcept {
    if (_Access == registry_access::all && !this_process::is_elevated()) {
        return nullptr;
    }

    HKEY _Result;
    return ::RegOpenKeyExW(reinterpret_cast<HKEY>(_Key), _Subkey, 0,
        static_cast<DWORD>(_Access), &_Result) == ERROR_SUCCESS ? _Result : nullptr;
}

// FUNCTION _Registry_key_exists
_NODISCARD bool _Registry_key_exists(
    const predefined_registry_key _Key, const wchar_t* const _Subkey) noexcept {
    HKEY _Handle = _Open_registry_key(_Key, _Subkey, registry_access::read); // the lowest access
    if (_Handle) {
        ::RegCloseKey(_Handle);
        return true;
    } else {
        return false;
    }
}

// FUNCTION registry_key constructors/destructor
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26495) // C26495: _Mycache is uninitialized (intentionally)
#endif // _MSC_VER
registry_key::registry_key() noexcept : _Myptr(nullptr) {}

registry_key::registry_key(registry_key& _Other) noexcept {
    *this = _Other;
}

registry_key::registry_key(registry_key&& _Other) noexcept {
    *this = _STD move(_Other);
}

registry_key::registry_key(const predefined_registry_key _Key,
    const wchar_t* const _Subkey, const registry_access _Access) noexcept : _Myptr(nullptr) {
    (void) open(_Key, _Subkey, _Access);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

registry_key::~registry_key() noexcept {
    close();
}

// FUNCTION registry_key::operator=
registry_key& registry_key::operator=(registry_key& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myptr        = _Other._Myptr;
        _Other._Myptr = nullptr;
        if (_Myptr) { // some key is open, copy cached data
            _Mycache        = _Other._Mycache;
            _Other._Mycache = _Cached_data{};
        }
    }

    return *this;
}

registry_key& registry_key::operator=(registry_key&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myptr        = _Other._Myptr;
        _Other._Myptr = nullptr;
        if (_Myptr) { // some key is open, copy cached data
            _Mycache        = _STD move(_Other._Mycache);
            _Other._Mycache = _Cached_data{};
        }
    }

    return *this;
}

// FUNCTION registry_key::operator HKEY
_NODISCARD registry_key::operator HKEY() noexcept {
    return _Myptr;
}

// FUNCTION registry_key::operator const HKEY
_NODISCARD registry_key::operator const HKEY() const noexcept {
    return _Myptr;
}

// FUNCTION registry_key::create
_NODISCARD bool registry_key::create(
    const predefined_registry_key _Key, const wchar_t* const _Subkey) noexcept {    
    if (_Myptr) { // some key is already open
        return false;
    }

    _Myptr = _Create_registry_key(_Key, _Subkey);
    if (_Myptr) { // update cached data
        _Mycache._Key    = _Key;
        _Mycache._Subkey = _Subkey;
        return true;
    } else {
        return false;
    }
}

// FUNCTION registry_key::open
_NODISCARD bool registry_key::open(const predefined_registry_key _Key,
    const wchar_t* const _Subkey, const registry_access _Access) noexcept {
    if (_Myptr) { // some key is already open
        return false;
    }

    _Myptr = _Open_registry_key(_Key, _Subkey, _Access);
    if (_Myptr) { // update cached data
        _Mycache._Key    = _Key;
        _Mycache._Subkey = _Subkey;
        return true;
    } else {
        return false;
    }
}

// FUNCTION registry_key::remove
_NODISCARD bool registry_key::remove() noexcept {
    if (!_Myptr) { // no key is open
        return false;
    }

    const HKEY _Key = reinterpret_cast<HKEY>(_Mycache._Key);
    if (is_tree()) { // remove a key with the subkeys
        if (::RegDeleteTreeW(
            _Key, _Mycache._Subkey) == ERROR_SUCCESS) { // second argument cannot be a null-pointer
            close();
            return true;
        }
    } else { // remove a single key
        if (::RegDeleteKeyW(
            _Key, _Mycache._Subkey) == ERROR_SUCCESS) { // second argument cannot be a null-pointer
            close();
            return true;
        }
    }

    return false;
}

// FUNCTION registry_key::close
void registry_key::close() noexcept {
    if (_Myptr) {
        ::RegCloseKey(_Myptr);
        _Myptr   = nullptr;
        _Mycache = _Cached_data{}; // clear cache
    }
}

// FUNCTION registry_key::is_open
_NODISCARD bool registry_key::is_open() const noexcept {
    return _Myptr != nullptr;
}

// FUNCTION registry_key::is_tree
_NODISCARD bool registry_key::is_tree() const noexcept {
    if (!_Myptr) { // no key handle is open
        return false;
    }

    DWORD _Count;
    if (::RegQueryInfoKeyW(_Myptr, nullptr, nullptr, nullptr, &_Count, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
        return false;
    }

    return _Count > 0;
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD