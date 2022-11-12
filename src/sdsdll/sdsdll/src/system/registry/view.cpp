// view.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/registry/view.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Is_string_type
_NODISCARD constexpr bool _Is_string_type(const registry_value_type _Type) noexcept {
    switch (_Type) {
    case registry_value_type::string:
    case registry_value_type::expand_string:
    case registry_value_type::multi_string:
        return true;
    default:
        return false;
    }
}

// FUNCTION registry_view_manager constructors/destructor
registry_view_manager::registry_view_manager() noexcept : _Mykey() {}

registry_view_manager::registry_view_manager(registry_key& _Key) : _Mykey(_Key) {}

registry_view_manager::registry_view_manager(registry_key&& _Key) : _Mykey(_STD move(_Key)) {}

registry_view_manager::~registry_view_manager() noexcept {}

// FUNCTION registry_view_manager::_Get_buffer_size
_NODISCARD DWORD registry_view_manager::_Get_buffer_size(const wchar_t* const _Name) const noexcept {
    DWORD _Result = 0;
    switch (::RegGetValueW(_Mykey, nullptr, _Name, RRF_RT_ANY, nullptr, nullptr, &_Result)) {
    case ERROR_SUCCESS:
    case ERROR_MORE_DATA: // ERROR_MORE_DATA is not an error in this case
        // Note: The RegGetValueW() adds room for the null-terminator. It is redundant,
        //       because the basic_string provides it.
        return _Result - sizeof(wchar_t);
    default:
        return 0;
    }
}

// FUNCTION registry_view_manager::key
_NODISCARD registry_key& registry_view_manager::key() noexcept {
    return _Mykey;
}

_NODISCARD const registry_key& registry_view_manager::key() const noexcept {
    return _Mykey;
}

// FUNCTION registry_view_manager::read_default_value
_NODISCARD wstring registry_view_manager::read_default_value() {
    return _Mykey.is_open() ? read_string(nullptr) : wstring{};
}

// FUNCTION registry_view_manager::read_integer
_NODISCARD uint32_t registry_view_manager::read_integer(const wchar_t* const _Name) noexcept {
    if (!_Mykey.is_open()) { // no key is open
        return 0;
    }

    uint32_t _Result = 0;
    DWORD _Bytes     = sizeof(uint32_t); // DWORD size (32-bit integer)
    return ::RegGetValueW(_Mykey, nullptr, _Name, RRF_RT_DWORD,
        nullptr, &_Result, &_Bytes) == ERROR_SUCCESS ? _Result : 0;
}

_NODISCARD uint32_t registry_view_manager::read_integer(const wstring_view _Name) noexcept {
    return read_integer(_Name.data());
}

_NODISCARD uint32_t registry_view_manager::read_integer(const wstring& _Name) {
    return read_integer(_Name.c_str());
}

// FUNCTION registry_view_manager::read_string
_NODISCARD wstring registry_view_manager::read_string(const wchar_t* const _Name) {
    if (!_Mykey.is_open()) { // no key is open
        return wstring{};
    }

    DWORD _Buf_size = _Get_buffer_size(_Name);
    wstring _Result(static_cast<size_t>(_Buf_size) / sizeof(wchar_t), wchar_t{0});
    if (::RegGetValueW(
        _Mykey, nullptr, _Name, RRF_RT_ANY, nullptr, _Result.data(), &_Buf_size) == ERROR_SUCCESS) {
        _Result.pop_back(); // remove additional null-terminator
        return _Result;
    } else {
        return wstring{};
    }
}

_NODISCARD wstring registry_view_manager::read_string(const wstring_view _Name) {
    return read_string(_Name.data());
}

_NODISCARD wstring registry_view_manager::read_string(const wstring& _Name) {
    return read_string(_Name.c_str());
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD