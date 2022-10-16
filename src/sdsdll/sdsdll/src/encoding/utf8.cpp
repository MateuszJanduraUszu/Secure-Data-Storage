// utf8.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <encoding/utf8.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Get_utf8_chunk_size
_NODISCARD constexpr size_t _Get_utf8_chunk_size(const char _Ch) noexcept {
    if ((_Ch & 0x80) == 0) { // 1 byte per word
        return 1;
    } else if ((_Ch & 0xE0) == 0xC0) { // 2 bytes per word
        return 2;
    } else if ((_Ch & 0xF0) == 0xE0) { // 3 bytes per word
        return 3;
    } else if ((_Ch & 0xF8) == 0xF0) { // 4 bytes per word
        return 4;
    } else { // code point too large for UTF-8
        return 0;
    }
}

// FUNCTION utf8_string constructors/destructor
utf8_string::utf8_string() noexcept : _Mystr() {}

utf8_string::utf8_string(const utf8_string& _Other) : _Mystr(_Other._Mystr) {}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26439) // C26439: declare function as 'noexcept'
#endif // _MSC_VER
utf8_string::utf8_string(utf8_string&& _Other) : _Mystr(_STD move(_Other._Mystr)) {}

utf8_string::utf8_string(const char* const _Text) : _Mystr(_Text) {}

utf8_string::utf8_string(const char* const _Text, const size_type _Count) : _Mystr(_Text, _Count) {}

utf8_string::utf8_string(const string_view _Text) : _Mystr(_Text.data(), _Text.size()) {}

utf8_string::utf8_string(const string& _Text) : _Mystr(_Text) {}

utf8_string::~utf8_string() noexcept {}

// FUNCTION utf8_string::from_utf16
_NODISCARD utf8_string utf8_string::from_utf16(const wchar_t* const _Text) {
    return from_utf16(_Text, string_traits<wchar_t, size_type>::length(_Text));
}

_NODISCARD utf8_string utf8_string::from_utf16(const wchar_t* const _Text, const size_type _Count) {
    string _Result;
    UnicodeString _Unc(_Text, static_cast<int32_t>(_Count));
    _Unc.toUTF8String(_Result);
    return _Result;
}

_NODISCARD utf8_string utf8_string::from_utf16(const wstring_view _Text) {
    return from_utf16(_Text.data(), _Text.size());
}

_NODISCARD utf8_string utf8_string::from_utf16(const wstring& _Text) {
    return from_utf16(_Text.c_str(), _Text.size());
}

// FUNCTION utf8_string::operator=
utf8_string& utf8_string::operator=(const utf8_string& _Other) {
    if (this != _SDSDLL addressof(_Other)) {
        _Mystr = _Other._Mystr;
    }

    return *this;
}

utf8_string& utf8_string::operator=(utf8_string&& _Other) {
    if (this != _SDSDLL addressof(_Other)) {
        _Mystr = _STD move(_Other._Mystr);
    }

    return *this;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

// FUNCTION utf8_string::empty
_NODISCARD bool utf8_string::empty() const noexcept {
    return _Mystr.empty();
}

// FUNCTION utf8_string::size
_NODISCARD utf8_string::size_type utf8_string::size() const noexcept {
    return _Mystr.size();
}

// FUNCTION utf8_string::data
_NODISCARD utf8_string::pointer utf8_string::data() noexcept {
    return _Mystr.data();
}

_NODISCARD utf8_string::const_pointer utf8_string::data() const noexcept {
    return _Mystr.data();
}

// FUNCTION utf8_string::c_str
_NODISCARD utf8_string::const_pointer utf8_string::c_str() const noexcept {
    return _Mystr.c_str();
}

// FUNCTION utf8_string::view
_NODISCARD string_view utf8_string::view() const noexcept {
    return string_view{_Mystr.c_str(), _Mystr.size()};
}

// FUNCTION utf8_string::str
_NODISCARD const string& utf8_string::str() const noexcept {
    return _Mystr;
}

// FUNCTION operator==
_NODISCARD bool operator==(const utf8_string& _Left, const utf8_string& _Right) {
    return _Left.str() == _Right.str();
}

// FUNCTION operator!=
_NODISCARD bool operator!=(const utf8_string& _Left, const utf8_string& _Right) {
    return _Left.str() != _Right.str();
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD