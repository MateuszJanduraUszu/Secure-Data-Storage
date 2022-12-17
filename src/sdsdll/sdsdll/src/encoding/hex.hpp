// hex.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_ENCODING_HEX_HPP_
#define _SDSDLL_ENCODING_HEX_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/string_traits.hpp>
#include <cstddef>
#include <cstdio>
#include <string>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// FUNCTION TEMPLATE _Fast_strcvt
template <class _From, class _To>
_NODISCARD constexpr basic_string<_To> _Fast_strcvt(const basic_string<_From>& _Str);

// FUNCTION TEMPLATE _String_to_hex
template <class _Elem>
_NODISCARD constexpr basic_string<_Elem> _String_to_hex(const _Elem* _Str, size_t _Count);

// ENUM CLASS hex_format
enum class hex_format : unsigned char {
    bytes,
    narrow,
    wide
};

// STRUCT TEMPLATE _Choose_native_hex_format
template <class>
struct _Choose_native_hex_format {};

template <>
struct _Choose_native_hex_format<char> {
    static constexpr hex_format value = hex_format::narrow;
};

template <>
struct _Choose_native_hex_format<unsigned char> {
    static constexpr hex_format value = hex_format::bytes;
};

template <>
struct _Choose_native_hex_format<wchar_t> {
    static constexpr hex_format value = hex_format::wide;
};

// CONSTANT TEMPLATE native_hex_format
template <class _Elem>
inline constexpr hex_format native_hex_format = _Choose_native_hex_format<_Elem>::value;

// STRUCT TEMPLATE _Choose_hex_elem
template <hex_format>
struct _Choose_hex_elem {};

template <>
struct _Choose_hex_elem<hex_format::bytes> {
    using type = unsigned char;
};

template <>
struct _Choose_hex_elem<hex_format::narrow> {
    using type = char;
};

template <>
struct _Choose_hex_elem<hex_format::wide> {
    using type = wchar_t;
};

// ALIAS TEMPLATE _Choose_hex_elem_t
template <hex_format _Fmt>
using _Choose_hex_elem_t = typename _Choose_hex_elem<_Fmt>::type;

// ALIAS TEMPLATE hex_string
template <hex_format _Fmt>
using hex_string = basic_string<_Choose_hex_elem_t<_Fmt>>;

// FUNCTION TEMPLATE as_hex
template <class _Elem, hex_format _Fmt = native_hex_format<_Elem>>
_SDSDLL_API _NODISCARD constexpr hex_string<_Fmt> as_hex(const _Elem* const _Str, const size_t _Count);

template <class _Elem, hex_format _Fmt = native_hex_format<_Elem>>
_SDSDLL_API _NODISCARD constexpr hex_string<_Fmt> as_hex(const _Elem* const _Str);

template <class _Elem, hex_format _Fmt = native_hex_format<_Elem>>
_SDSDLL_API _NODISCARD constexpr hex_string<_Fmt> as_hex(const basic_string_view<_Elem> _Str);

template <class _Elem, hex_format _Fmt = native_hex_format<_Elem>>
_SDSDLL_API _NODISCARD constexpr hex_string<_Fmt> as_hex(const basic_string<_Elem>& _Str);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_ENCODING_HEX_HPP_