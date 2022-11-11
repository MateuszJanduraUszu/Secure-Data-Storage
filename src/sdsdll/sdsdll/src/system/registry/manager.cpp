// manager.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/registry/manager.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION registry_manager constructors/destructor
registry_manager::registry_manager() noexcept : registry_view_manager() {}

registry_manager::registry_manager(registry_key& _Key) : registry_view_manager(_Key) {}

registry_manager::registry_manager(registry_key&& _Key) : registry_view_manager(_STD move(_Key)) {}

registry_manager::~registry_manager() noexcept {}

// FUNCTION register_manager::insert_integer
_NODISCARD bool registry_manager::insert_integer(
    const wchar_t* const _Name, const uint32_t _Value) noexcept {
    return modify_integer(_Name, _Value); // inserts if not exists
}

_NODISCARD bool registry_manager::insert_integer(
    const wstring_view _Name, const uint32_t _Value) noexcept {
    return modify_integer(_Name.data(), _Value); // inserts if not exists
}

_NODISCARD bool registry_manager::insert_integer(
    const wstring& _Name, const uint32_t _Value) {
    return modify_integer(_Name.c_str(), _Value); // inserts if not exists
}

// FUNCTION register_manager::insert_string
_NODISCARD bool registry_manager::insert_string(
    const wchar_t* const _Name, const wchar_t* const _Value) noexcept {
    return modify_string(_Name, _Value); // inserts if not exists
}

_NODISCARD bool registry_manager::insert_string(
    const wstring_view _Name, const wstring_view _Value) noexcept {
    return modify_string(_Name.data(), _Value.data()); // inserts if not exists
}

_NODISCARD bool registry_manager::insert_string(
    const wstring& _Name, const wstring& _Value) {
    return modify_string(_Name.c_str(), _Value.c_str()); // inserts if not exists
}

// FUNCTION registry_manager::modify_integer
_NODISCARD bool registry_manager::modify_integer(
    const wchar_t* const _Name, const uint32_t _New_value) noexcept {
    if (!_Mykey.is_open()) { // no key is open
        return false;
    }

    return ::RegSetValueExW(_Mykey, _Name, 0, REG_DWORD,
        reinterpret_cast<const unsigned char*>(&_New_value), sizeof(uint32_t)) == ERROR_SUCCESS;
}

_NODISCARD bool registry_manager::modify_integer(
    const wstring_view _Name, const uint32_t _New_value) noexcept {
    return modify_integer(_Name.data(), _New_value);
}

_NODISCARD bool registry_manager::modify_integer(
    const wstring& _Name, const uint32_t _New_value) {
    return modify_integer(_Name.c_str(), _New_value);
}

// FUNCTION registry_manager::modify_string
_NODISCARD bool registry_manager::modify_string(
    const wchar_t* const _Name, const wchar_t* const _New_value) noexcept {
    if (!_Mykey.is_open()) { // no key is open
        return false;
    }

    using _Traits = string_traits<wchar_t, DWORD>;
    return ::RegSetValueExW(_Mykey, _Name, 0, REG_SZ, reinterpret_cast<const unsigned char*>(_New_value),
        (_Traits::length(_New_value) + 1) * sizeof(wchar_t)) == ERROR_SUCCESS;
}

_NODISCARD bool registry_manager::modify_string(
    const wstring_view _Name, const wstring_view _New_value) noexcept {
    return modify_string(_Name.data(), _New_value.data());
}

_NODISCARD bool registry_manager::modify_string(
    const wstring& _Name, const wstring& _New_value) {
    return modify_string(_Name.c_str(), _New_value.c_str());
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD