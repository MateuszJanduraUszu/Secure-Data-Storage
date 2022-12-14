// utf16.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_ENCODING_UTF16_HPP_
#define _SDSDLL_ENCODING_UTF16_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <string>
#include <type_traits>
#include <unicode/unistr.h>

// ICU types
using ::icu::StringPiece;
using ::icu::UnicodeString;

// STD types
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// FUNCTION count_utf16_bytes
_SDSDLL_API _NODISCARD size_t count_utf16_bytes(const wchar_t* _Text, size_t _Count) noexcept;

// CLASS utf16_string
class _SDSDLL_API utf16_string { // stores a UTF-16 string
public:
    using value_type      = wchar_t;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = wchar_t*;
    using const_pointer   = const wchar_t*;
    using reference       = wchar_t&;
    using const_reference = const wchar_t&;

    // constructs a UTF-16 string from a UTF-8 string
    _NODISCARD static utf16_string from_utf8(const char* const _Text);
    _NODISCARD static utf16_string from_utf8(const char* const _Text, const size_type _Count);
    _NODISCARD static utf16_string from_utf8(const string_view _Text);
    _NODISCARD static utf16_string from_utf8(const string& _Text);

    utf16_string() noexcept;
    utf16_string(const utf16_string& _Other);
    utf16_string(utf16_string&& _Other);
    ~utf16_string() noexcept;

    utf16_string(const wchar_t* const _Text);
    utf16_string(const wchar_t* const _Text, const size_type _Count);
    utf16_string(const wstring_view _Text);
    utf16_string(const wstring& _Text);

    utf16_string& operator=(const utf16_string& _Other);
    utf16_string& operator=(utf16_string&& _Other);

    // checks if the current text is empty
    _NODISCARD bool empty() const noexcept;

    // returns the current text size
    _NODISCARD size_type size() const noexcept;

    // returns the current text as a mutable C-style string
    _NODISCARD pointer data() noexcept;

    // returns the current text as a non-mutable C-style string
    _NODISCARD const_pointer data() const noexcept;

    // returns the current text as a non-mutable C-style string
    _NODISCARD const_pointer c_str() const noexcept;

    // returns the current text as a view
    _NODISCARD wstring_view view() const noexcept;

    // returns the current text
    _NODISCARD const wstring& str() const noexcept;

private:
    wstring _Mystr;
};

// FUNCTION operator==
_SDSDLL_API _NODISCARD bool operator==(const utf16_string& _Left, const utf16_string& _Right);

// FUNCTION operator!=
_SDSDLL_API _NODISCARD bool operator!=(const utf16_string& _Left, const utf16_string& _Right);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_ENCODING_UTF16_HPP_