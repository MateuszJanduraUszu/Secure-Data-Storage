// utf16.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <encoding/utf16.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION utf16_string constructors/destructor
utf16_string::utf16_string() noexcept : _Mystr() {}

utf16_string::utf16_string(const utf16_string& _Other) : _Mystr(_Other._Mystr) {}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26439) // C26439: declare function as 'noexcept'
#endif // _MSC_VER
utf16_string::utf16_string(utf16_string&& _Other) : _Mystr(_STD move(_Other._Mystr)) {}

utf16_string::utf16_string(const wchar_t* const _Text) : _Mystr(_Text) {}

utf16_string::utf16_string(const wchar_t* const _Text, const size_type _Count) : _Mystr(_Text, _Count) {}

utf16_string::utf16_string(const wstring_view _Text) : _Mystr(_Text.data(), _Text.size()) {}

utf16_string::utf16_string(const wstring& _Text) : _Mystr(_Text) {}

utf16_string::~utf16_string() noexcept {}

// FUNCTION utf16_string::from_utf8
_NODISCARD utf16_string utf16_string::from_utf8(const char* const _Text) {
    return from_utf8(_Text, string_traits<char, size_type>::length(_Text));
}

_NODISCARD utf16_string utf16_string::from_utf8(const char* const _Text, const size_type _Count) {
    UnicodeString _Unc = UnicodeString::fromUTF8(StringPiece(_Text, static_cast<int>(_Count)));
    return {reinterpret_cast<const wchar_t*>(
        _Unc.getTerminatedBuffer()), static_cast<size_type>(_Unc.length())};
}

_NODISCARD utf16_string utf16_string::from_utf8(const string_view _Text) {
    return from_utf8(_Text.data(), _Text.size());
}

_NODISCARD utf16_string utf16_string::from_utf8(const string& _Text) {
    return from_utf8(_Text.c_str(), _Text.size());
}

// FUNCTION utf16_string::operator=
utf16_string& utf16_string::operator=(const utf16_string& _Other) {
    if (this != _SDSDLL addressof(_Other)) {
        _Mystr = _Other._Mystr;
    }

    return *this;
}

utf16_string& utf16_string::operator=(utf16_string&& _Other) {
    if (this != _SDSDLL addressof(_Other)) {
        _Mystr = _STD move(_Other._Mystr);
    }

    return *this;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

// FUNCTION utf16_string::empty
_NODISCARD bool utf16_string::empty() const noexcept {
    return _Mystr.empty();
}

// FUNCTION utf16_string::size
_NODISCARD typename utf16_string::size_type utf16_string::size() const noexcept {
    return _Mystr.size();
}

// FUNCTION utf16_string::data
_NODISCARD typename utf16_string::pointer utf16_string::data() noexcept {
    return _Mystr.data();
}

_NODISCARD typename utf16_string::const_pointer utf16_string::data() const noexcept {
    return _Mystr.data();
}

// FUNCTION utf16_string::c_str
_NODISCARD typename utf16_string::const_pointer utf16_string::c_str() const noexcept {
    return _Mystr.c_str();
}

// FUNCTION utf16_string::view
_NODISCARD wstring_view utf16_string::view() const noexcept {
    return wstring_view{_Mystr.c_str(), _Mystr.size()};
}

// FUNCTION utf16_string::str
_NODISCARD const wstring& utf16_string::str() const noexcept {
    return _Mystr;
}

// FUNCTION operator==
_NODISCARD bool operator==(const utf16_string& _Left, const utf16_string& _Right) {
    return _Left.str() == _Right.str();
}

// FUNCTION operator!=
_NODISCARD bool operator!=(const utf16_string& _Left, const utf16_string& _Right) {
    return _Left.str() != _Right.str();
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD