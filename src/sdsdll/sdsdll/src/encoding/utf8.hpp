// utf8.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_ENCODING_UTF8_HPP_
#define _SDSDLL_ENCODING_UTF8_HPP_
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
using ::icu::UnicodeString;

// STD types
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// FUNCTION _Get_utf8_chunk_size
_SDSDLL_API _NODISCARD constexpr size_t _Get_utf8_chunk_size(const char _Ch) noexcept;

// CLASS utf8_string
class _SDSDLL_API utf8_string { // stores a UTF-8 string
public:
    using value_type      = char;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = char*;
    using const_pointer   = const char*;
    using reference       = char&;
    using const_reference = const char&;

    // constructs a UTF-8 string from a UTF-16 string
    _NODISCARD static utf8_string from_utf16(const wchar_t* const _Text);
    _NODISCARD static utf8_string from_utf16(const wchar_t* const _Text, const size_type _Count);
    _NODISCARD static utf8_string from_utf16(const wstring_view _Text);
    _NODISCARD static utf8_string from_utf16(const wstring& _Text);

    utf8_string() noexcept;
    utf8_string(const utf8_string& _Other);
    utf8_string(utf8_string&& _Other);
    ~utf8_string() noexcept;

    utf8_string(const char* const _Text);
    utf8_string(const char* const _Text, const size_type _Count);
    utf8_string(const string_view _Text);
    utf8_string(const string& _Text);

    utf8_string& operator=(const utf8_string& _Other);
    utf8_string& operator=(utf8_string&& _Other);

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
    _NODISCARD string_view view() const noexcept;

    // returns the current text
    _NODISCARD const string& str() const noexcept;

private:
    string _Mystr;
};

// FUNCTION operator==
_SDSDLL_API _NODISCARD bool operator==(const utf8_string& _Left, const utf8_string& _Right);

// FUNCTION operator!=
_SDSDLL_API _NODISCARD bool operator!=(const utf8_string& _Left, const utf8_string& _Right);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_ENCODING_UTF8_HPP_